#ifndef USART_UTILITIES_H
#define	USART_UTILITIES_H

#include "USART0.h"

#ifdef	__cplusplus
extern "C" {
#endif

//This is the byte-level TX Function used by sendString
#define USART_SEND(C) USART0_sendByte(C)
    
//This is the byte-level transmit function used by sendString
#define USART_IS_READY() USART0_canTransmit()
    
    //Sends a constant string to the USART Driver
    void USART_sendString(const char* str);

#ifdef	__cplusplus
}
#endif

#endif	/* USART_UTILITIES_H */

