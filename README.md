# PolyMPC example for STM32F7

This repository contains a test example for the Nucleo-F767ZI devboard.

## Build

Requirements:
- make
- [GNU ARM Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) (arm-none-eabi-gcc, etc.)

```bash
cd polympc-stm32f7/
make
```

The compiled firmware is placed in the `/build` directory.

## Upload the firmware

### With OpenOCD
This requires installing [openocd](http://openocd.org/) first.
```bash
openocd -f openocd.cfg -c "program build/ch.elf verify reset" -c "shutdown"
```

### With ST-Link
The ST-Link/v2 mounts as a usb drive.
To update the firmware one can simply copy the binary `/build/ch.bin` to the drive.
```bash
cp build/ch.bin /Volumes/NODE_F767ZI/
```

## Connect serial console
The ST-Link debugger of the Nucleo-F767ZI also provides a serial connection to the STM32F7 UART3 which is by default configured to 38400 baud.

We recommend using pyserial
```
pip install pyserial
python -m serial.tools.miniterm /dev/tty.usbmodemXXX 38400
```
Note: The tty device needs to be changed to e.g. `/dev/ttyUSB0` on Linux.

## Debug with GDB

First launch the OpenOCD GDB server:
```openocd -f "interface/stlink-v2-1.cfg" -f "target/stm32f7x.cfg"```

Then connect with GDB:
```arm-none-eabi-gdb --eval-command='target remote localhost:3333' build/ch.elf```
