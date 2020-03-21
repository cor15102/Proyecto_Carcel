/*
 * File:   Carcelero.c
 * Author: joser
 *
 * Created on 2 de marzo de 2020, 02:53 PM
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

#include "I2Cmaestro.h"
#include "LCD.h"
#include "Oscilador.h"

#define _XTAL_FREQ 4000000

#define RS PORTDbits.RD6
#define LCDP PORTB
#define EN PORTDbits.RD7

void setup();
void luces();
void celda();
void ultrasonico();
void Humo();

uint8_t Luz;        // Variable donde se guarda lo que el PIC1 nos envia.
int y,y1,y2,y3;     // Variables para almacenar la centena, la decena y la unidad de la luz.
uint8_t grados;     // Variable para conocer el estado de las celdas.
uint8_t Distancia;  // Variable que guarda la conversion de tiempo a distancia.
int z,z1;     // Variables para almacenar la centena, la decena y la unidad de la distancia
uint8_t Humo;
int w,w1,w2,w3;

// Arreglo con caracteres para imprimir en la LCD
const char a[10] = {'0','1','2','3','4','5','6','7','8','9'};

void Humo()
{
    w  = Humo/100;
    w1 = Humo%100;
    w2 = w1/10;
    w3 = w1%10;
    
    colocar(20,1);
    mostrar(a[w]);
    colocar(21,1);
    mostrar(a[w2]);
    colocar(22,1);
    mostrar(a[w3]);
    
    if (Humo > 100)
    {
        colocar(16,2);
        imprimir("ALERTA");
        PORTAbits.RA4 = 1;
    }
    
    else
    {
        colocar(16,2);
        imprimir("      ");
        PORTAbits.RA4 = 0;
    }
}

void ultrasonico()
{
    z  = Distancia/10;
    z1 = Distancia%10;
    
    colocar(10,1);
    mostrar(a[z]);
    colocar(11,1);
    mostrar(a[z1]);
    
    if (Distancia < 15)
    {
        colocar(8,2);
        imprimir("ALERTA");
        PORTAbits.RA3 = 1;
    }
    
    else
    {
        colocar(8,2);
        imprimir("      ");
        PORTAbits.RA3 = 0;
    }
}

void luces()
{
    y  = Luz/100;       //Valor del entero del fotoresistor
    y1 = Luz%100;       //Valor de los decimales del fotoresistor
    y2 = y1/10;         //Primer decimal del fotoresustor 
    y3 = y1%10;         //Segunod decimal del fotoresistor
    
    colocar(1,2);
    mostrar(a[y]);
    colocar(2,2);
    mostrar(a[y2]);
    colocar(3,2);
    mostrar(a[y3]);
    
    if (Luz < 50)
    {
        colocar(4,2);
        imprimir("   ");
        colocar(5,1);
        imprimir("ON");
        PORTAbits.RA2 = 1;
    }
    
    else
    {
        colocar(5,1);
        imprimir("  ");
        colocar(4,2);
        imprimir("OFF");
        PORTAbits.RA2 = 0;
    }
}

void celda()
{
    if (grados == 0)
    {
        colocar(38,2);
        imprimir(" ");
        
        PORTAbits.RA1 = 0; // LED apagado si la puerta está cerrada
        
        colocar(39,2);
        imprimir("-");
    }
    
    else if (grados == 90)
    {
        colocar(39,2);
        imprimir(" ");
        
        PORTAbits.RA1 = 1; // LED encendido si la puerta está abierta

        colocar(38,2);
        imprimir("+");
    }
}

void setup()
{
    ANSEL  = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    TRISDbits.TRISD6 = 0;   // RS
    TRISDbits.TRISD7 = 0;   // EN
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
}

void main(void)
{
    iniciarOSC(6);  // Fosc = 4MHz
    
    setup();
    
    iniciarLCD();
    
    borrarv();
    
    I2C_Master_Init(100000);
    
    colocar(1,1);
    imprimir("Luz:");
    
    colocar(8,1);
    imprimir("X:");
    colocar(12,1);
    imprimir("cm");
    
    colocar(15,1);
    imprimir("Humo:");
    
    colocar(24,1);
    imprimir("Tem:");
    colocar(26,2);
    mostrar(0xDF);
    colocar(27,2);
    imprimir("C");
    
    colocar(29,1);
    imprimir("Hora:");
    colocar(31,2);
    imprimir(":");
    
    colocar(35,1);
    imprimir("PP");
    
    colocar(38,1);
    imprimir("CC");
    
    while(1)
    {
        // Recibo el valor del sensor de LUZ. PIC1 = REO1
        I2C_Master_Start();         // Iniciamos comunicacion
        I2C_Master_Write(0x11);     // Llamamos al REO1 y le indicamos que lo leeremos
        Luz = I2C_Master_Read(0);   // Guardamos en luz el valor del ADC enviado por el PIC1
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC2
        __delay_ms(100);
        
        // Recibo el estado de la puerta: abierta(90°) o cerrada(0°). PIC2 = REO2
        I2C_Master_Start();             // Iniciamos comunicacion 
        I2C_Master_Write(0x21);         // Llamamos al PIC1 y le indicamos que lo vamos a leer
        grados = I2C_Master_Read(0);    // Guardamos en grados la posicion del servo.
        I2C_Master_Stop();              // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);
        
        // Recibo el tiempo del sensor ultrasonico. PIC3 = REO3
        I2C_Master_Start();         // Iniciamos comunicacion
        I2C_Master_Write(0x31);     // Llamamos al REO1 y le indicamos que lo leeremos
        Distancia = I2C_Master_Read(0);   // Guardamos en luz el valor del ADC enviado por el PIC1
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC2
        __delay_ms(100);
        
        // Recibo el valor del sensor de humo. PIC4 = Reo4
        I2C_Master_Start();         // Iniciamos comunicacion
        I2C_Master_Write(0x41);     // Llamamos al REO1 y le indicamos que lo leeremos
        Humo = I2C_Master_Read(0);   // Guardamos en luz el valor del ADC enviado por el PIC1
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC2
        __delay_ms(100);
        
        luces();
        celda();
        ultrasonico();
        shift();
    }
}