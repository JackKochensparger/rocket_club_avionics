#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <SD.h>
#include <SPI.h>
#include "sensors.h"

class SDLogger {
public:
  void begin();
  void logData(float elapsedTime, const SensorManager& sensors, bool debugMode = false);
  bool isLogging() { return logging_active; }

private:
  File dataFile;
  bool logging_active;
  String filename;
  bool header_printed_to_serial;
  
  const int SD_CS_PIN = 25;  // Your actual CS pin
  
  String createTimestampFilename();
  void writeHeader();
  void writeHeaderToSerial();
  bool testSDCard();
  void logDataToSerial(float elapsedTime, const SensorManager& sensors);
};

#endif