
#include <stdint.h>
#include <pic16f887.h>

void iniciarOSC(uint8_t frec)
{
    switch(frec)
    {
        case 0:
            OSCCONbits.IRCF = 0b000;    // 31kHz
            break;
            
        case 1:
            OSCCONbits.IRCF = 0b001;    // 125kHz
            break;
            
        case 2:
            OSCCONbits.IRCF = 0b010;    // 250kHz
            break;
            
        case 3:
            OSCCONbits.IRCF = 0b001;    // 500kHz
            break;
            
        case 4:
            OSCCONbits.IRCF = 0b100;    // 1MHz
            break;
            
        case 5:
            OSCCONbits.IRCF = 0b101;    // 2MHz
            break;
            
        case 6:
            OSCCONbits.IRCF = 0b110;    // 4MHz
            break;
            
        case 7:
            OSCCONbits.IRCF = 0b111;    // 8MHz
            break;
            
        default:
            OSCCONbits.IRCF = 0b110;    // 4MHz
            break;
 
    }
    
    OSCCONbits.SCS = 1;
}
