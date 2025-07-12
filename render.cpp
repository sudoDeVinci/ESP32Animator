#include "render.h"

void render(Renderer& rend) {

    if (!rend.isRunning()) {
        debugln(">> No animation to render or simply not running");
        return;
    }

    // Check if the current animation is empty
    if (rend.isAnimationEmpty()) {
        debugln(">> Current animation is empty, stopping render");
        return;
    }

    // Create local copies of all settings
    RenderState state = rend.outputState();
    String currentName = state.currentAnimationName;
    String previousName = currentName;

    // Get a reference to the frames in the current animation
    const FrameBuffer& frames = rend.getCurrentAnimationFrames();
    size_t frameCount = frames.size();
    if (frameCount == 0) {
        debugln(">> No frames in the animation, stopping render");
        return;
    }

    size_t frameSize = frames[0].size();
    debugln(">> Starting render loop");

    for (size_t frameindex = 0; frameindex < frameCount && state.isRunning; frameindex++) {

        if (state.currentAnimationName != previousName) {
            debugln(">> Animation changed, stopping render");
            break;
        }

        if (!state.isRunning) {
            debugln(">> Animation stopped, stopping render");
            break;
        }

        const Frame& frame = frames[frameindex];
        frameSize = frame.size();

        rend.writeFrameToScreen(frame);

        if (rend.interruptableDelay((unsigned long)(state.frameDelayMs / state.speedCoefficient))) {
            debugln(">> Render interrupted, stopping");
            rend.setEarlyExit(false);
            break;
        }

        if (!state.repeat) {
            rend.setRunning(false);
            debugln(">> Animation finished, stopping render");
            break;
        }

        previousName = state.currentAnimationName;
        state = rend.outputState();
    }

}


/**
 * Fill a vector with pixel values for a range of indices
 * @param vec The vector to fill
 * @param start The starting index
 * @param end The ending index
 * @param value The value to fill in
 */
void fillVectorPixels(std::vector<Pixel>& vec,
                       uint8_t start,
                       uint8_t end,
                       uint8_t value) {
    for (uint8_t i = start; i < end; i++) {
        Pixel pixel = {
            static_cast<uint8_t>(i),
            value,
            value,
            value
        };
        vec.push_back(pixel);
    }
}


/**
 * Swap two uint8_t values
 * @param a The first value
 * @param b The second value
 */
void swap(uint8_t& a, uint8_t& b) {
    const uint8_t temp = a;
    a = b;
    b = temp;
}