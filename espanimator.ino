#include "render.h"
#include <Arduino.h>

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
      if (renderer.isRunning()) render(&renderer);
      if (renderer.interruptableDelay((unsigned long)(renderer.REPEATDELAY / renderer.SPEED))) renderer.setEarlyExit(false);
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
	renderer.RUNNING = false;
	xSemaphoreGive(renderer.LOCK);
	renderer.initScreen();
	vTaskDelay(200 / portTICK_PERIOD_MS);

	// Start with a breathing animation - use stack-based variable and pass by reference
	uint8_t BRIGHTNESS = static_cast<uint8_t>(renderer.PEAKBRIGHTNESS * 255);
	// Read animation from JSON
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
    vTaskDelay(100000 / portTICK_PERIOD_MS);
}