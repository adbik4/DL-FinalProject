/*
 * diodes.c
 *
 * Created: 04/11/2025 15:08:20
 *  Author: Adrian
 */ 

#include "main.h"
#include "diodes.h"

// Funkcja w?acza diode pb0
void diode_pb0_on(void)
{
	PORTC &= ~(1 << PORTC0);
}

// Funkcja wylacza diode pb0
void diode_pb0_off(void)
{
	PORTC |= (1 << PORTC0);
}

// Wlaczamy pb1
void diode_pb1_on(void)
{
	PORTB &= ~(1 << PORTB1);
}

// Wy?aczamy pb1
void diode_pb1_off(void)
{
	PORTB |= (1 << PORTB1);
}