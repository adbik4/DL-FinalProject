/*
 * uart.h
 *
 * Created: 02.12.2025 12:17:29
 *  Author: sebastian
 */ 


#ifndef UART_H_
#define UART_H_

#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD) - 1)

// Rozmiary buforów
#define UART_RX_BUFFER_SIZE 64
#define UART_TX_BUFFER_SIZE 64

// Funkcje UART
void uart_init(void);
void uart_transmit(uint8_t data);
uint8_t uart_receive(void);
uint8_t uart_available(void);
uint8_t uart_read(void);
void uart_print(const char* str);
void uart_println(const char* str);
void uart_print_int(int32_t num);
void uart_print_hex(uint32_t num);
void uart_flush(void);
uint8_t uart_read_line(char* buffer, uint8_t max_length);



#endif /* UART_H_ */