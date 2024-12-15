### Orginal image download URL
https://downloads.raspberrypi.org/raspios_full_armhf/images/raspios_full_armhf-2021-05-28/2021-05-07-raspios-buster-armhf-full.zip


#### Modification request list 
- [x] screen rotation for xrandr,fbcon,lightdm
- [x] Remove the wallpaper and change the solid color #202020  
- [x] Install all rpi-related debs in the apt source 

#### Packages in devterm apt source
1. devterm-audio-patch (check whether the 3.5 audio interface is plugged in, pull up a certain GPIO)
1. devterm-fan-temp-daemon-rpi (detect the temperature of rpi, raise the io drive fan, written in python)
1. devterm-kernel-rpi (modified kernel, 4.x series)
1. devterm-keyboard-firmware (Devterm keyboard firmware flashing tool, Advanced users only!)
1. devterm-thermal-printer (thermal printer program and systemd scripts)
1. devterm-thermal-printer-cups (CUPS filter for the thermal printer, and add the thermal printer to cups, so devterm printer can be seen in chromium) 




##### Add clockworkpi to apt source list
see https://github.com/clockworkpi/apt/tree/main/debian 
```
wget -O - https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo apt-key add -

echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee -a /etc/apt/sources.list.d/clockworkpi.list
```

##### Config xrandr 
```
sudo bash -c 'cat << EOF > etc/X11/Xsession.d/100custom_xrandr
xrandr --output DSI-1 --rotate right
EOF'
```

##### Config lightdm
in `/etc/lightdm/lightdm.conf`
```
greeter-setup-script=/etc/lightdm/setup.sh
```
*setup.sh*
```
#!/bin/bash
xrandr --output DSI-1 --rotate right
exit 0
```
`sudo chmod +x etc/lightdm/setup.sh`


##### Modify /etc/dphys-swapfile
```
CONF_SWAPSIZE=512
```

##### Change the default wallpaper
The following files changed 

* /etc/xdg/pcmanfm/LXDE-pi/desktop-items-0.conf 
* /etc/xdg/pcmanfm/LXDE-pi/desktop-items-1.conf 
* /etc/lightdm/pi-greeter.conf 
* /home/pi/.config/pcmanfm/LXDE-pi/desktop-items-0.conf 

Delete wallpaper address `temple.jpg`  
Modify the `desktop_bg` color to `#202020` 



#### config.txt

Add `dtparam=audio=on` to solve the problem of no sound card under HDMI

##### /etc/hostname to clockworkpi

##### /boot/cmdline.txt ,add fbcon=rotate:1, remove quiet 

### Image download url
https://forum.clockworkpi.com/t/devterm-os-cm3-image-files/7151/1

