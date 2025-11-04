#define F_CPU 16000000UL  // ustawiamy predkosc zegara

#include <main.h>
#include <diodes.h>
#include <display.h>

int main(void)
{
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
	

	 
    
    return 0;
}
