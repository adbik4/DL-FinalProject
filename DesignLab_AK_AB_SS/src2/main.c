#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <uart.h>
#include <diodes.h>
#include <main.h> 


int main(void)
{
	uart_init();
	_delay_ms(100);
	
	uart_println("=== UART ECHO TEST ===");
	uart_println("Wpisz cos i wcisnij ENTER");
	
	uint32_t counter = 0;
	
	while (1)
	{
		// Echo - odbierz i odeœlij
		if (uart_available()) {
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
		}
		
		// Co 2 sekundy licznik
		if (counter % 2 == 0) {
			uart_print("--- Alive: ");
			uart_print_int(counter / 2);
			uart_println(" ---");
		}
		
		_delay_ms(1000);
		counter++;
	}
	
	return 0;
}
