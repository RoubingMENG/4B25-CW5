/*
	Authored 2016-2018. Phillip Stanley-Marbell. Additional contributors,
	2018-onwards, see git log.

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	*	Redistributions of source code must retain the above
		copyright notice, this list of conditions and the following
		disclaimer.

	*	Redistributions in binary form must reproduce the above
		copyright notice, this list of conditions and the following
		disclaimer in the documentation and/or other materials
		provided with the distribution.

	*	Neither the name of the author nor the names of its
		contributors may be used to endorse or promote products
		derived from this software without specific prior written
		permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>

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

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"


extern volatile WarpI2CDeviceState	deviceMMA8451QState;
extern volatile uint32_t		gWarpI2cBaudRateKbps;



void
initMMA8451Q(const uint8_t i2cAddress, uint16_t operatingVoltageMillivolts)
{
	deviceMMA8451QState.i2cAddress			= i2cAddress;
	deviceMMA8451QState.operatingVoltageMillivolts	= operatingVoltageMillivolts;

	return;
}
void writeRegister(uint8_t deviceRegister, uint8_t payload)
{
	uint8_t		payloadByte[1], commandByte[1];
	i2c_device_t slave =
	{
		.address = deviceMMA8451QState.i2cAddress,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

	warpScaleSupplyVoltage(deviceMMA8451QState.operatingVoltageMillivolts);
	commandByte[0] = deviceRegister;
	payloadByte[0] = payload;
	warpEnableI2Cpins();

	I2C_DRV_MasterSendData(
							0 /* I2C instance */,
							&slave,
							commandByte,
							1,
							payloadByte,
							1);
}
int readRegister(uint8_t deviceRegister)
{
	uint8_t		cmdBuf[1] = {0xFF};
	i2c_device_t slave =
	{
		.address = deviceMMA8451QState.i2cAddress,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

	warpScaleSupplyVoltage(deviceMMA8451QState.operatingVoltageMillivolts);
	cmdBuf[0] = deviceRegister;
	warpEnableI2Cpins();

	I2C_DRV_MasterReceiveData(
							0 /* I2C peripheral instance */,
							&slave,
							cmdBuf,
							1,
							(uint8_t *)deviceMMA8451QState.i2cBuffer,
							1);
	int data = deviceMMA8451QState.i2cBuffer[0];
	return data;
}

void doubletap()
{
	writeRegister(0x2A, 0X08);
	writeRegister(0x21, 0x2A);
	writeRegister(0x23, 0x30);
	writeRegister(0x24, 0x30);
	writeRegister(0x25, 0x4F);
	writeRegister(0x26, 0x30);
	writeRegister(0x27, 0x50);
	writeRegister(0x28, 0x78);
	writeRegister(0x2D, 0x08);
	writeRegister(0x2E, 0x00);
	int data = readRegister(0x2A);
	data|=0x01;
	writeRegister(0x2A,data);
}

