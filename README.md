# ESP32 LED Animation Controller
LED animation controller for ESP32.

## Overview
Leverages the esp32's dual cores for LED animations. This allows menu usage and configuration while animations play uninterrupted by user-input.

## Dual-Core Utilization
The ESP32's dual-core capability is leveraged for optimal performance:

Core 0 Handles user interaction tasks
- Menus
- Button processing
- Sensors and antennaes

Core 1: Dedicated to time-sensitive operations
- Animation rendering
- LED strip updates
- Timing-related calculations

