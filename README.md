[![Compile](https://github.com/sudoDeVinci/ESP32Animator/actions/workflows/compile.yml/badge.svg?branch=main)](https://github.com/sudoDeVinci/ESP32Animator/actions/workflows/compile.yml)

# ESP32 LED Animation Controller
LED animation controller for ESP32.

## Overview
Leverages the esp32's dual cores for LED animations. This allows menu usage and configuration while animations play uninterrupted by user-input.

## Dual-Core Utilization
The ESP32's dual-core capability is leveraged for optimal performance:
 The render task takes care of
- Animation rendering
- LED strip updates
- Timing-related calculations

Leaving other tasks on either core free for sensor data and communication