#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H
#define DEBUG 1

#include "io.h"
#include <Arduino.h>
#include <vector>
#include <mutex>
#include <memory>
#include <array>
#include <string>
#include <cstdint>
#include <algorithm>


/**
 * @brief A single pixel in the animation with an index and RGB color values
 * @details Contains an index and RGB color values
 */
struct Pixel {
    uint16_t index;
    uint8_t r, g, b;

    Pixel(
        uint16_t idx,
        uint8_t red = 0,
        uint8_t green = 0,
        uint8_t blue = 0
    ) : index(idx), r(red), g(green), b(blue) {}

    /**
     * @brief Copy Assignment constructor
     * We dont wanna copy the index, just the color
     * @param other The pixel to copy
     */
    Pixel& operator=(const Pixel& other) {
        if (this != &other) {
            r = other.r;
            g = other.g;
            b = other.b;
        }
        return *this;
    }
};

using Frame = std::vector<Pixel>;
using FrameBuffer = std::vector<Frame>;

struct Animation {
private:
    std::string name_;
    uint32_t nameHash_;
    FrameBuffer frames_;
    mutable std::mutex mutex_;

public:
    Animation() : name_("NONE"), nameHash_(hash_string_runtime("NONE")) {}

    Animation(
        const std::string& namestr,
        const FrameBuffer& frames = FrameBuffer()
    ) : name_(namestr), nameHash_(hash_string_runtime(namestr)), frames_(frames) {}

    /**
     * @brief Fast runtime string hashing for animation name comparisons
     * @param str The string to hash
     * @return The hash of the string
     */
    inline uint32_t hash_string_runtime(const std::string& str) {
        uint32_t hash = 5381;
        for (size_t i = 0; i < str.length(); i++) {
            hash = ((hash << 5) + hash) + str[i];
        }
        return hash;
    }

    /**
     * @brief Constructor with name
     * @param namestr The name of the animation
     * @details Initializes the animation with a name and an empty frame buffer
     */
    Animation(const std::string& namestr) : name_(namestr), nameHash_(hash_string_runtime(namestr)) {
        debugf("Animation '%s' created with hash %zu\n", namestr.c_str(), nameHash_);
    }


    /**
     * @brief Destructor
     */
    ~Animation() = default;


    /**
     * @brief Copy constructor
     * @param other The animation to copy
     * @details Copies the name and frames from the other animation
     */
    Animation(const Animation& other) {
        std::lock_guard<std::mutex> lock(other.mutex_);
        name_ = other.name_;
        nameHash_ = other.nameHash_;
        frames_ = other.frames_;
        debugf("Animation '%s' copied\n", name_.c_str());
    }


    /**
     * @brief Copy assignment operator
     * @param other The animation to copy
     * @return A reference to the copied animation
     * @details Copies the name and frames from the other animation
     */
    Animation &operator=(const Animation &other) {
        if (this == &other) return *this;
        
        // Lock both objects in consistent order to prevent deadlock
        std::lock(mutex_, other.mutex_);
        std::lock_guard<std::mutex> lockthis(mutex_, std::adopt_lock);
        std::lock_guard<std::mutex> lockother(other.mutex_, std::adopt_lock);

        name_ = other.name_;
        nameHash_ = other.nameHash_;
        frames_ = other.frames_;
        return *this;
    }


    /**
     * @brief Move constructor
     * @param other The animation to move
     * @details Moves the name and frames from the other animation
     */
    Animation(Animation&& other) {
        std::lock_guard<std::mutex> lock(other.mutex_);
        name_ = std::move(other.name_);
        nameHash_ = other.nameHash_;
        frames_ = std::move(other.frames_);
    }


    /**
     * @brief Move assignment operator
     * @param other The animation to move
     * @return A reference to the moved animation
     */
    Animation& operator=(Animation&& other) {
        if (this == &other) return *this;
        
        std::lock(mutex_, other.mutex_);
        std::lock_guard<std::mutex> lockthis(mutex_, std::adopt_lock);
        std::lock_guard<std::mutex> lockother(other.mutex_, std::adopt_lock);
        
        name_ = std::move(other.name_);
        nameHash_ = other.nameHash_;
        frames_ = std::move(other.frames_);
        return *this;
    }


    /**
     * @brief Get the name of the animation
     * @return The name of the animation
     */
    const std::string& getName() const{
        std::lock_guard<std::mutex> lock(mutex_);
        return name_;
    }


    /**
     * @brief Set the name of the animation
     * @param namestr The new name for the animation
     */
    void setName(const std::string& namestr) {
        std::lock_guard<std::mutex> lock(this->mutex_);
        name_ = namestr;
        nameHash_ = hash_string_runtime(namestr);
    }


    /**
     * @brief Get the hash of the animation name for fast comparisons
     * @return The hash of the animation name
     */
    uint32_t getNameHash() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return nameHash_;
    }


    /**
     * @brief Get the count of frames in the animation
     * @return The number of frames in the animation
     */
    size_t frameCount() const {
        std::lock_guard<std::mutex> lock(this->mutex_);
        return frames_.size();
    }


    void setFrames(const FrameBuffer& frames) {
        std::lock_guard<std::mutex> lock(mutex_);
        debugf("Setting %zu frames for animation '%s'\n", frames.size(), name_.c_str());
        frames_ = frames;
    }


    /**
     * @brief Get a deep copy of the frames in the animation
     * @warning Creating a full new FrameBuffer - lots of memory allocation here.
     * @return A deep copy of the frame buffer
     */
    FrameBuffer getFramesDeepCopy() const {
        std::lock_guard<std::mutex> lock(mutex_);
        debugf("Deep copy requested for %zu frames\n", frames_.size());
        FrameBuffer copy = frames_;
        return copy;
    }

    /**
     * @brief Get a reference to the frames in the animation
     * @return A reference to the frame buffer
     * @details This method is thread-safe and locks the mutex while accessing frames_
     */
    const FrameBuffer& getFrames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return frames_;
    }


    /**
     * @brief Clear the frames in the animation
     * @details Clears the frame buffer and resets the animation name to "NONE"
     */
    void clearFrames() {
        std::lock_guard<std::mutex> lock(mutex_);
        frames_.clear();
        name_ = "NONE";
        nameHash_ = hash_string_runtime("NONE");
        debugln("Animation frames cleared");
    }
};


/**
 * @brief Load an animation from a file in the specified file system.
 * @param fs The file system to read from.
 * @param path The path to the animation file.
 * @return An Animation object loaded from the file, or an empty Animation if loading failed.
 */
Animation loadAnimation(fs::FS& fs, const std::string& path);

#endif