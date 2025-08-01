#include "animation.h"

/**
 * @brief Load an animation from a file in the specified file system.
 * @param fs The file system to read from.
 * @param path The path to the animation file.
 * @return An Animation object loaded from the file, or an empty Animation if loading failed.
 */
Animation loadAnimation(fs::FS& fs, const std::string& path) {
    std::string content = readFile(fs, path);
    if (content.empty()) {
        debugf("Failed to read animation file: %s\n", path.c_str());
        return Animation();
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, content);
    if (error) {
        debugf("Failed to parse animation JSON: %s\n", error.c_str());
        return Animation();
    }

    if (!doc["metadata"]["name"].is<std::string>() ||
    !doc["metadata"]["total_pixels"].is<uint16_t>() ||
    !doc["metadata"]["frame_count"].is<uint16_t>()) {
        debugf("Invalid or missing metadata fields in animation JSON.\n");
        return Animation();
    }

    std::string name = doc["metadata"]["name"].as<std::string>();
    uint16_t pixelCount = doc["metadata"]["total_pixels"].as<uint16_t>();
    uint16_t frameCount = doc["metadata"]["frame_count"].as<uint16_t>();

    FrameBuffer frames;
    frames.reserve(frameCount);
    for (JsonArray framejson : doc["frames"].as<JsonArray>()) {
        Frame frame;
        frame.reserve(framejson.size());
        for (JsonArray pixelarray : framejson) {
            if (pixelarray.size() != 4) {
                debugf("Invalid pixel data format.\n");
                return Animation();
            }
            Pixel pixel(
                pixelarray[0].as<uint16_t>(),
                pixelarray[1].as<uint8_t>(),
                pixelarray[2].as<uint8_t>(),
                pixelarray[3].as<uint8_t>()
            );
            frame.push_back(pixel);
        }
        frames.push_back(frame);
    }

    Animation animation(name, frames);
    debugf("Loaded animation '%s' with %zu frames and a total of %d pixels.\n", name.c_str(), frameCount, pixelCount);
    return animation;
}