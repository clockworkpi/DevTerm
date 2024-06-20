## Intro
The image based on raspberry pi os   
and it is better to run a Ubuntu 21.04 in a VirtualBox to do all the jobs  
The entire operation requires a certain experience in linux  
be careful  

## Start a chroot env
```
wget https://downloads.raspberrypi.org/raspios_full_armhf/images/raspios_full_armhf-2021-05-28/2021-05-07-raspios-buster-armhf-full.zip

unzip 2021-05-07-raspios-buster-armhf-full.zip

sudo losetup --show -f -P 2021-05-07-raspios-buster-armhf-full.img #assume loop0
sudo mount /dev/loop0p2 /mnt/p2
sudo mount /dev/loop0p1 /mnt/p2/boot

cd /mnt/p2
sudo mount --bind /dev dev/
sudo mount --bind /sys sys/
sudo mount --bind /proc proc/
sudo mount --bind /dev/pts dev/pts
#sudo mv etc/ld.so.preload etc/ld_so_preload
sudo chroot .
```

## Inside chroot 
```
curl https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo tee /etc/apt/trusted.gpg.d/clockworkpi.asc

echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee -a /etc/apt/sources.list.d/clockworkpi.list

sudo apt update && sudo apt install devterm-thermal-printer devterm-keyboard-firmware devterm-fan-temp-daemon-rpi devterm-thermal-printer-cups devterm-kernel-rpi devterm-audio-patch -y
```
**Config xrandr**
```
sudo bash -c 'cat << EOF > etc/X11/Xsession.d/100custom_xrandr
xrandr --output DSI-1 --rotate right
EOF'
```
**config lightdm** 

`/etc/lightdm/lightdm.conf`

`greeter-setup-script=/etc/lightdm/setup.sh`
```
sudo bash -c 'cat <<EOF >/etc/lightdm/setup.sh
#!/bin/bash
xrandr --output DSI-1 --rotate right
exit 0
EOF'
```
`sudo chmod +x /etc/lightdm/setup.sh`

**Modify /etc/dphys-swapfile**
```
CONF_SWAPSIZE=512
```

**Change the default wallpaper**   
The following files changed 

* /etc/xdg/pcmanfm/LXDE-pi/desktop-items-0.conf 
* /etc/xdg/pcmanfm/LXDE-pi/desktop-items-1.conf 
* /etc/lightdm/pi-greeter.conf 
* /home/pi/.config/pcmanfm/LXDE-pi/desktop-items-0.conf 

Delete wallpaper address `temple.jpg`,change the `desktop_bg` color to `#202020`   
change `/etc/hostname` to clockworkpi  
in `/boot/cmdline.txt` ,add `fbcon=rotate:1`, remove `quiet` 

### Now quit the chroot env
```
exit
#sudo mv etc/ld_so_preload etc/ld.so.preload

sudo umount /mnt/p2/dev/pts
sudo umount /mnt/p2/dev
sudo umount /mnt/p2/proc
sudo umount /mnt/p2/sys
##clear bash 
sudo rm -rf root/.bash_history
#sudo rm usr/bin/qemu-arm-static

cd -
sudo umount /mnt/p2/boot
sudo umount /mnt/p2
sudo losetup -D /dev/loop0 #assume loop0
```

### Flash the image to SD card 
* Linux
`sudo dd if=2021-05-07-raspios-buster-armhf-full.img of=/dev/sdX bs=8M status=progress`


