#include <main.h>
#include <display.h>

#define SEG_PORT_CTRL   PORTE
#define SEG_DDR_CTRL    DDRE
#define SEG_DIGIT1      PE0     // First digit control pin
#define SEG_DIGIT2      PE1     // Second digit control pin
#define SEG_DIGIT3      PE2     // Third digit control pin
#define SEG_DIGIT4      PE3     // Fourth digit control pin

// 7-Segment display segment pins
// Segments are connected to PD2-PD7, PB0-PB1
#define SEG_DDR_DATA1   DDRD    // For segments on Port D
#define SEG_PORT_DATA1  PORTD   // For segments on Port D
#define SEG_DDR_DATA2   DDRB    // For segments on Port B
#define SEG_PORT_DATA2  PORTB   // For segments on Port B

volatile uint8_t seg_digit = 0;    // Current digit being displayed (0-3)
uint8_t seg_values[4] = {0, 0, 0, 0}; // Values to display on each digit


// Bits mapped as: F E D C B G DP A
const uint8_t seven_seg_digits[] = {
	0b00000110,  // 0: A B C D E F
	0b11100111,  // 1: B C
	0b10010010,  // 2: A B D E G
	0b11000010,  // 3: A B C D G
	0b01100011,  // 4: B C F G
	0b01001010,  // 5: A C D F G
	0b00001010,  // 6: A C D E F G
	0b11100110,  // 7: A B C
	0b00000010,  // 8: A B C D E F G
	0b01000010,   // 9: A B C D F G
	
};

void SevenSeg_Init(void) {
	// Configure digit control pins as outputs (active LOW, initially high)
	SEG_DDR_CTRL |= (1 << SEG_DIGIT1) | (1 << SEG_DIGIT2) | (1 << SEG_DIGIT3) | (1 << SEG_DIGIT4);
	SEG_PORT_CTRL |= (1 << SEG_DIGIT1) | (1 << SEG_DIGIT2) | (1 << SEG_DIGIT3) | (1 << SEG_DIGIT4);
	
	// Configure segment pins as outputs (active LOW, initially high)
	// PD2-PD7 for segments
	SEG_DDR_DATA1 |= 0xFC;  // 0b11111100 = bits 2-7
	SEG_PORT_DATA1 |= 0xFC; // All segments off initially
	
	// PB0-PB1 for remaining segments
	SEG_DDR_DATA2 |= 0x03;  // 0b00000011 = bits 0-1
	SEG_PORT_DATA2 |= 0x03; // All segments off initially
	
	
	// Set up Timer0 for display multiplexing
	TCCR0A = (1 << WGM01);              // CTC mode
	TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler 64
	OCR0A = 249;                        // For 1ms interrupt at 16MHz: 16MHz/64/250 = 1kHz
	TIMSK0 = (1 << OCIE0A);             // Enable compare match interrupt
}

// Update a specific digit of the 7-segment display
void SevenSeg_Update(void) {
	// Turn off all digits
	SEG_PORT_CTRL |= (1 << SEG_DIGIT1) | (1 << SEG_DIGIT2) | (1 << SEG_DIGIT3) | (1 << SEG_DIGIT4);
	
	// Get pattern for current digit value (0-15)
	uint8_t pattern = 0xFF;  // All segments off by default
	if (seg_values[seg_digit] < 16) {
		pattern = seven_seg_digits[seg_values[seg_digit]];
	}
	
	// Output segment pattern to Port D (PD2-PD7)
	SEG_PORT_DATA1 &= 0x03;              // Clear segment bits
	SEG_PORT_DATA1 |= (pattern & 0xFC);  // Set new segment bits (bits 2-7)
	
	// Output segment pattern to Port B (PB0-PB1)
	SEG_PORT_DATA2 &= 0xFC;              // Clear segment bits
	SEG_PORT_DATA2 |= (pattern & 0x03);  // Set new segment bits (bits 0-1)
	
	// Select the current digit (active LOW)
	switch (seg_digit) {
		case 0:
		SEG_PORT_CTRL &= ~(1 << SEG_DIGIT1);
		break;
		case 1:
		SEG_PORT_CTRL &= ~(1 << SEG_DIGIT2);
		break;
		case 2:
		SEG_PORT_CTRL &= ~(1 << SEG_DIGIT3);
		break;
		case 3:
		SEG_PORT_CTRL &= ~(1 << SEG_DIGIT4);
		break;
	}
	
	// Move to next digit
	seg_digit = (seg_digit + 1) % 4;
}

// Timer0 Compare A interrupt for 7-segment display multiplexing
ISR(TIMER0_COMPA_vect) {
	SevenSeg_Update();
}

void SevenSeg_Show(int num)
{
	 // Ograniczenie do zakresu 0–9999
	 if (num < 0) num = 0;
	 if (num > 9999) num = 9999;
	 
	 // Rozbicie liczby na poszczególne cyfry (jednostki, dziesi¹tki, setki, tysi¹ce)
	 seg_values[3] = num % 10;          // Jednoœci
	 seg_values[2] = (int)(num / 10.0) % 10;   // Dziesi¹tki
	 seg_values[1] = (int)(num / 100.0) % 10;  // Setki
	 seg_values[0] = (int)(num / 1000.0) % 10; // Tysi¹ce
	
}