#ifndef LSM6DS3_H
#define LSM6DS3_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// LSM6DS3TR-C I2C address (default 0x6A, or 0x6B if SA0 high)
#define LSM6DS3_ADDR 0x6A

// Registers
#define LSM6DS3_WHO_AM_I 0x0F
#define LSM6DS3_CTRL1_XL 0x10
#define LSM6DS3_CTRL2_G 0x11
#define LSM6DS3_OUTX_L_XL 0x28
#define LSM6DS3_OUTX_H_XL 0x29
#define LSM6DS3_OUTY_L_XL 0x2A
#define LSM6DS3_OUTY_H_XL 0x2B
#define LSM6DS3_OUTZ_L_XL 0x2C
#define LSM6DS3_OUTZ_H_XL 0x2D
#define LSM6DS3_OUTX_L_G 0x22
#define LSM6DS3_OUTX_H_G 0x23
#define LSM6DS3_OUTY_L_G 0x24
#define LSM6DS3_OUTY_H_G 0x25
#define LSM6DS3_OUTZ_L_G 0x26
#define LSM6DS3_OUTZ_H_G 0x27

class LSM6DS3 {
public:
    LSM6DS3(i2c_inst_t *i2c_port, uint8_t addr = LSM6DS3_ADDR);
    bool begin();
    bool readAccel(float &x, float &y, float &z);
    bool readGyro(float &x, float &y, float &z);
    void calculateOrientation(float ax, float ay, float az, float &roll, float &pitch);
    float calculateYaw(float ax, float ay, float az, float mx, float my, float mz);
    uint8_t whoAmI();

private:
    i2c_inst_t *_i2c;
    uint8_t _addr;
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t *value);
    bool readRegisters(uint8_t reg, uint8_t *buffer, size_t len);
};

#endif