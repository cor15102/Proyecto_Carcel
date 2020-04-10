/*
 * File:   Datos.c
 * Author: joser
 *
 * Created on 10 de abril de 2020, 03:30 PM
 */

#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <pic16f887.h>

#include "Oscilador.h"
#include "UART.h"

#define _XTAL_FREQ 4000000

uint8_t x;
uint8_t celda, luz, dist, humo, tempe;

void ReadWrite();

void ReadWrite()
{
    x = UARTrecibir();
    __delay_ms(2);
    
    switch (x)
    {
        case 0:
            celda = UARTrecibir();
            __delay_ms(2);
            UARTenvINT(x);
            __delay_ms(2);
            UARTenvINT(celda);
            __delay_ms(2);
            break;
            
        case 1:
            luz = UARTrecibir();
            __delay_ms(2);
            UARTenvINT(x);
            __delay_ms(2);
            UARTenvINT(luz);
            __delay_ms(2);
            break;
            
        case 2:
            dist = UARTrecibir();
            __delay_ms(2);
            UARTenvINT(x);
            __delay_ms(2);
            UARTenvINT(dist);
            __delay_ms(2);
            break;
            
        case 3:
            humo = UARTrecibir();
            __delay_ms(2);
            UARTenvINT(x);
            __delay_ms(2);
            UARTenvINT(humo);
            __delay_ms(2);
            break;
            
        case 4:
            tempe = UARTrecibir();
            __delay_ms(2);
            UARTenvINT(x);
            __delay_ms(2);
            UARTenvINT(tempe);
            __delay_ms(2);
            break;
            
        default:
            UARTprint("Nada");
            break;
    }
}

void main(void) 
{
    iniciarOSC(6);
    
    iniciarUART();
    
    while(1)
    {
        ReadWrite();
    }
}
