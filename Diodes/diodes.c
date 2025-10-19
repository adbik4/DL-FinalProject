#define F_CPU 16000000UL  // ustawiamy predkosc zegara

#include <avr/io.h>
#include <util/delay.h>

// Funkcja włacza diode pb0 
void diode_pb0_on(void)
{
    PORTB &= ~(1 << PORTB0);  

// Funkcja wylacza diode pb0
void diode_pb0_off(void)
{
    PORTB |= (1 << PORTB0);   
}

// Wlaczamy pb1
void diode_pb1_on(void)
{
    PORTB &= ~(1 << PORTB1);  
}

// Wyłaczamy pb1
void diode_pb1_off(void)
{
    PORTB |= (1 << PORTB1);   
}

int main(void)
{
    // Ustawiamy pb0 i pb1 jako wyjscia
    DDRB |= (1 << DDB0) | (1 << DDB1);
    
    // Na poczatku diody wylaczamy
    diode_pb0_off();
    diode_pb1_off();
    
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
