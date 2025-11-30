#define F_CPU 16000000UL  // ustawiamy predkosc zegara

#include <main.h>
#include <diodes.h>
#include <display.h>
#include <uart.h>
#include <sensors.h>
//#include <spi.h>
//#include <sdcard.h>


int main(void)
{
	int16_t bmp_temperature;
	uint16_t bmp_pressure;
	int16_t ds18b20_temperature;
	
	// Initialize I2C
	I2C_Init();
	
	// Initialize button
	Button_Init();
	
	// Initialize 7-segment display
	SevenSeg_Init();
	
	// Initialize BMP280
	BMP280_Init();
	
	// Enable global interrupts
	sei();
    uart_init();
	
	uart_println("ATmega328PB UART Test");
	uart_println("System uruchomiony!");
	
    // Ustawiamy pb0 i pb1 jako wyjscia
    DDRB |= (1 << DDB0) | (1 << DDB1);
    
    // Na poczatku diody wylaczamy
    diode_pb0_off();
    diode_pb1_off();
	
SevenSeg_Init();   // Inicjalizacja wy?wietlacza
sei();             // W??czenie przerwa?


while (1)
{
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
			SevenSeg_Show(bmp_temperature); // Display as XXYY (XX.YY °C)
			break;
			
			case DISPLAY_BMP_PRESSURE:
			bmp_pressure = BMP280_ReadPressure();
			SevenSeg_Show(bmp_pressure); // Display as hPa (e.g., 1013 hPa)
			break;
			
			case DISPLAY_DS18B20_TEMPERATURE:
			ds18b20_temperature = DS18B20_ReadTemperature();
			if (ds18b20_temperature == -9999) {
				// Error reading DS18B20, show error code
				SevenSeg_Show(8888);
				} else {
				SevenSeg_Show(ds18b20_temperature); // Display as XXYY (XX.YY °C)
			}
			break;
			
			default:
			display_mode = DISPLAY_BMP_TEMPERATURE;
			break;
		}
		
		// Wait before next reading
		_delay_ms(500);
    /*
	for(int i=0; i < 9999; i++) {
		SevenSeg_Show(i);
		_delay_ms(200);
        */
	}



}


	 
    
    // Przykladowe uzycie
    while(1)
    {
        // Wlaczamy pb0 i czekamy chwile
        diode_pb0_on();
        _delay_ms(500);
        
        // Wylaczamy pb0, wlaczamy pb1 
        diode_pb0_off();
        diode_pb1_on();
        _delay_ms(500);
        
        // Wlaczamy obydwie dwiody
        diode_pb0_on();
        diode_pb1_on();
        _delay_ms(500);
        
        // Wylaczamy obydwie diody
        diode_pb0_off();
        diode_pb1_off();
        _delay_ms(500);
    }
	
	
	 while (1) {
		 if (uart_available()) {
			 uint8_t data = uart_read();
			 uart_transmit(data); // Echo odebranych danych
		 }
		 
		 _delay_ms(1000);
		 uart_print("Tick: ");
		 uart_print_int(counter++);
		 uart_println("");
	 }

	 
    return 0;
}


