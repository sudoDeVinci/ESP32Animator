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

        /**
         * @brief Displays the appropriate menu based on current state
         * @details Routes to the correct display method based on currentState
         */
        void displayCurrentMenu();
        
        /**
         * @brief Processes user input based on current menu state
         * @param input The character input from serial
         * @details Routes to the correct input processor based on currentState
         */
        void processInput(char input);

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

        // Input processing methods
        /**
         * @brief Processes input for the main menu
         * @param input The character input from serial
         */
        void processMainMenuInput(char input);
        
        /**
         * @brief Processes input for the animation selection menu
         * @param input The character input from serial
         */
        void processAnimationMenuInput(char input);
        
        /**
         * @brief Processes input for the brightness settings menu
         * @param input The character input from serial
         */
        void processBrightnessMenuInput(char input);
        
        /**
         * @brief Processes input for the speed settings menu
         * @param input The character input from serial
         */
        void processSpeedMenuInput(char input);
        
        /**
         * @brief Processes input for the LED count settings menu
         * @param input The character input from serial
         */
        void processLEDCountMenuInput(char input);
        
        /**
         * @brief Processes input for the repeat settings menu
         * @param input The character input from serial
         */
        void processRepeatMenuInput(char input);
        
        /**
         * @brief Processes input for the mode settings menu
         * @param input The character input from serial
         */
        void processModeMenuInput(char input);
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

// Button handling functions
/**
 * @brief Handles button press events based on interactive mode
 * @param upButtonPressed Reference to up button pressed state
 * @param downButtonPressed Reference to down button pressed state
 * @param renderer Pointer to the LED renderer
 * @details Routes to the appropriate handler based on the current MODE setting
 */
void handleButtonPress(volatile bool &upButtonPressed, volatile bool &downButtonPressed, Renderer* renderer);

/**
 * @brief Applies the moving bar animation based on button presses
 * @param renderer Pointer to the LED renderer
 * @param upPressed True if up button was pressed
 * @param downPressed True if down button was pressed
 * @details Moves a light bar up or down based on button input
 */
void applyMovingBarAnimation(Renderer* renderer, bool upPressed, bool downPressed);

/**
 * @brief Applies the growing bar animation based on button presses
 * @param renderer Pointer to the LED renderer
 * @param upPressed True if up button was pressed
 * @param downPressed True if down button was pressed
 * @details Grows or shrinks a centered light bar based on button input
 */
void applyGrowingBarAnimation(Renderer* renderer, bool upPressed, bool downPressed);

/**
 * @brief Applies the extending bar animation based on button presses
 * @param renderer Pointer to the LED renderer
 * @param upPressed True if up button was pressed
 * @param downPressed True if down button was pressed
 * @details Extends or contracts a centered light bar based on button input
 */
void applyExtendingBarAnimation(Renderer* renderer, bool upPressed, bool downPressed);

/**
 * @brief Applies the half fade animation based on button presses
 * @param renderer Pointer to the LED renderer
 * @param upPressed True if up button was pressed
 * @param downPressed True if down button was pressed
 * @details Fades top or bottom half of LEDs based on button input
 */
void applyHalfFadeAnimation(Renderer* renderer, bool upPressed, bool downPressed);

/**
 * @brief Applies the grow up animation based on button press
 * @param renderer Pointer to the LED renderer
 * @param upPressed True if up button was pressed
 * @details Grows light upward from middle when up button is pressed
 */
void applyGrowUpAnimation(Renderer* renderer, bool upPressed);

/**
 * @brief Applies the grow down animation based on button press
 * @param renderer Pointer to the LED renderer
 * @param downPressed True if down button was pressed
 * @details Grows light downward from middle when down button is pressed
 */
void applyGrowDownAnimation(Renderer* renderer, bool downPressed);

#endif // CLI_H
