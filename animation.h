#pragma once
#ifndef IO_H
#define IO_H
#define DEBUG 1

#include <Arduino.h>
#include <vector>
#include <mutex>

#if DEBUG == 1
/**
 * @brief Print debug information when DEBUG is enabled
 * @param ... Arguments to pass to Serial.print
 */
#define debug(...) Serial.print(__VA_ARGS__)

/**
 * @brief Print debug information with newline when DEBUG is enabled
 * @param ... Arguments to pass to Serial.println
 */
#define debugln(...) Serial.println(__VA_ARGS__)

/**
 * @brief Print formatted debug information when DEBUG is enabled
 * @param ... Arguments to pass to Serial.printf
 */
#define debugf(...) Serial.printf(__VA_ARGS__)
#else
#define debug(...)
#define debugln(...)
#define debugf(...)
#endif

struct Animation {
    std::vector<std::vector<std::array<uint8_t, 4>>>* FRAMES; // Animation frames
    String NAME; // Animation name (dynamically allocated)
    SemaphoreHandle_t LOCK;      // Mutex for thread-safe access

    /**
     * @brief Default constructor
     * @details Initializes name to nullptr
     */
    Animation() : NAME("NONE") {
        LOCK = xSemaphoreCreateMutex();
        FRAMES = new std::vector<std::vector<std::array<uint8_t, 4>>>;
    }

    /**
     * @brief Constructor with name
     * @param name The name of the animation
     * @details Initializes the name field with the provided value
     */
    Animation(String namestr) : NAME(namestr) {
        LOCK = xSemaphoreCreateMutex();
        FRAMES = new std::vector<std::vector<std::array<uint8_t, 4>>>;
    }
  
    /**
     * @brief Copy constructor
     * @param other The animation to copy
     * @details Copies the name and frames from the other animation
     */
    Animation(const Animation &other) {
        NAME = other.NAME;
        FRAMES = other.FRAMES;
        LOCK = xSemaphoreCreateMutex();
    }

    /**
     * @brief Move constructor
     * @param other The animation to move
     * @details Moves the name and frames from the other animation
     */
    Animation(Animation &&other) {
        NAME = std::move(other.NAME);
        FRAMES = std::move(other.FRAMES);
        LOCK = other.LOCK;
        other.LOCK = nullptr;
    }

    /**
     * @brief Copy assignment operator
     * @param other The animation to copy
     * @return A reference to the copied animation
     * @details Copies the name and frames from the other animation
     */
    Animation &operator=(const Animation &other) {
        NAME = other.NAME;
        FRAMES = other.FRAMES;
        return *this;
    }

    /**
     * @brief Destructor
     * @details Deletes the name and lock, and clears the frames
     * @note This is not thread-safe, and should only be called from the main thread or where the lock is held
     */
    ~Animation() {
        if (LOCK != nullptr) {
            vSemaphoreDelete(LOCK);
            LOCK = nullptr;
        }
        
        if (FRAMES != nullptr) {
            FRAMES->clear();
            delete FRAMES;
            FRAMES = nullptr;
        }
        
        // Don't delete NAME - it's not a pointer
        // Remove: if (NAME != nullptr) { delete &NAME; }
    }

    String getName() {
        xSemaphoreTake(LOCK, portMAX_DELAY);
        String name = NAME;
        xSemaphoreGive(LOCK);
        return name;
    }
};

#endif // IO_H