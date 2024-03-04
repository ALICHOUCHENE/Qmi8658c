#include "Qmi8658c.h"                     // Include the library for QMI8658C sensor

#define QMI_ADDRESS 0x6B                  // Define QMI8658C I2C address
#define QMI8658C_I2C_FREQUENCY 80000      // Define I2C frequency as 80kHz (in Hz)

// Declare an instance of Qmi8658c
Qmi8658c qmi8658c(QMI_ADDRESS, QMI8658C_I2C_FREQUENCY);

/* QMI8658C configuration */
qmi8658_cfg_t qmi8658_cfg = {
  .qmi8658_mode = qmi8658_mode_dual,      // Set the QMI8658C mode to dual mode
  .acc_scale = acc_scale_2g,              // Set the accelerometer scale to ±2g
  .acc_odr = acc_odr_8000,                // Set the accelerometer output data rate (ODR) to 8000Hz
  .gyro_scale = gyro_scale_16dps,         // Set the gyroscope scale to ±16 dps
  .gyro_odr = gyro_odr_8000,              // Set the gyroscope output data rate (ODR) to 8000Hz
};

qmi8658_result_t qmi8658_result;

void setup() {
  // Setup serial communication
  Serial.begin(9600);

  // Initialize QMI8658C sensor with provided configuration
  qmi8658_result = qmi8658c.open(&qmi8658_cfg);
  delay(1000); // Delay for sensor initialization
  Serial.print("Open result: ");
  Serial.println(qmi8658c.resultToString(qmi8658_result)); // Print initialization result
  Serial.print("Device ID: 0x");
  Serial.println(qmi8658c.deviceID, HEX); // Print device ID
  Serial.print("Device Revision ID: 0x");
  Serial.println(qmi8658c.deviceRevisionID, HEX); // Print device revision ID
}

qmi_data_t data; // Declare a variable to store sensor data

void loop() {
  qmi8658c.read(&data); // Read sensor data from QMI8658C sensor

  // Print accelerometer data
  Serial.print("acc_x: ");
  Serial.print(data.acc_xyz.x);
  Serial.print(" | acc_y: ");
  Serial.print(data.acc_xyz.y);
  Serial.print(" | acc_z: ");
  Serial.println(data.acc_xyz.z);

  // Print gyroscope data
  Serial.print("gyro_x: ");
  Serial.print(data.gyro_xyz.x);
  Serial.print(" | gyro_y: ");
  Serial.print(data.gyro_xyz.y);
  Serial.print(" | gyro_z: ");
  Serial.println(data.gyro_xyz.z);
  
  delay(100);
}
