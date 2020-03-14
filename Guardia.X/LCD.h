/*
 * Archivo: LCDon.h
 * 
 * Autor: Rodrigo Corona
 * Carne: 15102
 * Fecha: 10/02/2020
 * 
 * Descripción: Librería para configurar y usar la LCD.
 * 
 */

#define RS PORTDbits.RD6
#define LCDP PORTB
#define EN PORTDbits.RD7

void comando(const char x)
{
    RS = 0; 
    LCDP = x;
    EN = 1;
    __delay_ms(4);
    EN = 0;
}

void iniciarLCD(void)
{
    RS = 0;
       
    comando(0);
    
    __delay_ms(15);
    
    comando(0x30);
    __delay_ms(4);
    
    comando(0x30);
    __delay_us(160);
    
    comando(0x30);
    __delay_us(160);
    
    comando(0x38);
    
    comando(0x10);
    
    comando(0x01);
    
    comando(0x06);
    
    comando(0x0F);
    
}

void borrarv(void)
{
    comando(0);
    comando(1);
}

void colocar(const char x,const char y)
{
    char temp;
    
    if (y == 1)
    {
        temp = 0x80 + x - 1;
        comando(temp);
    }
    else if (y == 2)
    {
        temp = 0xC0 + x - 1;
        comando(temp);
    }
}

void imprimir(const char *texto)
{
    for(int i = 0; texto[i] != '\0'; i++)
    {        
        RS = 1;
        LCDP = texto[i];
        EN = 1;
        __delay_ms(2);
        EN = 0;
    }
}

void mostrar(const char t)
{
    RS = 1;
    LCDP = t;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}