/*
 * Autor:   Rodrigo Corona
 * Carne:   15102
 * Fecha:   21/02/2020
 * 
 * Descripcion: Librería para configuraciones del I2C
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <pic16f887.h>

#define _XTAL_FREQ 4000000

void I2C_blanco(void)        // Modo maestro
{
    // Control de registro 1
    SSPCONbits.WCOL     = 0;    // Write colision detect apagado = SIN COLISION
    SSPCONbits.SSPOV    = 0;    // No overflow. No hay bit indicador.
    SSPCONbits.SSPEN    = 1;    // Habilita el puerto serial sincrono. Configura SDA(RC4) y SCL(RC3) como fuente
    SSPCONbits.CKP      = 0;    // Clock polarity, holds clock low while CPU responds to SSPIF
    SSPCONbits.SSPM   = 0b1000; // Master mode.   clock = (Fosc/(4*SSPADD + 1))
    
    // Control de registro 2
    SSPCON2 = 0;    // Todos 0
    
    // Baud Rate Generator con un clk de 100kHz que es lo estandar
    long clock = 100000;
    SSPADD = ((_XTAL_FREQ/(4*clock))-1);
    
    // STATUS REGISTER
    SSPSTAT = 0;
    
    TRISCbits.TRISC3 = 1;   // SCL
    TRISCbits.TRISC4 = 1;   // SDA
}

void Blanco_Espera()
{
    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
    /*
     * Función de espera: 
     * Mientras se esté iniciada una comunicación,
     * esté habilitado una recepción, esté habilitado una parada
     * esté habilitado un reinicio de la comunicación, esté iniciada
     * una comunicación o se este transmitiendo, el IC2 PIC se esperará
     * antes de realizar algún trabajo
     */
}

void Blanco_repiteStart()
{
    Blanco_Espera();
    SSPCON2bits.RSEN = 1;   // reinicia la comunicacion I2C
}

void Blanco_Stop()
{
    Blanco_Espera();
    SSPCON2bits.PEN = 1;    // Detiene la comunicacion I2C
}

void Blanco_Escribe(unsigned d)
{
    Blanco_Espera();
    SSPBUF = d;         // Buffer se carga con lo que se quiera enviar
}

unsigned short Blanco_lee(unsigned short E)
{
    unsigned short temp;
    
    Blanco_Espera();
    SSPCON2bits.RCEN = 1;
    
    Blanco_Espera();
    temp = SSPBUF;
    
    Blanco_Espera();
    if (E == 1)
    {
        SSPCON2bits.ACKDT = 0;
        
    } else
    {
        SSPCON2bits.ACKDT = 1;
    }
    
    SSPCON2bits.ACKEN = 1;  // Inicia la secuencia de acknowledge
    return temp;            // Regresa el valor leído
}