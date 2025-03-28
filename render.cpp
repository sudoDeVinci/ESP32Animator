#include "render.h"

/**
 * Render the current animation with the given renderer settings.
 * @param rend The renderer to use
 */
void render(Renderer* rend) {

    //debugln(">> Started render task");

    xSemaphoreTake(rend->LOCK, portMAX_DELAY);
    
    // Early safety checks
    if (rend == nullptr) return;
    
    //debugln(">> Checking if animation is running");
    if (!rend->RUNNING) {
        debugln(">> No animation to render or simply not running");
        xSemaphoreGive(rend->LOCK);
        return;
    }

    // Check if the current animation is empty
    //debugln(">> Checking if current animation is empty");
    xSemaphoreTake(rend->CURRENTANIMATION.LOCK, portMAX_DELAY);
    if (rend->CURRENTANIMATION.FRAMES->empty()) {
        debugln(">> Current animation is empty, stopping render");
        xSemaphoreGive(rend->CURRENTANIMATION.LOCK);
        xSemaphoreGive(rend->LOCK);
        return;
    }
    xSemaphoreGive(rend->CURRENTANIMATION.LOCK);

    //debugln(">> Taking stock of settings and animation data");
    
    // Create local copies of all settings
    bool isRunning = rend->RUNNING;
    bool repeat = rend->REPEAT;
    float brightness = rend->PEAKBRIGHTNESS;
    uint16_t delay = rend->DELAY;
    float speed = rend->SPEED;
    uint8_t ledCount = rend->LEDCOUNT;

    // Local copies of the current animation data
    //debugln(">> Copying current animation data");
    xSemaphoreTake(rend->CURRENTANIMATION.LOCK, portMAX_DELAY);
    String currentName = rend->CURRENTANIMATION.NAME;
    String previousName  = currentName;    
    // Make a FULL copy of just the current frame data to prevent
    // any possibility of accessing freed memory
    //debugln(">> Copying frame data");
    std::vector<std::vector<std::array<uint8_t, 4>>> frames(*(rend ->CURRENTANIMATION.FRAMES));
    std::vector<std::array<uint8_t, 4>> frame = frames[0];
    size_t frameCount = frames.size();
    size_t frameSize = frames[0].size();
    xSemaphoreGive(rend->CURRENTANIMATION.LOCK);
    xSemaphoreGive(rend->LOCK);

    //debugln(">> Starting render loop");

    // Loop through all frames
    for (size_t frameIdx = 0; frameIdx < frameCount && isRunning; frameIdx++) {
        // Get the current frame settings
        //debugln(">> Getting current frame settings");
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
            //debugln(">> Animation changed, stopping render");
            break;
        }

        // Check if the animation has stopped
        if (!isRunning) {
            //debugln(">> Animation stopped, stopping render");
            break;
        }

        // Clear the screen
        //debugln(">> Clearing screen");
        rend -> clearScreen();

        //debugln(">> Rendering frame " + String(frameIdx) + " of " + String(frameCount));
        frame = frames[frameIdx];
        frameSize = frame.size();

        for (size_t pixelIdx = 0; pixelIdx < frameSize; pixelIdx++) {
            std::array<uint8_t, 4> pixel = frame[pixelIdx];
            if (pixel[0] > ledCount) continue;

            // debugln(">> Setting pixel " + String(pixel[0]) + " to " + String(pixel[1]) + ", " + String(pixel[2]) + ", " + String(pixel[3]));
            rend->setPixelColor(pixel[0],
                static_cast<uint8_t>(pixel[1]),
                static_cast<uint8_t>(pixel[2]),
                static_cast<uint8_t>(pixel[3])
            );
        }

        xSemaphoreTake(rend->LOCK, portMAX_DELAY);
        //debugln(">> Showing screen");
        rend->SCREEN.show();
        xSemaphoreGive(rend->LOCK);

        previousName = currentName;

        //debugln(">> Delaying for " + String(delay / speed) + "ms");
        // Delay for the frame duration
        //vTaskDelay((delay / speed) / portTICK_PERIOD_MS);
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
 * Create a new breathing animation
 * WARNING: Animation is dynamically allocated and must be freed.
 * @param ledCount The number of LEDs in the strip
 * @param minBrightness The minimum brightness
 * @param maxBrightness The maximum brightness
 * @param frequency The frequency of the breathing
 * @return The new animation
 * @details A smooth, sinusoidal increase and decrease in brightness, mimicking breathing.
 */
Animation* createBreatheAnimation(uint8_t ledCount,
                                float minBrightness,
                                float maxBrightness,
                                float frequency) {
    debugln(">> Creating breathe animation");
    Animation* animation = new Animation("Breathe");
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    const int frameCount = 90;

    debugln("Reserving memory for frames");
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    frames->reserve(frameCount);
    
    for (int i = 0; i < frameCount; i++) {
        debugln("Creating frame " + String(i) + " of " + String(frameCount));
        // Use an eased sine wave for more natural breathing
        float t = (float)i / frameCount;
        float easedT = 0.5f - 0.5f * cos(t * PI * 2);
        
        // Map to brightness range
        float brightness = minBrightness + (maxBrightness - minBrightness) * easedT;

        // Set all LEDs to the calculated brightness
        uint8_t pixelval = static_cast<uint8_t>(brightness * 255);

        // Create a new frame vector for this frame
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);  // Reserve space for all LEDs

        for (int led = 0; led < ledCount; led++) {
            std::array<uint8_t, 4> pixel = {
                static_cast<uint8_t>(led),
                pixelval,
                pixelval,
                pixelval
            };
            frame.push_back(pixel);  // Add pixel to the frame
        }
        
        // Add the complete frame to the frames vector
        frames->push_back(frame);
    }
    
    xSemaphoreGive(animation->LOCK);
    debugln("Breathe animation created - returning pointer");
    return animation;
}


/**
 * Create a new growing bar animation
 * WARNING: Animation is dynamically allocated and must be freed.
 * @param ledCount The number of LEDs in the strip
 * @param maxBrightness The maximum brightness of the bar
 * @param startHeight The starting height of the bar
 * @param endHeight The ending height of the bar
 * @param abruptFade If true, the bar will abruptly fade in and out
 * @return The new animation
 * @details A bright bar starts at X% of the height on each side and each “up” button press causes bar to grow towards the top, and each “down” button press causes bar to shrink.
 */
Animation* createGrowingBarAnimation(uint8_t ledCount,
                                     uint8_t maxBrightness,
                                     uint8_t startHeight,
                                     uint8_t endHeight,
                                     bool abruptFade) {
    Animation* animation = new Animation("Growing Bar");   
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;

    // Default values if not specified
    if (startHeight == 0) startHeight = max(1, (int)(ledCount * 0.1));
    if (endHeight == 0) endHeight = ledCount;
    
    int middleLed = ledCount / 2;

    debugln(">> Allocating space for all frames");
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    frames->reserve(endHeight - startHeight + 5);
    debugln(">> Creating frames for growing bar animation");

    for (int height = startHeight; height <= endHeight; height++) {
        debugln(">> Allocating frame for height " + String(height) + " of " + String(endHeight));
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);
        int halfHeight = height / 2;

        for (int led = 0; led < ledCount; led++) {
            // Determine if this LED should be lit (within the growing bar)
            bool withinBar = (led >= middleLed - halfHeight && led <= middleLed + halfHeight);

            if (withinBar || !abruptFade) {
                // Calculate brightness based on whether LED is in the bar
                uint8_t brightness = withinBar ? maxBrightness : 0;
                
                // For soft fade, calculate a gradient based on distance from the bar edge
                if (!abruptFade && !withinBar) {
                    int distance = min(
                        abs(led - (middleLed - halfHeight)),
                        abs(led - (middleLed + halfHeight))
                    );
                    brightness = max(0, maxBrightness - (distance * 5)); // Fade over ~5 LEDs
                }
                
                std::array<uint8_t, 4> pixel = {
                    static_cast<uint8_t>(led),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness)
                };
                frame.push_back(pixel);
            }
        }
        frames->push_back(frame);
    }

    xSemaphoreGive(animation->LOCK);

    debugln(">> Growing bar animation created - returning pointer");
    return animation;
}


/**
 * @brief Creates a shrinking bar animation
 * @param ledCount Number of LEDs in the strip
 * @param maxBrightness Maximum brightness of the bar
 * @param startHeight Starting height of the bar (0 = auto)
 * @param endHeight Ending height of the bar (0 = auto)
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with shrinking bar effect
 * @details A bright bar shrinks from full height to the center
 */
Animation* createShrinkingBarAnimation(uint8_t ledCount,
                                       uint8_t maxBrightness,
                                       uint8_t startHeight,
                                       uint8_t endHeight,
                                       bool abruptFade) {
    Animation* animation = new Animation("Shrinking Bar");
    
    // Default values if not specified
    if (startHeight == 0) startHeight = ledCount;
    if (endHeight == 0) endHeight = max(1, (int)(ledCount * 0.1));

    int middleLed = ledCount / 2;

    debugln(">> Allocating space for all frames");
    debugln("Reserving memory for frames");
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    size_t we = startHeight - endHeight + 5;
    debugf(">>Attempting to reserve frame vector of %d\n", we);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames->reserve(we);
    debugln(">> Creating frames for shrinking bar animation");

    for (int height = startHeight; height >= endHeight; height--) {
        debugln(">> Allocating frame for height " + String(height) + " of " + String(endHeight));
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);
        int halfHeight = height / 2;

        for (int led = 0; led < ledCount; led++) {
            // Determine if this LED should be lit (within the shrinking bar)
            bool withinBar = (led >= middleLed - halfHeight && led <= middleLed + halfHeight);
            
            if (withinBar || !abruptFade) {
                // Calculate brightness based on whether LED is in the bar
                uint8_t brightness = withinBar ?  maxBrightness : 0;
                
                // For soft fade, calculate a gradient based on distance from the bar edge
                if (!abruptFade && !withinBar) {
                    int distance = min(
                        abs(led - (middleLed - halfHeight)),
                        abs(led - (middleLed + halfHeight))
                    );
                    brightness = max(0,  maxBrightness - (distance * 5)); // Fade over ~5 LEDs
                }
                
                std::array<uint8_t, 4> pixel = {
                    static_cast<uint8_t>(led),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness)
                };
                frame.push_back(pixel);
            }
        }
        frames->push_back(frame);
    }
    xSemaphoreGive(animation->LOCK);

    debugln(">> Shrinking bar animation created - returning pointer");
    return animation;
}


/**
 * @brief Creates an extending bar animation
 * @param ledCount Number of LEDs in the strip
 * @param maxBrightness Maximum brightness of the bar
 * @param endDistance Maximum distance from center (0 = auto)
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with extending bar effect
 * @details A bright line starts at the middle and extends symmetrically
 */
Animation* createExtendingBarAnimation(uint8_t ledCount,
                                       uint8_t maxBrightness,
                                       uint8_t endDistance,
                                       bool abruptFade) {
    
    Animation* animation = new Animation("Extending Bar");
    
    // Default value if not specified
    if (endDistance == 0) endDistance = ledCount / 2;
    int middleLed = ledCount / 2;

    debugln(">> Allocating space for all frames");
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames->reserve(endDistance + 1);
    debugln(">> Creating frames for extending bar animation");

    
    for (int extent = 0; extent <= endDistance; extent++) {
        debugln(">> Allocating frame for extent " + String(extent) + " of " + String(endDistance));
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);
        
        for (int led = 0; led < ledCount; led++) {
            // Determine if this LED should be lit (within the extending bar)
            bool withinBar = (led >= middleLed - extent && led <= middleLed + extent);
            
            if (withinBar || !abruptFade) {
                // Calculate brightness based on whether LED is in the bar
                uint8_t brightness = withinBar ? maxBrightness : 0;
                
                // For soft fade, calculate a gradient based on distance from the bar edge
                if (!abruptFade && !withinBar) {
                    int distance = min(
                        abs(led - (middleLed - extent)),
                        abs(led - (middleLed + extent))
                    );
                    brightness = max(0, maxBrightness - (distance * 25)); // Fade over ~5 LEDs
                }
                
                std::array<uint8_t, 4> pixel = {
                    static_cast<uint8_t>(led),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness)
                };
                frame.push_back(pixel);
            }
        }
        frames->push_back(frame);
    }
    
    xSemaphoreGive(animation->LOCK);

    debugln(">> Extending bar animation created - returning pointer");
    return animation;
}


/**
 * @brief Creates an extinguishing bar animation
 * @param ledCount Number of LEDs in the strip
 * @param maxBrightness Maximum brightness of the bar
 * @param retentionTime Time to keep the bar at full brightness (ms)
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with extinguishing bar effect
 */
Animation* createExtinguishingBarAnimation(uint8_t ledCount,
                                           uint8_t maxBrightness,
                                           uint16_t retentionTime,
                                           bool abruptFade) {

    Animation* animation = new Animation("Extinguishing Bar");

    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;

    int middleLed = ledCount / 2;
    
    debugln(">> Allocating space for all frames");
    frames->reserve(middleLed + 20);
    debugln(">> Creating frames for growing bar animation");

    std::vector<std::array<uint8_t, 4>> allOnFrame;
    allOnFrame.reserve(ledCount);

    for (int led = 0; led < ledCount; led ++) {
        std::array<uint8_t, 4> pixel = {
            (uint8_t)led,
            maxBrightness,
            maxBrightness,
            maxBrightness
        };
        allOnFrame.push_back(pixel);
    };

    frames -> push_back(allOnFrame);

    for (int extent = middleLed; extent >= 0; extent--) {
        debugf(">> Reserving space for frame %d of %d", (middleLed - extent, middleLed));
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);

        for (int led = 0; led < ledCount; led++) {
            // Determine if this LED should be lit (within the contracting bar)
            bool withinBar = (led >= middleLed - extent && led <= middleLed + extent);
            
            if (withinBar || !abruptFade) {
                // Calculate brightness based on whether LED is in the bar
                uint8_t brightness = withinBar ? maxBrightness : 0;
                
                // For soft fade, calculate a gradient based on distance from the bar edge
                if (!abruptFade && !withinBar) {
                    int distance = min(
                        abs(led - (middleLed - extent)),
                        abs(led - (middleLed + extent))
                    );
                    brightness = max(0, maxBrightness - (distance * 25)); // Fade over ~5 LEDs
                }
                
                std::array<uint8_t, 4> pixel = {
                    static_cast<uint8_t>(led),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness),
                    static_cast<uint8_t>(brightness)
                };
                frame.push_back(pixel);
            }
        }
        frames -> push_back(frame);
    }

    // Add retention time by duplicating the last frame
    debugln(">> Defining retention frame");
    if (retentionTime > 0) {
        std::vector<std::array<uint8_t, 4>> retentionFrame;
        std::array<uint8_t, 4> pixel = {
            (uint8_t)middleLed,  // LED index
            maxBrightness,                 // R
            maxBrightness,                 // G
            maxBrightness                  // B
        };

        retentionFrame.push_back(pixel);
        
        // Add retention frame multiple times based on time
        debugln(">> Adding retention full frames");
        uint16_t retentionFrames = static_cast<uint8_t>(retentionTime / 100); // Assuming 100ms per frame
        for (uint16_t i = 0; i < retentionFrames; i++) {
            std::vector<std::array<uint8_t, 4>> retframe(retentionFrame);
            frames -> push_back(retframe);
        }
    }

    // Final frame with all LEDs off
    debugln(">> Defining final frame");
    std::vector<std::array<uint8_t, 4>> allOffFrame;
    for (int led = 0; led < ledCount; led++) {
        std::array<uint8_t, 4> pixel = {
            (uint8_t)led,    // LED index
            0,               // R
            0,               // G
            0                // B
        };
        allOffFrame.push_back(pixel);
    }

    debugln(">> Adding rentention off frames");
    frames -> push_back(allOffFrame);

    xSemaphoreGive(animation->LOCK);
    
    debugln(">> Extinguishing Bar animation created - returning pointer");
    return animation;
}


/**
 * @brief This requires button input so we only render the first frame.
 * @param ledCount Number of LEDs in the strip
 * @param brightness Brightness of the bar
 * @param barSize Size of the bar
 * @return Animation object with moving bar effect
 */
Animation* createMovingBarAnimation(uint8_t ledCount,
                                    uint8_t brightness,
                                    uint8_t barSize) {
    Animation* animation = new Animation("Moving Bar");

    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames -> reserve(1);

    std::vector<std::array<uint8_t, 4>> frame;

    if (barSize == 0) barSize = 1;
    
    int middleLed = ledCount / 2;

    for (int i = 0; i < barSize; i++) {
        int led = middleLed - barSize/2 + i;
        if (led >= 0 && led < ledCount) {
            std::array<uint8_t, 4> pixel = {
                (uint8_t)led,    // LED index
                brightness,             // R
                brightness,             // G
                brightness              // B
            };
            frame.push_back(pixel);
        }
    }
    frames -> push_back(frame);
    xSemaphoreGive(animation->LOCK);

    return animation;
}


/**
 * @brief Creates a new animation that grows from the center
 * @param ledCount Number of LEDs in the strip
 * @param brightness Brightness of the bar
 * @param endDistance Maximum distance from center
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with growing bar effect
 */
Animation* createGrowUpAnimation(uint8_t ledCount,
                                 uint8_t brightness,
                                 uint8_t endDistance,
                                 bool abruptFade) {
    Animation* animation = new Animation("Grow Up");

    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames -> reserve(endDistance + 5);

    // Default value if not specified
    if (endDistance == 0) endDistance = ledCount / 2;
    
    int middleLed = ledCount / 2;

    for (int extent = 0; extent <= endDistance; extent++) {
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);
        uint8_t brightnessmod = brightness;
        for (int led = middleLed; led <= middleLed + extent && led < ledCount; led++) {
            // For soft fade, add a gradient at the top edge
            if (!abruptFade && led == middleLed + extent) uint8_t brightnessmod = brightness / 2; // Half brightness for edge LED

            std::array<uint8_t, 4> pixel = {
                (uint8_t)led,    // LED index
                brightnessmod,             // R
                brightnessmod,             // G
                brightnessmod              // B
            };
            frame.push_back(pixel);
        }

        frames -> push_back(frame);
    }

    xSemaphoreGive(animation->LOCK);
    return animation;
}


/**
 * WARNING: Animation is dynamically allocated and must be freed.
 * @brief Creates a new animation that grows down from the center
 * @param ledCount Number of LEDs in the strip
 * @param brightness Brightness of the bar
 * @param endDistance Maximum distance from center
 * @param abruptFade If true, bar has sharp edges; if false, edges fade
 * @return Animation object with growing bar effect
 */
Animation* createGrowDownAnimation(uint8_t ledCount,
                                  uint8_t brightness,
                                  uint8_t endDistance,
                                  bool abruptFade) {
    Animation* animation = new Animation("Grow Down");

    if (endDistance == 0) endDistance = ledCount / 2;
    int middleLed = ledCount / 2;

    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames -> reserve(endDistance + 5);


    for (int extent = 0; extent <= endDistance; extent++) {
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);
        uint8_t brightnessmod = brightness;

        for (int led = middleLed; led >= middleLed - extent && led >= 0; led--) {
            // For soft fade, add a gradient at the top edge
            if (!abruptFade && led == middleLed + extent) uint8_t brightnessmod = brightness / 2; // Half brightness for edge LED
            std::array<uint8_t, 4> pixel = {
                (uint8_t)led,    // LED index
                brightnessmod,             // R
                brightnessmod,             // G
                brightnessmod              // B
            };
            frame.push_back(pixel);
        }

        frames -> push_back(frame);
    }

    xSemaphoreGive(animation->LOCK);
    return animation;
}

/**
 * @brief Starts with all LEDs on, and top half dims when “up” button pressed and bottom half dims when “down” button pressed
 * @details This animation requires button input for dimming, so we just create one frame
 * @param ledCount Number of LEDs in the strip
 * @param dimLevel Maximum brightness of the half
 * @param gradientFade If true, the fade is gradual; if false, it is abrupt
 * @return Animation object with first frame of half fade effect
 */
Animation* createHalfFadeAnimation(uint8_t ledCount,
                                   float dimLevel,
                                   bool gradientFade) {
    Animation* animation = new Animation("Half Fade");
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames -> reserve(1);

    // Create initial frame with all LEDs on
    std::vector<std::array<uint8_t, 4>> frame;
    for (int led = 0; led < ledCount; led++) {
        std::array<uint8_t, 4> pixel = {
            (uint8_t)led,    // LED index
            255,             // R
            255,             // G
            255              // B
        };
        frame.push_back(pixel);
    }
    frames -> push_back(frame);
    xSemaphoreGive(animation->LOCK);
    return animation;
}


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
                               float minBrightness, 
                               float maxBrightness,
                               float attackProportion,
                               float frequency) {
    debugln(">> Creating pulse animation");
    Animation* animation = new Animation("Pulse");
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    
    // Reduce frame count for clearer pulse effect
    // The wild flashing was likely due to too many frames with small differences
    const int frameCount = max(10, int(60 / frequency));
    
    debugln("Creating pulse animation with " + String(frameCount) + " frames (frequency: " + String(frequency) + ")");
    
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    frames->reserve(frameCount);
    
    // Calculate attack and decay frame counts
    int attackFrames = max(1, int(frameCount * attackProportion));
    int decayFrames = frameCount - attackFrames;
    
    debugln("Attack frames: " + String(attackFrames) + ", Decay frames: " + String(decayFrames));
    
    
    // Create the main pulse frames
    for (int i = 0; i < frameCount; i++) {
        float brightness;
        
        // Fast attack phase
        if (i < attackFrames) {
            // More dramatic exponential rise instead of quadratic
            float progress = float(i) / float(attackFrames);
            // Faster rise with cubic curve
            brightness = minBrightness + (maxBrightness - minBrightness) * 
                         (progress * progress * progress);
        } 
        // Slow decay phase
        else {
            // Exponential decay curve for more natural falloff
            float decayProgress = float(i - attackFrames) / float(decayFrames);
            // Adjust decay rate - slower decay for more noticeable effect
            float decayRate =  2.5 * frequency; // Higher frequency = faster decay
            brightness = maxBrightness - (maxBrightness - minBrightness) * 
                         (1.0 - exp(-decayRate * decayProgress));
        }
        
        // Create a new frame
        std::vector<std::array<uint8_t, 4>> frame;
        frame.reserve(ledCount);
        
        // Convert brightness to pixel value
        uint8_t pixelval = static_cast<uint8_t>(brightness * 255);
        
        // Set all LEDs to the calculated brightness
        for (int led = 0; led < ledCount; led++) {
            std::array<uint8_t, 4> pixel = {
                static_cast<uint8_t>(led),
                pixelval,
                pixelval,
                pixelval
            };
            frame.push_back(pixel);
        }
        
        // Add the frame to the animation
        frames->push_back(frame);
    }
    
    xSemaphoreGive(animation->LOCK);
    debugln("Pulse animation created with " + String(frameCount + 2) + " frames");
    return animation;
}


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
                                            uint8_t brightness ) {
    Animation* animation = new Animation("Circling Bright Dot");
    
    int frameCount = ledCount;
    
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames->reserve(frameCount);
    
    // For each frame in the animation
    for (int frame = 0; frame < frameCount; frame++) {
        std::vector<std::array<uint8_t, 4>> frameData;
        frameData.reserve(abruptFade ? 1 : trailLength); // Optionally reserve space for the dot and its trail
        
        // Calculate the current position (floating point for sub-LED positioning)
        float position = ((float)frame / 1.0f);
        
        // Reverse direction if counter-clockwise
        if (!clockwise) {
            position = ledCount - position;
        }
        
        // Calculate the main dot position
        float mainPosition = fmod(position, ledCount);
        int mainLed = (int)mainPosition;
        
        // Calculate brightness for fractional position (interpolation between LEDs)
        float fractional = mainPosition - mainLed;
        uint8_t currentBrightness = brightness;
        
        // Add the main bright dot
        std::array<uint8_t, 4> mainPixel = {
            static_cast<uint8_t>(mainLed % ledCount),
            currentBrightness,
            currentBrightness,
            currentBrightness
        };
        frameData.push_back(mainPixel);
        
        // Add trail if not using abrupt fade
        if (!abruptFade) {
            for (int i = 1; i < trailLength; i++) {
                // Calculate the trail LED position
                int trailLed;
                if (clockwise) {
                    trailLed = (mainLed - i + ledCount) % ledCount;
                } else {
                    trailLed = (mainLed + i) % ledCount;
                }
                
                // Calculate trail brightness - exponential decay
                uint8_t trailBrightness = currentBrightness * pow(0.20f, i);
                
                // Skip if trail is too dim
                if (trailBrightness < 5) continue;
                
                std::array<uint8_t, 4> trailPixel = {
                    static_cast<uint8_t>(trailLed),
                    trailBrightness,
                    trailBrightness,
                    trailBrightness
                };
                frameData.push_back(trailPixel);
            }
        }
        
        frames->push_back(frameData);
    }
    
    xSemaphoreGive(animation->LOCK);
    debugln("Circling Bright Dot animation created with " + String(frameCount) + " frames");
    return animation;
}


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
                                          uint8_t backgroundBrightness) {
    Animation* animation = new Animation("Circling Dark Spot");
    
    // Use the same frame count as the bright dot animation for consistency
    int frameCount = ledCount;
    
    xSemaphoreTake(animation->LOCK, portMAX_DELAY);
    std::vector<std::vector<std::array<uint8_t, 4>>>* frames = animation->FRAMES;
    frames->reserve(frameCount);
    
    // For each frame in the animation
    for (int frame = 0; frame < frameCount ; frame++) {
        std::vector<std::array<uint8_t, 4>> frameData;
        frameData.reserve(ledCount); // Reserve space for all LEDs
        
        // Calculate the current position (same method as bright dot)
        float position = ((float)frame / 1.0f);
        
        // Reverse direction if counter-clockwise
        if (!clockwise) {
            position = ledCount - position;
        }
        
        // Calculate the main dark spot position
        float mainPosition = fmod(position, ledCount);
        int mainLed = (int)mainPosition;
        
        // For each LED in the strip
        for (int led = 0; led < ledCount; led++) {
            uint8_t ledBrightness = backgroundBrightness;
            
            // Calculate how dark this LED should be
            if (abruptTransition) {
                // Sharp transition - just make the exact spot dark
                if (led == mainLed) {
                    ledBrightness = 0; // Completely dark
                }
            } else {
                // Calculate distance to dark spot center, accounting for strip wrapping
                int distance;
                
                // Using the same wrapping approach as in bright dot
                if (clockwise) {
                    distance = (led - mainLed + ledCount) % ledCount;
                    if (distance > ledCount/2) distance = ledCount - distance;
                } else {
                    distance = (mainLed - led + ledCount) % ledCount;
                    if (distance > ledCount/2) distance = ledCount - distance;
                }
                
                // If this LED is within the spot width
                if (distance < spotWidth) {
                    // Use the same exponential approach but inverted for darkness
                    // The closer to center, the darker it gets
                    float darknessFactor = (float)distance / spotWidth;
                    
                    // Exponential falloff - similar to trailBrightness calculation in bright dot
                    ledBrightness = backgroundBrightness * min(1.0, pow(darknessFactor, 2));
                }
            }
            
            // Create pixel data
            std::array<uint8_t, 4> pixel = {
                static_cast<uint8_t>(led),
                ledBrightness,
                ledBrightness,
                ledBrightness
            };
            frameData.push_back(pixel);
        }
        
        frames->push_back(frameData);
    }

    xSemaphoreGive(animation->LOCK);
    debugln("Circling Dark Spot animation created with " + String(frameCount) + " frames");
    return animation;
}









