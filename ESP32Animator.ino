#include "render.h"
#include <Arduino.h>

#define LED_PIN 42

// Global renderer
Renderer renderer = Renderer(
	10,
	LED_PIN
);

// Task handles
TaskHandle_t renderTaskHandle = NULL;

/**
 * Render task: Handles LED animation rendering
 * @param parameters Task parameters
 */
void renderTask(void* parameters) {
	RenderState state = renderer.outputState();
    while (true) {
      if (renderer.isRunning()) state = render(renderer);
      if (renderer.interruptableDelay((unsigned long)(state.frameDelayMs / state.speedCoefficient))) renderer.setEarlyExit(false);
    }
}

void setup() {
	// Initialize serial communication
	Serial.begin(115200);
	while (!Serial) vTaskDelay(100 / portTICK_PERIOD_MS);

	// Initialize the NeoPixel screen - Give it time to set up
	renderer.initializeScreen();
	vTaskDelay(100 / portTICK_PERIOD_MS);

	// Create the render task
	xTaskCreatePinnedToCore(
		renderTask,         // Function to run
		"RenderTask",       // Task name
		2097152,            // Stack size (bytes)
		NULL,               // Task parameters
		2,                  // Priority (higher than default)
		&renderTaskHandle,  // Task handle;
		1                   // Core to run on (dedicate core 1 to rendering)
	);
}

/**
 * Main loop function
 */
void loop() {
    // Main loop is empty since we're using FreeRTOS tasks
    vTaskDelay(1000000 / portTICK_PERIOD_MS);
}
