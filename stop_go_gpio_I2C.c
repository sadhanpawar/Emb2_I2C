// Stop Go C Example (Basic)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red LED:
//   PF1 drives an NPN transistor that powers the red LED
// Green LED:
//   PF3 drives an NPN transistor that powers the green LED
// Pushbutton:
//   SW1 pulls pin PF4 low (internal pull-up is used)

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "stop_go_I2C.h"
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "gpio.h"
#include "wait.h"
#include "nvic.h"
#include "i2c0.h"

// Pins
#define RED_LED PORTF,1
#define GREEN_LED PORTF,3
#define PUSH_BUTTON PORTF,4
#define I2C0_INT PORTB, 6

#define I2C_ADDR    (0x20)

#define IO_EXP_LED1()     0x09, 0x01
#define IO_EXP_LED2()     0x09, 0x02
#define IO_EXP_EN_INTR()  0x02, 0x10

/* Bank 0 */
static uint8_t mcp2308Configuration[][3] = {
                                        {I2C_ADDR, 0x05,0x02}, /* IOCON: 0x00110010 */
                                        {I2C_ADDR, 0x00,0x10}, /* IODIRA 5th pin Ip, 0-3 Ops*/
                                        {I2C_ADDR, 0x01,0x00}, /* IPOLA  same logic */
                                        {I2C_ADDR, 0x06,0x10}, /* GPPU: pull ups for inputs */
                                        {I2C_ADDR, 0x03,0x10}, /* DEFVAL: enabled for inputs(pull down) */
                                        {I2C_ADDR, 0x04,0x10}, /* INTCON: */

};


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------


// Blocking function that returns only when SW1 is pressed
void waitPbPress()
{
    uint32_t data = 0;

    while( true ) /*1st bit */
    {
        data = readI2c0Register(I2C_ADDR,0x09);
        data = (data & 0xF0);
        if(data == false ) {
            break;
        }
    }
}

// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

    // Enable clocks
    enablePort(PORTF);

    // Configure LED and pushbutton pins
    selectPinPushPullOutput(GREEN_LED);
    selectPinPushPullOutput(RED_LED);
    selectPinDigitalInput(PUSH_BUTTON);
    enablePinPullup(PUSH_BUTTON);
    

    /* enable clocks */
    enablePort(PORTB);
    selectPinDigitalInput(I2C0_INT);
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    uint8_t count = 10;

	// Initialize hardware
	initHw();

	configI2c0Mcp2308();

	writeI2c0Register(I2C_ADDR,IO_EXP_LED1());

	/* wait till push button is pressed */
	waitPbPress();

	writeI2c0Register(I2C_ADDR,IO_EXP_LED2());

	/*clear the interrupt in spi slave dev */
	while( count > 0 ) {
	    (void)readI2c0Register(I2C_ADDR,0x09);
	    count--;
	}

	waitMicrosecond(1000000);

    clearPinInterrupt(I2C0_INT);

    /* enable the nvic gpio port B interrupt */
    enableNvicInterrupt(INT_GPIOB);
    enablePinInterrupt(I2C0_INT);
    selectPinInterruptRisingEdge(I2C0_INT);

    /* enable interrupt in slave device */
    writeI2c0Register(I2C_ADDR,IO_EXP_EN_INTR());

    /*write to led */
    writeI2c0Register(I2C_ADDR,IO_EXP_LED1());

    // Endless loop
    while(true);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

void configI2c0Mcp2308(void)
{
    /* Configure I2C module with SW controlled FSS */
    initI2c0();

    writeI2c0RegData();
}

void writeI2c0RegData(void)
{
    
    uint8_t i;

    for( i = 0; i < sizeof(mcp2308Configuration)/sizeof(mcp2308Configuration[0]) ;i++ )
    {
        
        writeI2c0Register(mcp2308Configuration[i][0],
                                mcp2308Configuration[i][1],
                                mcp2308Configuration[i][2]);

        waitMicrosecond(100000);
    }

    /*clear the interrupts if there are any */
    (void)readI2c0Register(I2C_ADDR,0x09);
}

void mcpGpioPortBIntHandler(void)
{
    /*clear the interrupt in spi slave dev */
    (void)readI2c0Register(I2C_ADDR,0x09);

    /* toggle the led */
    writeI2c0Register(I2C_ADDR,IO_EXP_LED2());

    clearPinInterrupt(I2C0_INT);
}
