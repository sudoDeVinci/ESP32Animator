#include "cli.h"
#include <Arduino.h>

/**
 * Blocking function to get a full line of input from the serial monitor
 * @return String The full line of input from the serial monitor
 */
String MenuSystem::getSerialInput()
{
    String outstr = "";
    bool stringComplete = false;
    int inputint = -1;
    char input;

    while (Serial.available() > 0 || !stringComplete)
    {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        inputint = Serial.read();

        // Consume any extra newlines or carriage returns
        while (Serial.available() && (Serial.peek() == '\n' || Serial.peek() == '\r'))
        {
            Serial.read();
        }

        if (inputint == -1)
            continue;

        input = (char)inputint;
        outstr += input;
        if (outstr.length())
            stringComplete = true;

        Serial.println(">> Input: " + outstr);
    }

    return outstr;
}

/**
 * Main update loop for menu system
 */
void MenuSystem::update()
{
    String outstr = "";
    if (Serial.available() > 0)
        outstr += getSerialInput();
    if (outstr.length() > 0)
    {
        Serial.println(">> Processing input: " + outstr);
        Serial.println(">> Length: " + String(outstr.length()));

        this->lastInputTime = millis();
        this->processInput(outstr);
        this->needsRedraw = true;
    }

    if (needsRedraw)
    {
        this->displayCurrentMenu();
        needsRedraw = false;
    }
}

/***********************************************************
 * ------------------- MENU PRINOUTS --------------------- *
 **********************************************************/

/**
 * Display main menu options
 */
void MenuSystem::displayMainMenu()
{
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);
    Serial.println("\nMain Menu:");
    Serial.println("1. Select Animation");
    Serial.println("2. Adjust Brightness (Current: " + String(renderer->PEAKBRIGHTNESS) + ")");
    Serial.println("3. Set Animation Speed (Current: " + String(renderer->SPEED) + "x)");
    Serial.println("4. Set LED Count (Current: " + String(renderer->LEDCOUNT) + ")");
    Serial.println("5. Toggle Repeat (Current: " + String(renderer->REPEAT ? "ON" : "OFF") + ")");
    Serial.println("6. Set Interactive Mode (Current: " + String(renderer->MODE ? renderer->MODE : "NONE") + ")");
    Serial.println("7. System Information");
    Serial.println("\nEnter selection (1-7):");
    xSemaphoreGive(renderer->LOCK);
}

/**
 * Display animation selection menu
 */
void MenuSystem::displayAnimationMenu()
{
    Serial.println("\nSelect Animation:");
    Serial.println("1. Breathe");
    Serial.println("2. Growing Bar");
    Serial.println("3. Shrinking Bar");
    Serial.println("4. Extending Bar");
    Serial.println("5. Extinguishing Bar");
    Serial.println("6. Moving Bar");
    Serial.println("7. Grow Up");
    Serial.println("8. Grow Down");
    Serial.println("9. Half Fade");
    Serial.println("10. Pulse");
    Serial.println("11. Circling Bright Dot");
    Serial.println("12. Circling Dark Dot");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-12):");
}

/**
 * Display brightness settings menu
 */
void MenuSystem::displayBrightnessMenu()
{
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);
    Serial.println("\nAdjust Brightness:");
    Serial.println("Current brightness: " + String(renderer->PEAKBRIGHTNESS));
    Serial.println("1. 10%");
    Serial.println("2. 25%");
    Serial.println("3. 50%");
    Serial.println("4. 75%");
    Serial.println("5. 100%");
    Serial.println("6. OFF");
    Serial.println("7. Custom value");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-6):");
    xSemaphoreGive(renderer->LOCK);
}

/**
 * Display speed settings menu
 */
void MenuSystem::displaySpeedMenu()
{
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);
    Serial.println("\nSet Animation Speed:");
    Serial.println("Current speed: " + String(renderer->SPEED) + "x");
    Serial.println("1. 0.25x (Slow)");
    Serial.println("2. 0.5x");
    Serial.println("3. 1.0x (Normal)");
    Serial.println("4. 1.5x");
    Serial.println("5. 2.0x (Fast)");
    Serial.println("6. Custom value");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-6):");
    xSemaphoreGive(renderer->LOCK);
}

/**
 * Display LED count settings menu
 */
void MenuSystem::displayLEDCountMenu()
{
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);
    Serial.println("\nSet LED Count:");
    Serial.println("Current count: " + String(renderer->LEDCOUNT));
    Serial.println("1. 5 LEDs");
    Serial.println("2. 10 LEDs");
    Serial.println("3. 15 LEDs");
    Serial.println("4. 20 LEDs");
    Serial.println("5. Custom count");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-5):");
    xSemaphoreGive(renderer->LOCK);
}

/**
 * Display repeat settings menu
 */
void MenuSystem::displayRepeatMenu()
{
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);
    Serial.println("\nToggle Animation Repeat:");
    Serial.println("Current setting: " + String(renderer->REPEAT ? "ON" : "OFF"));
    Serial.println("1. Enable Repeat");
    Serial.println("2. Disable Repeat");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-2):");
    xSemaphoreGive(renderer->LOCK);
}

/**
 * Display mode settings menu
 */
void MenuSystem::displayModeMenu()
{
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);
    Serial.println("\nSet Interactive Mode:");
    Serial.println("Current mode: " + String(renderer->MODE ? renderer->MODE : "NONE"));
    Serial.println("1. Moving Bar (use buttons to move light bar)");
    Serial.println("2. Growing Bar (use buttons to grow/shrink bar)");
    Serial.println("3. Extending Bar (use buttons to expand/contract from center)");
    Serial.println("4. Half Fade (use buttons to fade top/bottom half)");
    Serial.println("5. Grow Up (press up button to grow upward)");
    Serial.println("6. Grow Down (press down button to grow downward)");
    Serial.println("7. Disable Interactive Mode");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-7):");
    xSemaphoreGive(renderer->LOCK);
}

/**
 * Display system information
 */
void MenuSystem::displaySystemInfo()
{
    xSemaphoreTake(this->renderer->LOCK, portMAX_DELAY);
    xSemaphoreTake(this->renderer->CURRENTANIMATION.LOCK, portMAX_DELAY);
    Serial.println("\nSystem Information:");
    Serial.println("LED Count: " + String(renderer->LEDCOUNT));
    Serial.println("LED Pin: " + String(renderer->PIN));
    Serial.println("Animation Speed: " + String(renderer->SPEED) + "x");
    Serial.println("Peak Brightness: " + String(renderer->PEAKBRIGHTNESS * 100) + "%");
    Serial.println("Repeat: " + String(renderer->REPEAT ? "ON" : "OFF"));
    Serial.println("Repeat Delay: " + String(renderer->REPEATDELAY) + "ms");
    Serial.println("Interactive Mode: " + String(renderer->MODE ? renderer->MODE : "NONE"));
    Serial.println("Animation: " + String(renderer->CURRENTANIMATION.NAME ? renderer->CURRENTANIMATION.NAME : "None"));
    Serial.println("Running: " + String(renderer->RUNNING ? "YES" : "NO"));
    Serial.println("\nPress 0 to return to Main Menu:");
    xSemaphoreGive(this->renderer->CURRENTANIMATION.LOCK);
    xSemaphoreGive(this->renderer->LOCK);
}

/**
 * Display appropriate menu based on current state
 */
void MenuSystem::displayCurrentMenu()
{
    // Clear screen with a bunch of newlines
    Serial.println("\n\n\n\n");

    // Title header
    Serial.println("=== ESP32 LED Controller ===");

    switch (this->currentState)
    {
    case MenuState::MAIN:
        displayMainMenu();
        break;
    case MenuState::ANIMATION_SELECT:
        displayAnimationMenu();
        break;
    case MenuState::BRIGHTNESS_SETTINGS:
        displayBrightnessMenu();
        break;
    case MenuState::SPEED_SETTINGS:
        displaySpeedMenu();
        break;
    case MenuState::LED_COUNT_SETTINGS:
        displayLEDCountMenu();
        break;
    case MenuState::REPEAT_SETTINGS:
        displayRepeatMenu();
        break;
    case MenuState::MODE_SETTINGS:
        displayModeMenu();
        break;
    case MenuState::SYSTEM_INFO:
        displaySystemInfo();
        break;
    }
}

/***********************************************************
 * --------------- MENU SELECTION METHODS ---------------- *
 ***********************************************************/

void MenuSystem::processInput(String input)
{
    switch (this->currentState)
    {
    case MenuState::MAIN:
        processMainMenuInput(input);
        break;
    case MenuState::ANIMATION_SELECT:
        processAnimationMenuInput(input);
        break;

    case MenuState::BRIGHTNESS_SETTINGS:
        processBrightnessMenuInput(input);
        break;

    case MenuState::SPEED_SETTINGS:
        processSpeedMenuInput(input);
        break;

    case MenuState::LED_COUNT_SETTINGS:
        processLEDCountMenuInput(input);
        break;
    /*
    case MenuState::REPEAT_SETTINGS:
        processRepeatMenuInput(input);
        break;
    case MenuState::MODE_SETTINGS:
        processModeMenuInput(input);
        break;
    */
    default:
        // For any other state, return to main menu
        this->currentState = MenuState::MAIN;
        break;
    }
}

void MenuSystem::processMainMenuInput(String input)
{
    // Check if input is a single character
    if (input.length() == 1)
    {
        // Convert the first character to an integer
        char option = input.charAt(0);

        switch (option)
        {
        case '1':
            currentState = MenuState::ANIMATION_SELECT;
            break;
        case '2':
            currentState = MenuState::BRIGHTNESS_SETTINGS;
            break;
        case '3':
            currentState = MenuState::SPEED_SETTINGS;
            break;
        case '4':
            currentState = MenuState::LED_COUNT_SETTINGS;
            break;
        case '5':
            currentState = MenuState::REPEAT_SETTINGS;
            break;
        case '6':
            currentState = MenuState::MODE_SETTINGS;
            break;
        case '7':
            currentState = MenuState::SYSTEM_INFO;
            break;
        default:
            // Invalid input, just redraw
            Serial.println("Invalid option. Please try again.");
            break;
        }
    }
    else if (input.length() > 1)
    {
        // Handle multi-character input, or just inform the user
        Serial.println("Please enter a single digit (1-7).");
    }
}

void MenuSystem::processAnimationMenuInput(String input)
{
    if (input == "0")
    {
        this->currentState = MenuState::MAIN;
        return;
    };

    Animation *newAnimation = nullptr;
    bool validInput = true;
    xSemaphoreTake(renderer->LOCK, portMAX_DELAY);

    uint8_t BRIGHTNESS = static_cast<uint8_t>(renderer->PEAKBRIGHTNESS * 255);

    if (input == "1")
    {
        newAnimation = createBreatheAnimation(renderer->LEDCOUNT, 0.025, renderer->PEAKBRIGHTNESS, renderer->frequency);
    }
    else if (input == "2")
    {
        newAnimation = createGrowingBarAnimation(renderer->LEDCOUNT, BRIGHTNESS, 0, 0, renderer->abruptFade);
    }
    else if (input == "3")
    {
        newAnimation = createShrinkingBarAnimation(renderer->LEDCOUNT, BRIGHTNESS, 0, 0, renderer->abruptFade);
    }
    else if (input == "4")
    {
        newAnimation = createExtendingBarAnimation(renderer->LEDCOUNT, BRIGHTNESS, 0, renderer->abruptFade);
    }
    else if (input == "5")
    {
        newAnimation = createExtinguishingBarAnimation(renderer->LEDCOUNT, BRIGHTNESS, 500, renderer->abruptFade);
    }
    else if (input == "6")
    {
        newAnimation = createMovingBarAnimation(renderer->LEDCOUNT, BRIGHTNESS);
    }
    else if (input == "7")
    {
        newAnimation = createGrowUpAnimation(renderer->LEDCOUNT, BRIGHTNESS, 0, renderer->abruptFade);
    }
    else if (input == "8")
    {
        newAnimation = createGrowDownAnimation(renderer->LEDCOUNT, BRIGHTNESS, 0, renderer->abruptFade);
    }
    else if (input == "9")
    {
        newAnimation = createHalfFadeAnimation(renderer->LEDCOUNT);
    }
    else if (input == "10")
    {
        newAnimation = createPulseAnimation(renderer->LEDCOUNT, 0.015, renderer->PEAKBRIGHTNESS, 0.15, renderer->frequency);
    }
    else if (input == "11")
    {
        newAnimation = createCirclingBrightDotAnimation(renderer->LEDCOUNT, renderer->abruptFade, true, 3, BRIGHTNESS);
    }
    else if (input == "12")
    {
        newAnimation = createCirclingDarkSpotAnimation(renderer->LEDCOUNT, renderer->abruptFade, true, 3, BRIGHTNESS);
    }
    else
    {
        validInput = false;
    }
    xSemaphoreGive(renderer->LOCK);

    if (validInput)
    {
        renderer->setAnimation(*newAnimation);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        delete newAnimation;
    }
    else
    {
        Serial.println("Invalid option. Please try again.");
    }
}

void MenuSystem::processBrightnessMenuInput(String input)
{
    bool validSelection = true;
    float bt = 0.0;
    String inin = "";
    auto option = input.charAt(0);

    switch (option)
    {
    case '0':
        currentState = MenuState::MAIN;
        return;
    case '1':
        renderer->setPeakBrightness(0.1);
        break;
    case '2':
        renderer->setPeakBrightness(0.25);
        break;
    case '3':
        renderer->setPeakBrightness(0.5);
        break;
    case '4':
        renderer->setPeakBrightness(0.75);
        break;
    case '5':
        renderer->setPeakBrightness(1.0);
        break;
    case '6':
        renderer->setPeakBrightness(0.0);
        break;
    case '7':
        Serial.println("\nEnter custom brightness (0.0-1.0):");
        inin = getSerialInput();
        bt = inin.toFloat();
        if (bt >= 0.0 && bt <= 1.0)
        {
            renderer->setPeakBrightness(bt);
        }
        else
        {
            validSelection = false;
        }
        break;
    default:
        validSelection = false;
        break;
    }

    if (validSelection)
    {
        Serial.println("\nBrightness set to: " + String(renderer->PEAKBRIGHTNESS));
        vTaskDelay(100);
        // currentState = MenuState::MAIN;
    }
    else
    {
        Serial.println("Invalid brightness option. Please try again.");
    }
}

void MenuSystem::processSpeedMenuInput(String input)
{
    bool validSelection = true;
    char option = input.charAt(0);
    String inin = "";
    float speed = 0.0;

    switch (option)
    {
        case '0':
            currentState = MenuState::MAIN;
            return;
        case '1':
            renderer->setSpeed(0.25);
            break;
        case '2':
            renderer->setSpeed(0.5);
            break;
        case '3':
            renderer->setSpeed(1.0);
            break;
        case '4':
            renderer->setSpeed(1.5);
            break;
        case '5':
            renderer->setSpeed(2.0);
            break;
        case '6':
            Serial.println("\nEnter custom speed (0.0-5.0):");
            inin = getSerialInput();
            speed = inin.toFloat();
            if (speed >= 0.0000 && speed <= 10.000000) renderer->setSpeed(speed);
            else validSelection = false;
            break;
        default:
            validSelection = false;
            break;
    }

    if (validSelection)
    {
        Serial.println("\nSpeed set to: " + String(renderer->SPEED) + "x");
        vTaskDelay(100 / portTICK_PERIOD_MS);
        // this->currentState = MenuState::MAIN;
    }
    else
    {
        Serial.println("Invalid speed option. Please try again.");
    }
}


void MenuSystem::processLEDCountMenuInput(String input)
{
    bool validSelection = true;
    char option = input.charAt(0);
    String inin = "";
    uint8_t ledCount = 0;
    uint8_t maxLEDCount = renderer->getMaxLEDCount();

    switch (option)
    {
        case '0':
            currentState = MenuState::MAIN;
            return;
        case '1':
            renderer->setLEDCount(5);
            break;
        case '2':
            renderer->setLEDCount(10);
            break;
        case '3':
            renderer->setLEDCount(15);
            break;
        case '4':
            renderer->setLEDCount(20);
            break;
        case '5':
            Serial.println("\nEnter custom LED count (1-25):");
            inin = getSerialInput();
            ledCount = static_cast<uint8_t>(inin.toInt());
            if (ledCount >= 1 && ledCount <= maxLEDCount) renderer->setLEDCount(ledCount);
            else validSelection = false;
            break;

        default:
            validSelection = false;
            break;
    }

    if (validSelection)
    {
        Serial.println("\nLED count set to: " + String(renderer->LEDCOUNT));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        // this->currentState = MenuState::MAIN;
    }
    else
    {
        Serial.println("Invalid LED count option. Please try again.");
    }
}