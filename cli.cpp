#include "cli.h"
#include <Arduino.h>

/**
 * Main update loop for menu system
 */
void MenuSystem::update() {
    // Check for user input
    if (Serial.available()) {
      char input = Serial.read();
      
      // Consume any extra newlines or carriage returns
      while (Serial.available() && (Serial.peek() == '\n' || Serial.peek() == '\r')) {
        Serial.read();
      }
      
      if (input >= ' ' && input <= '~') {  // Printable characters
        this -> processInput(input);
        lastInputTime = millis();
        this -> needsRedraw = true;
      }
    }
    
    // Display menu if needed
    if (needsRedraw) {
      this -> displayCurrentMenu();
      this -> needsRedraw = false;
    }
}



/***********************************************************
 * ------------------- MENU PRINOUTS --------------------- *
 **********************************************************/

/**
 * Display main menu options
 */
void MenuSystem::displayMainMenu() {
    Serial.println("\nMain Menu:");
    Serial.println("1. Select Animation");
    Serial.println("2. Adjust Brightness (Current: " + String(renderer->PEAKBRIGHTNESS) + ")");
    Serial.println("3. Set Animation Speed (Current: " + String(renderer->SPEED) + "x)");
    Serial.println("4. Set LED Count (Current: " + String(renderer->LEDCOUNT) + ")");
    Serial.println("5. Toggle Repeat (Current: " + String(renderer->REPEAT ? "ON" : "OFF") + ")");
    Serial.println("6. Set Interactive Mode (Current: " + String(renderer->MODE ? renderer->MODE : "NONE") + ")");
    Serial.println("7. System Information");
    Serial.println("\nEnter selection (1-7):");
  }
  
  /**
   * Display animation selection menu
   */
  void MenuSystem::displayAnimationMenu() {
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
  void MenuSystem::displayBrightnessMenu() {
    Serial.println("\nAdjust Brightness:");
    Serial.println("Current brightness: " + String(renderer->PEAKBRIGHTNESS));
    Serial.println("1. 10%");
    Serial.println("2. 25%");
    Serial.println("3. 50%");
    Serial.println("4. 75%");
    Serial.println("5. 100%");
    Serial.println("6. Custom value");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-6):");
  }
  
  /**
   * Display speed settings menu
   */
  void MenuSystem::displaySpeedMenu() {
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
  }
  
  /**
   * Display LED count settings menu
   */
  void MenuSystem::displayLEDCountMenu() {
    Serial.println("\nSet LED Count:");
    Serial.println("Current count: " + String(renderer->LEDCOUNT));
    Serial.println("1. 30 LEDs");
    Serial.println("2. 60 LEDs");
    Serial.println("3. 90 LEDs");
    Serial.println("4. 120 LEDs");
    Serial.println("5. Custom count");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-5):");
  }
  
  /**
   * Display repeat settings menu
   */
  void MenuSystem::displayRepeatMenu() {
    Serial.println("\nToggle Animation Repeat:");
    Serial.println("Current setting: " + String(renderer->REPEAT ? "ON" : "OFF"));
    Serial.println("1. Enable Repeat");
    Serial.println("2. Disable Repeat");
    Serial.println("0. Return to Main Menu");
    Serial.println("\nEnter selection (0-2):");
  }
  
  /**
   * Display mode settings menu
   */
  void MenuSystem::displayModeMenu() {
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
  }
  
  /**
   * Display system information
   */
  void MenuSystem::displaySystemInfo() {
    Serial.println("\nSystem Information:");
    Serial.println("LED Count: " + String(renderer->LEDCOUNT));
    Serial.println("LED Pin: " + String(renderer->PIN));
    Serial.println("Animation Speed: " + String(renderer->SPEED) + "x");
    Serial.println("Peak Brightness: " + String(renderer->PEAKBRIGHTNESS * 100) + "%");
    Serial.println("Repeat: " + String(renderer->REPEAT ? "ON" : "OFF"));
    Serial.println("Repeat Delay: " + String(renderer->REPEATDELAY) + "ms");
    Serial.println("Interactive Mode: " + String(renderer->MODE ? renderer->MODE : "NONE"));
    Serial.println("Animation: " + String(renderer->CURRENTANIMATION.name ? renderer->CURRENTANIMATION.name : "None"));
    Serial.println("Running: " + String(renderer->RUNNING ? "YES" : "NO"));
    Serial.println("\nPress 0 to return to Main Menu:");
}