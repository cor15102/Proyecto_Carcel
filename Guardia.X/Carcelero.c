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
#include "UART.h"

#define _XTAL_FREQ 4000000

#define RS PORTDbits.RD6
#define LCDP PORTB
#define EN PORTDbits.RD7
#define dir PORTDbits.RD0
#define step PORTDbits.RD1
#define enable PORTDbits.RD2

void setup();
void luces();
void celda();
void ultrasonico();
void humo();
void temperatura();

char read_RTC(char address);
void write_RTC(void);
unsigned char MSB(unsigned char x);
unsigned char  LSB(unsigned char x);
void initTimer1(void);

uint8_t Luz;        // Variable donde se guarda lo que el PIC1 nos envia.
int y,y1,y2,y3;     // Variables para almacenar la centena, la decena y la unidad de la luz.
uint8_t grados;     // Variable para conocer el estado de las celdas.
uint8_t Distancia;  // Variable que guarda la conversion de tiempo a distancia.
int z,z1;           // Variables para almacenar la centena, la decena y la unidad de la distancia
uint8_t Humo;       // Variable donde almacenamos la distancia enviada por el PIC3
int w,w1,w2,w3;     // Variables para almacenar la centena, la decena y la unidad del humo
uint8_t Temp, calor;// Variables donde almaceno la temperatura (8bits) y lo mapeo de 0 a 99°C
int t,t1,t2,t3;     // Variables para almacenar la centena, la decena y la unidad de la temperatura
int i;              // Contador para envio de datos seriales

int data, dat, sensor;
char v1[];
char i_stepper;
int  minute, hour, hr, ap, flanco;
char time[]="00:00:00";
char date[]="00-00-00";
char enter[]="\n";

// Arreglo con caracteres para imprimir en la LCD
const char a[10] = {'0','1','2','3','4','5','6','7','8','9'};
// Arreglo para almacenar la informacion que enviaremos por comunicacion serial
int b[5] = {0,0,0,0,0};    // 0.Celda  1.luz  2.distancia  3.humo  4. temperatura

//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void)
{
     if(PIR1bits.TMR1IF == 1)
     {  
        flanco ++;         // Toggle 
        if (flanco == 2)
        {
            flanco = 0;   
        }
        if (flanco == 1)
        {
            step = 1;
        }
        if (flanco == 0)
        {
            step = 0;
        }
        
        // valor a cargar al TMR1 5536 -> 0x15A0
        TMR1H = 0xE9;
        TMR1L = 0x28;
        PIR1bits.TMR1IF = 0;
    }
}

void temperatura()
{
    calor = (500.0*Temp)/255;
    
    t  = calor/100;       //Valor del entero del fotoresistor
    t1 = calor%100;       //Valor de los decimales del fotoresistor
    t2 = t1/10;
    t3 = t1%10;
    
    colocar(24,2);
    mostrar(a[t2]);
    colocar(25,2);
    mostrar(a[t3]);
    
    if (calor > 30)
    {
        PORTAbits.RA5 = 1;  // Si la temperatura es mayor a 30°C, encendemos ventilador
    }
    
    else
    {
        PORTAbits.RA5 = 0; // Temperatura menor a 30°C apaga el ventilador
    }
    
    b[4] = calor;
}

void humo()
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
    
    if (Humo > 50)
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
    
    b[3] = Humo;
}

void ultrasonico()
{
    z  = Distancia/10;
    z1 = Distancia%10;
    
    colocar(10,1);
    mostrar(a[z]);
    colocar(11,1);
    mostrar(a[z1]);
    
    if (Distancia < 8)
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
    
    b[2] = Distancia;
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
    
    b[1] = Luz;
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
    
    b[0] = grados;
}

void setup()
{
    ANSEL  = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    TRISCbits.TRISC6 = 1;
    
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
    setup();        // Configuraciones
    iniciarLCD();   // Iniciamos LCD
    borrarv();      // Borramos visualizador
    I2C_Master_Init(100000);    // Iniciamos maestro
    write_RTC();
    initTimer1();
    
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
    mostrar(0xDF); // Simbolo de ° grado
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
    
    iniciarUART();      // Iniciamos UART
    
    i = 0;      // Inicializamos contador en 0
    
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
        
        // Recibo el valor del sensor de temperatura. PIC5 = Reo5
        I2C_Master_Start();         // Iniciamos comunicacion
        I2C_Master_Write(0x51);     // Llamamos al REO1 y le indicamos que lo leeremos
        Temp = I2C_Master_Read(0);  // Guardamos en luz el valor del ADC enviado por el PIC1
        I2C_Master_Stop();          // Detenemos I2C. No mas lectura del PIC2
        __delay_ms(100);
        
        // Ejecucion de cada metodo. Conversion de valores y activadores.
        luces();
        celda();
        ultrasonico();
        humo();
        temperatura();
        
        // Prueba RTC
        if (minute == 8)
        {
            dir = 0;
            enable = 0;
            __delay_ms(200);
            
        } 
        else if (minute == 10)
        {
            dir = 1;
            enable = 0;  
            __delay_ms(200);
        }
        else
        {
          enable = 1;
        }
              
        minute = read_RTC(1);
        hour = read_RTC(2);
        hr = hour & 0b00111111;
        
        time[0] = MSB(hr);
        time[1] = LSB(hr);
        time[3] = MSB(minute);
        time[4] = LSB(minute);
        
        colocar(29,2);
        mostrar(time[0]);
        colocar(30,2);
        mostrar(time[1]);
        colocar(32,2);
        mostrar(time[3]);
        colocar(33,2);
        mostrar(time[4]);
        
        __delay_ms(50);
        
        // Movimiento de la pantalla
        shift();
        
        // Envia el valor del contador i
        UARTenvINT(i);
        __delay_ms(10);
        
        // Envia los valores del arreglo b[], que son los valores de los sensores
        UARTenvINT(b[i]);
        __delay_ms(10);
        
        // Incremento del contador
        i++;
        
        // Si el contador es igual a 5 es porque ya envio todos los valores
        // Lo reiniciamos para volver a recorrer el arreglo b[]
        if (i == 5)
        {
            i = 0;
        }
    }
}

char read_RTC(char address)
{
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(address);
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        dat = I2C_Master_Read(0);  // lee lo que el esclavo este enviando
        I2C_Master_Stop();
        return dat;
}

void write_RTC(void)
{
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x00);
        
        I2C_Master_Write(0b00010001);
        I2C_Master_Write(0b00010001);
        I2C_Master_Write(0b00000001);
        
        I2C_Master_Write(0b00000001);
        I2C_Master_Write(0b00000001);
        I2C_Master_Write(0b00000011);
        I2C_Master_Write(0b00100000);
        I2C_Master_Stop();
}

unsigned char MSB(unsigned char x)           //Display Most Significant Bit of BCD number
{
  return ((x >> 4) + '0');
}

unsigned char  LSB(unsigned char x)          //Display Least Significant Bit of BCD number
{
  return ((x & 0x0F) + '0');
}
void initTimer1(void)
{
    T1CONbits.T1GINV = 0;
    T1CONbits.TMR1GE = 0;       // Always counting
    T1CONbits.T1CKPS0 = 1;      // Prescaler 1:8
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1OSCEN = 0;      // LP oscillator is off
    T1CONbits.TMR1CS = 0;       // Reloj Fosc/4 modo temporizador
    T1CONbits.TMR1ON = 1;       // Timer 1 ON
    
    // T1CON = 0x21;            // Otras formas de haberlo configurado directamente
    // T1CON = 33;
    // T1CON = 0b00010001;
    
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.TMR1IF = 0;        // Limpiamos la bandera de interrupción TMR1
    PIE1bits.TMR1IE = 1;        // Habilitamos interrupción TMR1
    
    // valor a cargar al TMR1 5536 -> 0x15A0
    TMR1H = 0xF4;
    TMR1L = 0x02;
}