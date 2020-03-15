/*
 * File:   Reo1.c
 * Author: joser
 *
 * Created on 13 de marzo de 2020, 06:09 PM
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

#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// =============================================================================
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <pic16f887.h>

#include "ADC.h"
#include "I2Cesclavo.h"
#include "Oscilador.h"

#define _XTAL_FREQ 4000000

// =============================================================================
//                  Declaramos las funciones a utilizar
void setup();

// =============================================================================
//                  Declaramos las variables globales a usar

uint8_t luz, humo, tempe;  // Variables en las que guardamos lo leido
uint8_t z;          // Limpia los datos inservibles del buffer de I2C
uint8_t dato;       // Como diputado en el congreso: no hace nada y ocupa espacio
uint8_t x;        // Variable que compara las peticiones del maestro.

// =============================================================================
void __interrupt() isr(void)
{
    // ==== Interrupciones ADC ==== // 
    if (PIR1bits.ADIF == 1)     // Interrupcion del ADC
    {
        PIR1bits.ADIF = 0;      // Limpio bandera
        __delay_ms(2);
    }
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
            //if (x == 1)
            //{
                z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
                BF = 0;                 // Limpiamos bandera
                SSPBUF = luz;           // El valor del sensor de LUZ lo cargamos al Buffer
                SSPCONbits.CKP = 1;     // Habilitamos reloj
                __delay_us(250);
                while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
            //}
            
            /*else if (x == 2)
            {
                z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
                BF = 0;                 // Limpiamos bandera
                SSPBUF = humo;          // El valor del HUMO lo cargamos al Buffer
                SSPCONbits.CKP = 1;     // Habilitamos reloj
                __delay_us(250);
                while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
            }
            
            else if (x == 3)
            {
                z = SSPBUF;             // Lo que tiene el buffer lo guardamos para descartarlo
                BF = 0;                 // Limpiamos bandera
                SSPBUF = tempe;         // La temperatura lo cargamos al Buffer
                SSPCONbits.CKP = 1;     // Habilitamos reloj
                __delay_us(250);
                while(SSPSTATbits.BF);  // Esperamos a que se envien los datos
            }*/
        }
       
        PIR1bits.SSPIF = 0;
    }
}

// =============================================================================
//                  Configuracion de puertos y registros
// =============================================================================
void setup()
{
    ANSEL = 0;
    ANSELH = 0;
    
    ANSELbits.ANS0 = 1;     // Sensor de luz: entrada analógica.
    ANSELbits.ANS1 = 1;     // Sensor de humo: entrada analógica.
    //ANSELbits.ANS2 = 1;     // Sensor de temperatura: entrada analógica.
    
    TRISAbits.TRISA0 = 1;   // Sensor de luz: entrada analógica.
    TRISAbits.TRISA1 = 1;   // Sensor de humo: entrada analógica.
    //TRISAbits.TRISA2 = 1;   // Sensor de temperatura: entrada analógica.
    
    PORTA = 0;  // Limpiamos puertos
    
    INTCONbits.GIE = 1;     // Activamos todas las interrupciones
    INTCONbits.PEIE = 1;    // Interrupcion periferica habilitada
    PIE1bits.ADIE = 1;      // Habilita la interrupcion del ADC
}

void main(void) 
{
    iniciarOSC(6);  // Frecuencia de PIC a 4MHz
    
    setup();        // COnfiguramos puertos y registros
    
    iniciarADC(2);  // Configuramos ADC para lectura de luz y humo
    
    I2C_Negro(0x20);    // Este sera un esclavo con direccion 0x20
    
    __delay_ms(1000);   // Esperamos a que todo se configure
    
    while(1)
    {
        ADCON0bits.CHS = 0b0000;    // CHANNEL AN0 = RA0
        luz = leerADC(0);          // Guardamos en luz el ADDRESS del ADC
        __delay_ms(2);
        
        /*ADCON0bits.CHS = 0b0001;    // CHANNEL AN1 = RA1
        humo = leerADC(0);          // Guardamos en humo el ADDRESS del ADC
        __delay_ms(2);*/
        
        /*ADCON0bits.CHS = 0b0010;    // CHANNEL AN2 = RA2
        tempe = leerADC(0);         // Guardamos en tempe el ADDRESS del ADC
        __delay_ms(2);*/
    }
}
