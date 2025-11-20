/*
 * uart.c
 * 
 * UART driver implementation for ATmega328PB
 */
#include <avr/interrupt.h>
#include "uart.h"
#include <main.h>
#include <string.h>

// Bufory cykliczne dla transmisji i odbioru
static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

void uart_init(void) {
    // Ustaw prêdkoœæ transmisji
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    
    // W³¹cz odbiornik i nadajnik
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // W³¹cz przerwania odbioru
    UCSR0B |= (1 << RXCIE0);
    
    // Ustaw format ramki: 8 bitów danych, 1 bit stopu, bez parzystoœci
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // W³¹cz globalne przerwania
    sei();
}

void uart_transmit(uint8_t data) {
    // Czekaj a¿ bufor nadawczy bêdzie pusty
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Wyœlij dane
    UDR0 = data;
}

uint8_t uart_receive(void) {
    // Czekaj na dane
    while (!(UCSR0A & (1 << RXC0)));
    
    // Zwróæ odebrane dane
    return UDR0;
}

uint8_t uart_available(void) {
    // SprawdŸ czy s¹ dane w buforze
    return (rx_head != rx_tail);
}

void uart_print(const char* str) {
    while (*str) {
        uart_transmit(*str++);
    }
}

void uart_println(const char* str) {
    uart_print(str);
    uart_transmit('\r');
    uart_transmit('\n');
}

void uart_print_int(int32_t num) {
    char buffer[12]; // Wystarczy dla int32_t
    char* ptr = buffer + sizeof(buffer) - 1;
    uint8_t negative = 0;
    
    *ptr = '\0';
    
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        *--ptr = '0' + (num % 10);
        num /= 10;
    }
    
    if (negative) {
        *--ptr = '-';
    }
    
    uart_print(ptr);
}

void uart_print_hex(uint32_t num) {
    const char hex_chars[] = "0123456789ABCDEF";
    char buffer[9];
    char* ptr = buffer + sizeof(buffer) - 1;
    
    *ptr = '\0';
    
    if (num == 0) {
        uart_print("0x0");
        return;
    }
    
    while (num > 0) {
        *--ptr = hex_chars[num & 0x0F];
        num >>= 4;
    }
    
    uart_print("0x");
    uart_print(ptr);
}

void uart_flush(void) {
    uint8_t dummy;
    while (UCSR0A & (1 << RXC0)) {
        dummy = UDR0;
    }
    (void)dummy; // Unikaj ostrze¿enia o nieu¿ywanej zmiennej
}

// Przerwanie odbioru UART - zapisuje dane do bufora cyklicznego
// UWAGA: ATmega328PB u¿ywa USART0_RX_vect (nie USART_RX_vect)
ISR(USART0_RX_vect) {
    uint8_t data = UDR0;
    uint8_t next_head = (rx_head + 1) % UART_RX_BUFFER_SIZE;
    
    // Jeœli bufor nie jest pe³ny, zapisz dane
    if (next_head != rx_tail) {
        rx_buffer[rx_head] = data;
        rx_head = next_head;
    }
    // W przeciwnym razie dane s¹ tracone (overflow)
}

// Funkcja do odczytu z bufora cyklicznego (nieblokuj¹ca)
uint8_t uart_read(void) {
    // Wy³¹cz przerwania podczas czytania z bufora
    uint8_t sreg = SREG;
    cli();
    
    if (rx_head == rx_tail) {
        SREG = sreg; // Przywróæ stan przerwañ
        return 0; // Brak danych
    }
    
    uint8_t data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % UART_RX_BUFFER_SIZE;
    
    SREG = sreg; // Przywróæ stan przerwañ
    return data;
}

// Funkcja do odczytu linii tekstu (do znaku nowej linii)
uint8_t uart_read_line(char* buffer, uint8_t max_length) {
    uint8_t count = 0;
    char c;
    
    while (count < max_length - 1) {
        if (!uart_available()) {
            continue;
        }
        
        c = uart_read();
        
        if (c == '\r' || c == '\n') {
            break;
        }
        
        buffer[count++] = c;
    }
    
    buffer[count] = '\0';
    return count;
}
