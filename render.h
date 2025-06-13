#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <Adafruit_NeoPixel.h>
#include "animation.h"
#include <math.h>

/**
 * @brief Renderer class for managing LED animations
 * @details Contains configuration, state, and the current animation
 * for the LED strip. Provides thread-safe access to animation data.
 */
struct Renderer {
    bool earlyExit;
    bool RUNNING;                // Flag indicating if animation is currently running
    bool REPEAT;                 // Flag indicating if animation should repeat
    uint8_t LEDCOUNT;            // Number of LEDs in the strip
    uint8_t PIN;                 // GPIO pin connected to the LED strip
    uint16_t DELAY;              // Delay between animation frames (ms)
    uint16_t REPEATDELAY;        // Delay between animation repeats (ms)
    float SPEED;                 // Animation speed multiplier
    float PEAKBRIGHTNESS;        // Maximum brightness (0.0-1.0)
    Adafruit_NeoPixel SCREEN;    // NeoPixel LED controller
    Animation CURRENTANIMATION;  // Currently loaded animation
    SemaphoreHandle_t LOCK;      // Mutex for thread-safe access

    /**
     * @brief Constructor
     * @details Initializes the mutex and interactive animation parameters
     */
    Renderer(
            uint8_t ledCount = 10,
            uint8_t pin = 42, 
            uint16_t delay = 50,
            uint16_t repeatDelay = 50, 
            float speed = 1.0f,
            float peakBrightness = 0.40f,
            bool repeat = true,
            bool running = false)
        :LEDCOUNT(ledCount),
        PIN(pin),
        DELAY(delay),
        REPEATDELAY(repeatDelay),
        SPEED(speed),
        PEAKBRIGHTNESS(peakBrightness),
        REPEAT(repeat),
        RUNNING(running) {
        LOCK = xSemaphoreCreateMutex();
    }

    /**
     * @brief Destructor
     * @details Frees resources
     */
    ~Renderer() {
        if (LOCK != nullptr) {
            vSemaphoreDelete(LOCK);
            LOCK = nullptr;
        }
    }

    /**
     * @brief Set the current animation
     * @param anim The animation to set
     * @details Copies the animation data to the current animation
     */
    void setAnimation(Animation& anim) {
        // Set the current animation as non-running
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->RUNNING = false;
        xSemaphoreGive(LOCK);
        
        // Give the other thread time to stop rendering task
        vTaskDelay(this->REPEATDELAY / portTICK_PERIOD_MS);

        // Safely copy the animation data
        xSemaphoreTake(LOCK, portMAX_DELAY);
        xSemaphoreTake(anim.LOCK, portMAX_DELAY);
        xSemaphoreTake(CURRENTANIMATION.LOCK, portMAX_DELAY);
        
        debugln("Clearing old animation");
        CURRENTANIMATION.FRAMES->clear();
        
        debugln("Copying new animation data");
        // Deep copy each frame
        for (const auto& frame : *(anim.FRAMES)) {
            CURRENTANIMATION.FRAMES->push_back(frame);
        }
        
        CURRENTANIMATION.NAME = anim.NAME;
        xSemaphoreGive(CURRENTANIMATION.LOCK);
        xSemaphoreGive(anim.LOCK);

        // Start the animation
        debugln("Setting new animation as running");
        this->RUNNING = true;
        xSemaphoreGive(LOCK);

        debugln(">> New animation " + CURRENTANIMATION.NAME + " set with " + 
                String(CURRENTANIMATION.FRAMES->size()) + " frames");
    }

    /**
     * @brief Checks if an animation is currently running
     * @return True if running, false otherwise
     */
    bool isRunning() const {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        bool running = RUNNING;
        xSemaphoreGive(LOCK);
        return running;
      }
    
    /**
     * @brief Sets the running flag
     * @param running The new running flag
     */
    void setRunning(bool running) {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        RUNNING = running;
        xSemaphoreGive(LOCK);
    }
    
    /**
     * @brief Clears the screen
     * @details Clears the LED strip
     */
    void clearScreen() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        SCREEN.clear();
        xSemaphoreGive(LOCK);
    }

    /**
     * @brief Get the peak brightness
     */
    float getPeakBrightness() const {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        float brightness = PEAKBRIGHTNESS;
        xSemaphoreGive(LOCK);
        return brightness;
    }

    /**
     * @brief Set the peak brightness
     * @param brightness The new peak brightness
     */
    void setPeakBrightness(float brightness) { 
        // Set the current animation as non-running
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->RUNNING = false;
        this->PEAKBRIGHTNESS = min(1.0f, max(0.0f, brightness));
        xSemaphoreGive(LOCK);
        
        // Give the other thread time to stop rendering task
        vTaskDelay(this->REPEATDELAY / portTICK_PERIOD_MS);
    }

    /**
     * @brief Set Screen pixel value
     * @param pixel The pixel to set
     */
    void setPixelColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
        if (pixel >= LEDCOUNT) return;
        
        xSemaphoreTake(LOCK, portMAX_DELAY);
        SCREEN.setPixelColor(pixel, SCREEN.Color(r, g, b));
        xSemaphoreGive(LOCK);
    }

    /**
     * @brief Set the repeat flag
     * @param repeat The new repeat flag
     */
    void setRepeat(bool repeat) {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->REPEAT = repeat;
        xSemaphoreGive(LOCK);
        if (repeat) this->setRunning(repeat);
    }

    /**
     * @brief Get the repeat flag
     * @return The repeat flag state
     */
    bool getRepeat() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        bool repeat = this->REPEAT;
        xSemaphoreGive(LOCK);
        return repeat;
    }

    /**
     * @brief Set the Animation Speed
     * @param count The new LED count
     */
    void setSpeed(float speed) {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->SPEED = speed;
        xSemaphoreGive(LOCK);
        this->setEarlyExit(true);
    }

    /**
     * @brief Print configuration values for debugging
     * @details Outputs LED count, pin, speed, and brightness
     */
    void print() const {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        debugln("LED COUNT: " + String(LEDCOUNT));
        debugln("PIN: " + String(PIN));
        debugln("SPEED: " + String(SPEED));
        debugf("PEAK BRIGHTNESS: %f\n", PEAKBRIGHTNESS);
        debugln();
        xSemaphoreGive(LOCK);
    }
    
    /**
     * @brief Initialize the NeoPixel screen
     * @details Creates a new Adafruit_NeoPixel object and clears the screen
     */
    void initScreen() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        debugln("Initializing NeoPixel screen");
        // Create a fresh NeoPixel object
        Adafruit_NeoPixel* SCREENPTR = new Adafruit_NeoPixel(this->LEDCOUNT, PIN, NEO_GRB + NEO_KHZ800);
        this->SCREEN = *SCREENPTR;
        SCREEN.begin();
        // Clear all pixels and update once
        for (int i = 0; i < LEDCOUNT; i++) {
            SCREEN.setPixelColor(i, SCREEN.Color(0, 0, 0));
        }
        SCREEN.show();
        debugln("NeoPixel screen initialized");
        xSemaphoreGive(LOCK);
    }

    /**
     * @brief Show the NeoPixel screen
     */
    void showScreen() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        SCREEN.show();
        xSemaphoreGive(LOCK);
    }
    
    /**
     * @brief Get the current animation name
     * @return The name of the current animation
     */
    String getCurrentAnimationName() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        xSemaphoreTake(CURRENTANIMATION.LOCK, portMAX_DELAY);
        String name = CURRENTANIMATION.NAME;
        xSemaphoreGive(CURRENTANIMATION.LOCK);
        xSemaphoreGive(LOCK);
        return name;
    }

    /**
     * @brief Set the early exit flag
     * @param exit The new early exit flag
     */
    void setEarlyExit(bool exit) {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->earlyExit = exit;
        xSemaphoreGive(LOCK);
    }

    /**
     * @brief Get the early exit flag state
     * @return The early exit flag state
     */
    bool getEarlyExit() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        bool exit = this->earlyExit;
        xSemaphoreGive(LOCK);
        return exit;
    }

    /**
     * Delay for portions of a given time in steps, allowing for renderer to exit delays early.
     * NOTE: This isn't `TECHNICALLY` thread-safe in theory, and should only be called from the main/render threads.
     * It's mostly safe however since no data is changed between loops, and all the render method calls used
     * are thread-safe.
     * @brief Performs a delay on a given thread that can be interrupted early.
     * @param milliseconds Total delay time in milliseconds
     * @param chunkSize Size of each delay chunk in milliseconds (default: 10ms)
     * @return true if interrupted, false if completed the full delay
     */
    bool interruptableDelay(unsigned long milliseconds, unsigned long chunkSize = 10) {
        // Calculate how many chunks we need
        unsigned long chunks = milliseconds / chunkSize;
        unsigned long remainder = milliseconds % chunkSize;
        bool interrupted = false;
        
        // Delay in chunks, checking for early exit between each
        for (unsigned long i = 0; i < chunks; i++) {
            interrupted = this->getEarlyExit();
            if (interrupted) return true;
            vTaskDelay(chunkSize / portTICK_PERIOD_MS);
        }

        // Handle any remaining milliseconds that didn't fit into chunks
        if (remainder > 0) {
            interrupted = this->getEarlyExit();
            if (interrupted) return true;
            vTaskDelay(remainder / portTICK_PERIOD_MS);
        }
        
        // Check one final time in case it was set during the last delay
        interrupted = this->getEarlyExit();
        return interrupted;
    }

    void updateMovingBarAnimation(bool upPressed, bool downPressed);
};

/**
 * Render the current animation with the given renderer settings.
 * @param rend The renderer to use
 */
void render(Renderer* rend);


#endif // RENDER_H