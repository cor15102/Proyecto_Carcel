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

#define _XTAL_FREQ 125000
#define Trigger PORTBbits.RB0
#define Echo PORTBbits.RB2

void setup();
//void cronometro();

uint8_t tiempoL, tiempoH;   // Guardamos el tiempo del TIMER1
uint8_t z;          // Limpia los datos inservibles del buffer de I2C
uint8_t dato;       // Como diputado en el congreso: no hace nada y ocupa espacio
uint8_t x;          // Variable que compara las peticiones del maestro.

void __interrupt() isr(void)
{
    // =========================================================================
    //                          Comunicacion I2C
    // =========================================================================
    if(PIR1bits.SSPIF == 1)     // Maestro se comunica con el esclavo 
    {
        SSPCONbits.CKP = 0;     // Apagamos el reloj de I2C
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL))    // En caso de colisiones y errores, resetea I2C
        {
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }
    // =========================================================================
        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW)      // Recepcion de datos
        {   
            // Esuchamos lo que el patron ordene
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            x = SSPBUF;                 // Guardar en el "x" el valor recibido
            __delay_us(250);
        }
        
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW)     // Envio de datos > Esclavo a Maestro
        {
            if (x == 5)
            {
                z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
                BF = 0;                 // Limpiamos bandera
                SSPBUF = tiempoL;       // El valor del sensor de LUZ lo cargamos al Buffer
                SSPCONbits.CKP = 1;     // Habilitamos reloj
                __delay_us(250);
                while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
            }
            
            else if (x == 6)
            {
                z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
                BF = 0;                 // Limpiamos bandera
                SSPBUF = tiempoH;       // El valor del HUMO lo cargamos al Buffer
                SSPCONbits.CKP = 1;     // Habilitamos reloj
                __delay_us(250);
                while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
            }
        }
       
        PIR1bits.SSPIF = 0;
    }
}

/*void cronometro(void)
{
    TMR1L = 0;  // Reiniciamos 
    TMR1H = 0;  // el cronometro
    
    Trigger = 1;
    __delay_us(10);
    Trigger = 0;        // Envio de pulso para activar el sensor ultrasonico
    
    while(!Echo);       // Esperamos que el sensor nos regrese el pulso
    
    T1CONbits.TMR1ON = 1;   // Encendemos el timer1
    while(Echo);            // Esperamos a que el pulso baje
    T1CONbits.TMR1ON = 0;   // Apagamos el timer
    
    tiempoL = TMR1L;        // Guardamos los 8 bits del TMR1 low
    tiempoH =  (TMR1H<<8);  // Guardamos los 2 bits sobrantes
    // Estos tiempos se iran al master para que él los sume
}*/

void setup()
{
    ANSEL = 0;
    ANSELH = 0;
    
    TRISBbits.TRISB0 = 0;   // Trigger ultrasonico
    TRISBbits.TRISB2 = 1;   // Echo ultrasonico
    
    INTCONbits.GIE = 1;     // Activamos todas las interrupciones
    INTCONbits.PEIE = 1;    // Interrupcion periferica habilitada
    PIE1bits.ADIE = 1;      // Habilita la interrupcion del ADC
    PIE1bits.TMR1IE = 1;    // Habilitamos la interrupcion del Timer1
    
    T1CON = 0x10;           // COnfiguracion del timer 1
}

void main(void) 
{
    iniciarOSC(6);
    
    setup();
    
    iniciarPWM();
    
    I2C_Negro(0x40);
    
    g0();
    
    __delay_ms(1000);   // Esperamos a que todo se configure
    
    while(1)
    {
        //cronometro();
        
        if (x == 0)
        {
            g90();
        }
        
        else if (x == 90)
        {
            g0();
        }
    }
}
