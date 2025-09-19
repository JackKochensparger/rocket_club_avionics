#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include <SparkFunBME280.h>
#include <SparkFun_ISM330DHCX.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

struct SensorData {
  // Environmental
  float temperature_C;
  float humidity_percent;
  float pressure_Pa;
  
  // IMU - Acceleration in mg (milligravity)
  float accel_X_mg;
  float accel_Y_mg;
  float accel_Z_mg;
  float accel_total_mg;
  
  // IMU - Gyroscope in dps (degrees per second)
  float gyro_X_dps;
  float gyro_Y_dps;
  float gyro_Z_dps;
  
  // Battery
  float battery_voltage_V;
  float battery_percent;
  
  // Status flags
  bool temp_sensor_ok;
  bool imu_sensor_ok;
  bool battery_sensor_ok;
};

class SensorManager {
public:
  void begin();
  void readAll();
  SensorData data;

private:
  // Declare exactly like working code
  BME280 envSensor;
  SparkFun_ISM330DHCX imuSensor;
  SFE_MAX1704X lipo = SFE_MAX1704X(MAX1704X_MAX17048);
  
  // Working code data structures
  sfe_ism_data_t _accelData;
  sfe_ism_data_t _gyroData;
  
  void initEnvironmental();
  void initIMU();
  void initBattery();
  void readEnvironmental();
  void readIMU();
  void readBattery();
};

#endif