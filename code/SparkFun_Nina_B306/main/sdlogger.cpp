#include "sdlogger.h"
#include <SPI.h>
#include <SD.h>

void SDLogger::begin() {
  logging_active = false;
  header_printed_to_serial = false;

  Serial.println("Starting SD card initialization...");
  Serial.print("Using CS pin: ");
  Serial.println(SD_CS_PIN);

  // Set the CS pin as an output and ensure it's high
  // This is a critical step for many SD card modules to work correctly.
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  delay(10); // A small delay can help stabilize the bus

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization FAILED");
    return;
  }
  Serial.println("SD card found");

  if (!testSDCard()) {
    Serial.println("SD card test FAILED");
    return;
  }
  Serial.println("SD card test passed");

  filename = createTimestampFilename();
  Serial.print("Creating file: ");
  Serial.println(filename);

  dataFile = SD.open(filename.c_str(), FILE_WRITE);
  if (!dataFile) {
    Serial.println("Could not create data file");
    return;
  }

  writeHeader();

  logging_active = true;
  Serial.println("SD logging is now ACTIVE");
}

String SDLogger::createTimestampFilename() {
  unsigned long timestamp = millis();
  char filename[20];
  sprintf(filename, "DATA_%06lu.CSV", timestamp);
  return String(filename);
}

void SDLogger::writeHeader() {
  dataFile.println("# ROCKET DATA LOG");
  dataFile.println("# Time(s),Temp(C),Humidity(%),Pressure(Pa),AccelX(mg),AccelY(mg),AccelZ(mg),AccelTotal(mg),GyroX(dps),GyroY(dps),GyroZ(dps),BattVolt(V),BattPercent(%)");
  dataFile.flush();
}

void SDLogger::writeHeaderToSerial() {
  if (!header_printed_to_serial) {
    Serial.println("# ROCKET DATA LOG");
    Serial.println("# Time(s),Temp(C),Humidity(%),Pressure(Pa),AccelX(mg),AccelY(mg),AccelZ(mg),AccelTotal(mg),GyroX(dps),GyroY(dps),GyroZ(dps),BattVolt(V),BattPercent(%)");
    header_printed_to_serial = true;
  }
}

bool SDLogger::testSDCard() {
  File testFile = SD.open("TEST.TMP", FILE_WRITE);
  if (!testFile) {
    return false;
  }

  testFile.println("TEST");
  testFile.close();

  testFile = SD.open("TEST.TMP", FILE_READ);
  if (!testFile) {
    return false;
  }

  String testData = testFile.readString();
  testFile.close();
  SD.remove("TEST.TMP");

  return (testData.indexOf("TEST") >= 0);
}

void SDLogger::logDataToSerial(float elapsedTime, const SensorManager& sensors) {
  writeHeaderToSerial();

  Serial.print(elapsedTime, 1);
  Serial.print(",");
  Serial.print(sensors.data.temperature_C, 2);
  Serial.print(",");
  Serial.print(sensors.data.humidity_percent, 1);
  Serial.print(",");
  Serial.print(sensors.data.pressure_Pa, 0);
  Serial.print(",");
  Serial.print(sensors.data.accel_X_mg, 1);
  Serial.print(",");
  Serial.print(sensors.data.accel_Y_mg, 1);
  Serial.print(",");
  Serial.print(sensors.data.accel_Z_mg, 1);
  Serial.print(",");
  Serial.print(sensors.data.accel_total_mg, 1);
  Serial.print(",");
  Serial.print(sensors.data.gyro_X_dps, 2);
  Serial.print(",");
  Serial.print(sensors.data.gyro_Y_dps, 2);
  Serial.print(",");
  Serial.print(sensors.data.gyro_Z_dps, 2);
  Serial.print(",");
  Serial.print(sensors.data.battery_voltage_V, 3);
  Serial.print(",");
  Serial.println(sensors.data.battery_percent, 1);
}

void SDLogger::logData(float elapsedTime, const SensorManager& sensors, bool debugMode) {
  if (logging_active && dataFile) {
    dataFile.print(elapsedTime, 1);
    dataFile.print(",");
    dataFile.print(sensors.data.temperature_C, 2);
    dataFile.print(",");
    dataFile.print(sensors.data.humidity_percent, 1);
    dataFile.print(",");
    dataFile.print(sensors.data.pressure_Pa, 0);
    dataFile.print(",");
    dataFile.print(sensors.data.accel_X_mg, 1);
    dataFile.print(",");
    dataFile.print(sensors.data.accel_Y_mg, 1);
    dataFile.print(",");
    dataFile.print(sensors.data.accel_Z_mg, 1);
    dataFile.print(",");
    dataFile.print(sensors.data.accel_total_mg, 1);
    dataFile.print(",");
    dataFile.print(sensors.data.gyro_X_dps, 2);
    dataFile.print(",");
    dataFile.print(sensors.data.gyro_Y_dps, 2);
    dataFile.print(",");
    dataFile.print(sensors.data.gyro_Z_dps, 2);
    dataFile.print(",");
    dataFile.print(sensors.data.battery_voltage_V, 3);
    dataFile.print(",");
    dataFile.println(sensors.data.battery_percent, 1);

    static int writeCount = 0;
    writeCount++;
    if (writeCount >= 10) {
      dataFile.flush();
      writeCount = 0;
    }
  }

  if (debugMode) {
    logDataToSerial(elapsedTime, sensors);
  }
}