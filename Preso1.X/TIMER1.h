/*
 *  Autor:  Rodrigo Corona
 *  Carne:  15102
 *  Fecha:  Miercoles 4 de marzo de 2020
 * 
 *  Descripcion:
 *  Librería para configuracion del TIMER1
 */

#include <pic16f887.h>

#define _XTAL_FREQ 4000000
#define Trigger PORTDbits.RD0

// ======================================================================
//                     Iniciador del timer 1
// ======================================================================
void iniciarTMR1(void)
{
    T1CON = 0x10;       // COnfiguracion del timer 1
}

void timerSS()
{
    Trigger = 1;
    __delay_us(10);
    Trigger = 0;
}