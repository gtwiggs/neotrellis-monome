#include "LIS2MDL.h"

LIS2MDL::LIS2MDL(i2c_inst_t *i2c_port, uint8_t addr) : _i2c(i2c_port), _addr(addr) {}

bool LIS2MDL::begin() {
    // Check WHO_AM_I
    uint8_t who = whoAmI();
    if (who != 0x40) {
        return false;
    }

    // Configure magnetometer
    // CFG_REG_A: ODR=100 (100Hz), MD=00 (continuous mode)
    uint8_t cfg_a = 0x80; // 10000000
    if (!writeRegister(LIS2MDL_CFG_REG_A, cfg_a)) return false;

    // CFG_REG_B: OFF_CANC=1 (offset cancellation), LPF=1 (low pass filter)
    uint8_t cfg_b = 0x03; // 00000011
    if (!writeRegister(LIS2MDL_CFG_REG_B, cfg_b)) return false;

    // CFG_REG_C: BDU=1 (block data update), BLE=0 (LSB first), I2C_DIS=0, INT_MAG=0, ST=0
    uint8_t cfg_c = 0x10; // 00010000
    return writeRegister(LIS2MDL_CFG_REG_C, cfg_c);
}

bool LIS2MDL::readMag(float &x, float &y, float &z) {
    uint8_t buffer[6];
    if (!readRegisters(LIS2MDL_OUTX_L_REG, buffer, 6)) {
        return false;
    }

    int16_t rawX = (int16_t)((buffer[1] << 8) | buffer[0]);
    int16_t rawY = (int16_t)((buffer[3] << 8) | buffer[2]);
    int16_t rawZ = (int16_t)((buffer[5] << 8) | buffer[4]);

    // Convert to gauss (sensitivity is 1.5 mG/LSB, so 1500 uT/LSB)
    // Convert to uT (microtesla)
    const float scale = 1.5f; // mG per LSB
    x = rawX * scale * 0.1f; // Convert mG to uT
    y = rawY * scale * 0.1f;
    z = rawZ * scale * 0.1f;

    return true;
}

uint8_t LIS2MDL::whoAmI() {
    uint8_t value;
    if (readRegister(LIS2MDL_WHO_AM_I, &value)) {
        return value;
    }
    return 0;
}

bool LIS2MDL::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    int ret = i2c_write_blocking(_i2c, _addr, buffer, 2, false);
    return ret == 2;
}

bool LIS2MDL::readRegister(uint8_t reg, uint8_t *value) {
    int ret = i2c_write_blocking(_i2c, _addr, &reg, 1, true);
    if (ret != 1) return false;
    ret = i2c_read_blocking(_i2c, _addr, value, 1, false);
    return ret == 1;
}

bool LIS2MDL::readRegisters(uint8_t reg, uint8_t *buffer, size_t len) {
    int ret = i2c_write_blocking(_i2c, _addr, &reg, 1, true);
    if (ret != 1) return false;
    ret = i2c_read_blocking(_i2c, _addr, buffer, len, false);
    return ret == (int)len;
}