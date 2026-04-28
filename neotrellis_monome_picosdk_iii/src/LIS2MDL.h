#ifndef LIS2MDL_H
#define LIS2MDL_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// LIS2MDL I2C address (default 0x1E, or 0x1C if SA1 high)
#define LIS2MDL_ADDR 0x1E

// Registers
#define LIS2MDL_WHO_AM_I 0x4F
#define LIS2MDL_CFG_REG_A 0x60
#define LIS2MDL_CFG_REG_B 0x61
#define LIS2MDL_CFG_REG_C 0x62
#define LIS2MDL_OUTX_L_REG 0x68
#define LIS2MDL_OUTX_H_REG 0x69
#define LIS2MDL_OUTY_L_REG 0x6A
#define LIS2MDL_OUTY_H_REG 0x6B
#define LIS2MDL_OUTZ_L_REG 0x6C
#define LIS2MDL_OUTZ_H_REG 0x6D

class LIS2MDL {
public:
    LIS2MDL(i2c_inst_t *i2c_port, uint8_t addr = LIS2MDL_ADDR);
    bool begin();
    bool readMag(float &x, float &y, float &z);
    uint8_t whoAmI();

private:
    i2c_inst_t *_i2c;
    uint8_t _addr;
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t *value);
    bool readRegisters(uint8_t reg, uint8_t *buffer, size_t len);
};

#endif