#include "render.h"

RenderState render(Renderer& rend) {

    if (!rend.isRunning()) {
        debugln(">> Animation simply not running");
        return rend.outputState();
    }

    debugln(">> Animation is still running");

    // Check if the current animation is empty
    if (rend.isAnimationEmpty()) {
        debugln(">> Current animation is empty, stopping render");
        return rend.outputState();
    }

    debugln(">> Animation isn't empty");

    // Create local copies of all settings
    RenderState state = rend.outputState();
    uint32_t currentNameHash = state.currentAnimationHash;
    uint32_t previousNameHash = currentNameHash;

    debugln(">>Got the current render state");

    // Get a reference to the frames in the current animation
    const FrameBuffer& frames = rend.getCurrentAnimationFrames();
    size_t frameCount = frames.size();
    if (frameCount == 0) {
        debugln(">> No frames in the animation, stopping render");
        return rend.outputState();
    }

    debugln(">> Retrieved frame buffer");

    size_t frameSize = frames[0].size();
    debugln(">> Starting render loop");

    for (size_t frameindex = 0; frameindex < frameCount && state.isRunning; frameindex++) {

        if (state.currentAnimationHash != previousNameHash) {
            debugln(">> Animation changed, stopping render");
            return rend.outputState();
        }

        if (!state.isRunning) {
            debugln(">> Animation stopped, stopping render");
            return rend.outputState();
        }

        const Frame& frame = frames[frameindex];
        frameSize = frame.size();

        rend.writeFrameToScreen(frame);

        if (rend.interruptableDelay((unsigned long)(state.frameDelayMs / state.speedCoefficient))) {
            debugln(">> Render interrupted, stopping");
            rend.setEarlyExit(false);
            return rend.outputState();
        }

        if (!state.repeat) {
            rend.setRunning(false);
            debugln(">> Animation finished, stopping render");
            return rend.outputState();
        }

        previousNameHash = state.currentAnimationHash;
        state = rend.outputState();
    }

    // If we reach here, the animation has finished or was interrupted
    return state;
}