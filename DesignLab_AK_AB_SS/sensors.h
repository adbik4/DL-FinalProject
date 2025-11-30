#include <main.h>
#define SCL_CLOCK 100000L  // 100kHz I2C clock

// BMP280 I2C address (SDO pin to GND = 0x76, SDO to VCC = 0x77)
#define BMP280_ADDR 0x76

// BMP280 Register addresses
#define BMP280_REG_ID           0xD0
#define BMP280_REG_RESET        0xE0
#define BMP280_REG_STATUS       0xF3
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_PRESS_MSB    0xF7
#define BMP280_REG_TEMP_MSB     0xFA

// Calibration data registers - Temperature
#define BMP280_REG_DIG_T1_LSB   0x88
#define BMP280_REG_DIG_T1_MSB   0x89
#define BMP280_REG_DIG_T2_LSB   0x8A
#define BMP280_REG_DIG_T2_MSB   0x8B
#define BMP280_REG_DIG_T3_LSB   0x8C
#define BMP280_REG_DIG_T3_MSB   0x8D

// Calibration data registers - Pressure
#define BMP280_REG_DIG_P1_LSB   0x8E
#define BMP280_REG_DIG_P1_MSB   0x8F
#define BMP280_REG_DIG_P2_LSB   0x90
#define BMP280_REG_DIG_P2_MSB   0x91
#define BMP280_REG_DIG_P3_LSB   0x92
#define BMP280_REG_DIG_P3_MSB   0x93
#define BMP280_REG_DIG_P4_LSB   0x94
#define BMP280_REG_DIG_P4_MSB   0x95
#define BMP280_REG_DIG_P5_LSB   0x96
#define BMP280_REG_DIG_P5_MSB   0x97
#define BMP280_REG_DIG_P6_LSB   0x98
#define BMP280_REG_DIG_P6_MSB   0x99
#define BMP280_REG_DIG_P7_LSB   0x9A
#define BMP280_REG_DIG_P7_MSB   0x9B
#define BMP280_REG_DIG_P8_LSB   0x9C
#define BMP280_REG_DIG_P8_MSB   0x9D
#define BMP280_REG_DIG_P9_LSB   0x9E
#define BMP280_REG_DIG_P9_MSB   0x9F

// DS18B20 Configuration - Data pin on PC3
#define DS18B20_PORT    PORTC
#define DS18B20_DDR     DDRC
#define DS18B20_PIN     PINC
#define DS18B20_DQ      PC3

// DS18B20 Commands
#define DS18B20_CMD_CONVERTTEMP     0x44
#define DS18B20_CMD_RSCRATCHPAD     0xBE
#define DS18B20_CMD_WSCRATCHPAD     0x4E
#define DS18B20_CMD_CPYSCRATCHPAD   0x48
#define DS18B20_CMD_RECEEPROM       0xB8
#define DS18B20_CMD_RPWRSUPPLY      0xB4
#define DS18B20_CMD_SEARCHROM       0xF0
#define DS18B20_CMD_READROM         0x33
#define DS18B20_CMD_MATCHROM        0x55
#define DS18B20_CMD_SKIPROM         0xCC
#define DS18B20_CMD_ALARMSEARCH     0xEC

// Button configuration - SW0 on ATmega328PB Xplained Mini
#define BUTTON_PORT     PORTC
#define BUTTON_PIN      PINC
#define BUTTON_DDR      DDRC
#define BUTTON_BIT      PC2     // SW0 is connected to PB7