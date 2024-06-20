## Intro
The image based on raspberry pi os  
for raspberry pi cm4 with devterm cm4 adapter  
and it is better to run a Ubuntu 21.04 in a VirtualBox to do all the jobs  
The entire operation requires a certain experience in linux  
be careful  

## Start a chroot env
```bash
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
```bash
curl https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo tee /etc/apt/trusted.gpg.d/clockworkpi.asc

echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee -a /etc/apt/sources.list.d/clockworkpi.list

sudo apt update && sudo apt install devterm-thermal-printer-cm4 devterm-fan-temp-daemon-cm4 devterm-kernel-cm4-rpi devterm-audio-patch devterm-wiringpi-cm4-cpi -y

sudo apt install -y devterm-thermal-printer-cups
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

**Screen rotation**  
change 
```
/etc/skel/.config/monitors.xml 
/home/{pi,rpi-first-boot-wizard}/.config/monitors.xml 
```
to be like
```xml
<monitors version="2">
  <configuration>
    <logicalmonitor>
      <x>0</x>
      <y>0</y>
      <primary>yes</primary>
      <monitor>
        <monitorspec>
          <connector>DSI-1</connector>
          <vendor>unknown</vendor>
          <product>unknown</product>
          <serial>unknown</serial>
        </monitorspec>
        <mode>
          <width>480</width>
          <height>1280</height>
          <rate>60.000</rate>
        </mode>
      </monitor>
      <transform>
        <rotation>right</rotation>
      </transform>
    </logicalmonitor>
  </configuration>
</monitors>
```
for uConsole, replace **480 to 720** in above **monitors.xml**

### console rotation
/boot/cmdline.txt   add **fbcon=rotate:1** in the end of 
```
console=.........  fbcon=rotate:1

```


### Now quit the chroot env
```bash
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

