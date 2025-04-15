# Loramesh ESP32 firmware for tinySSB

## Required tools for compiling and flashing devices


We use the Arduino CLI to compile and Flash devices, along with the esptool command-line tool from espressif.  The makefile will install esptool if you don't have it already installed.

### Installing Arduino CLI

Follow the instructions given [here](https://arduino.github.io/arduino-cli/0.20/installation/)


## Supported devices
### LilyGo 
Heltec, Heltec3, T5gray, TBeam, TDeck, TWatch, TWrist, WLpaper

## Compiling the firmware for your device

We've included a Makefile that should find your device, and the required tools for completing a firmware compilation.

Go to the loramesh directory, and then run the following command, replacing `TBeam` with the appropriate board name for your device (see Supported Devices).

```bash
make BOARD=TBeam firmware
```

Note: If you're using an older version 07 TBeam run the following command:
```bash
make BOARD=TBeam FLAG=TBEAM_07 firmware
```

## Installing the Firmware on your device

Flashing firmware to your ESP32 LoRa device is generally done by running the following `make`command.  The Makefile should be able to do the rest, as long as your device is connected via a "data capable" usb connector.  A power only usb connector will not suffice.

```bash
make BOARD=TBeam flash
```

replacing `TBeam` with the board name of your device (see Supported Devices).


## Troubleshooting

### Unable to detect serial port
The Makefile looks for all "known" variants of device names that show up.  If your system does not have the correct driver installed or creates a serial port name we haven't seen yet, then there will likely be problems when flashing your device.

Open up a terminal and run the following command before and after plugging in your device with an appropriate data usb cable:

macOS
```bash
ls /dev/tty.*
```

linux
```bash
ls /dev/ttyUSB*
```

The serial port for your device should show up in this list after you plug it in. If not, you may need to install a USB to UART driver like the one [here](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads). However, if you do see your device, please open a GitHub issue with the information you get from the `ls` command above along with your device name and we will add it to the Makefile.

### Find your Arduino CLI install

Run the following command, this may also be useful information.  If it does not find anything please see the section on installing the Arduino CLI.
```bash
which arduino-cli
```

### Add user into dialout or uucp group
In Linux, if you get errors about not being able to flash the device, you may need to add your user to the UUCP usergroup:

```
gpasswd -d your_username uucp
```

### Other errors
Otherwise open an issue and let us know what was discovered.  Thanks!




