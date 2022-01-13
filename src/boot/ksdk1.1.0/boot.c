#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"
#include "fsl_lpuart_driver.h"
#include "fsl_gpio_driver.h"
#include "warp.h"
#include "errstrs.h"
#include "gpio_pins.h"
#include "SEGGER_RTT.h"
/* include device SSD1331 and MMA8451Q */
#include "devSSD1331.h"
#include "devMMA8451Q.h"
volatile WarpI2CDeviceState			deviceMMA8451QState;
/* definition of lights turn on and off */
#define ON 1
#define OFF 0
volatile i2c_master_state_t				i2cMasterState;
volatile spi_master_state_t				spiMasterState;
volatile spi_master_user_config_t			spiUserConfig;

volatile uint32_t					gWarpSpiBaudRateKbps			= kWarpDefaultSpiBaudRateKbps;
char							gWarpPrintBuffer[kWarpDefaultPrintBufferSizeBytes];
volatile uint16_t					gWarpCurrentSupplyVoltage		= kWarpDefaultSupplyVoltageMillivolts;
volatile uint32_t					gWarpI2cBaudRateKbps			= kWarpDefaultI2cBaudRateKbps;

static void						lowPowerPinStates(void);

/* pins used in the system*/
enum
{
	SW2			= GPIO_MAKE_PIN(HW_GPIOB,0),	//input of turning on lights
	SW3			= GPIO_MAKE_PIN(HW_GPIOB,5),	//input of turning off lights
	A3144Input		= GPIO_MAKE_PIN(HW_GPIOB, 13), //input of hall effect sensor
	LightON		= GPIO_MAKE_PIN(HW_GPIOB, 7),	//output of lights control
	BrakeLight		= GPIO_MAKE_PIN(HW_GPIOA, 5),	//output of brakelights control
	TAP			= GPIO_MAKE_PIN(HW_GPIOA, 12)	//input of double-tap
};

clock_manager_error_code_t clockManagerCallbackRoutine(clock_notify_struct_t *  notify, void *  callbackData);

/*
 *	static clock callback table.
 */
clock_manager_callback_user_config_t		clockManagerCallbackUserlevelStructure =
									{
										.callback	= clockManagerCallbackRoutine,
										.callbackType	= kClockManagerCallbackBeforeAfter,
										.callbackData	= NULL
									};

static clock_manager_callback_user_config_t *	clockCallbackTable[] =
									{
										&clockManagerCallbackUserlevelStructure
									};

clock_manager_error_code_t
clockManagerCallbackRoutine(clock_notify_struct_t *  notify, void *  callbackData)
{
	clock_manager_error_code_t result = kClockManagerSuccess;

	switch (notify->notifyType)
	{
		case kClockManagerNotifyBefore:
			break;
		case kClockManagerNotifyRecover:
		case kClockManagerNotifyAfter:
			break;
		default:
			result = kClockManagerError;
		break;
	}

	return result;
}

void
lowPowerPinStates(void)
{
	/*
	 *	Following Section 5 of "Power Management for Kinetis L Family" (AN5088.pdf),
	 *	we configure all pins as output and set them to a known state. We choose
	 *	to set them all to '0' since it happens that the devices we want to keep
	 *	deactivated (SI4705) also need '0'.
	 */

	/*
	 *			PORT A
	 */
	/*
	 *	For now, don't touch the PTA0/1/2 SWD pins. Revisit in the future.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 0, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 1, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 2, kPortMuxAlt3);

	/*
	 *	PTA3 and PTA4 are the EXTAL0/XTAL0. They are also connected to the clock output
	 *	of the RV8803 (and PTA4 is a sacrificial pin for PTA3), so do not want to drive them.
	 *	We however have to configure PTA3 to Alt0 (kPortPinDisabled) to get the EXTAL0
	 *	functionality.
	 *
	 *	NOTE:	kPortPinDisabled is the equivalent of `Alt0`
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 3, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTA_BASE, 4, kPortPinDisabled);

	/*
	 *	Disable PTA5
	 *
	 *	NOTE: Enabling this significantly increases current draw
	 *	(from ~180uA to ~4mA) and we don't need the RTC on revC.
	 *
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 5, kPortPinDisabled);

	/*
	 *	Section 2.6 of Kinetis Energy Savings – Tips and Tricks says
	 *
	 *		"Unused pins should be configured in the disabled state, mux(0),
	 *		to prevent unwanted leakage (potentially caused by floating inputs)."
	 *
	 *	However, other documents advice to place pin as GPIO and drive low or high.
	 *	For now, leave disabled. Filed issue #54 low-power pin states to investigate.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTA_BASE, 8, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9, kPortPinDisabled);

	/*
	 *	NOTE: The KL03 has no PTA10 or PTA11
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 12, kPortPinDisabled);


	/*
	 *			PORT B
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 1, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 2, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 5, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 6, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 7, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 10, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 11, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 13, kPortPinDisabled);
}
void
warpScaleSupplyVoltage(uint16_t voltageMillivolts)
{
	if (voltageMillivolts == gWarpCurrentSupplyVoltage)
	{
		return;
	}
}
void
warpEnableSPIpins(void)
{
	CLOCK_SYS_EnableSpiClock(0);

	/*	kWarpPinSPI_MISO_UART_RTS_UART_RTS --> PTA6 (ALT3)	*/
	PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortMuxAlt3);

	/*	kWarpPinSPI_MOSI_UART_CTS --> PTA7 (ALT3)	*/
	PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortMuxAlt3);


		/*	kWarpPinSPI_SCK	--> PTB0	(ALT3)		*/
		PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortMuxAlt3);

	/*
	 *	Initialize SPI master. See KSDK13APIRM.pdf Section 70.4
	 */
	uint32_t			calculatedBaudRate;
	spiUserConfig.polarity		= kSpiClockPolarity_ActiveHigh;
	spiUserConfig.phase		= kSpiClockPhase_FirstEdge;
	spiUserConfig.direction	= kSpiMsbFirst;
	spiUserConfig.bitsPerSec	= gWarpSpiBaudRateKbps * 1000;
	SPI_DRV_MasterInit(0 /* SPI master instance */, (spi_master_state_t *)&spiMasterState);
	SPI_DRV_MasterConfigureBus(0 /* SPI master instance */, (spi_master_user_config_t *)&spiUserConfig, &calculatedBaudRate);
}
void
warpEnableI2Cpins(void)
{
	CLOCK_SYS_EnableI2cClock(0);

	/*
	 *	Setup:
	 *
	 *		PTB3/kWarpPinI2C0_SCL_UART_TX	-->	(ALT2 == I2C)
	 *		PTB4/kWarpPinI2C0_SDA_UART_RX	-->	(ALT2 == I2C)
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortMuxAlt2);
	PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortMuxAlt2);

	I2C_DRV_MasterInit(0 /* I2C instance */, (i2c_master_state_t *)&i2cMasterState);
}


void
warpPrint(const char *fmt, ...)
{
	int	fmtlen;
	va_list	arg;

	/*
	 *	We use an ifdef rather than a C if to allow us to compile-out
	 *	all references to SEGGER_RTT_*printf if we don't want them.
	 *
	 *	NOTE: SEGGER_RTT_vprintf takes a va_list* rather than a va_list
	 *	like usual vprintf. We modify the SEGGER_RTT_vprintf so that it
	 *	also takes our print buffer which we will eventually send over
	 *	BLE. Using SEGGER_RTT_vprintf() versus the libc vsnprintf saves
	 *	2kB flash and removes the use of malloc so we can keep heap
	 *	allocation to zero.
	 */
	#if (WARP_BUILD_ENABLE_SEGGER_RTT_PRINTF)
		/*
		 *	We can't use SEGGER_RTT_vprintf to format into a buffer
		 *	since SEGGER_RTT_vprintf formats directly into the special
		 *	RTT memory region to be picked up by the RTT / SWD mechanism...
		 */
		va_start(arg, fmt);
		fmtlen = SEGGER_RTT_vprintf(0, fmt, &arg, gWarpPrintBuffer, kWarpDefaultPrintBufferSizeBytes);
		va_end(arg);

		if (fmtlen < 0)
		{
			SEGGER_RTT_WriteString(0, gWarpEfmt);

			return;
		}

	#else
		/*
		 *	If we are not compiling in the SEGGER_RTT_printf,
		 *	we just send the format string of warpPrint()
		 */
		SEGGER_RTT_WriteString(0, fmt);

	#endif

	return;
}

/* initialization of the system */
void	Sys_init()
{	
	/*
	 *	Enable clock for I/O PORT A and PORT B
	 */
	CLOCK_SYS_EnablePortClock(0);
	CLOCK_SYS_EnablePortClock(1);
	/*
	 *	Set board crystal value (Warp revB and earlier).
	 */
	g_xtal0ClkFreq = 32768U;
	/*
	 *	Setup SEGGER RTT to output as much as fits in buffers.
	 *
	 *	Using SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL can lead to deadlock, since
	 *	we might have SWD disabled at time of blockage.
	 */
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
	/*
	 *	Configure Clock Manager to default, and set callback for Clock Manager mode transition.
	 *
	 *	See "Clocks and Low Power modes with KSDK and Processor Expert" document (Low_Power_KSDK_PEx.pdf)
	 */
	CLOCK_SYS_Init(	g_defaultClockConfigurations,
			CLOCK_CONFIG_NUM, /* The default value of this is defined in fsl_clock_MKL03Z4.h as 2 */
			&clockCallbackTable,
			ARRAY_SIZE(clockCallbackTable)
			);
	CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_RUN, kClockManagerPolicyForcible);
	/*
	 *	Switch CPU to Very Low Power Run (VLPR) mode
	 */
	warpSetLowPowerMode(kWarpPowerModeVLPR, 0 /* Sleep Seconds */);
	/*
	 *	Initialize the GPIO pins with the appropriate pull-up, etc.,
	 *	defined in the inputPins and outputPins arrays (gpio_pins.c).
	 *
	 *	See also Section 30.3.3 GPIO Initialization of KSDK13APIRM.pdf
	 */
	GPIO_DRV_Init(inputPins  /* input pins */, outputPins  /* output pins */);

	/*
	 *	Make sure the SWD pins, PTA0/1/2 SWD pins in their ALT3 state (i.e., as SWD).
	 *
	 *	See GitHub issue https://github.com/physical-computation/Warp-firmware/issues/54
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 0, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 1, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 2, kPortMuxAlt3);

	/*
	 *	Note that it is lowPowerPinStates() that sets the pin mux mode,
	 *	so until we call it pins are in their default state.
	 */
	lowPowerPinStates();
	/* initialization of device SSD1331 */
	devSSD1331init();
	/* set pins of inputs and outputs used by the system */
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio); //SW2
	PORT_HAL_SetMuxMode(PORTB_BASE, 5u, kPortMuxAsGpio); //SW3
	PORT_HAL_SetMuxMode(PORTB_BASE, 13u, kPortMuxAsGpio);//A3144input
	PORT_HAL_SetMuxMode(PORTB_BASE, 7u, kPortMuxAsGpio);//lignton
	PORT_HAL_SetMuxMode(PORTA_BASE, 5u, kPortMuxAsGpio);//brakelight
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAlt3);//tap
	
	GPIO_DRV_ClearPinOutput(LightON);	
	GPIO_DRV_ClearPinOutput(BrakeLight);

	OSA_Init();
	/* initialization of device MMA8451Q */
	initMMA8451Q(	0x1D	/* i2cAddress */,kWarpDefaultSupplyVoltageMillivoltsMMA8451Q	);
	doubletap();
};

int main(void)
{
	Sys_init();
	int prespeed = 0;
	int speed = 0;
	int predistance=0;
	int distance = 0;
	int time;
	uint8_t tap=0;
	
	while(1)
	{
	/* 
	detect double-tap
	if double-tap, tap+1
	*/
	if(!GPIO_DRV_ReadPinInput(TAP))
	{	
		OSA_TimeDelay(100);
		tap++;
	}
	//detect input of Hall effect sensor, input =1 means the magnet pass, distance +2m
	if (!GPIO_DRV_ReadPinInput(A3144Input))
			{distance += 2;
			OSA_TimeDelay(100);}
	
	//detect input of lights controls, switch on/off lights
	if (!GPIO_DRV_ReadPinInput(SW2))
		{
		GPIO_DRV_SetPinOutput(LightON);
		displaylight(ON);
		}
	else if (!GPIO_DRV_ReadPinInput(SW3))
		{
		GPIO_DRV_ClearPinOutput(LightON);	
		displaylight(OFF);
		}	
	//calculate spped and display speed/distance
	time =OSA_TimeGetMsec();
	if (time>3000)//display refreshes every 3 seconds. 
	{
	OSA_Init();
	speed = (distance-predistance)/(time/1000)*360; //change m/s to km/hr
	predistance = distance;
	
	if(!(tap%2))
		displaydistance(distance);
	else
		displayspeed(speed);	
	
	if(prespeed >speed +3)//slow down or not
		{
		GPIO_DRV_SetPinOutput(BrakeLight);
		brakelight(ON);
		}
	else
		{
		GPIO_DRV_ClearPinOutput(BrakeLight);
		brakelight(OFF);
		}
	prespeed = speed;
	}
	}
	return 0;
}
