#include "LSM6DS3.h"

LSM6DS3::LSM6DS3(i2c_inst_t *i2c_port, uint8_t addr) : _i2c(i2c_port), _addr(addr) {}

bool LSM6DS3::begin() {
    // Check WHO_AM_I
    uint8_t who = whoAmI();
    if (who != 0x69) {
        return false;
    }

    // Configure accelerometer: ODR=104Hz, FS=2g, enable
    // CTRL1_XL: ODR=0100 (104Hz), FS=00 (2g), LPF2_XL_EN=0
    uint8_t ctrl1_xl = 0x40; // 01000000
    return writeRegister(LSM6DS3_CTRL1_XL, ctrl1_xl);
}

bool LSM6DS3::readAccel(float &x, float &y, float &z) {
    uint8_t buffer[6];
    if (!readRegisters(LSM6DS3_OUTX_L_XL, buffer, 6)) {
        return false;
    }

    int16_t rawX = (int16_t)((buffer[1] << 8) | buffer[0]);
    int16_t rawY = (int16_t)((buffer[3] << 8) | buffer[2]);
    int16_t rawZ = (int16_t)((buffer[5] << 8) | buffer[4]);

    // Convert to g (FS=2g, so range -2 to +2g)
    const float scale = 2.0f / 32768.0f;
    x = rawX * scale;
    y = rawY * scale;
    z = rawZ * scale;

    return true;
}

uint8_t LSM6DS3::whoAmI() {
    uint8_t value;
    if (readRegister(LSM6DS3_WHO_AM_I, &value)) {
        return value;
    }
    return 0;
}

bool LSM6DS3::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    int ret = i2c_write_blocking(_i2c, _addr, buffer, 2, false);
    return ret == 2;
}

bool LSM6DS3::readRegister(uint8_t reg, uint8_t *value) {
    int ret = i2c_write_blocking(_i2c, _addr, &reg, 1, true);
    if (ret != 1) return false;
    ret = i2c_read_blocking(_i2c, _addr, value, 1, false);
    return ret == 1;
}

bool LSM6DS3::readRegisters(uint8_t reg, uint8_t *buffer, size_t len) {
    int ret = i2c_write_blocking(_i2c, _addr, &reg, 1, true);
    if (ret != 1) return false;
    ret = i2c_read_blocking(_i2c, _addr, buffer, len, false);
    return ret == (int)len;
}