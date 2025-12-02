# Dokumentacja projektu
## Schemat blokowy
![Schemat Blokowy](Images/Block_Diagram.jpg)

## Schemat ideowy
![alt text](Images/DL_Schematic_state4.png)

## Design PCB
![alt text](Images/DL_PCB_state3.png)

## Podgląd 3D
![alt text](Images/DL_PCB_preview2.png)

### Blok mikrokontrolera (ATmega328PB)
*Obliczanie wartości pojemności C1 i C2 potrzebnych do zewnętrznego oscylatora MCU:*    

Wybraliśmy CY16BPS ponieważ jego parametry należą do najpopularniejszych do użycia w projektach z tym MCU.    
Oto jego najważniejsze parametry:   
- F_XTAL = 16 MHz
- C_load = 20 pF
- ESR = 50 ohm

Dla uproszczenia obliczeń, niech C1 = C2.    
Korzystając ze wzoru z dokumentacji ATmega,   
**C = 2 * C_L + C_S**  
gdzie     
C_L - Pojemnosć obciążenia kryształu,   
C_S - Pojemność pasożytnicza płytki PCB, niech wynosi 2 pF
    
Dla naszego kryształu C_L wynosi 20 pF    
**Stąd wynika, że C powinno mieć wartość około 42 pF.**


### Diody LED do sygnalizacji
*Obliczanie wartości rezystancji R1, R2 i R3 potrzebnych do uregulowania pracy diód:*     
    
Napięcie zasilania: 3.3V
Kolor diód LED: zielony co oznacza spadek napięcia około 2.2V   
Chcemy ustalić bezpieczny punkt pracy, powiedzmy 10mA    
Z prawa ohma: R = U / I = (3.3 - 2.2) / 10 mA = **110 ohm**