#define F_CPU 16000000UL  // ustawiamy predkosc zegara

#include <main.h>
#include <diodes.h>
#include <display.h>
#include <uart.h>
//#include <spi.h>
//#include <sdcard.h>


int main(void)
{
	uart_init();
	
	uart_println("ATmega328PB UART Test");
	uart_println("System uruchomiony!");
	
    // Ustawiamy pb0 i pb1 jako wyjscia
    DDRB |= (1 << DDB0) | (1 << DDB1);
    
    // Na poczatku diody wylaczamy
    diode_pb0_off();
    diode_pb1_off();
	
SevenSeg_Init();   // Inicjalizacja wyœwietlacza
sei();             // W³¹czenie przerwañ


while (1)
{
	for(int i=0; i < 9999; i++) {
		SevenSeg_Show(i);
		_delay_ms(200);
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
