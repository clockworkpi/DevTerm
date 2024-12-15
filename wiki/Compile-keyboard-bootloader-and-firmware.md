# Build bin

```
git clone https://github.com/rogerclarkmelbourne/STM32duino-bootloader

cd STM32duino-bootloader

make generic-pc13

```
this will produce `bootloader_only_binaries/generic_boot20_pc13.bin`  
## Convert bootloader to hex format
using `srec_cat` to convert   
windows: http://srecord.sourceforge.net/  
linux: sudo apt install srecord  

```
srec_cat bootloader_only_binaries/generic_boot20_pc13.bin -Binary -offset 0x08000000 -output bootloader_only_binaries/generic_boot20_pc13.hex -Intel
```

## Next is to use stm32Cube to flash the bootloader
* Download the stm32cube programmer  
   [stm32cube programmer](https://www.st.com/en/development-tools/stm32cubeprog.html)

* Put 1 ON in the back of keyboard
* Connect keyboard with a usb-serial convert by fpc, in order of [IO MAP](https://github.com/clockworkpi/DevTerm/wiki/Keyboard-with-FPC-60pin-0.5mm)
* Click connect on STM32cube programmer
* flash it

![2022-09-26_13-04](https://user-images.githubusercontent.com/523580/192197890-dcb6d6fc-0ef5-4870-b9d3-ae03fc5f5110.png)
![2022-09-26_13-04_1](https://user-images.githubusercontent.com/523580/192197900-c7f8b448-3812-403f-b7b0-728248533790.png)
![2022-09-26_13-09](https://user-images.githubusercontent.com/523580/192198205-8edecc5f-c30e-4837-8b3c-7f97c97ba40a.png)



# Flash Arudino 
## Arduino IDE
* add stm32duino pacakage index in **Preference panel** : http://dan.drown.org/stm32duino/package_STM32duino_index.json as **Additional Boards Manager URLs:** and install STM32F1xx/GD32F1xx boards

![image](https://user-images.githubusercontent.com/523580/192198944-fd0fbf55-0977-42bb-9256-f7bcdf331466.png)

* verbose all outputs:  
![image](https://user-images.githubusercontent.com/523580/192198967-d4738c42-897e-412a-b630-afab45d274c3.png)

* Select upload method ,board type is **Generic STM32F103R series **  
![image](https://user-images.githubusercontent.com/523580/192198991-9757e354-c8ec-4859-95e9-dba732773fb1.png)

* Set cpu mhz to 48Mhz  
![image](https://user-images.githubusercontent.com/523580/192199043-7f877adb-830e-4323-8799-06e115843b46.png)


* Put 1 OFF in the back of keyboard
* User arduino IDE to open [devterm_keyboard.ino](https://github.com/clockworkpi/DevTerm/blob/main/Code/devterm_keyboard/devterm_keyboard.ino)
* flash it 
![image](https://user-images.githubusercontent.com/523580/192199494-8778bfcc-fcff-4bd0-9f51-f034a080f981.png)


# CN version  
https://shimo.im/docs/Tc8RVQWdjvXtwhYv/ 《GD32f103rgt6/CKSF103R* 与stm32duino bootloader》

custom keyboard:

https://forum.clockworkpi.com/t/keyboard-stuck-in-bootloader-mode/8830/11

