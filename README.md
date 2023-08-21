# DevTerm

## DevTerm OS Images

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
 This version is optimized for immersive writing and comes pre-installed with [Obsidian](https://obsidian.md/).  


### R01
* http://dl.clockworkpi.com/DevTerm_R01_v0.2a.img.bz2   
md5sum 49aa472a6e4d81a48e0a1a26436f02c2



After downloading the files, you will need to extract or decompress them. Please keep in mind that MacOS 11.6 or a higher version is required to extract 7z files.  
  
To flash the OS image, you can use the following tools:  
  
- For Windows and macOS users, [Etcher](https://etcher.balena.io/) can be used to flash the image.  
- Linux users can employ the "dd" command to flash the image.  



## DevTerm Keyboard Firmware
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


## Thermal Printer Testing Commands

* How to print "Hello World".  
`echo "hello world\n\n\n\n\n\n\n\n\n\n" > /tmp/DEVTERM_PRINTER_IN`

* How to print a self test page.  
`echo -en "\x12\x54" >  /tmp/DEVTERM_PRINTER_IN`

* How to print a text file.  
`curl -L https://github.com/clockworkpi/DevTerm/raw/main/README.md | tee >  /tmp/DEVTERM_PRINTER_IN`  


* [Learning more...](https://github.com/clockworkpi/DevTerm/tree/main/Code/thermal_printer)


## Assembly guidelines

* [Assembly guidelines](https://github.com/clockworkpi/DevTerm/blob/main/Clockwork_DevTerm_Assembly_Guidelines.pdf)  

## Schematic

* [DevTerm A06 core mainboard v3.14 schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_DevTerm_A06_Core_for_Mainboard_V3.14_Schematic.pdf)
* [DevTerm battery schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_DevTerm_Battery_Schematic.pdf) 
* [DevTerm Ext schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_DevTerm_Ext_Schematic.pdf)
* [DevTerm keyboard schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_DevTerm_Keyboard_Schematic.pdf)
* [DevTerm keyboard trackball schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_DevTerm_Keyboard_Trackball_Schematic.pdf)
* [DevTerm R01 core mainboard v3.14 schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_DevTerm_R01_Core_for_Mainboard_V3.14_Schematic.pdf)
* [Mainboard v3.14 schematic](https://github.com/clockworkpi/DevTerm/blob/main/Schematics/clockwork_Mainboard_V3.14_Schematic.pdf)
* [CM4 adapter schematic](https://github.com/clockworkpi/DevTerm/blob/main/clockwork_Adapter_CM4_Schematic.pdf)
* [Learning more...](Schematics/)

## Gearbox 

Gearbox(A06 only) is a script tool used to adjust the big.LITTLE architecture of the A06 chip.  
you can get it by running 
```
sudo apt update 
sudo apt install -y devterm-gearbox-a06
```

In latest os image of A06, default Gearbox is set to use 4 LITTLE core with 816Mhz,GPU at 400Mhz

you can run `a06-gearbox` to see the current core status.

```
Current Status:
+-----------------------------------+-----------------+-----------+
|            Cortex-A53             |   Cortex-A72    | Mali-T860 |
+--------+--------+--------+--------+--------+--------+-----------+
| CPU 0  | CPU 1  | CPU 2  | CPU 3  | CPU 4  | CPU 5  |    GPU    |
+--------+--------+--------+--------+--------+--------+-----------+
| 816Mhz | 816Mhz | 816Mhz | 816Mhz |  OFF   |  OFF   |   400MHz  |
+--------+--------+--------+--------+--------+--------+-----------+
CPU Governor: schedutil    GPU Governor: simple_ondemand
```

Run `sudo a06-gearbox -s n` to set gear

There are 6 gears in gearbox

```
               1 for simple writing tasks with long battery life.
               2 for browsing most websites with long battery life.
               3 for most 2D games and emulators.
               4 for playing videos and 3D games.
               5 for performance-first tasks.
               6 for max performance, max power (usage).

```


## Community
Please visit our [Github Wiki](https://github.com/clockworkpi/DevTerm/wiki) and https://forum.clockworkpi.com for more information.

