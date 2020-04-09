/*
 * 
 *  Autor:  Rodrigo Corona
 *  Carne:  15102
 *  Fecha:  14/02/2020 5:28 pm
 * 
 *  Descripcion: Libreria para conexion UART
 * 
 */

// Inicializacion de la comunicacion UART.

void iniciarUART(void)
{
    TXSTAbits.TXEN = 1;     // Habilita el circuito transmisor
    TXSTAbits.SYNC = 0;     // Modo asincrono
    RCSTAbits.SPEN = 1;     // Habilita el EUSART y configura automaticamente TX/CK como salida
    TXSTAbits.TX9 = 0;      // 8 bits
    RCSTAbits.CREN = 1;     // Habilita la recepcion continua de datos.
    
    SPBRG = 25;             // Segun pagina 165, tabla #1, frecuencia de 4MHz, Baud Rate = 9,600 y SPBRG = 25.
    SPBRGH = 0;             // No importa que valor tome, pues esta conectado a un AND con BRG16 y este es 0.
    BAUDCTLbits.BRG16 = 0;  // Ver página 163 y 164.
    TXSTAbits.BRGH = 1;     // Segun pagina 165, tabla #1, frecuencia de 4MHz, Baud Rate = 9,600 y SPBRG = 25.
}

/*uint8_t UARTrecibir(void)
{
    while(PIR1bits.RCIF == 0)
    {
        // Esperamos a recibir informacion
    }
    
    if(RCSTAbits.OERR)      // Si hay un OVERRUN
    {
        RCSTAbits.CREN = 0; // Limpiamos los bits de error
        RCSTAbits.CREN = 1;
    }
    
    return RCREG;    
}

void UARTenviar(char data)
{
    while (TXSTAbits.TRMT == 0)
    {
        // Esperamos a que el registro TSR este vacio
    }
    
    TXREG = data;   // Cargamos la informacion al registro
}

void UARTprint(char *data)
{
    uint8_t i = 0;
    
    while(data[i] != '\0')
    {
        UARTenviar(data[i++]);
    }
}*/

void UARTmostrar(double x)
{
    while (TXSTAbits.TRMT == 0)
    {
        // Esperamos a que el registro TSR este vacio
    }
    
    TXREG = x;
}