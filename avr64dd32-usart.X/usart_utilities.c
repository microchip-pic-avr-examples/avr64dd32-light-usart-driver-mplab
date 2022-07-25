#include "usart_utilities.h"

//Sends a constant string to the USART Driver
void USART_sendString(const char* str)
{
    uint8_t index = 0;
    while (str[index] != '\0')
    {
        if (USART_IS_READY())
        {
            USART_SEND(str[index]);
            index++;
        }
    }
}