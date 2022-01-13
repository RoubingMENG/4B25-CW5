

#define		WARP_BUILD_ENABLE_SEGGER_RTT_PRINTF		1
#define		WARP_BUILD_BOOT_TO_CSVSTREAM			0
#define		WARP_BUILD_BOOT_TO_VLPR				1
#define		WARP_BUILD_DISABLE_SUPPLIES_BY_DEFAULT		0

/*
 *	NOTE: The choice of WARP_BUILD_ENABLE_GLAUX_VARIANT is defined via the Makefile build rules.
 *	The commented line below should never be uncommented. It is just here to stress a point that
 *	you shouldn't try to enable the Glaux build in this way.
 */


/*
 *	Define this here to activate FRDMKL03-specific behavior. Unlike the Glaux
 *	build variant above, we currently require users wanting to build for the
 *	KL03 to manually set this here.
 */
#define		WARP_BUILD_ENABLE_FRDMKL03			1
#define		WARP_BUILD_ENABLE_DEVMMA8451Q			1


typedef enum
{
	/*
	 *	Speeds
	 */
	kWarpDefaultI2cBaudRateKbps				= 200,
	kWarpDefaultUartBaudRateBps				= 115200,
	kWarpDefaultSpiBaudRateKbps				= 10000,

	/*
	 *	Times
	 */
	kWarpDefaultSleeptimeSeconds				= 0,
	kWarpDefaultI2cTimeoutMilliseconds			= 5,
	kWarpDefaultUartTimeoutMilliseconds			= 1000,
	kWarpDefaultSpiTimeoutMicroseconds			= 5,
	kWarpDefaultMenuPrintDelayMilliseconds		= 10,
	kWarpDefaultSupplySettlingDelayMilliseconds		= 1,

	/*
	 *	Sizes
	*/
	kWarpDefaultPrintBufferSizeBytes			= 64,
	kWarpMemoryCommonSpiBufferBytes			= 64,
	kWarpSizesI2cBufferBytes				= 4,
	kWarpSizesSpiBufferBytes				= 7,
	kWarpSizesUartBufferBytes				= 8,
	kWarpSizesBME680CalibrationValuesCount		= 41,

	/*
	 *	Voltages
	 */
	kWarpDefaultSupplyVoltageMillivolts			= 1800,
	kWarpDefaultSupplyVoltageMillivoltsMMA8451Q		= 1800
} WarpDefaults;
