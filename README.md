# ESP32 LED Animation Controller
LED animation controller for ESP32. MVC architecture, dual-core task distribution, and an interactive CLI.

## Overview
Leverages the esp32's dual cores for LED animations controlled via a CLI over serial. This allows menu usage and configuration while animations play uninterrupted by user-input.

## Dual-Core Utilization
The ESP32's dual-core capability is leveraged for optimal performance:

Core 0 Handles user interaction tasks
- Menu interface (Serial comms)
- Button processing

Core 1: Dedicated to time-sensitive operations
- Animation rendering
- LED strip updates
- Timing-related calculations

