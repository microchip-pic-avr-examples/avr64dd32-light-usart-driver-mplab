/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "USART0.h"
#include "ringBuffer.h"
#include "usart_utilities.h"

FUSES = {
	.WDTCFG = 0x00, // WDTCFG {PERIOD=OFF, WINDOW=OFF}
	.BODCFG = 0x00, // BODCFG {SLEEP=DISABLE, ACTIVE=DISABLE, SAMPFREQ=128Hz, LVL=BODLEVEL0}
	.OSCCFG = 0x00, // OSCCFG {CLKSEL=OSCHF}
	.SYSCFG0 = 0xD8, // SYSCFG0 {EESAVE=CLEAR, RSTPINCFG=RST, UPDIPINCFG=UPDI, CRCSEL=CRC16, CRCSRC=NOCRC}
	.SYSCFG1 = 0x0C, // SYSCFG1 {SUT=8MS, MVSYSCFG=DUAL}
	.CODESIZE = 0x00, // CODESIZE {CODESIZE=User range:  0x0 - 0xFF}
	.BOOTSIZE = 0x00, // BOOTSIZE {BOOTSIZE=User range:  0x0 - 0xFF}
};

LOCKBITS = 0x5CC5C55C; // {KEY=NOLOCK}

//Size of Ring Buffer
#define RING_BUFFER_SIZE 255

//Memory for Ring Buffer
static char textBuffer[RING_BUFFER_SIZE];

//Ring Buffer Structure
static RingBuffer ringBuffer;

//Function to Load Data Into Ring Buffer
void loadCharacterToBuffer(char c)
{
    ringBuffer_loadCharacter(&ringBuffer, c);
}

//LED GPIO Functions
#define LED_init() do { PORTF.OUTSET = PIN5_bm; PORTF.DIRSET = PIN5_bm; } while (0)
#define LED_turnOn() do { PORTF.OUTCLR = PIN5_bm; } while (0)
#define LED_turnOff() do { PORTF.OUTSET = PIN5_bm; } while (0)
#define LED_toggle() do { PORTF.OUTTGL = PIN5_bm; } while (0)

int main(void) {
    
    //Setup LED
    LED_init();
    
    //Setup USART
    USART0_initIO();
    USART0_init();
    
    //Enable TX and RX
    USART0_enableTX();
    USART0_enableRX();
    
    //Configure Function to Load into Ring Buffer
    USART0_setRXCallback(&loadCharacterToBuffer);
    
    //Init Ring Buffer
    ringBuffer_createBuffer(&ringBuffer, &textBuffer[0], RING_BUFFER_SIZE);
    
    //Print Welcome Message
    USART_sendString("Welcome to the LED Command Processor!\r\n");
    USART_sendString("Type HELP, LED ON, LED BLINK, or LED OFF followed by ENTER\r\n");
    
    //Enable Interrupts
    sei();
    
    while (1) {
        
        //Look for the Deliminator
        if (ringBuffer_find(&ringBuffer, "\r\n"))
        {
            //We may have received a command
            
            //Match Command String
            //The "\r\n" sequence at the end ensures an exact match
            if (ringBuffer_find(&ringBuffer, "HELP\r\n"))
            {
                USART_sendString("Type HELP, LED ON, LED TOGGLE, or LED OFF followed by ENTER\r\n");
            }
            else if (ringBuffer_find(&ringBuffer, "LED ON\r\n"))
            {
                USART_sendString("LED turned on\r\n");
                LED_turnOn();
            }
            else if (ringBuffer_find(&ringBuffer, "LED TOGGLE\r\n"))
            {
                USART_sendString("LED toggled\r\n");
                LED_toggle();
            }
            else if (ringBuffer_find(&ringBuffer, "LED OFF\r\n"))
            {
                USART_sendString("LED turned off\r\n");
                LED_turnOff();
            }
            else
            {
                USART_sendString("Invalid Command Received\r\n");
            }
            
            //Advance the buffer to the newline character
            ringBuffer_advanceToString(&ringBuffer, "\r\n");
        }
    }
}
