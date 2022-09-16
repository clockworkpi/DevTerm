<!-- Arduino 1.8.13

http://dan.drown.org/stm32duino/package_STM32duino_index.json

STM32F1xx/GD32F1xx boards 
by stm32duino version 2021.2.22

GENERIC STM32F103R series

gd32f1_generic_boot20_pc13.bin
generic_boot20_pc13.bin -->

# DevTerm Keyboard Tutorial

Specific support for the STM32 hardware is available at the [Arduino for STM32 forum](https://www.stm32duino.com/) and the repository for ["Roger's core"](https://github.com/rogerclarkmelbourne/Arduino_STM32/) (also referred to as [LibMaple](https://www.stm32duino.com/viewforum.php?f=45) in the forum). There's also an ["official" Arduino core](https://github.com/stm32duino/Arduino_Core_STM32/) with more support, but the [USBComposite](https://github.com/arpruss/USBComposite_stm32f1) library (which drives the main functions of the keyboard) is not available for it, so we can't use it here.

## How to build the firmware

First of all, you **CAN'T** compile this from DevTerm itself. The compiler (`arm-none-eabi-gcc 4.8.3-2014q1`) has no support for `aarch64` as far as I know, but workarounds are *possible* (you can try using a newer version of `arm-none-eabi-gcc`). 

Install [Arduino CLI](https://arduino.github.io/arduino-cli/), create a configuration file and add the STM32 package url to the board manager:

```bash
arduino-cli config init
arduino-cli config add board_manager.additional_urls http://dan.drown.org/stm32duino/package_STM32duino_index.json
```

Install the core:

```bash
arduino-cli core update-index
arduino-cli core install stm32duino:STM32F1
```

(Optinal) Check which variant of the board you need to compile to (STM32F103R8 is the default):

```bash
arduino-cli board details --fqbn stm32duino:STM32F1:genericSTM32F103R
```

Add the DSP library (for the trackball) to the Arduino Library folder:
```bash
cd ~/Arduino/libraries
git clone https://github.com/dangpzanco/dsp
```

To compile, just run `build_bin.sh`. This script runs `arduino-cli compile` with the default options (change it according to your keyboard variant):

```bash
arduino-cli compile --fqbn stm32duino:STM32F1:genericSTM32F103R:device_variant=STM32F103R8,upload_method=DFUUploadMethod,cpu_speed=speed_72mhz,opt=osstd devterm_keyboard.ino --output-dir upload
```

The firmware `devterm_keyboard.ino.bin` will be saved to the `upload` directory.

### Customizing the trackball

TO DO (see the comments in `trackball.ino`)

## How to upload the firmware

Uploading directly via `arduino-cli` (or the Arduino IDE) might be possible, but I couldn't do it consistently.

One of the main problems when uploading the firmware is reliably resetting the keyboard to the DFU mode without pressing the reset button on the back of the board. If something goes wrong, you might need a paperclip to "press" it. The reset button is not actually soldered to the board, so you need to short it with some small metal thingy (I've been using the back of a SIM card ejector).

### Setup device permissions

In order to have user access to serial port, you need to add this `udev` rule to `50-devterm_keyboard.rules`:
```text
SUBSYSTEM=="usb", ATTRS{idVendor}=="1eaf", ATTRS{idProduct}=="0024", SYMLINK+="ttyACM0", MODE="664", GROUP="uucp"
SUBSYSTEM=="usb", ATTRS{idVendor}=="1eaf", ATTRS{idProduct}=="0003", MODE="664", GROUP="uucp"
```

This is a bit of a trial and error, so also check the [Arduino support](https://support.arduino.cc/hc/en-us/articles/360016495679-Fix-port-access-on-Linux) for extra steps.

### Uploading

The `upload_bin.sh` script inside the `upload` folder uses a Python script (`dfumode.py`) to reset the keyboard, which relies on the [pySerial](https://pyserial.readthedocs.io/en/latest/) library to send the commands via serial. You should install it before running the script:
```bash
pip install pyserial
```

Just run `upload_bin.sh` and it should enter DFU mode and upload the firmware `devterm_keyboard.ino.bin`:
```bash
cd upload
./upload_bin.sh
```

If the upload fails because of permissions, you should still be able to upload with `sudo`, but you might need to install the package `python-pyserial`:

```bash
pamac install python-pyserial # on Manjaro
sudo apt install python3-serial # on Debian/Ubuntu
sudo ./upload_bin.sh
```

The original upload method is via `upload-reset.c`, so it's included as well.
