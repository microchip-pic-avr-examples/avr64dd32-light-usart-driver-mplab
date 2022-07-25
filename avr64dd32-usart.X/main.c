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

//LED Functions
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
        if (ringBuffer_find(&ringBuffer, "\r\n"))
        {
            //We may have received a command
            
            //Match Command String
            //The "\r\n" sequence at the end ensures an exact match
            if (ringBuffer_find(&ringBuffer, "HELP\r\n"))
            {
                USART_sendString("Type HELP, LED ON, LED BLINK, or LED OFF followed by ENTER\r\n");
            }
            if (ringBuffer_find(&ringBuffer, "LED ON\r\n"))
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
            
            //Advance to end of the new line
            ringBuffer_advanceToString(&ringBuffer, "\r\n");
        }
    }
}
