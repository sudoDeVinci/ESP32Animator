#pragma once
#ifndef CLI_H
#define CLI_H

#include "render.h"


/**
 * @brief Enumeration of all available menu states
 * @details Controls the context of the current user interface and
 * determines which menu is displayed and which input handlers are active
 */
enum class MenuState {
    MAIN,               // Main menu selection screen
    ANIMATION_SELECT,   // Animation selection menu
    BRIGHTNESS_SETTINGS,// Brightness adjustment menu
    SPEED_SETTINGS,     // Animation speed settings
    LED_COUNT_SETTINGS, // LED count configuration
    REPEAT_SETTINGS,    // Animation repeat toggle
    MODE_SETTINGS,      // Interactive mode settings
    SYSTEM_INFO         // Display current system settings
};


/**
 * @brief Menu system class - manages menu state and interactions
 * @details Implements the Model-View-Controller pattern for the CLI,
 * handling user input, state transitions, and display updates
 */
class MenuSystem {
    private:
        Renderer* renderer;          // Reference to the LED renderer
        MenuState currentState;      // Current menu state
        bool needsRedraw;            // Flag to indicate menu needs redrawing
        unsigned long lastInputTime; // Timestamp of last user input

        // Menu display methods
        /**
         * @brief Displays the main menu options
         */
        void displayMainMenu();
        
        /**
         * @brief Displays the animation selection menu
         */
        void displayAnimationMenu();
        
        /**
         * @brief Displays the brightness adjustment menu
         */
        void displayBrightnessMenu();
        
        /**
         * @brief Displays the animation speed settings menu
         */
        void displaySpeedMenu();
        
        /**
         * @brief Displays the LED count configuration menu
         */
        void displayLEDCountMenu();
        
        /**
         * @brief Displays the animation repeat toggle menu
         */
        void displayRepeatMenu();
        
        /**
         * @brief Displays the interactive mode settings menu
         */
        void displayModeMenu();
        
        /**
         * @brief Displays the system information screen
         */
        void displaySystemInfo();

        void displayCurrentMenu();

        String getSerialInput();

        /**
         * @brief Processes input for the main menu
         * @param input The character input from serial
         */
        void processMainMenuInput(String input);

        void processInput(String input);

        void processAnimationMenuInput(String input);

        void processBrightnessMenuInput(String input);

    public:
        /**
         * @brief Set the current menu state to MAIN and trigger a redraw.
         */
        MenuSystem(Renderer* rend) {
            renderer = rend;
            currentState = MenuState::MAIN;
            needsRedraw = true;
            lastInputTime = millis();
        }

        /**
         * @brief Updates the menu system
         * @details Checks for user input and updates display if needed
         * @note Should be called repeatedly in a loop
         */
        void update();

        /**
         * @brief Forces redraw of the current menu
         */
        void forceRedraw() { needsRedraw = true; }
};

#endif // CLI_H
