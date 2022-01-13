## Overview
In `boot.c`, the `main` is the function to realize the project. After initialized the system, in the loop `while(1)`, we first read the input of MMA8451Q to detect the double-tap. Then, we read the input of A3144 to detect the magnet to get the increase of distance. Also, we read ths input of SW2 and SW3 to control lights. The next step, we calculate speed and dsitance and send the data to functions in `devSSD1331.*` to display. The display is determined by the number of tap. Finally, we compare the speed with the previous to control the brake lights.

The drivers for the individual sensors are in `devXXX.c` for sensor `XXX`.

## Source File Descriptions
##### `CMakeLists.txt`
This is the CMake configuration file. Edit this to change the default size of the stack and heap.


##### `SEGGER_RTT.*`
This is the implementation of the SEGGER Real-Time Terminal interface. Do not modify.

##### `SEGGER_RTT_Conf.h`
Configuration file for SEGGER Real-Time Terminal interface. You can increase the size of `BUFFER_SIZE_UP` to reduce text in the menu being trimmed.

##### `SEGGER_RTT_printf.c`
Implementation of the SEGGER Real-Time Terminal interface formatted I/O routines. Do not modify.

##### `devSSD1331.*`
Driver for OLED

##### `devMMA8451Q.*`
Driver for MMA8451Q.

##### `gpio_pins.c`
Definition of I/O pin configurations using the KSDK `gpio_output_pin_user_config_t` structure.

##### `gpio_pins.h`
Definition of I/O pin mappings and aliases for different I/O pins to symbolic names relevant to the Warp hardware design, via `GPIO_MAKE_PIN()`.

##### `startup_MKL03Z4.S`
Initialization assembler.

##### `warp.h`
Constant and data structure definitions.
