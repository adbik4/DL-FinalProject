#include "sensors.h"
#include "main.h"


volatile display_mode_t display_mode = DISPLAY_BMP_TEMPERATURE;

// Calibration parameters for BMP280
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
int32_t t_fine;

// ============= DS18B20 Functions =============

void DS18B20_SetPinOutput(void) {
	DS18B20_DDR |= (1 << DS18B20_DQ);
}

void DS18B20_SetPinInput(void) {
	DS18B20_DDR &= ~(1 << DS18B20_DQ);
}

void DS18B20_SetPinLow(void) {
	DS18B20_PORT &= ~(1 << DS18B20_DQ);
}

void DS18B20_SetPinHigh(void) {
	DS18B20_PORT |= (1 << DS18B20_DQ);
}

uint8_t DS18B20_ReadPin(void) {
	return (DS18B20_PIN & (1 << DS18B20_DQ)) ? 1 : 0;
}

uint8_t DS18B20_Reset(void) {
	uint8_t response;
	
	DS18B20_SetPinOutput();
	DS18B20_SetPinLow();
	_delay_us(480);
	
	DS18B20_SetPinInput();
	DS18B20_SetPinHigh();
	_delay_us(70);
	
	response = DS18B20_ReadPin();
	_delay_us(410);
	
	return response;
}

void DS18B20_WriteBit(uint8_t bit) {
	DS18B20_SetPinOutput();
	DS18B20_SetPinLow();
	_delay_us(1);
	
	if (bit) {
		DS18B20_SetPinHigh();
	}
	
	_delay_us(60);
	DS18B20_SetPinHigh();
	_delay_us(1);
}

uint8_t DS18B20_ReadBit(void) {
	uint8_t bit = 0;
	
	DS18B20_SetPinOutput();
	DS18B20_SetPinLow();
	_delay_us(1);
	
	DS18B20_SetPinInput();
	DS18B20_SetPinHigh();
	_delay_us(14);
	
	if (DS18B20_ReadPin()) {
		bit = 1;
	}
	
	_delay_us(45);
	
	return bit;
}

void DS18B20_WriteByte(uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		DS18B20_WriteBit(byte & 0x01);
		byte >>= 1;
	}
}

uint8_t DS18B20_ReadByte(void) {
	uint8_t byte = 0;
	
	for (uint8_t i = 0; i < 8; i++) {
		byte >>= 1;
		if (DS18B20_ReadBit()) {
			byte |= 0x80;
		}
	}
	
	return byte;
}

int16_t DS18B20_ReadTemperature(void) {
	uint8_t temp_lsb, temp_msb;
	int16_t temp;
	
	// Reset and check presence
	if (DS18B20_Reset()) {
		return -9999; 
	}
	
	// Skip ROM 
	DS18B20_WriteByte(DS18B20_CMD_SKIPROM);
	
	// Start temperature conversion
	DS18B20_WriteByte(DS18B20_CMD_CONVERTTEMP);
	
	// Wait for conversion (750ms for 12-bit resolution)
	_delay_ms(750);
	
	// Reset again
	if (DS18B20_Reset()) {
		return -9999; // Error
	}
	
	// Skip ROM
	DS18B20_WriteByte(DS18B20_CMD_SKIPROM);
	
	// Read scratchpad
	DS18B20_WriteByte(DS18B20_CMD_RSCRATCHPAD);
	
	// Read temperature bytes
	temp_lsb = DS18B20_ReadByte();
	temp_msb = DS18B20_ReadByte();
	
	// Combine bytes
	temp = (temp_msb << 8) | temp_lsb;
	
	// Convert to 0.1°C resolution

	return (temp * 10) / 16;
}

// ============= I2C Functions =============

void I2C_Init(void) {
	// Set I2C bit rate
	TWSR0 = 0x00; // Prescaler = 1
	TWBR0 = ((F_CPU / SCL_CLOCK) - 16) / 2;
	
	// Enable TWI
	TWCR0 = (1 << TWEN);
}

void I2C_Start(void) {
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
}

void I2C_Stop(void) {
	TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	_delay_us(100);
}

void I2C_Write(uint8_t data) {
	TWDR0 = data;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
}

uint8_t I2C_Read_ACK(void) {
	TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR0 & (1 << TWINT)));
	return TWDR0;
}

uint8_t I2C_Read_NACK(void) {
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
	return TWDR0;
}

// ============= BMP280 Functions =============

void BMP280_WriteReg(uint8_t reg, uint8_t value) {
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 0); // Write mode
	I2C_Write(reg);
	I2C_Write(value);
	I2C_Stop();
}

uint8_t BMP280_ReadReg(uint8_t reg) {
	uint8_t value;
	
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 0); // Write mode
	I2C_Write(reg);
	I2C_Start(); // Repeated start
	I2C_Write((BMP280_ADDR << 1) | 1); // Read mode
	value = I2C_Read_NACK();
	I2C_Stop();
	
	return value;
}

void BMP280_ReadCalibration(void) {
	uint8_t calib[24];
	
	// Read all calibration data
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 0);
	I2C_Write(BMP280_REG_DIG_T1_LSB);
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 1);
	
	for (uint8_t i = 0; i < 23; i++) {
		calib[i] = I2C_Read_ACK();
	}
	calib[23] = I2C_Read_NACK();
	I2C_Stop();
	
	// Temperature calibration
	dig_T1 = (calib[1] << 8) | calib[0];
	dig_T2 = (calib[3] << 8) | calib[2];
	dig_T3 = (calib[5] << 8) | calib[4];
	
	// Pressure calibration
	dig_P1 = (calib[7] << 8) | calib[6];
	dig_P2 = (calib[9] << 8) | calib[8];
	dig_P3 = (calib[11] << 8) | calib[10];
	dig_P4 = (calib[13] << 8) | calib[12];
	dig_P5 = (calib[15] << 8) | calib[14];
	dig_P6 = (calib[17] << 8) | calib[16];
	dig_P7 = (calib[19] << 8) | calib[18];
	dig_P8 = (calib[21] << 8) | calib[20];
	dig_P9 = (calib[23] << 8) | calib[22];
}

void BMP280_Init(void) {
	_delay_ms(100); 
	
	uint8_t id = BMP280_ReadReg(BMP280_REG_ID);
	if (id != 0x58) {
		
		return;
	}
	
	
	BMP280_WriteReg(BMP280_REG_RESET, 0xB6);
	_delay_ms(100);
	
	// Read calibration data
	BMP280_ReadCalibration();
	
	
	BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27);
	
	
	BMP280_WriteReg(BMP280_REG_CONFIG, 0xA0);
	
	_delay_ms(100);
}

int32_t BMP280_CompensateTemp(int32_t adc_T) {
	int32_t var1, var2, T;
	
	var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	
	return T; // Temperature in 0.01 degrees Celsius
}

uint32_t BMP280_CompensatePressure(int32_t adc_P) {
	int64_t var1, var2, p;
	
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
	var2 = var2 + (((int64_t)dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
	
	if (var1 == 0) {
		return 0; 
	}
	
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
	
	return (uint32_t)p; 
}

int16_t BMP280_ReadTemperature(void) {
	uint8_t temp_data[3];
	int32_t adc_T;
	
	// Read temperature registers
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 0);
	I2C_Write(BMP280_REG_TEMP_MSB);
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 1);
	
	temp_data[0] = I2C_Read_ACK(); // MSB
	temp_data[1] = I2C_Read_ACK(); // LSB
	temp_data[2] = I2C_Read_NACK(); // XLSB
	I2C_Stop();
	
	// Combine into 20-bit value
	adc_T = ((int32_t)temp_data[0] << 12) | ((int32_t)temp_data[1] << 4) | ((int32_t)temp_data[2] >> 4);
	
	// Compensate temperature
	int32_t temp = BMP280_CompensateTemp(adc_T);
	
	return (int16_t)(temp / 10); // Convert to 0.1 degrees Celsius
}

uint16_t BMP280_ReadPressure(void) {
	uint8_t press_data[3];
	uint8_t temp_data[3];
	int32_t adc_P, adc_T;
	
	// Read temperature first (needed for pressure compensation)
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 0);
	I2C_Write(BMP280_REG_TEMP_MSB);
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 1);
	
	temp_data[0] = I2C_Read_ACK();
	temp_data[1] = I2C_Read_ACK();
	temp_data[2] = I2C_Read_NACK();
	I2C_Stop();
	
	adc_T = ((int32_t)temp_data[0] << 12) | ((int32_t)temp_data[1] << 4) | ((int32_t)temp_data[2] >> 4);
	BMP280_CompensateTemp(adc_T); 
	
	// Read pressure registers
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 0);
	I2C_Write(BMP280_REG_PRESS_MSB);
	I2C_Start();
	I2C_Write((BMP280_ADDR << 1) | 1);
	
	press_data[0] = I2C_Read_ACK(); // MSB
	press_data[1] = I2C_Read_ACK(); // LSB
	press_data[2] = I2C_Read_NACK(); // XLSB
	I2C_Stop();
	
	// Combine into 20-bit value
	adc_P = ((int32_t)press_data[0] << 12) | ((int32_t)press_data[1] << 4) | ((int32_t)press_data[2] >> 4);
	
	// Compensate pressure
	uint32_t pressure = BMP280_CompensatePressure(adc_P);
	

	return (uint16_t)((pressure / 256) / 100);
}

// ============= Button Functions =============

void Button_Init(void) {
	// Configure button pin as input with pull-up
	BUTTON_DDR &= ~(1 << BUTTON_BIT);
	BUTTON_PORT |= (1 << BUTTON_BIT);
}

uint8_t Button_IsPressed(void) {
	static uint8_t button_state = 0;
	
	// Read button
	uint8_t current = !(BUTTON_PIN & (1 << BUTTON_BIT));
	
	if (current && !button_state) {
		// Button just pressed
		_delay_ms(50); // Debounce delay
		if (!(BUTTON_PIN & (1 << BUTTON_BIT))) {
			button_state = 1;
			return 1;
		}
		} else if (!current && button_state) {
		// Button released
		button_state = 0;
	}
	
	return 0;
}


	