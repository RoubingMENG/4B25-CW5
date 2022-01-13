#include <stdint.h>

/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"
#include "fsl_gpio_driver.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"


volatile uint8_t	inBuffer[1];
volatile uint8_t	payloadBytes[1];

enum
{	
	kSSD1331PinMOSI	= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 10),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOB, 6),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 11),
};
void
writeCommand(uint8_t commandByte)
{
	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);
	payloadBytes[0] = commandByte;
	SPI_DRV_MasterTransfer(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[0],
					(uint8_t * restrict)&inBuffer[0],
					1		/* transfer size */);
	
}
void SetPosition(int x,int y,int wid,int len)
{
	
	writeCommand(0x15);
	writeCommand(x);
	writeCommand(x+wid-1);
	writeCommand(0x75);
	writeCommand(y);
	writeCommand(y+len-1);
}

void displayspeed(int speed)
{
	int a;
	int i,m;
	int j=0;
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x07);
	writeCommand(0x5F);
	writeCommand(0x3F);
	writeCommand(kSSD1331CommandDISPLAYOFF);
	//SPEED symbol
	SetPosition(70,0,24,6);
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&SPEED,
				(uint8_t * restrict)&inBuffer[0],
				288		/* transfer size */);
	//km/hr symbol
	SetPosition(50,50,39,13);
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	for (m=0;m<3;m++)
	SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&KM+m*338,
				(uint8_t * restrict)&inBuffer[0],
				338		/* transfer size */);
	for (i=0;i<4;i++)
	{
	a=speed % 10;
	if (i==2)
	{	//show point
		SetPosition(38,42,3,3);
		GPIO_DRV_SetPinOutput(kSSD1331PinDC);
		SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&point,
				(uint8_t * restrict)&inBuffer[0],
				18		/* transfer size */);
		j=5;
	}
	//show number
	SetPosition(60-17*i-j,10,15,35);
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	for(m=0;m<3;m++)
	SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&NB[a][0][0]+m*350,
				(uint8_t * restrict)&inBuffer[0],
				350		/* transfer size */);
	speed/=10;
	}
	writeCommand(kSSD1331CommandDISPLAYON);
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
}

void displaydistance(int distance)
{	
	int a;
	int i,m;
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x07);
	writeCommand(0x5F);
	writeCommand(0x3F);
	writeCommand(kSSD1331CommandDISPLAYOFF);
	SetPosition(70,0,24,6);
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&DISTA,
				(uint8_t * restrict)&inBuffer[0],
				288		/* transfer size */);
	SetPosition(80,50,9,13);
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&MS,
				(uint8_t * restrict)&inBuffer[0],
				234		/* transfer size */);
	for (i=0;i<5;i++)
	{
	a= distance% 10;
	SetPosition(72-17*i,10,15,35);
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	for(m=0;m<3;m++)
	SPI_DRV_MasterTransfer(0	/* master instance */,
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict)&NB[a][0][0]+m*350,
				(uint8_t * restrict)&inBuffer[0],
				350		/* transfer size */);
	distance/=10;
	if(!distance)
	break;
	}
	writeCommand(kSSD1331CommandDISPLAYON);
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
}

int brakelight(int a)
{	
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);
	if (a)
	{
		SetPosition(8,0,5,6);
		GPIO_DRV_SetPinOutput(kSSD1331PinDC);
		SPI_DRV_MasterTransfer(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)(&lightRed),
					(uint8_t * restrict)&inBuffer[0],
					60		/* transfer size */);
	}
		
	else
	{
		writeCommand(kSSD1331CommandCLEAR);
		writeCommand(0x08);
		writeCommand(0x00);
		writeCommand(0x0D);
		writeCommand(0x06);
	}
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	return 0;	
}

int displaylight(int a)
{
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);
	if (a)
	{
		SetPosition(0,0,5,6);
		GPIO_DRV_SetPinOutput(kSSD1331PinDC);
		SPI_DRV_MasterTransfer(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)(&lightYellow),
					(uint8_t * restrict)&inBuffer[0],
					60		/* transfer size */);
	}
	else
	{
		writeCommand(kSSD1331CommandCLEAR);
		writeCommand(0x00);
		writeCommand(0x00);
		writeCommand(0x05);
		writeCommand(0x06);
	}	
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	return 0;
}

int
devSSD1331init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	warpEnableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 10u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 6u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 11u, kPortMuxAsGpio);


	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);
	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	writeCommand(kSSD1331CommandDISPLAYOFF);	// 0xAE
	writeCommand(kSSD1331CommandSETREMAP);	// 0xA0
	writeCommand(0x73);				// RGB Color,vertical 
	writeCommand(kSSD1331CommandSTARTLINE);	// 0xA1
	writeCommand(0x0);
	writeCommand(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	writeCommand(0x0);
	writeCommand(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	writeCommand(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	writeCommand(0x3F);				// 0x3F 1/64 duty
	writeCommand(kSSD1331CommandSETMASTER);	// 0xAD
	writeCommand(0x8E);
	writeCommand(kSSD1331CommandPOWERMODE);	// 0xB0
	writeCommand(0x0B);
	writeCommand(kSSD1331CommandPRECHARGE);	// 0xB1
	writeCommand(0x11);
	writeCommand(kSSD1331CommandCLOCKDIV);	// 0xB3
	writeCommand(0xF0);				// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8A
	writeCommand(0x80);
	writeCommand(kSSD1331CommandPRECHARGEB);	// 0x8B
	writeCommand(0x80);
	writeCommand(kSSD1331CommandPRECHARGEC);	// 0x8C
	writeCommand(0x80);
	writeCommand(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandVCOMH);		// 0xBE
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandMASTERCURRENT);	// 0x87
	writeCommand(0x0F);
	writeCommand(kSSD1331CommandCONTRASTA);	// 0x81
	writeCommand(0x80);
	writeCommand(kSSD1331CommandCONTRASTB);	// 0x82
	writeCommand(0x80);
	writeCommand(kSSD1331CommandCONTRASTC);	// 0x83
	writeCommand(0x80);
	writeCommand(kSSD1331CommandDISPLAYON);	// Turn on oled panel

	/*
	 *	To use fill commands, you will have to issue a command to the display to enable them. See the manual.
	 */
	writeCommand(kSSD1331CommandFILL);
	writeCommand(0x01);

	/*
	 *	Clear Screen
	 */
	
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);


	/*
	 *	Any post-initialization drawing commands go here.
	 */


	return 0;
}
