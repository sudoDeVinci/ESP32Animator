[![Compile](https://github.com/sudoDeVinci/ESP32Animator/actions/workflows/compile.yml/badge.svg?branch=main)](https://github.com/sudoDeVinci/ESP32Animator/actions/workflows/compile.yml)

# ESP32Animator

> **Dual-core LED animation engine for ESP32 microcontrollers**

A thread-safe animation component designed to use NeoPixel LED strips. Built to leverage the ESP32's dual-core architecture for smooth, uninterrupted animations while keeping your main application responsive.

## ✨ Features

### 🚀 **Dual-Core Architecture**
- **Dedicated render core** for pixel-perfect timing
- **Thread-safe Non-blocking animations** that won't freeze your UI
- **Interruptible delays** for responsive control

### 🎭 **Advanced Animation System**
- **Dynamic speed control** with real-time adjustment
- **Brightness control** with peak brightness limiting
- **Repeat modes** for looping animations

### 🔧 **Developer-Friendly**
- **RAII compliant** with modern STL containers
- **Memory efficient** with smart copying and moving
- **Extensible design** for custom animation types

## 🏗️ Architecture

```
┌─────────────────┐    ┌─────────────────┐
│   Core 0        │    │   Core 1        │
│                 │    │                 │
│ • Main App      │    │ • Render Task   │
│ • User Input    │    │ • LED Updates   │
│ • Sensors       │    │ • Timing        │
│ • Communication │    │ • Frame Buffer  │
└─────────────────┘    └─────────────────┘
```

The rendering engine runs on a dedicated core, ensuring your animations stay smooth even when your main application is busy processing sensors, handling user input, or managing network communications.

## 🚀 Quick Start

### Basic Setup
```cpp
#include "render.h"

// Create renderer with 10 LEDs on pin 42
Renderer renderer(10, 42);

void setup() {
    Serial.begin(115200);
    
    // Initialize the LED strip
    renderer.initializeScreen();
    
    // Create dedicated render task on core 1
    xTaskCreatePinnedToCore(
        renderTask,
        "RenderTask", 
        2097152,      // 2MB stack
        NULL, 
        2,            // High priority
        &renderTaskHandle,
        1             // Core 1
    );
}

void renderTask(void* parameters) {
    while (true) {
        if (renderer.isRunning()) {
            render(renderer);
        }
        vTaskDelay(1);
    }
}
```

### Creating Animations
```cpp
// Create an animation
Animation myAnimation("rainbow_wave");

// Build frame data
FrameBuffer frames;
for (int frame = 0; frame < 30; frame++) {
    Frame currentFrame;
    for (int led = 0; led < 10; led++) {
        // Calculate rainbow colors based on frame and LED position
        uint8_t hue = (frame * 8 + led * 25) % 255;
        currentFrame.push_back(Pixel(led, r, g, b));
    }
    frames.push_back(currentFrame);
}

myAnimation.setFrames(frames);
renderer.setAnimation(myAnimation);
```

## 🎛️ API Reference

### Renderer Control
```cpp
// Speed control (0.1x to 10x)
renderer.setSpeed(2.0f);              // 2x speed

// Brightness control (0.0 to 1.0)
renderer.setPeakBrightness(0.8f);     // 80% brightness

// Animation control
renderer.setRepeat(true);             // Loop animation
renderer.setRunning(false);           // Pause animation
```

### Animation Management
```cpp
Animation anim("my_animation");

// Frame manipulation
anim.setFrames(frameBuffer);
size_t count = anim.frameCount();
FrameBuffer frames = anim.getFramesDeepCopy();

// Thread-safe operations
std::string name = anim.getName();
uint32_t hash = anim.getNameHash();   // Fast comparison
```

### Pixel Control
```cpp
// Individual pixel control
Pixel redPixel(0, 255, 0, 0);         // LED 0: Red
renderer.setPixelColor(redPixel);

// Frame-based updates
Frame frame = {
    Pixel(0, 255, 0, 0),              // Red
    Pixel(1, 0, 255, 0),              // Green  
    Pixel(2, 0, 0, 255)               // Blue
};
renderer.writeFrameToScreen(frame);
```

## 🔧 Configuration

### Hardware Setup
- **ESP32** development board
- **NeoPixel LED strip** (WS2812B/SK6812)
- **Power supply** appropriate for your LED count
- **Data pin** connection (default: pin 42 for ESP32S3)

### Dependencies
- **Adafruit NeoPixel Library**
- **ESP32 Arduino Core**
- **FreeRTOS** (included with ESP32 core)