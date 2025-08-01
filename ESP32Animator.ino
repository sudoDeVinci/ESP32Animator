#include "io.h"
#include "animation.h"
#include "render.h"
#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
#include "SD_MMC.h"

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
		debugln("Render loopin!");
		if (renderer.isRunning()) state = render(renderer);	
		if (renderer.interruptableDelay((unsigned long)(state.frameDelayMs / state.speedCoefficient))) renderer.setEarlyExit(false);
	}
}

void setup() {
	// Initialize serial communication
	Serial.begin(115200);
	while (!Serial) delay(10 / portTICK_PERIOD_MS);


	sdmmcInit();
	fs::FS& fs = determineFileSystem();

	FileWrapper root(fs, "/");
	const std::vector<FileWrapper>& files = root.listDir();
	for (const FileWrapper& file : files) {
		debugf("File: %s, Path: %s, Is Directory: %d\n", 
		file.getName().c_str(), file.getPath().c_str(), file.isDirectory());
	}

	const std::string folder = "animations";
	const FileWrapper& animationdir = root.getDir(folder);
	debugf("Animation Directory is: %s at path %s", animationdir.getName().c_str(), animationdir.getPath().c_str());
	
	const std::string animationfile = "00-big_eye.json";
	const FileWrapper& animationJson = animationdir.getFile(animationfile);
	debugf("Animation Json File is: %s at path %s", animationJson.getName().c_str(), animationJson.getPath().c_str());

	Animation animation = loadAnimation(fs, animationJson.getPath());

	renderer.setLedCount(100);
	renderer.setAnimation(animation);
	renderer.setPeakBrightness(0.075f);
	renderer.setframeDelayms(125);
	renderer.setrepeatDelayms(2000);

	// Initialize the NeoPixel screen - Give it time to set up
	renderer.initializeScreen();
	vTaskDelay(100 / portTICK_PERIOD_MS);
	renderer.setRunning(true);

	// Create the render task
	if (xTaskCreatePinnedToCore(
		renderTask,         // Function to run
		"RenderTask",       // Task name
		102400,            // Stack size (bytes)
		NULL,               // Task parameters
		2,                  // Priority (higher than default)
		&renderTaskHandle,  // Task handle;
		1                   // Core to run on (dedicate core 1 to rendering)
  ) != pdPASS) {
    debugln("Failed to create render task!");
}
}

/**
 * Main loop function
 */
void loop() {
    // Main loop is empty since we're using FreeRTOS tasks
    vTaskDelay(1000000 / portTICK_PERIOD_MS);
}
