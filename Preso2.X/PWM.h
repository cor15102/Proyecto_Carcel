

#include <pic16f887.h>

#define _XTAL_FREQ 125000

void iniciarPWM()
{
//    // Habilitamos interrupciones
//    INTCONbits.GIE = 1;
//    INTCONbits.PEIE = 1;
    
    // Usamos CCP2 como pin para PWM
    TRISCbits.TRISC1 = 1;   // Ajustamos puerto como entrada para
                            // evitar una mala señal de salida
    PR2 = 154;             // PR2 = 156 = 0x9C
    CCP2CON = 0b00001100;   // DC2B = 11.  CCP2M = 1100 (PWM mode)
    
    // TIMER 2
    PIR1bits.TMR2IF = 0;     // Limpiamos bandera de interrupcion
    T2CONbits.T2CKPS = 0b01; // Prescaler 1:4
    T2CONbits.TMR2ON = 1;    // TIMER2 is on
    TRISCbits.TRISC1 = 0;    // Asignamos el pin RC1 de salida
    // Estamos listos para iniciar
}

void g0(void)
{
    // Posicion 0 grados - 0.64ms
    CCP2CONbits.DC2B1 = 0;
    CCP2CONbits.DC2B0 = 1;
    CCPR2L = 0b00000101;
}

void g90(void)
{
    // Posicion 90 grados
    CCP2CONbits.DC2B1 = 1;
    CCP2CONbits.DC2B0 = 1;
    CCPR2L = 0b00001011;
}