/*
 *  Autor: Rodrigo Corona
 *  Carne: 15102
 *  Fecha: 12 de marzo de 2020
 * 
 *  Descripcion: Libreria creada para configuracion y uso del PWM.
 *               Para mayor información, consultar la pagina 128 a la 131
 *               del manual del PIC16F887 (PWM mode).
 * 
 */
#include <pic16f887.h>

#define _XTAL_FREQ 1000000

void iniciarPWM()
{
    // Usamos CCP2 como pin para PWM
    TRISCbits.TRISC1 = 1;   // Apagamos salida para primero configurar todo
    PR2 = 200;              // ...
    CCP2CON = 0b00001100;   // DC2B = xx.  CCP2M = 1100 (PWM mode)
    
    // TIMER 2
    PIR1bits.TMR2IF = 0;     // Limpiamos bandera de interrupcion
    T2CONbits.T2CKPS = 0b10; // Prescaler 1:16
    T2CONbits.TMR2ON = 1;    // TIMER2 is on
    TRISCbits.TRISC1 = 0;    // Asignamos el pin RC1 de salida
    // Estamos listos para iniciar
}

void g0(void)
{
    // Posicion 0 grados - 0.64ms
    CCP2CONbits.DC2B1 = 1;
    CCP2CONbits.DC2B0 = 1;
    CCPR2L = 0b00000011;
}

void g90(void)
{
    // Posicion 90 grados
    CCP2CONbits.DC2B1 = 0;
    CCP2CONbits.DC2B0 = 0;
    CCPR2L = 0b00011001;
}