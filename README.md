# Loramesh ESP32 firmware for tinySSB

## Required tools for compiling and flashing devices

We use the Arduino CLI to compile and Flash devices, along with the esptool.py command-line tool from espressif.  

### Installing Arduino CLI

Follow the instructions given [here](https://arduino.github.io/arduino-cli/0.20/installation/)

If using Homebrew
```bash
brew update
brew install arduino-cli
```

### Installing Esptool.py
This tool requires Python 3 to be installed on your system, to verify that you have Python 3 installed:
```bash
python3 --version
```

should output `Python 3` something, otherwise you'll need to install it for your system

For macOS this can be done through Homebrew:
```bash
brew install python
```

in Linux, if you don't already have python installed you can use your package manager
```bash
sudo apt install python3 python3-pip
```

From here you can install esptool.py:
```bash
pip install esptool
```
and verify that it's installed correctly by running:
```bash
esptool.py version
```

## Supported devices
### LilyGo 
Heltec, Heltec3, T5gray, TBeam, TDeck, TWatch, TWrist, WLpaper

## Compiling firmware for your device

We've included a Makefile that should find your device, and the required tools (see above) for completing a complete firmware compilation.

Go to the loramesh directory, and then run the following command, replacing `TDECK` with the appropriate board name for your device (see Supported Devices).
```bash
make BOARD=TBeam firmware
```

Note: If you're using an older version 07 TBeam run the following command:
```bash
make BOARD=TBeam FLAG=TBEAM_07 firmware flash
```

## Installing Firmware on Device
Flashing firmware to your ESP32 LoRa device is generally done by running the following `make`command.  The Makefile should be able to do the rest, as long as your device is connected viaa "data capable" usb connector.  A power only usb connector will not suffice.

```bash
make board=TBEAM flash
```

replacing `TBEAM` with the board name of your device (see Supported Devices).


## Troubleshooting
### Identify your serial port
The Makefile looks for all "known" variants of device names that show up.  If your system creates a serial port name that we haven't seen then there will likely be problems.

Open up a terminal and run the following command before and after plugging in your device with an appropriate data usb cable:

macOS
```bash
ls /dev/tty.*
```

linux
```bash
ls /dev/ttyUSB*
```

the new device should show up in this list. Please open an GitHub issue and give us this information and we will add it to the Makefile.

## Find your Arduino CLI install

Run the following command, this may also be useful information.  If it does not find anything please see the section on installing the Arduino CLI.
```bash
which arduino-cli
```

Otherwise open an issue and let us know what was discovered.  Thanks!




