/*
  NINA-B306 Simple Data Logger
  Clean restart focused on reliable SD card logging
  
  File Structure:
  - main.cpp (this file - rename to main.ino for Arduino IDE)
  - sensors.h
  - sensors.cpp
  - sdlogger.h
  - sdlogger.cpp
*/

#include "sensors.h"
#include "sdlogger.h"

// DEBUG MODE - Set to true to enable serial logging
const bool DEBUG_MODE = true;  // Change to false to disable debug output

// Timing
unsigned long startTime = 0;
unsigned long lastLogTime = 0;
const unsigned long LOG_INTERVAL = 100; // 100ms = 0.1 second precision

// Global objects
SensorManager sensors;
SDLogger logger;

void setup() {
  // Initialize Serial FIRST for diagnostics
  Serial.begin(115200);

  delay(1000);  // Give serial time to initialize

  logger.begin();

  if (DEBUG_MODE) {
    Serial.println("Initializing sensors...");
  }
  sensors.begin();  // This will now print detailed diagnostics
  
  
  if (DEBUG_MODE) {
    //Serial.println(DEBUG_MODE);
    Serial.println("DEBUG_MODE: Enabled");
  }
  // Record exact start time in milliseconds
  startTime = millis(); 
  
  if (DEBUG_MODE) {
    if (logger.isLogging()) {
      Serial.println("SD logging: ACTIVE");
    } else {
      Serial.println("SD logging: FAILED - Check SD card");
    }
    Serial.println("Setup complete. Starting data logging...");
    Serial.println();
  }
  
  // Small delay to let everything stabilize
  delay(500);
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastLogTime >= LOG_INTERVAL) {
    // Read all sensors
    sensors.readAll();
    
    // Calculate elapsed time in seconds with 0.1s precision
    float elapsedTime = (currentTime - startTime) / 1000.0;
    
    // Log the data (will log to both SD and Serial if DEBUG_MODE is true)
    logger.logData(elapsedTime, sensors, DEBUG_MODE);
    
    lastLogTime = currentTime;
  }
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}