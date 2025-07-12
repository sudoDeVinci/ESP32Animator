#ifndef NEW_RENDER_H
#define NEW_RENDER_H

#include <Adafruit_NeoPixel.h>
#include "animation.h"
#include <math.h>


struct RenderState{
    volatile bool exitEarly = false; // Flag to exit rendering early
    volatile bool isRunning = false; // Flag to indicate if rendering is active
    volatile bool repeat = true; // Flag to indicate if the animation should repeat
    uint8_t ledCount = 10; // Number of LEDs in the strip
    uint8_t pin = 42; // Pin number for the NeoPixel strip
    uint16_t frameDelayMs = 50; // Delay between frames in milliseconds
    uint16_t repeatDelayMs = 50; // Delay before repeating the animation in milliseconds
    float speedCoefficient = 1.0f; // Speed coefficient for animation playback
    float peakBrightnessCoefficient = 0.40f; // Peak brightness coefficient for LED colors
    String currentAnimationName = "NONE"; // Name of the current animation

    RenderState(
        bool exitEarly = false,
        bool isRunning = false,
        bool repeat = true,
        uint8_t ledCount = 10,
        uint8_t pin = 42,
        uint16_t frameDelayMs = 50,
        uint16_t repeatDelayMs = 50,
        float speedCoefficient = 1.0f,
        float peakBrightnessCoefficient = 0.40f,
        String currentAnimationName = "NONE"
    ):
        exitEarly(exitEarly),
        isRunning(isRunning),
        repeat(repeat),
        ledCount(ledCount),
        pin(pin),
        frameDelayMs(frameDelayMs),
        repeatDelayMs(repeatDelayMs),
        speedCoefficient(speedCoefficient),
        peakBrightnessCoefficient(peakBrightnessCoefficient),
        currentAnimationName(currentAnimationName)
    {}


    RenderState(const RenderState& other) {
        exitEarly = other.exitEarly;
        isRunning = other.isRunning;
        repeat = other.repeat;
        ledCount = other.ledCount;
        pin = other.pin;
        frameDelayMs = other.frameDelayMs;
        repeatDelayMs = other.repeatDelayMs;
        speedCoefficient = other.speedCoefficient;
        peakBrightnessCoefficient = other.peakBrightnessCoefficient;
        currentAnimationName = other.currentAnimationName;
    }

    RenderState& operator=(const RenderState& other) {
        if (this == &other) return *this;

        exitEarly = other.exitEarly;
        isRunning = other.isRunning;
        repeat = other.repeat;
        ledCount = other.ledCount;
        pin = other.pin;
        frameDelayMs = other.frameDelayMs;
        repeatDelayMs = other.repeatDelayMs;
        speedCoefficient = other.speedCoefficient;
        peakBrightnessCoefficient = other.peakBrightnessCoefficient;
        currentAnimationName = other.currentAnimationName;

        return *this;
    }
};

/**
 * @brief Renderer class for managing LED animations
 * @details Contains configuration, state, and the current animation
 * for the LED strip. Provides thread-safe access to animation data.
 */
struct Renderer {
private:
    volatile bool exitEarly;
    volatile bool isRunning;
    volatile bool repeat;
    uint8_t ledCount;
    uint8_t pin;
    uint16_t frameDelayMs;
    uint16_t repeatDelayMs;
    float speedCoefficient;
    float peakBrightnessCoefficient;
    mutable std::mutex mutex_;
    Adafruit_NeoPixel screen;
    Animation currentAnimation;

public:
    Renderer(
        uint8_t ledCount = 10,
        uint8_t pin = 42, 
        uint16_t frameDelayMs = 50,
        uint16_t repeatDelayMs = 50, 
        float speedCoef = 1.0f,
        float peakBrightnessCoef = 0.40f,
        bool repeat = true,
        bool isRunning = false
        ):
        ledCount(ledCount),
        pin(pin),
        frameDelayMs(frameDelayMs),
        repeatDelayMs(repeatDelayMs),
        speedCoefficient(speedCoef),
        peakBrightnessCoefficient(peakBrightnessCoef),
        repeat(repeat),
        isRunning(isRunning),
        exitEarly(false),
        screen(ledCount, pin, NEO_GRB + NEO_KHZ800)
    {}

    Renderer(const RenderState& state) {
        ledCount = state.ledCount;
        pin = state.pin;
        frameDelayMs = state.frameDelayMs;
        repeatDelayMs = state.repeatDelayMs;
        speedCoefficient = state.speedCoefficient;
        peakBrightnessCoefficient = state.peakBrightnessCoefficient;
        repeat = state.repeat;
        isRunning = state.isRunning;
        exitEarly = state.exitEarly;
    }

    RenderState outputState() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return RenderState{
            exitEarly,
            isRunning,
            repeat,
            ledCount,
            pin,
            frameDelayMs,
            repeatDelayMs,
            speedCoefficient,
            peakBrightnessCoefficient,
            currentAnimation.getName()
        };
    }

    /**
     * @brief Destructor
     * @details Clears the screen before destruction
     */
    ~Renderer() {
        screen.clear();
        screen.show();
        debugln("Renderer destroyed and screen cleared");
    }

    void setAnimation(const Animation& anim) {
        // Set the current animation as non-running
        {
            std::lock_guard<std::mutex> lock(mutex_);
            this->isRunning = false;
        }
        
        // Give the other thread time to stop rendering task
        vTaskDelay(this->repeatDelayMs / portTICK_PERIOD_MS);

        // Safely copy the animation data
        {
            std::lock_guard<std::mutex> lock(mutex_);

            debugln("Copying new animation data");
            currentAnimation = anim; // This will properly copy all data
        
            this->isRunning = true;
        }

        debugln(">> New animation " + currentAnimation.getName() + " set with " + 
                String(currentAnimation.frameCount()) + " frames");
    }

    /**
     * @brief Checks if an animation is currently running
     * @return True if running, false otherwise
     */
    bool isRunning() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isRunning;
    }

    /**
     * @brief Sets the running state of the renderer
     * @param running The new running state
     */
    void setRunning(bool running) {
        std::lock_guard<std::mutex> lock(mutex_);
        isRunning = running;
    }

    /**
     * @brief Initializes the NeoPixel screen
     * @details Sets up the NeoPixel strip with the specified LED count and pin
     */
    void initializeScreen() {
        std::lock_guard<std::mutex> lock(mutex_);
        screen = Adafruit_NeoPixel(ledCount, pin, NEO_GRB + NEO_KHZ800);
        screen.begin();

        for (uint8_t i = 0; i < ledCount; i++) {
            screen.setPixelColor(i, screen.Color(0, 0, 0)); // Initialize all pixels to off
        }
        screen.show();
        debugln("NeoPixel screen initialized");
    }

    /**
     * @brief Clears the LED strip
     * @details Clears the LED strip and updates the screen
     */
    void clearScreen() {
        std::lock_guard<std::mutex> lock(mutex_);
        screen.clear();
    }

    /**
     * @brief Shows the current state of the LED strip
     * @details Updates the LED strip to reflect the current pixel colors
     */
    void showScreen() const {
        std::lock_guard<std::mutex> lock(mutex_);
        screen.show();
    }

    /**
     * @brief Gets the peak brightness coefficient
     * @return The peak brightness coefficient
     */
    float getPeakBrightness() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return peakBrightnessCoefficient;
    }

    /**
     * @brief Sets the peak brightness coefficient
     * @param brightness The new peak brightness coefficient
     */
    void setPeakBrightness(float brightness) {
        std::lock_guard<std::mutex> lock(mutex_);
        peakBrightnessCoefficient = std::clamp(brightness, 0.0f, 1.0f);
    }

    /**
     * @brief Sets an LED at a given pixel index to a specific color
     * @param pixel The pixel index and RGB color values
     */
    void setPixelColor(const Pixel& pixel) {
        if (pixel[0] >= ledCount) return;

        std::lock_guard<std::mutex> lock(mutex_);
        screen.setPixelColor(pixel[0], screen.Color(pixel[1], pixel[2], pixel[3]));
    }

    /**
     * @brief Writes a frame to the screen
     * @param frame The frame to write
     * @details This method is thread-safe and locks the mutex while writing the frame
     */
    void writeFrameToScreen(const Frame& frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const Pixel& pixel : frame) {
            if (pixel[0] >= ledCount) continue;
            screen.setPixelColor(
                pixel[0],
                static_cast<uint8_t>(pixel[1] * peakBrightnessCoefficient),
                static_cast<uint8_t>(pixel[2] * peakBrightnessCoefficient),
                static_cast<uint8_t>(pixel[3] * peakBrightnessCoefficient)
            );
        }
        screen.show();
    }

    /**
     * @brief Sets the repeat state of the renderer
     * @param repeat The new repeat state
     */
    void setRepeat(bool repeat) {
        std::lock_guard<std::mutex> lock(mutex_);
        this->repeat = repeat;
        if (repeat) {
            this->setRunning(repeat);
        }
    }

    /**
     * @brief Gets the repeat state of the renderer
     * @return The repeat state
     */
    bool getRepeat() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return repeat;
    }

    /**
     * @brief Sets the animation speed coefficient
     * @param speed The new speed coefficient
     */
    void setSpeed(float speed) {
        std::lock_guard<std::mutex> lock(mutex_);
        speedCoefficient = std::max(0.1f, speed); // Ensure speed is not zero
    }

    /**
     * @brief Gets the animation speed coefficient
     * @return The current speed coefficient
     */
    float getSpeed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return speedCoefficient;
    }


    /**
     * @brief Gets the LED count
     * @return The number of LEDs in the strip
     */
    uint8_t getLedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return ledCount;
    }

    /**
     * @brief Sets the LED count
     * @param count The new LED count
     */
    void setLedCount(uint8_t count) {
        if (count == 0 || count > 255) return;
        std::lock_guard<std::mutex> lock(mutex_);
        ledCount = count;
        screen.updateLength(ledCount);
        screen.begin();
        debugln("LED count set to " + String(ledCount));
    }
    
    
    void print() const {
        std::lock_guard<std::mutex> lock(mutex_);
        debugln("LED COUNT: " + String(ledCount));
        debugln("PIN: " + String(pin));
        debugln("SPEED: " + String(speedCoefficient));
        debugf("PEAK BRIGHTNESS: %f\n", peakBrightnessCoefficient);
        debugln();
    }

    /**
     * @brief Gets the current animation name
     * @return The name of the current animation
     */
    String getCurrentAnimationName() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentAnimation.getName();
    }

    /**
     * @brief Sets the early exit flag
     * @param exit The new early exit flag
     */
    void setEarlyExit(bool exit) {
        std::lock_guard<std::mutex> lock(mutex_);
        exitEarly = exit;
    }

    /**
     * @brief Checks if the current animation is empty
     * @return True if the current animation has no frames, false otherwise
     */
    bool isAnimationEmpty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentAnimation.getFrames().empty();
    }

    /**
     * @brief Gets the early exit flag state
     * @return The early exit flag state
     */
    bool getEarlyExit() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return exitEarly;
    }

    const FrameBuffer& getCurrentAnimationFrames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentAnimation.getFrames();
    }

    bool interruptableDelay(
        const unsigned long milliseconds,
        const unsigned long checkEveryMs = 10
    ) {
        unsigned long checks = milliseconds / checkEveryMs;
        unsigned long remainder = milliseconds % checkEveryMs;
        bool interrupted = false;

        // Delay in chunks, checking for early exit between each
        for (unsigned long i = 0; i < checks; i++) {
            interrupted = this->getEarlyExit();
            if (interrupted) return true;
            vTaskDelay(checkEveryMs / portTICK_PERIOD_MS);
        }

        // Handle any remaining milliseconds
        if (remainder > 0) {
            interrupted = this->getEarlyExit();
            if (interrupted) return true;
            vTaskDelay(remainder / portTICK_PERIOD_MS);
        }

        interrupted = this->getEarlyExit();
        return interrupted;
    }
};

/**
 * Render the current animation with the given renderer settings.
 * @param rend The renderer to use
 */
void render(Renderer& rend);

#endif