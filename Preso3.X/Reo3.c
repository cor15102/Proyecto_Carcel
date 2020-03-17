/*
 * File:   Reo3.c
 * Author: joser
 *
 * Created on 16 de marzo de 2020, 10:49 PM
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
#include <stdio.h>
#include <stdint.h>
#include <pic16f887.h>

#include "I2Cesclavo.h"
#include "Oscilador.h"

#define _XTAL_FREQ 4000000
#define TRIGGER PORTDbits.RD4
#define ECHO PORTDbits.RD6

void setup();
void cronometro();

int tiempo, distancia;
uint8_t metro;
uint8_t z, dato, basura;

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
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            basura = SSPBUF;            // Maestro no nos enviará nada, pero si el buffer se llena lo limpiamos
            __delay_us(250);
            
        }
        
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW)     // Envio de datos > Esclavo a Maestro
        {
            z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
            BF = 0;                 // Limpiamos bandera
            SSPBUF = metro;             // Enviamos al maestro el estado de la puerta
            SSPCONbits.CKP = 1;     // Habilitamos reloj
            __delay_us(250);
            while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
        }
       
        PIR1bits.SSPIF = 0;
    }
}

void cronometro(void)
{
    TMR1L = 0;  // Reiniciamos 
    TMR1H = 0;  // el cronometro
        
    TRIGGER = 1;
    __delay_us(10);
    TRIGGER = 0;        // Activamos el sensor
    
    while(!ECHO);           // Esperamos a que se active el pulso
    T1CONbits.TMR1ON = 1;   // Encendemos el timer1
    while(ECHO);            // Esperamos a que el pulso baje
    T1CONbits.TMR1ON = 0;   // Apagamos el timer
    
    tiempo = TMR1L |(TMR1H<<8);         // Guardamos el tiempo que el ECHO estuvo en 1
    distancia = (tiempo/58.82) + 1;
    
    metro = (0b11111111 & distancia);
}

void setup()
{
    ANSEL  = 0;
    ANSELH = 0;
    
    TRISDbits.TRISD4 = 0;   // Para activar el sensor HC-SR04 (TRIGGER)
    TRISDbits.TRISD6 = 1;   // Lo que devuelve el sensor HC-SR04 (ECHO)
    
    PORTD = 0;      // Limpiamos el puerto D
    
    T1CON = 0x10;   // Configuramos y activamos el TIMER1
}

void main(void) 
{
    iniciarOSC(6);      // Fosc = 4MHz
    
    setup();            // Configuracion de puertos
    
    I2C_Esclavo(0x30);  // Este pic es esclavo con direccion 0x30
    
    while(1)
    {
        cronometro();
    }
    
}
