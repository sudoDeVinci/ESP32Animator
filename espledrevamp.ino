#include "render.h"
#include "cli.h"
#include <Arduino.h>

#define LED_PIN 42

// Global renderer
Renderer renderer;
MenuSystem menuSystem(&renderer);

// Task handles
TaskHandle_t renderTaskHandle = NULL;
TaskHandle_t cliTaskHandle = NULL;

/**
 * Render task: Handles LED animation rendering
 * @param parameters Task parameters
 */
void renderTask(void* parameters) {
    while (true) {
      if (renderer.isRunning()) render(&renderer);
      if (renderer.interruptableDelay((unsigned long)(renderer.REPEATDELAY / renderer.SPEED))) renderer.setEarlyExit(false);
      // if (renderer.getRepeat()) renderer.setRunning(true);
    }
}

/**
 * Menu task: Handles user input and button presse callbacks
 */
void menuTask(void* parameters) {
  while (true) {
    menuSystem.update();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

/**
 * Main setup function
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial) vTaskDelay(100 / portTICK_PERIOD_MS);

    // Initialize renderer with default settings
    xSemaphoreTake(renderer.LOCK, portMAX_DELAY);
    renderer.LEDCOUNT = 10;
    renderer.PIN = LED_PIN;
    renderer.DELAY = 50;
    renderer.REPEATDELAY = 50;
    renderer.SPEED = 1;
    renderer.PEAKBRIGHTNESS = 0.40f;
    renderer.REPEAT = true;
    renderer.MODE = "NONE";
    renderer.RUNNING = false;
    xSemaphoreGive(renderer.LOCK);
    renderer.initScreen();
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // Start with a breathing animation - use stack-based variable and pass by reference
    Animation* breathe = createCirclingDarkSpotAnimation(renderer.LEDCOUNT, false, true, 3, 100);
    renderer.setAnimation(*breathe);
    delete breathe;  // Clean up our animation after copying its data
    debugln("About to start render task");

    // Create the render task
    xTaskCreatePinnedToCore(
        renderTask,         // Function to run
        "RenderTask",       // Task name
        16384,              // Stack size (bytes)
        NULL,               // Task parameters
        1,                  // Priority
        &renderTaskHandle,  // Task handle;
        0                   // Core to run on
    );

    // Create the CLI task
    xTaskCreatePinnedToCore(
        menuTask,           // Function to run
        "CLITask",          // Task name
        2048,              // Stack size (bytes)
        NULL,               // Task parameters
        1,                  // Priority
        &cliTaskHandle,     // Task handle
        1                   // Core to run on
    );
}

/**
 * Main loop function
 */
void loop() {
    // Main loop is empty since we're using FreeRTOS tasks
    vTaskDelay(100000 / portTICK_PERIOD_MS);
}