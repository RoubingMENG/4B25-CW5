# NAME: Roubing MENG
# college: Girton
# CRSid: rm2053
This is the final project of 4B25 Embedded System. The project is based on Warp firmware. So this folder only contains modified files. You only need to download the extra repository [tools] from https://github.com/physical-computation/Warp-hardware/tree/master/tools.
The complete project can also be found at https://github.com/https://github.com/RoubingMENG/4B25-CW5/.

The project uses FRDM-KL03Z with three devices: 0.96 OLED, MMA8451Q 3-axis accelerometer and A3144 Hall effect sensor. 
The project is aimed to be installed on the bike to show the speed and distance of the ride, the system can also control the switch of lights. 

**Prerequisites:** You need an arm cross-compiler such as `arm-none-eabi-gcc` installed as well as a working `cmake` (installed, e.g., via `apt-get` on Linux or via [MacPorts](https://www.macports.org) on macOS). On Ubuntu, the package you need is `gcc-arm-none-eabi`. You will also need an installed copy of the SEGGER [JLink commander](https://www.segger.com/downloads/jlink/), `JlinkExe`, which is available for Linux, macOS, and Windows (here are direct links for downloading it for [macOS](https://www.segger.com/downloads/jlink/JLink_MacOSX.pkg), and [Linux tgz 64-bit](https://www.segger.com/downloads/jlink/JLink_Linux_x86_64.tgz)).

## 1.  Compiling the Warp firmware
First, edit [setup.conf](setup.conf) to set the variable `ARMGCC_DIR` and `JLINKPATH`. If your `arm-none-eabi-gcc` is in `/usr/local/bin/arm-none-eabi-gcc`, then you want to set  `ARMGCC_DIR` to `/usr/local`. In the following, this `README.md` will refer to the top of the repository as `$TREEROOT`.

Second, edit [`tools/scripts/warp.jlink.commands`](tools/scripts/warp.jlink.commands) to replace `<full-path-to-warp-firmware>` with the full path to your Warp firmware directory.

Third, build the Warp firmware by

	make warp

Fourth, load the Warp firmware to hardware by

	make load-warp

##2. The layout 
The project is currently all in `src/boot/ksdk1.1.0/`. The main function is in `boot.c`. And there are files for device OLED: `devSSD1331.*` and for device MMA8451Q: `devMMA8451Q.*`.
There is a more specific README.md about the layout of files and the comments of codes in `src/boot/ksdk1.1.0/`.
