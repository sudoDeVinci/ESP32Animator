#pragma once
#ifndef IO_H
#define IO_H
#define DEBUG 1

#include <Arduino.h>
#include <vector>
#include <mutex>
#include <memory>

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

// Type aliases for code clarity and maintainability
using Pixel = std::array<uint8_t, 4>;
using Frame = std::vector<Pixel>;
using FrameBuffer = std::vector<Frame>;

struct Animation {
private:
    String name_;
    std::unique_ptr<FrameBuffer> frames_;
    mutable std::mutex mutex_;

public:
    Animation() : name("NONE") {}
  
    explicit Animation(const String& namestr) 
        : name_(namestr), frames_(std::make_unique<FrameBuffer>()) {
        debugf("Animation '%s' created\n", namestr.c_str());
    }

    /**
     * @brief Move constructor
     * @param other The animation to move
     * @details Moves the name and frames from the other animation
     */
    Animation(Animation &&other) {
        std::lock_guard<std::mutex> lck(other.LOCK);
        this->name = std::move(other.name);
        *(this->frame_ptr) = *(other.frame_ptr);
        other.frame_ptr->clear();
        delete other.frame_ptr;
    }

    /**
     * @brief Copy assignment operator
     * @param other The animation to copy
     * @return A reference to the copied animation
     * @details Copies the name and frames from the other animation
     */
    Animation &operator=(const Animation &other) {
        std::lock_guard<std::mutex> lck(other.LOCK);
        this->name = other.name;
        *(this->frame_ptr) = *(other.frame_ptr);
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

        if (frame_ptr != nullptr) {
            frame_ptr->clear();
            delete frame_ptr;
            frame_ptr = nullptr;
        }

        if (name!= nullptr) {
            name.clear();
            delete &name;
        }
    }

    String getName() {
        std::lock_guard<std::mutex> lck(this->LOCK);
        String name = NAME;
        return name;
    }

    void setName(const String& namestr) {
        std::lock_guard<std::mutex> lck(this->LOCK);
        this->name = namestr;
    }

    int frameCount() {
        std::lock_guard<std::mutex> lck(this->LOCK);
        int count = frame_ptr->size();
        return count;
    }

    std::vector<std::vector<std::array<uint8_t, 4>>> getFramesDeepCopy() {
        std::lock_guard<std::mutex> lck(this->LOCK);
        std::vector<std::vector<std::array<uint8_t, 4>>> framesCopy = *frame_ptr;
        return framesCopy;
    }
};

#endif // IO_H