#include "render.h"

#define LED_PIN 42

// Global renderer
Renderer renderer;

// Task handles
TaskHandle_t renderTaskHandle = NULL;

/**
 * Render task: Handles LED animation rendering
 * @param parameters Task parameters
 */
void renderTask(void* parameters) {
    while (true) {
      if (renderer.isRunning()) {
        render(&renderer);
      }
      vTaskDelay(renderer.DELAY / renderer.SPEED);
    }
}

/**
 * Main setup function
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial) vTaskDelay(10);

    // Initialize renderer with default settings
    xSemaphoreTake(renderer.LOCK, portMAX_DELAY);
    renderer.LEDCOUNT = 15;
    renderer.PIN = LED_PIN;
    renderer.DELAY = 50;
    renderer.REPEATDELAY = 2500;
    renderer.SPEED = 1;
    renderer.PEAKBRIGHTNESS = 0.25;
    renderer.REPEAT = true;
    renderer.MODE = "NONE";
    renderer.RUNNING = false;
    xSemaphoreGive(renderer.LOCK);

    renderer.initScreen();
    vTaskDelay(200);

    // Start with a breathing animation - use stack-based variable and pass by reference
    Animation* breathe = createExtinguishingBarAnimation(renderer.LEDCOUNT, 100);
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
}

/**
 * Main loop function
 */
void loop() {
    // Main loop is empty since we're using FreeRTOS tasks
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}