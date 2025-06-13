#include "render.h"

/**
 * Render the current animation with the given renderer settings.
 * @param rend The renderer to use
 */
void render(Renderer* rend) {
    xSemaphoreTake(rend->LOCK, portMAX_DELAY);
    
    // Early safety checks
    if (rend == nullptr) return;

    if (!rend->RUNNING) {
        debugln(">> No animation to render or simply not running");
        xSemaphoreGive(rend->LOCK);
        return;
    }

    // Check if the current animation is empty
    xSemaphoreTake(rend->CURRENTANIMATION.LOCK, portMAX_DELAY);
    if (rend->CURRENTANIMATION.FRAMES->empty()) {
        debugln(">> Current animation is empty, stopping render");
        xSemaphoreGive(rend->CURRENTANIMATION.LOCK);
        xSemaphoreGive(rend->LOCK);
        return;
    }
    xSemaphoreGive(rend->CURRENTANIMATION.LOCK);

    // Create local copies of all settings
    bool isRunning = rend->RUNNING;
    bool repeat = rend->REPEAT;
    float brightness = rend->PEAKBRIGHTNESS;
    uint16_t delay = rend->DELAY;
    float speed = rend->SPEED;
    uint8_t ledCount = rend->LEDCOUNT;


    // Make a FULL copy of just the current frame data to prevent any possibility of accessing freed memory
    xSemaphoreTake(rend->CURRENTANIMATION.LOCK, portMAX_DELAY);
    String currentName = rend->CURRENTANIMATION.NAME;
    String previousName  = currentName;    

    debugln(">> Copying frame data");
    std::vector<std::vector<std::array<uint8_t, 4>>> frames(*(rend ->CURRENTANIMATION.FRAMES));
    std::vector<std::array<uint8_t, 4>> frame = frames[0];
    size_t frameCount = frames.size();
    size_t frameSize = frames[0].size();
    xSemaphoreGive(rend->CURRENTANIMATION.LOCK);
    xSemaphoreGive(rend->LOCK);

    debugln(">> Starting render loop");

    for (size_t frameIdx = 0; frameIdx < frameCount && isRunning; frameIdx++) {
        // Get the current frame settings
        xSemaphoreTake(rend->LOCK, portMAX_DELAY);
        isRunning = rend->RUNNING;
        repeat = rend->REPEAT;
        brightness = rend->PEAKBRIGHTNESS;
        delay = rend->DELAY;
        speed = rend->SPEED;
        currentName = rend->CURRENTANIMATION.getName();
        ledCount = rend->LEDCOUNT;
        xSemaphoreGive(rend->LOCK);

        // Check if the animation has changed
        if (currentName != previousName) {
            debugln(">> Animation changed, stopping render");
            break;
        }

        // Check if the animation has stopped
        if (!isRunning) {
            debugln(">> Animation stopped, stopping render");
            break;
        }

        // Clear the screen
        debugln(">> Clearing screen");
        rend -> clearScreen();

        frame = frames[frameIdx];
        frameSize = frame.size();

        for (size_t pixelIdx = 0; pixelIdx < frameSize; pixelIdx++) {
            std::array<uint8_t, 4> pixel = frame[pixelIdx];
            if (pixel[0] > ledCount) continue;

            rend->setPixelColor(
                pixel[0],
                static_cast<uint8_t>(pixel[1]),
                static_cast<uint8_t>(pixel[2]),
                static_cast<uint8_t>(pixel[3])
            );
        }

        rend->showScreen();

        previousName = currentName;

        // Delay for the frame duration
        if (rend->interruptableDelay((unsigned long)(delay / speed))) {
            debugln(">> Render interrupted, stopping");
            rend->setEarlyExit(false);
            break;
        };
    }

    // Repeat the animation if necessary
    if (!rend -> REPEAT) rend -> setRunning(false);
}

/**
 * Fill a vector with pixel values for a range of indices
 * @param vec The vector to fill
 * @param start The starting index
 * @param end The ending index
 * @param value The value to fill in
 */
void fillVectorPixels(std::vector<std::array<uint8_t, 4>>* vec,
                       uint8_t start,
                       uint8_t end,
                       uint8_t value) {
    for (uint8_t i = start; i < end; i++) {
        std::array<uint8_t, 4> pixel = {
            static_cast<uint8_t>(i),
            value,
            value,
            value
        };
        vec->push_back(pixel);
    }
}

void FadeInFrames(std::vector<std::vector<std::array<uint8_t, 4>>>* frames,
                uint8_t start,
                uint8_t end,
                uint8_t minvalue,
                uint8_t maxvalue,
                uint8_t fadeFrames) {
    for (uint8_t i = 0; i < fadeFrames; i++) {
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(end - start + 1);
        for (uint8_t j = start; j < end; j++) {
            uint8_t brightness = static_cast<uint8_t>(
                minvalue + (maxvalue - minvalue) * (i / static_cast<float>(fadeFrames))
            );
            std::array<uint8_t, 4> pixel = {
                static_cast<uint8_t>(j),
                brightness,
                brightness,
                brightness
            };
            frame.push_back(pixel);
        }
        frames->push_back(frame);
    }
}

/**
 * Swap two uint8_t values
 * @param a The first value
 * @param b The second value
 */
void swap(uint8_t* a, uint8_t* b) {
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}