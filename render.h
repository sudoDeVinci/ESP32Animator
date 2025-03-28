#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <Adafruit_NeoPixel.h>
#include "animation.h"
#include <math.h>


/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @param ledCount The number of LEDs in the strip
 * @param minBrightness The minimum brightness coefficient
 * @param maxBrightness The maximum brightness coefficient
 * @param frequency The frequency of the breathing
 * @return The new animation
 * @details A smooth, sinusoidal increase and decrease in brightness, mimicking breathing.
 */
Animation* createBreatheAnimation(uint8_t ledCount, float minBrightness = 0.025, float maxBrightness = 1.0, float frequency = 1);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @param ledCount The number of LEDs in the strip
 * @param maxBrightness The maximum brightness of the bar
 * @param startHeight The starting height of the bar
 * @param endHeight The ending height of the bar
 * @param abruptFade If true, the bar will abruptly fade in and out
 * @return The new animation
 * @details A bright bar starts at X% of the height on each side and each “up” button press causes bar to grow towards the top, and each “down” button press causes bar to shrink.
 */
Animation* createGrowingBarAnimation(uint8_t ledCount, uint8_t maxBrightness, uint8_t startHeight = 0, uint8_t endHeight = 0, bool abruptFade = false);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief Creates a shrinking bar animation
 * @param ledCount Number of LEDs in the strip
 * @param maxBrightness Maximum brightness of the bar
 * @param startHeight Starting height of the bar (0 = auto)
 * @param endHeight Ending height of the bar (0 = auto)
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with shrinking bar effect
 * @details A bright bar shrinks from full height to the center
 */
Animation* createShrinkingBarAnimation(uint8_t ledCount, uint8_t maxBrightness, uint8_t startHeight = 0, uint8_t endHeight = 0, bool abruptFade = true);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief Creates an extending bar animation
 * @param ledCount Number of LEDs in the strip
 * @param maxBrightness Maximum brightness of the bar
 * @param endDistance Maximum distance from center (0 = auto)
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with extending bar effect
 * @details A bright line starts at the middle and extends symmetrically
 */
Animation* createExtendingBarAnimation(uint8_t ledCount, uint8_t maxBrightness, uint8_t endDistance = 0, bool abruptFade = false);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief Creates an extinguishing bar animation
 * @param ledCount Number of LEDs in the strip
 * @param maxBrightness Maximum brightness of the bar
 * @param retentionTime Time to keep the bar at full brightness (ms)
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with extinguishing bar effect
 */
Animation* createExtinguishingBarAnimation(uint8_t ledCount, uint8_t maxBrightness, uint16_t retentionTime = 500, bool abruptFade = false);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief This requires button input so we only render the first frame.
 * @param ledCount Number of LEDs in the strip
 * @param brightness Brightness of the bar
 * @param barSize Size of the bar
 * @return Animation object with moving bar effect
 */
Animation* createMovingBarAnimation(uint8_t ledCount, uint8_t brightness, uint8_t barSize = 5);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief Creates a new animation that grows up from the center
 * @param ledCount Number of LEDs in the strip
 * @param brightness Brightness of the bar
 * @param endDistance Maximum distance from center
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with growing bar effect
 */
Animation* createGrowUpAnimation(uint8_t ledCount, uint8_t brightness, uint8_t endDistance = 0, bool abruptFade = true);

/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief Creates a new animation that grows down from the center
 * @param ledCount Number of LEDs in the strip
 * @param brightness Brightness of the bar
 * @param endDistance Maximum distance from center
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with growing bar effect
 */
Animation* createGrowDownAnimation(uint8_t ledCount, uint8_t brightness, uint8_t endDistance = 0, bool abruptFade = true);

/**
 * @brief Starts with all LEDs on, and top half dims when “up” button pressed and bottom half dims when “down” button pressed
 * @details This animation requires button input for dimming, so we just create one frame
 * @param ledCount Number of LEDs in the strip
 * @param dimLevel Maximum brightness of the half
 * @param gradientFade If true, the fade is gradual; if false, it is abrupt
 * @return Animation object with first frame of half fade effect
 */
Animation* createHalfFadeAnimation(uint8_t ledCount, float dimLevel = 0.2, bool gradientFade = false);

/**
 * Create a new pulse animation with quick rise and slow decay
 * @param ledCount The number of LEDs in the strip
 * @param minBrightness The minimum brightness (0.0-1.0)
 * @param maxBrightness The maximum brightness (0.0-1.0)
 * @param attackProportion What fraction of the cycle is the attack (0.0-1.0)
 * @param frequency Relative speed of the animation (higher = faster)
 * @return Animation* Pointer to the created animation
 */
Animation* createPulseAnimation(uint8_t ledCount, 
                                float minBrightness = 0.015, 
                                float maxBrightness = 0.30,
                                float attackProportion = 0.15,
                                float frequency = 1.5);


/**
 * Create an animation where a bright dot circles around the LED strip
 * @param ledCount The number of LEDs in the strip
 * @param abruptFade If false, the dot has a faded trail; if true, it's just a single dot
 * @param clockwise Direction of movement (true = clockwise, false = counterclockwise)
 * @param trailLength Number of LEDs in the trail (ignored if abruptFade is true)
 * @param brightness Maximum brightness of the dot (0-255)
 * @return Animation* Pointer to the created animation
 */
Animation* createCirclingBrightDotAnimation(uint8_t ledCount,
                                            bool abruptFade,
                                            bool clockwise,
                                            uint8_t trailLength,
                                            uint8_t brightness);

/**
 * Create an animation where a dark spot circles around a fully lit LED strip
 * @param ledCount The number of LEDs in the strip
 * @param abruptTransition If false, the spot has a gradual edge; if true, it's a sharp dark spot
 * @param clockwise Direction of movement (true = clockwise, false = counterclockwise)
 * @param spotWidth Width of the dark spot in LEDs (wider with gradual transitions)
 * @param backgroundBrightness Brightness of the lit background (0-255)
 * @return Animation* Pointer to the created animation
 */
Animation* createCirclingDarkSpotAnimation(uint8_t ledCount,
                                           bool abruptTransition,
                                           bool clockwise,
                                           uint8_t spotWidth,
                                           uint8_t backgroundBrightness);


/**
 * @brief Renderer class for managing LED animations
 * @details Contains configuration, state, and the current animation
 * for the LED strip. Provides thread-safe access to animation data.
 */
struct Renderer {
    bool RUNNING;                // Flag indicating if animation is currently running
    bool REPEAT;                 // Flag indicating if animation should repeat
    uint8_t LEDCOUNT;            // Number of LEDs in the strip
    uint8_t PIN;                 // GPIO pin connected to the LED strip
    uint16_t DELAY;              // Delay between animation frames (ms)
    uint16_t REPEATDELAY;        // Delay between animation repeats (ms)
    float SPEED;                 // Animation speed multiplier
    float PEAKBRIGHTNESS;        // Maximum brightness (0.0-1.0)
    String MODE;                 // Current interactive mode (dynamically allocated)
    Adafruit_NeoPixel SCREEN;    // NeoPixel LED controller
    Animation CURRENTANIMATION;  // Currently loaded animation
    SemaphoreHandle_t LOCK;      // Mutex for thread-safe access

    // Animation button-specific parameters
    int barPosition;         // For MovingBarAnimation - current bar position
    int currentExtent;       // For ExtendingBarAnimation, GrowUpAnimation, GrowDownAnimation
    float topBrightness;     // For HalfFadeAnimation - top half brightness
    float bottomBrightness;  // For HalfFadeAnimation - bottom half brightness
    float frequency;           // For Breathe and Pulse - frequency of the animation
    bool abruptFade;
    float minSpeed = 0.005;
    float maxSpeed = 5.000;
    bool earlyExit = false;
    uint8_t maxLEDCount = 50;

    /**
     * @brief Constructor
     * @details Initializes the mutex and interactive animation parameters
     */
    Renderer() {
        LOCK = xSemaphoreCreateMutex();
        barPosition = 0;
        currentExtent = 0;
        topBrightness = 1.0;
        bottomBrightness = 1.0;
        frequency = 1.5f;
        abruptFade = false;

        RUNNING = false;
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
        // The Animation destructor will handle its own resources
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

        this->regenerateAnimation();
    }

    /**
     * @brief Set the LED count
     * @param count The new LED count
     */
    void setLEDCount(uint8_t count) {
        // Set the current animation as non-running
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->RUNNING = false;
        this->LEDCOUNT = count;
        xSemaphoreGive(LOCK);

        // Give the other thread time to stop rendering task
        vTaskDelay(this->REPEATDELAY / portTICK_PERIOD_MS);

        // Reinitialize the screen and animation
        // this->initScreen();
        this->regenerateAnimation();
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
     * @brief Set the LED count
     * @param count The new LED count
     */
    void setSpeed(float speed) {
        speed = speed + 1e-5; // Avoid division by zero
        xSemaphoreTake(LOCK, portMAX_DELAY);
        this->SPEED = max(this->minSpeed, min(this->maxSpeed, speed));
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
        Adafruit_NeoPixel* SCREENPTR = new Adafruit_NeoPixel(this->maxLEDCount, PIN, NEO_GRB + NEO_KHZ800);
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
     * @brief Regenerate the current animation
     * @details Re-creates the current animation with the current settings
     */
    void regenerateAnimation() {
        String name = this->getCurrentAnimationName();
        if (name == "NONE") return;

        Animation* animation = nullptr;
        uint8_t BRIGHTNESS = static_cast<uint8_t>(this->PEAKBRIGHTNESS * 255);
    
        debugln("Regenerating animation: " + name);
        
        if (name == "Breathe") animation = createBreatheAnimation(this->LEDCOUNT,
                                                                  0.010,
                                                                  this->PEAKBRIGHTNESS,
                                                                  this->frequency);
        else if (name == "Growing Bar") animation = createGrowingBarAnimation(this->LEDCOUNT,
                                                                              BRIGHTNESS,
                                                                              0,
                                                                              0,
                                                                              this->abruptFade);
        else if (name == "Shrinking Bar") animation = createShrinkingBarAnimation(this->LEDCOUNT,
                                                                                  BRIGHTNESS,
                                                                                  0,
                                                                                  0,
                                                                                  this->abruptFade);
        else if (name == "Extending Bar") animation = createExtendingBarAnimation(this->LEDCOUNT,
                                                                                  BRIGHTNESS,
                                                                                  0,
                                                                                  this->abruptFade);
        else if (name == "Extinguishing Bar") animation = createExtinguishingBarAnimation(this->LEDCOUNT,
                                                                                          BRIGHTNESS,
                                                                                          500,
                                                                                          this->abruptFade);
        else if (name == "Moving Bar") animation = createMovingBarAnimation(this->LEDCOUNT,
                                                                          BRIGHTNESS,
                                                                          3);
        else if (name == "Grow Up") animation = createGrowUpAnimation(this->LEDCOUNT,
                                                                    BRIGHTNESS,
                                                                    0,
                                                                    this->abruptFade);
        else if (name == "Grow Down") animation = createGrowDownAnimation(this->LEDCOUNT,
                                                                        BRIGHTNESS,
                                                                        0,
                                                                        this->abruptFade);
        else if (name == "Half Fade") animation = createHalfFadeAnimation(this->LEDCOUNT,
                                                                          0.10,
                                                                          this->abruptFade);
        else if (name == "Pulse") animation = createPulseAnimation(this->LEDCOUNT,
                                                                   0.010f,
                                                                   this->PEAKBRIGHTNESS,
                                                                   0.15f,
                                                                   this->frequency);
        else if (name == "Circling Bright Dot") animation = createCirclingBrightDotAnimation(this->LEDCOUNT,
                                                                                            this->abruptFade,
                                                                                            true,
                                                                                            3,
                                                                                            BRIGHTNESS);
        else if (name == "Circling Dark Spot") animation = createCirclingDarkSpotAnimation(this->LEDCOUNT,
                                                                                           this->abruptFade,
                                                                                           true,
                                                                                           3,
                                                                                           BRIGHTNESS);
        else {
            debugln("Invalid animation name: " + name);
            return;
        }

        if (animation != nullptr) {
            this->setAnimation(*animation);
            this->setEarlyExit(true);
            delete animation;
            debugln("Animation regenerated successfully");
        } else {
            debugln("Failed to regenerate animation");
        }
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

    uint8_t getMaxLEDCount() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        uint8_t count = this->maxLEDCount;
        xSemaphoreGive(LOCK);
        return count;
    }

    /**
     * NOTE: This isn't `TECHNICALLY` thread-safe, and should only be called from the main/render threads.
     *       It's mostly safe however since no data is changed between loops, and all method calls are thread-safe.
     * @brief Performs a delay that can be interrupted
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
};

/**
 * Render the current animation with the given renderer settings.
 * @param rend The renderer to use
 */
void render(Renderer* rend);


#endif // RENDER_H