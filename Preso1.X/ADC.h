/*
 * Autor: Rodrigo Corona
 * Carne: 1502
 * 
 * Libreria para activar el ADC
 * 
 */

#pragma config FOSC = INTRC_NOCLKOUT    // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#define _XTAL_FREQ 4000000

void iniciarADC(int conv, int ch)
{
    // ===============================================
    // Comparador para elegir ADC clock source
    // ===============================================
    
    switch(conv)
    {
        case 1:
            ADCON0bits.ADCS = 0b00;     // Fosc/2.
            break;                      // USAR UNICAMENTE CON 1MHz
            
        case 2:
            ADCON0bits.ADCS = 0b01;     // Fosc/8
            break;                      // USAR UNICAMENTE CON 4MHz
            
        case 3:
            ADCON0bits.ADCS = 0b10;     // Fosc/32
            break;                      // USAR UNICAMENTE CON 8MHz y 20MHz
            
        default:
            ADCON0bits.ADCS = 0b11;     // Frc
            break;                      // Usable con cualquier frecuencia
    }
    
    // ===============================================
    // Para seleccionar el canal(puerto) de lectura.
    // ===============================================
    switch(ch)
    {
        case 0:
            ADCON0bits.CHS = 0b0000;    // CHANNEL AN0 = RA0
            break;
            
        case 1:
            ADCON0bits.CHS = 0b0001;    // CHANNEL AN1 = RA1
            break;
            
        case 2:
            ADCON0bits.CHS = 0b0010;    // CHANNEL AN2 = RA2
            break;
            
        case 3:
            ADCON0bits.CHS = 0b0011;    // CHANNEL AN3 = RA3
            break;
            
        case 4:
            ADCON0bits.CHS = 0b0100;    // CHANNEL AN4 = RA5
            break;
            
        case 5:
            ADCON0bits.CHS = 0b0101;    // CHANNEL AN5 = RE0
            break;
            
        case 6:
            ADCON0bits.CHS = 0b0110;    // CHANNEL AN6 = RE1
            break;
            
        case 7:
            ADCON0bits.CHS = 0b0111;    // CHANNEL AN7 = RE2
            break;
            
        case 8:
            ADCON0bits.CHS = 0b1000;    // CHANNEL AN8
            break;
            
        case 9:
            ADCON0bits.CHS = 0b1001;    // CHANNEL AN9
            break;
            
        case 10:
            ADCON0bits.CHS = 0b1010;    // CHANNEL AN10
            break;
            
        case 11:
            ADCON0bits.CHS = 0b1011;    // CHANNEL AN11
            break;
            
        case 12:
            ADCON0bits.CHS = 0b1100;    // CHANNEL AN12
            break;
            
        case 13:
            ADCON0bits.CHS = 0b1101;    // CHANNEL AN13
            break;
            
        case 14:
            ADCON0bits.CHS = 0b1110;    // CHANNEL CVref
            break;
            
        case 15:
            ADCON0bits.CHS = 0b1111;    // CHANNEL Fixed Ref (0.6V fixed voltage reference)
            break;
            
        default:
            ADCON0bits.CHS = 0b0000;    // CHANNEL AN0
            break;
            
    }
    
    ADCON0bits.GO_DONE = 1;     // Estado de conversion: 1 habilitado, 0 dehabilitado.
    
    ADCON0bits.ADON = 1;        // ADC esta habilitado
    
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda (0)
    
    ADCON1bits.VCFG1 = 0;       // Voltaje de referencia Vss = tierra
    
    ADCON1bits.VCFG0 = 0;       // Voltaje de referencia Vdd = Vin    
}



// ===============================================
// Lectura de datos
// ===============================================
unsigned int leerADC(unsigned char channel)
{
    if(channel > 7)
    {
        return 0;
    }
    
    ADCON0 &= 0xC5;             // Codigo tomado de:
    ADCON0 |= channel<<3;       // https://pwmlabs.wordpress.com/2016/06/25/lectura-analogica-pic16f877a-con-xc8/
    __delay_ms(2);              // Martes 4 de febrero de 2020.
    GO_nDONE = 1;
    
    while(GO_nDONE)
    {
    }

    return (ADRESH); //<<8)+ADRESL
}