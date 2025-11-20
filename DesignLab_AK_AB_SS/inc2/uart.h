/*
 * uart.h
 * 
 * UART driver for ATmega328PB
 * TXD: PD1 (pin 3)
 * RXD: PD0 (pin 2)
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>

// Domyœlna prêdkoœæ transmisji (mo¿na zmieniæ przed kompilacj¹)
#ifndef BAUD
#define BAUD 9600
#endif

// Rozmiar bufora (mo¿esz zwiêkszyæ jeœli potrzebujesz)
#define UART_RX_BUFFER_SIZE 128
#define UART_TX_BUFFER_SIZE 128

// Makro do obliczenia wartoœci rejestru UBRR
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

/**
 * @brief Inicjalizacja UART
 * 
 * Konfiguruje UART z parametrami:
 * - 8 bitów danych
 * - 1 bit stopu
 * - Bez parzystoœci
 * - Prêdkoœæ zgodna z BAUD
 */
void uart_init(void);

/**
 * @brief Wysy³a pojedynczy bajt przez UART
 * 
 * @param data Bajt do wys³ania
 */
void uart_transmit(uint8_t data);

/**
 * @brief Odbiera pojedynczy bajt z UART (funkcja blokuj¹ca)
 * 
 * @return Odebrany bajt
 */
uint8_t uart_receive(void);

/**
 * @brief Sprawdza czy s¹ dostêpne dane do odbioru
 * 
 * @return 1 jeœli dane s¹ dostêpne, 0 jeœli nie
 */
uint8_t uart_available(void);

/**
 * @brief Wysy³a ci¹g znaków (string) przez UART
 * 
 * @param str WskaŸnik na string zakoñczony znakiem NULL
 */
void uart_print(const char* str);

/**
 * @brief Wysy³a string z now¹ lini¹ na koñcu
 * 
 * @param str WskaŸnik na string zakoñczony znakiem NULL
 */
void uart_println(const char* str);

/**
 * @brief Wysy³a liczbê ca³kowit¹ jako tekst
 * 
 * @param num Liczba do wys³ania
 */
void uart_print_int(int32_t num);

/**
 * @brief Wysy³a liczbê w formacie szesnastkowym
 * 
 * @param num Liczba do wys³ania
 */
void uart_print_hex(uint32_t num);

/**
 * @brief Czyszcza bufor odbiorczy
 */
void uart_flush(void);

/**
 * @brief Odbiera bajt z bufora (nieblokuj¹ca)
 * 
 * @return Odebrany bajt lub 0 jeœli brak danych
 */
uint8_t uart_read(void);

/**
 * @brief Odbiera liniê tekstu do bufora
 * 
 * @param buffer WskaŸnik na bufor
 * @param max_length Maksymalna d³ugoœæ
 * @return Liczba odebranych znaków
 */
uint8_t uart_read_line(char* buffer, uint8_t max_length);

#endif /* UART_H_ */