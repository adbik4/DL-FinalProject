#ifndef SENSORS_H_
#define SENSORS_H_

#include <avr/io.h>

#define SCL_CLOCK 100000L

#define BMP280_ADDR 0x76

#define BMP280_REG_ID           0xD0
#define BMP280_REG_RESET        0xE0
#define BMP280_REG_STATUS       0xF3
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_PRESS_MSB    0xF7
#define BMP280_REG_TEMP_MSB     0xFA

#define BMP280_REG_DIG_T1_LSB   0x88
#define BMP280_REG_DIG_T1_MSB   0x89
#define BMP280_REG_DIG_T2_LSB   0x8A
#define BMP280_REG_DIG_T2_MSB   0x8B
#define BMP280_REG_DIG_T3_LSB   0x8C
#define BMP280_REG_DIG_T3_MSB   0x8D

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

#define DS18B20_PORT    PORTC
#define DS18B20_DDR     DDRC
#define DS18B20_PIN     PINC
#define DS18B20_DQ      PC3

#define DS18B20_CMD_CONVERTTEMP     0x44
#define DS18B20_CMD_RSCRATCHPAD     0xBE
#define DS18B20_CMD_SKIPROM         0xCC

#define BUTTON_PORT     PORTC
#define BUTTON_PIN      PINC
#define BUTTON_DDR      DDRC
#define BUTTON_BIT      PC2

typedef enum {
	DISPLAY_BMP_TEMPERATURE,
	DISPLAY_BMP_PRESSURE,
	DISPLAY_DS18B20_TEMPERATURE,
	DISPLAY_MODE_COUNT
} display_mode_t;




// DS18B20
uint8_t DS18B20_Reset(void);
void DS18B20_WriteByte(uint8_t byte);
uint8_t DS18B20_ReadByte(void);
int16_t DS18B20_ReadTemperature(void);

// I2C
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
uint8_t I2C_Read_ACK(void);
uint8_t I2C_Read_NACK(void);

// BMP280
void BMP280_Init(void);
uint8_t BMP280_ReadReg(uint8_t reg);
void BMP280_WriteReg(uint8_t reg, uint8_t value);
void BMP280_ReadCalibration(void);
int16_t BMP280_ReadTemperature(void);
uint16_t BMP280_ReadPressure(void);

// Button
void Button_Init(void);
uint8_t Button_IsPressed(void);

#endif /* SENSORS_H_ */
