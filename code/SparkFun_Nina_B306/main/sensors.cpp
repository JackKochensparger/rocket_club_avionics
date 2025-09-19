#include "sensors.h"

void SensorManager::begin() {
  Serial.println("Starting sensor initialization...");
  
  Wire.begin();
  
  // Initialize exactly like your working code
  initBattery();
  initEnvironmental();
  initIMU();
  
  // Clear data structure
  memset(&data, 0, sizeof(data));
  
  Serial.println("Sensor initialization complete");
}

void SensorManager::initBattery() {
  Serial.println("Initializing MAX17048 Battery Fuel Gauge...");

  if (!lipo.begin()) {
    Serial.println("MAX17048 not detected. Please check wiring.");
    data.battery_sensor_ok = false;
  } else {
    lipo.setThreshold(20);
    lipo.clearAlert();
    data.battery_sensor_ok = true;
    Serial.println("MAX17048 initialized successfully");
  }
}

void SensorManager::initEnvironmental() {
  Serial.println("Initializing BME280 Environmental Sensor...");
  envSensor.setI2CAddress(0x76); // Default address on NINA-B306

  if (!envSensor.beginI2C()) {
    Serial.println("The BME280 did not respond. Please check address and wiring.");
    data.temp_sensor_ok = false;
  } else {
    data.temp_sensor_ok = true;
    Serial.println("BME280 initialization successful.");
  }
}

void SensorManager::initIMU() {
  Serial.println("Initializing ISM330DHCX...");

  if (!imuSensor.begin()) {
    Serial.println("The ISM330DHCX did not respond. Please check address and wiring.");
    data.imu_sensor_ok = false;
    return;
  }

  Serial.println("ISM330DHCX Initialization successful. Resetting device settings...");

  imuSensor.deviceReset();

  while(!imuSensor.getDeviceReset()){
    delay(1);
  }
    
  Serial.println("Device reset complete. Configuring ISM330DHCX...");
  delay(100);

  imuSensor.setDeviceConfig();
  imuSensor.setBlockDataUpdate();

  // Set accelerometer output data rate and data precision
  imuSensor.setAccelDataRate(ISM_XL_ODR_104Hz);
  imuSensor.setAccelFullScale(ISM_4g);

  // Set gyro output data rate and data precision
  imuSensor.setGyroDataRate(ISM_GY_ODR_104Hz);
  imuSensor.setGyroFullScale(ISM_500dps);

  // Turn on accelerometer's filter and apply settings.
  imuSensor.setAccelFilterLP2();
  imuSensor.setAccelSlopeFilter(ISM_LP_ODR_DIV_100);

  // Turn on gyro's filter and apply settings.
  imuSensor.setGyroFilterLP1();
  imuSensor.setGyroLP1Bandwidth(ISM_MEDIUM);

  data.imu_sensor_ok = true;
  Serial.println("ISM330DHCX successfully configured.");
}

void SensorManager::readAll() {
  readEnvironmental();
  readIMU();
  readBattery();
}

void SensorManager::readEnvironmental() {
 
  //if (temp_sensor_ok): {
  data.temperature_C = envSensor.readTempC();
  data.humidity_percent = envSensor.readFloatHumidity();
  data.pressure_Pa = envSensor.readFloatPressure();
  //} else {
  //  data.temperature_C = -999.0;
  //  data.humidity_percent = -999.0;
  //  data.pressure_Pa = -999.0;
  //}
}

void SensorManager::readIMU() {
  if (1==1) {
  //if (data.imu_sensor_ok && imuSensor.checkStatus()) {
    sfe_ism_data_t accelData;
    sfe_ism_data_t gyroData;
    
    if (imuSensor.getAccel(&accelData) && imuSensor.getGyro(&gyroData)) {
      // Use same scaling as your working code
      data.accel_X_mg = accelData.xData;
      data.accel_Y_mg = accelData.yData;
      data.accel_Z_mg = accelData.zData;
      
      data.accel_total_mg = sqrt(
        data.accel_X_mg * data.accel_X_mg +
        data.accel_Y_mg * data.accel_Y_mg +
        data.accel_Z_mg * data.accel_Z_mg
      );
      
      data.gyro_X_dps = gyroData.xData / 1000.0;
      data.gyro_Y_dps = gyroData.yData / 1000.0;
      data.gyro_Z_dps = gyroData.zData / 1000.0;
    } else {
      data.accel_X_mg = -999.0;
      data.accel_Y_mg = -999.0;
      data.accel_Z_mg = -999.0;
      data.accel_total_mg = -999.0;
      data.gyro_X_dps = -999.0;
      data.gyro_Y_dps = -999.0;
      data.gyro_Z_dps = -999.0;
    }
  } else {
    data.accel_X_mg = -999.0;
    data.accel_Y_mg = -999.0;
    data.accel_Z_mg = -999.0;
    data.accel_total_mg = -999.0;
    data.gyro_X_dps = -999.0;
    data.gyro_Y_dps = -999.0;
    data.gyro_Z_dps = -999.0;
  }
}

void SensorManager::readBattery() {
  if (1==1) {
  //if (data.battery_sensor_ok) {
    data.battery_voltage_V = lipo.getVoltage();
    data.battery_percent = lipo.getSOC();
  } else {
    data.battery_voltage_V = -999.0;
    data.battery_percent = -999.0;
  }
}