usb-serial | keyboard IO number | keyboard IO register 
-- | -- | --
3v3 | 3 | 3v3
TXD | 16 | PA10
RXD | 15 | PA9
GND | 1 | GND
 

USB  | keyboard IO number | keyboard IO register
-- | -- | --
5v power | 5 | 5v
GND | 22 | GND
data + | 18 | PA12(USB_DP)
data - | 17 | PA11(USB_DM)

can not have USB and usb-serial both ON  
sudo stm32flash -r devterm.kbd.0.3_48mhz.bin -S 0x08000000:65536 /dev/ttyUSB0    
srec_cat devterm.kbd.0.3_48mhz.bin -Binary -offset 0x08000000 -output devterm.kbd.0.3_48mhz.hex -Intel  

stm32flash -w devterm_kbd.bin -v  -S 0x08000000 /dev/ttyUSB0  

![WechatIMG50](https://user-images.githubusercontent.com/523580/186061860-02efc332-eae9-4200-ae37-c312af5d0f96.jpeg)



