# DevTerm

## OS images

### A04
* http://dl.clockworkpi.com/DevTerm_A04_v0.2h.img.bz2 (32bit)  
md5sum acec1d02a37bfbffc9ed025fd718948e

### A06
* http://dl.clockworkpi.com/DevTerm_A06_v0.2h.img.bz2   
md5sum 26f52bfde573479960d8696f407d19b9  

### CM3
* http://dl.clockworkpi.com/DevTerm_CM3_v0.1a.img.bz2 (32bit)  
md5sum bef6c111863f8d2e6ef1cb23be354152  

### CM4
* http://dl.clockworkpi.com/DevTerm_CM4_v0.2b_64bit.img.7z    
md5sum 8b2fdebe254dfa1f5f245cebb85b1e84

* http://dl.clockworkpi.com/DevTerm_CM4_v0.1.img.bz2 (32bit)    
md5sum 7938ed1cdda98ba6f28049a819c12dc1  

* http://dl.clockworkpi.com/DevTerm_CM4_v0.3e_xfce_64bit.img.7z (based on [RPI-lite](https://downloads.raspberrypi.org/raspios_lite_armhf/images/raspios_lite_armhf-2023-05-03/2023-05-03-raspios-bullseye-armhf-lite.img.xz) with xfce)  
md5sum ab081eabf24ae501dc3f40a9126b7e5a

### R01
* http://dl.clockworkpi.com/DevTerm_R01_v0.2a.img.bz2   
md5sum 49aa472a6e4d81a48e0a1a26436f02c2




After downloading the files, you need to uncompress them. Please note that MacOS requires version 11.6 or higher to uncompress 7z files.

For flashing the OS image, you can use the following tools:

* Windows and macOS users can use [Etcher](https://etcher.balena.io/) to flash the image.
* Linux users can use the "dd" command to flash the image.


## DevTerm keyboard firmware
DevTerm keyboard firmware flash program available. You can download it from this link: [DevTerm Keyboard Firmware Flash Program](https://github.com/clockworkpi/DevTerm/raw/main/Bin/devterm_keyboard_flash.tar.gz).

Here's how you can flash the firmware on DevTerm(A06 or CM4) or a PC running Ubuntu 22.04:

1. Download the devterm_keyboard_flash.tar.gz file.
2. Extract the contents of the archive: `tar zxvf devterm_keyboard_flash.tar.gz`.
3. Install the required package using the following command: `sudo apt install -y dfu-util`.
4. Navigate to the extracted directory: `cd devterm_keyboard_flash`.
5. Execute the flash script with root privileges: `sudo ./flash.sh`.
6. If everything goes well, you will see a progress bar indicating the flashing process.
7. If any issues occur or the keyboard loses control (which is unlikely), simply reboot DevTerm to resolve it.
8. Rest assured that this flash program will not brick your keyboard.

