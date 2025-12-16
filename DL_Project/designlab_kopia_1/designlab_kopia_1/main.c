/*
 * main.c
 *
 * Created: 12/2/2025 12:01:43 PM
 *  Author: sebastian
 */ 

#include "main.h"
#include "diodes.h"
#include "display.h"
#include "uart.h"
#include "sensors.h"

int main(void) {
	int16_t bmp_temperature;
	uint16_t bmp_pressure;
	int16_t ds18b20_temperature;
	volatile display_mode_t display_mode = DISPLAY_BMP_TEMPERATURE;
	uint32_t counter = 0;
	
	sei();
	
	DDRC |= (1 << DDC0) | (1 << DDC1);
	 PORTC &= ~(1 << PORTC0);
	
	
	uart_init();
	

	
	uart_println("ATmega328PB UART Test");
	uart_println("System uruchomiony!");

	

	sei();
	
	// Initialize I2C
	I2C_Init();
	
	// Initialize button
	Button_Init();
	
	// Initialize 7-segment display
	SevenSeg_Init();
	
	// Initialize BMP280
	BMP280_Init();
	
	sei();
	
	while (1) {
		diode_pb0_on();
		_delay_ms(5);
		diode_pb0_off();		
		_delay_ms(5);
		// Echo - odbierz i odeœlij
		if (uart_available()) {
			//diode_pb0_on();
			uint8_t data = uart_read();
			
			// Poka¿ co przysz³o
			uart_print("RX[");
			uart_print_hex(data);
			uart_print("]: ");
			uart_transmit(data);
			
			// Jeœli Enter, nowa linia
			if (data == '\r' || data == '\n') {
				uart_println(" <ENTER>");
				} else {
				uart_println("");
			}
			
			_delay_ms(100);  
			
		}
		
		// Co 2 sekundy licznik
		if (counter % 2 == 0) {
			
			uart_print("--- Alive: ");
			uart_print_int(counter / 2);
			uart_print(" Temperature: ");
			bmp_temperature = BMP280_ReadTemperature();
			uart_print_int(bmp_temperature);
			uart_print(" Pressure: ");
			bmp_pressure = BMP280_ReadPressure();
			uart_print_int(bmp_pressure);
			uart_print(" DS Temperature: ");
			ds18b20_temperature = DS18B20_ReadTemperature();
			uart_print_int(ds18b20_temperature);
			uart_println(" ---");
			_delay_ms(50);  
			
			
		}
		
		_delay_ms(1000);
		counter++;
	
		// Check if button is pressed
		if (Button_IsPressed()) {
			// Cycle through display modes
			display_mode = (display_mode + 1) % DISPLAY_MODE_COUNT;
			_delay_ms(200); // Additional debounce
		}
		
		// Read and display sensor data based on current mode
		switch (display_mode) {
			case DISPLAY_BMP_TEMPERATURE:
			bmp_temperature = BMP280_ReadTemperature();
			SevenSeg_Show(bmp_temperature); 
			break;
			
			case DISPLAY_BMP_PRESSURE:
			bmp_pressure = BMP280_ReadPressure();
			SevenSeg_Show(bmp_pressure); 
			break;
			
			case DISPLAY_DS18B20_TEMPERATURE:
			ds18b20_temperature = DS18B20_ReadTemperature();
			if (ds18b20_temperature == -9999) {
				
				SevenSeg_Show(8888);
				} else {
				SevenSeg_Show(ds18b20_temperature); 
			}
			break;
			
			default:
			display_mode = DISPLAY_BMP_TEMPERATURE;
			break;
		}
		
		_delay_ms(500);
	}
	
	return 0;
}
