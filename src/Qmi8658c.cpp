/*
 * Copyright (c) 2024 CHOUCHENE Ali
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <Wire.h>
#include <Arduino.h>
#include "qmi8658c.h"

/* Structure for QMI context */
typedef struct {
    uint16_t acc_sensitivity;   // Sensitivity value for the accelerometer.
    uint8_t acc_scale;          // Scale setting for the accelerometer.
    uint16_t gyro_sensitivity;  // Sensitivity value for the gyroscope.
    uint8_t gyro_scale;         // Scale setting for the gyroscope.
} qmi_ctx_t;

qmi_ctx_t qmi_ctx;  // QMI context instance.

/* Accelerometer sensitivity table */
uint16_t acc_scale_sensitivity_table[4] = {
    ACC_SCALE_SENSITIVITY_2G,   // Sensitivity for ±2g range.
    ACC_SCALE_SENSITIVITY_4G,   // Sensitivity for ±4g range.
    ACC_SCALE_SENSITIVITY_8G,   // Sensitivity for ±8g range.
    ACC_SCALE_SENSITIVITY_16G   // Sensitivity for ±16g range.
};
                                  
/* Gyroscope sensitivity table */
uint16_t gyro_scale_sensitivity_table[8] = {
    GYRO_SCALE_SENSITIVITY_16DPS,     // Sensitivity for ±16 degrees per second range.
    GYRO_SCALE_SENSITIVITY_32DPS,     // Sensitivity for ±32 degrees per second range.
    GYRO_SCALE_SENSITIVITY_64DPS,     // Sensitivity for ±64 degrees per second range.
    GYRO_SCALE_SENSITIVITY_128DPS,    // Sensitivity for ±128 degrees per second range.
    GYRO_SCALE_SENSITIVITY_256DPS,    // Sensitivity for ±256 degrees per second range.
    GYRO_SCALE_SENSITIVITY_512DPS,    // Sensitivity for ±512 degrees per second range.
    GYRO_SCALE_SENSITIVITY_1024DPS,   // Sensitivity for ±1024 degrees per second range.
    GYRO_SCALE_SENSITIVITY_2048DPS    // Sensitivity for ±2048 degrees per second range.
};

/*################################# Functions for i2c communication #################################*/

// Write a byte of data to a register in the QMI8658 device.

void Qmi8658c::qmi8658_write(uint8_t reg, uint8_t value) {

    // Send register address and data to write
    Wire.beginTransmission(this->deviceAdress);
    Wire.write(reg);        // Specify the register you want to write to
    Wire.write(value);      // Write the value to the register
    Wire.endTransmission(); // End transmission
}

// Read a byte of data from a register in the QMI8658 device.

uint8_t Qmi8658c::qmi8658_read(uint8_t reg) {

    uint8_t value;
    unsigned long startTime;

    // Request data from a specific register
    Wire.beginTransmission(this->deviceAdress);
    Wire.write(reg);             // Specify the register you want to read from
    Wire.endTransmission(false); // Do not release the I2C bus
    // Request data from the sensor
    Wire.requestFrom(this->deviceAdress, 1); // Request 1 byte of data


    startTime = millis(); // Record the start time
    // Wait for data to become available
    while(Wire.available() < 1) {
        // Wait
        // timout for breaking the loop
        if (millis() - startTime > 1000) 
         return;
    }

  // Read data from the register
  value = Wire.read();
  Wire.endTransmission();
  
  return value;
}

/* ####################### comman function for accelerometer, gyroscope and magnetometer ####################### */

// Set the mode of operation for the QMI8658 device.

void Qmi8658c::select_mode(qmi8658_mode_t qmi8658_mode ) {
  
  uint8_t qmi8658_ctrl7_reg = this->qmi8658_read(QMI8658_CTRL7);
    
  qmi8658_ctrl7_reg = (0xFC & qmi8658_ctrl7_reg) | qmi8658_mode;
  this->qmi8658_write(QMI8658_CTRL7,qmi8658_ctrl7_reg);   
}


/*################################# Functions for accelerometr #################################*/

// function to set output data rate of the accelerometer

void Qmi8658c::acc_set_odr(acc_odr_t odr) {

    uint8_t qmi8658_ctrl2_reg = this->qmi8658_read(QMI8658_CTRL2);
    
    qmi8658_ctrl2_reg = (0xF0 & qmi8658_ctrl2_reg) | odr;
    this->qmi8658_write(QMI8658_CTRL2,qmi8658_ctrl2_reg); 
}

// function to set the scale of the accelerometer

void Qmi8658c::acc_set_scale(acc_scale_t acc_scale) {

    uint8_t qmi8658_ctrl2_reg = this->qmi8658_read(QMI8658_CTRL2);
    qmi_ctx.acc_scale = acc_scale;
    qmi_ctx.acc_sensitivity = acc_scale_sensitivity_table[acc_scale];
    
    qmi8658_ctrl2_reg = (0x8F & qmi8658_ctrl2_reg) | (acc_scale << 4);
    this->qmi8658_write(QMI8658_CTRL2,qmi8658_ctrl2_reg);
}

/*################################# Functions for gyroscope #################################*/

// Set Gyroscope Output Data Rate (ODR)

void Qmi8658c::gyro_set_odr(gyro_odr_t odr){

    uint8_t qmi8658_ctrl3_reg = this->qmi8658_read(QMI8658_CTRL3);
    
    qmi8658_ctrl3_reg = (0xF0 & qmi8658_ctrl3_reg) | odr;
    this->qmi8658_write(QMI8658_CTRL3,qmi8658_ctrl3_reg); 
}

// Set Gyroscope Full-scale

void Qmi8658c::gyro_set_scale(gyro_scale_t gyro_scale) {

    uint8_t qmi8658_ctrl3_reg = this->qmi8658_read(QMI8658_CTRL3);
    qmi_ctx.gyro_scale = gyro_scale;
    qmi_ctx.gyro_sensitivity = gyro_scale_sensitivity_table[gyro_scale];
    
    qmi8658_ctrl3_reg = (0x8F & qmi8658_ctrl3_reg) | (gyro_scale << 4);
    this->qmi8658_write(QMI8658_CTRL3,qmi8658_ctrl3_reg);
}


/*################################# General functions for qmi configuration #################################*/

// Reset all regesters of the qmi8658 : 
// To be done before starting interfacing with the sensor to make sure it's on a "konwn" state

void Qmi8658c:: qmi_reset(){
  
    this->qmi8658_write(QMI8658_RESET,0xB0);
}

/*################################# constructor function #################################*/

//Initializes a new instance of the Qmi8658c class with the specified device address and frequency.

Qmi8658c::Qmi8658c(uint8_t deviceAdress, uint32_t deviceFrequency) {
    this->deviceFrequency = deviceFrequency;
    this->deviceAdress = deviceAdress;

    // clear context
    memset(&qmi_ctx, 0, sizeof(qmi_ctx_t));
    qmi_ctx.acc_scale = acc_scale_2g;
    qmi_ctx.acc_sensitivity = ACC_SCALE_SENSITIVITY_2G;
    qmi_ctx.gyro_scale = gyro_scale_16dps;
    qmi_ctx.gyro_sensitivity = GYRO_SCALE_SENSITIVITY_16DPS;

    // initiate IIC bus
    Wire.begin(); // start IIC bus
    Wire.setClock(deviceFrequency);
}

/*##################################### class functions ############################*/

// Open communication with the QMI8658 sensor and initializes it with the provided configuration settings.
// Return a status code indicating success or failure of the operation.

qmi8658_result_t Qmi8658c::open(qmi8658_cfg_t* qmi8658_cfg) {

    qmi8658_result_t ret;
    uint8_t qmi8658_ctrl7;
    
    this->qmi_reset();
    this->select_mode(qmi8658_cfg->qmi8658_mode);

    // set accelerometr and gyroscope scale and ODR   
    this->acc_set_odr(qmi8658_cfg->acc_odr);
    this->acc_set_scale(qmi8658_cfg->acc_scale);
    this->gyro_set_odr(qmi8658_cfg->gyro_odr);
    this->gyro_set_scale(qmi8658_cfg->gyro_scale);

    // read device ID and revision ID
    this->deviceID = this->qmi8658_read(QMI8658_WHO_AM_I);
    this->deviceRevisionID = this->qmi8658_read(QMI8658_REVISION);

    qmi8658_ctrl7 = qmi8658_read(QMI8658_CTRL7);
    ret =(qmi8658_ctrl7 & 0x03) == qmi8658_cfg->qmi8658_mode ? qmi8658_result_open_success : qmi8658_result_open_error;
    
    return ret;
}

// Read data from the QMI8658 sensor and stores it in the provided data structure.

void Qmi8658c::read(qmi_data_t* data) {

    // read accelerometer data
    int16_t acc_x = (((int16_t)this->qmi8658_read(QMI8658_ACC_X_H) << 8) | this->qmi8658_read(QMI8658_ACC_X_L));
    int16_t acc_y = (((int16_t)this->qmi8658_read(QMI8658_ACC_Y_H) << 8) | this->qmi8658_read(QMI8658_ACC_Y_L));
    int16_t acc_z = (((int16_t)this->qmi8658_read(QMI8658_ACC_Z_H) << 8) | this->qmi8658_read(QMI8658_ACC_Z_L));
    data->acc_xyz.x = (float)acc_x/qmi_ctx.acc_sensitivity;
    data->acc_xyz.y = (float)acc_y/qmi_ctx.acc_sensitivity;
    data->acc_xyz.z = (float)acc_z/qmi_ctx.acc_sensitivity;

    // read gyroscope data
    int16_t rot_x = (((int16_t)this->qmi8658_read(QMI8658_GYR_X_H) << 8) | this->qmi8658_read(QMI8658_GYR_X_L));
    int16_t rot_y = (((int16_t)this->qmi8658_read(QMI8658_GYR_Y_H) << 8) | this->qmi8658_read(QMI8658_GYR_Y_L));
    int16_t rot_z = (((int16_t)this->qmi8658_read(QMI8658_GYR_Z_H) << 8) | this->qmi8658_read(QMI8658_GYR_Z_L));
    data->gyro_xyz.x = (float)rot_x/qmi_ctx.gyro_sensitivity;
    data->gyro_xyz.y = (float)rot_y/qmi_ctx.gyro_sensitivity;
    data->gyro_xyz.z = (float)rot_z/qmi_ctx.gyro_sensitivity;

    // read temperature data
    int16_t temp = (((int16_t)this->qmi8658_read(QMI8658_TEMP_H) << 8) | this->qmi8658_read(QMI8658_TEMP_L));
    data->temperature = (float)temp/256;
}

// Close communication with the QMI8658 sensor.
// Return a status code indicating success or failure of the operation.

qmi8658_result_t Qmi8658c::close() {
  uint8_t qmi8658_ctrl7_reg, qmi8658_ctrl1_reg;
  qmi8658_result_t ret;
  
  qmi8658_ctrl7_reg = this->qmi8658_read(QMI8658_CTRL7);
  
  // disable accelerometer, gyroscope, magnetometer and attitude engine
   qmi8658_ctrl7_reg &= 0xF0; 
  this->qmi8658_write(QMI8658_CTRL7,qmi8658_ctrl7_reg);
  
  // disable sensor by turning off the internal 2 MHz oscillator 
   qmi8658_ctrl1_reg = this->qmi8658_read(QMI8658_CTRL1);
   qmi8658_ctrl1_reg |= (1 << 0); 
   this->qmi8658_write(QMI8658_CTRL1,qmi8658_ctrl1_reg);

   // read these two registers
   qmi8658_ctrl7_reg = this->qmi8658_read(QMI8658_CTRL7);
   qmi8658_ctrl1_reg = this->qmi8658_read(QMI8658_CTRL1);

   ret = (!(qmi8658_ctrl7_reg & 0x0F) && (qmi8658_ctrl1_reg & 0x01)) ? qmi8658_result_close_success : qmi8658_result_close_error;

   return ret;
}

// Converts a qmi8658_result_t enum value into a corresponding string.
char* Qmi8658c::resultToString(qmi8658_result_t result) {
  switch(result){
    case qmi8658_result_open_success :
      return "open-success";
    case qmi8658_result_open_error :
      return "open-error";
    case qmi8658_result_close_success :
      return "close-success";
    case qmi8658_result_close_error:
      return "close-error";
  }
  return "unknown-error"; // make the compiler happy!
}
