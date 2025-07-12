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

void setup() {
	// Initialize serial communication
	Serial.begin(115200);
	while (!Serial) vTaskDelay(100 / portTICK_PERIOD_MS);
	
	// Initialize renderer with default settings
	renderer = Renderer(
		pin = LED_PIN,
		peakBrightnessCoef = 0.50f,

	);

	// Initialize the NeoPixel screen - Give it time to set up
	renderer.initializeScreen();
	vTaskDelay(100 / portTICK_PERIOD_MS);

	// Create the render task
	xTaskCreatePinnedToCore(
		renderTask,         // Function to run
		"RenderTask",       // Task name
		2097152,            // Stack size (bytes)
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
    vTaskDelay(1_000_000 / portTICK_PERIOD_MS);
}