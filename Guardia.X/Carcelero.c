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

void setup();
void conversion();
void on_off();
void celdas();

uint8_t MQ2, Fotor, LM35;           // Variables que guardan lo leido de los esclavos
int Humo,Luz,Tiempo;
double Tempe;
int x,x1,x2,x3;          // Variables para obtener los enteros y decimales
int y,y1,y2,y3;
int z,z1,z2,z3,z4;
int estado1, estado2, posicion;

// Arreglo con caracteres para imprimir en la LCD
const char a[10] = {'0','1','2','3','4','5','6','7','8','9'};

// ------------------ Funcion para mapear valores del ADC ------------------- //
void conversion()                   // Convertir valores para desplegar en LCD 
{
    Humo = (MQ2*1);     //Voltaje del Sensor MQ2
    x = Humo/100;       //Valor del entero del sensor MQ2
    x1 = Humo%100;      //Valor de los decimales del sensor MQ2
    x2 = x1/10;         //Primer decimal del sensor MQ2
    x3 = x1%10;         //Segundo decimal del sensor MQ2
    
    Luz = (Fotor*1);    //Voltaje del fotoresistor
    y = Luz/100;        //Valor del entero del fotoresistor
    y1 = Luz%100;       //Valor de los decimales del fotoresistor
    y2 = y1/10;         //Primer decimal del fotoresustor 
    y3 = y1%10;         //Segunod decimal del fotoresistor
    
    Tempe = (LM35*5.0)/255;  
    z = Tempe*100;
    z1  = z/100;
    z2 = z%100;
    z3 = z2/10;
    z4 = z2%10;
    
    // Esto lo hara el maestro
    /*distancia = (Tiempo/5.882) + 1;     // 34,000 = velocidad del sonido en cm
    
    x  = distancia/100;  // Obtenemos la centena
    x1 = distancia%100;  // Obtenemos el residuo para la decima y unidad
    x2 = x1/10;     // Obtenemos la decena
    x3 = x1%10;     // Obtenemos la unidad*/
 }

void on_off()   // Indicarcion de encedido o apagado en LCD
{
    if (Humo > 100)   // Para verificar sensor de Humo
    {
        colocar(1,2);
        imprimir("   ");   // Borro palabra OFF en LCD
        colocar(1,2);
        imprimir("ON");    // Imprimo ON en LCD
        
        PORTAbits.RA3 = 1;  // Encendemos ventilador de humo
    }
    else
    {
        colocar(1,2);
        imprimir("  ");     // Borro palabra ON en LCD
        colocar(1,2);
        imprimir("OFF");    // Imprimo la palabra OFF en LCD
        
        PORTAbits.RA3 = 0;  // Apagamos ventilador de humo
    }
    
// =============================================================================    
    if (Luz > 50)             // Para verificar sensor de Luz
    {
        colocar(6,2);
        imprimir("  ");     // Borro palabra ON en LCD
        colocar(6,2);
        imprimir("OFF");    // Imprimo la palabra OFF en LCD
        
        PORTAbits.RA4 = 0;  // Apagamos Luces 
    }
    else
    {
        colocar(6,2);
        imprimir("   ");    // Borro palabra OFF en LCD
        colocar(6,2);
        imprimir("ON");     // Imprimo palabra ON en LCD
        
        PORTAbits.RA4 = 1;  // Enciendo luces de noche. 
    }
    
    if (Tempe > 30)
    {
        colocar(10,2);
        imprimir("   ");    // Borro palabra OFF
        colocar(10,2);
        imprimir("ON");     // Imprimo palabra ON en LCD
        
        PORTAbits.RA2 = 1;  // Encendemos ventilador en cuarto de control. 
    }
    else
    {
        colocar(10,2);
        imprimir("   ");    // Borro palabra ON
        colocar(10,2);
        imprimir("OFF");    // Imprimo OFF en LCD
        
        PORTAbits.RA2 = 1;  // Apagamos ventilador en cuarto de control.
    }
}

void celdas()
{
    if (PORTAbits.RA0 == 1 && estado1 == 0)
    {
        posicion = 90;
        estado1  = 1;
        PORTAbits.RA5 = 1;
    }
    else if (PORTAbits.RA0 == 0 && estado1 == 1)
    {
        estado1 = 0;
    }
    
    if (PORTAbits.RA1 == 1 && estado2 == 0)
    {
        posicion = 0;
        estado2  = 1;
        PORTAbits.RA5 = 0;
    }
    else if (PORTAbits.RA0 == 0 && estado2 == 1)
    {
        estado2 = 0;
    }
}

void setup()
{
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISAbits.TRISA0 = 1;   // Boton abrir celda
    TRISAbits.TRISA1 = 1;   // Boton cerrar celda
    TRISB = 0;      // LCD
    
    PORTA = 0;
    PORTB = 0;
}

void main(void) 
{
    iniciarOSC(6);  // Frecuencia de 4MHz
    
    setup();        // Configuramos puertos
    
    I2C_Master_Init(100000);   // Configuramos el PIC como maestro
    
    iniciarLCD();   // Iniciamos LCD
    
    borrarv();      // Limpio pantalla
    
    colocar(1,1);
    imprimir("Humo");
    
    colocar(6,1);
    imprimir("Luz");
    
    colocar(10,1);
    imprimir("Temp");
    
    while(1)
    {
        // =====================================================================
        // Solicito valor del sensor de humo       
        I2C_Master_Start();         // Iniciamos comunicacion 
        I2C_Master_Write(0x20);     // Llamamos al PIC1 y le indicamos que lo vamos a leer
        I2C_Master_Write(2);        // Solicitamos el valor del sensor de HUMO
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);
        
        // Recibo el valor del sensor de humo
        I2C_Master_Start();         // Iniciamos comunicacion 
        I2C_Master_Write(0x21);     // Llamamos al PIC1 y le indicamos que lo vamos a leer
        MQ2 = I2C_Master_Read(0);   // Guardamos en variable "MQ2" lo que nos manda el PIC (8bits)
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);

        // =====================================================================
        // Solicito valor del sensor de LUZ
        I2C_Master_Start();         // Iniciamos comunicacion 
        I2C_Master_Write(0x20);     // Llamamos al PIC1 y le indicamos que lo vamos a leer
        I2C_Master_Write(1);        // Solicitamos el valor del sensor de LUZ
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);
        
        // Recibo el valor del sensor de LUZ 
        I2C_Master_Start();         // Iniciamos comunicacion
        I2C_Master_Write(0x21);     // Llamamos al PIC1 y le indicamos que lo leeremos
        Fotor = I2C_Master_Read(0);   // Guardamos en la variable "Fotor" lo que nos manda el PIC2 (8bits)
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC2
        __delay_ms(100);
        
        // =====================================================================
        // Solicito valor del sensor de temperatura       
        I2C_Master_Start();         // Iniciamos comunicacion 
        I2C_Master_Write(0x20);     // Llamamos al PIC1 y le indicamos que lo vamos a leer
        I2C_Master_Write(3);        // Solicitamos el valor del sensor de HUMO
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);
        
        // Recibo el valor del sensor de temperatura
        I2C_Master_Start();         // Iniciamos comunicacion 
        I2C_Master_Write(0x21);     // Llamamos al PIC1 y le indicamos que lo vamos a leer
        LM35 = I2C_Master_Read(0);  // Guardamos en variable "LM35" lo que nos manda el PIC (8bits)
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);
        
        // =====================================================================
        // Envio posicion para abrir/cerrar las celdas       
        I2C_Master_Start();         // Iniciamos comunicacion 
        I2C_Master_Write(0x40);     // Llamamos al PIC1 y le indicamos que lo vamos a leer
        I2C_Master_Write(posicion); // Solicitamos el valor del sensor de HUMO
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC1
        __delay_ms(100);
        
        conversion();       // Convierto los valores recibidos para imprimir en LCD
     
//        colocar(1,2);       // Colocamos cursor en posición 
//        mostrar(a[x]);     // Imprime valor entero. 
//        colocar(2,2);       // Colocamos cursor en posición 
//        mostrar(a[x2]);     // Imprime primer decimal del sensor MQ2
//        colocar(3,2);       // Colocamos cursor en posición 
//        mostrar(a[x3]);     // Imprime segundo decimal del sensor MQ2
//        __delay_ms(2);      // Delay para ejecutar siguiente instrucción
//        
//        colocar(9,2);       // Colocamos cursor en posición 
//        mostrar(a[y]);     // Imprime valor entero. 
//        colocar(10,2);      // Colocamos cursor en posición 
//        mostrar(a[y2]);     // Imprime primer decimal del sensor Luz
//        colocar(11,2);      // Colocamos cursor en posición 
//        mostrar(a[y3]);     // Imprime segundo decimal del sensor Luz
//        __delay_ms(2);      // Delay para ejecutar siguiente instrucción
        
        on_off();
        
        celdas();
    }
}
