/*
 * File:   Reo2.c
 * Author: Rodrigo Corona
 *
 * Created on 13 de marzo de 2020, 07:16 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
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
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>

#include "I2Cesclavo.h"
#include "PWM.h"
#include "Oscilador.h"

#define _XTAL_FREQ 1000000

void setup();
void OPEN_CLOSE();

uint8_t E, basura;
uint8_t estado1, estado2;
uint8_t z, dato;

// =============================================================================
//                      Funcion de interrupciones
// =============================================================================
void __interrupt() isr(void)
{
    // =========================================================================
    // Interrupcion para comunicacion I2C
    // =========================================================================
   
    if(PIR1bits.SSPIF == 1)
    {

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL))    // En caso de colisiones y errores, resetea I2C
        {
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW)      // Recepcion de datos
        {
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupci�n recepci�n/transmisi�n SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepci�n se complete
            basura = SSPBUF;            // Maestro no nos enviar� nada, pero si el buffer se llena lo limpiamos
            __delay_us(250);
            
        }
        
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW)     // Envio de datos > Esclavo a Maestro
        {
            z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
            BF = 0;                 // Limpiamos bandera
            SSPBUF = E;             // Enviamos al maestro el estado de la puerta
            SSPCONbits.CKP = 1;     // Habilitamos reloj
            __delay_us(250);
            while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
        }
       
        PIR1bits.SSPIF = 0;
    }
}

void OPEN_CLOSE()
{
    if (PORTBbits.RB7 == 1 && estado1 == 0)
    {
        E = 90;
        g90();
        estado1 = 1;
    }
    else if (PORTBbits.RB7 == 0 && estado1 == 1)
    {
        estado1 = 0;
    }
    
    if (PORTBbits.RB5 == 1 && estado2 == 0)
    {
        E = 0;
        g0();
        estado2 = 1;
    }
    else if (PORTBbits.RB5 == 0 && estado2 == 1)
    {
        estado2 = 0;
    }
}

void setup()
{
    ANSEL  = 0;
    ANSELH = 0;     //Todos los puertos digitales
    
    TRISBbits.TRISB7 = 1; // Boton abrir
    TRISBbits.TRISB5 = 1; // Boton cerrar
    
    PORTB = 0;      // Limpiamos el puerto
    PORTC = 0;
}

void main (void)
{
    iniciarOSC(4);      // Fosc = 1MHz
    
    setup();            // Configuramos puertos
    
    I2C_Esclavo(0x20);    // Esclavo con direccion 0x40
    
    iniciarPWM();
    
    E = 0;  // Inicializo la variable indicadora
    g0();
    
    while(1)
    {
        OPEN_CLOSE();
    }
}