### Img file

[ubuntu-21.04-preinstalled-server-armhf+raspi.img](https://cdimage.ubuntu.com/releases/21.04/release/ubuntu-21.04-preinstalled-server-armhf+raspi.img.xz)

### Prepare
Edit /boot/cmdline.txt and /etc/fstab of this img   
**/boot/cmdline.txt**  
```
console=serial0,115200 console=tty1 root=PARTUUID=78e1086a-02  rootfstype=ext4 elevator=deadline rootwait fixrtc splash fbcon=rotate:1
```
to use HDMI console,consider to remove **fbcon=rotate:1** in `/boot/cmdline.txt`

**/etc/fstab**  
```
PARTUUID=78e1086a-02   	   /         ext4   discard,errors=remount-ro       0 1
PARTUUID=78e1086a-01       /boot/    vfat   defaults        0       1
```

### Enter chroot
```
sudo losetup -P /dev/loop10 ubuntu-21.04-preinstalled-server-armhf+raspi.img
sudo mount /dev/loop10p2 /mnt/p2
sudo mount /dev/loop10p1 /mnt/p2/boot
cd /mnt/p2
sudo mount --bind /dev dev/
sudo mount --bind /sys sys/
sudo mount --bind /proc proc/
sudo mount --bind /dev/pts dev/pts

sudo chroot .
```

### Inside chroot
```
sudo unlink /etc/resolv.conf
echo -en "nameserver 1.1.1.1\nnameserver 8.8.8.8\n" > /etc/resolv.conf

sudo apt remove linux-image-raspi linux-image-5.11.0-1007-raspi -y
sudo apt install net-tools network-manager -y

wget -O - https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo apt-key add 
echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee -a /etc/apt/sources.list.d/clockworkpi.list
sudo apt update && sudo apt install devterm-thermal-printer devterm-keyboard-firmware devterm-fan-temp-daemon-rpi devterm-thermal-printer-cups devterm-kernel-rpi devterm-audio-patch devterm-backlight-rpi -y

```

```
mkdir /etc/lightdm/lightdm.conf.d/ -p

sudo bash -c 'cat <<EOF >/etc/lightdm/lightdm.conf.d/99-cpi.conf 
[SeatDefaults]

greeter-setup-script=/etc/lightdm/setup.sh
EOF'

sudo bash -c 'cat <<EOF >/etc/lightdm/setup.sh
#!/bin/bash
xrandr --output DSI-1 --rotate right
exit 0
EOF'

sudo chmod +x /etc/lightdm/setup.sh

sudo bash -c 'cat << EOF > /etc/X11/Xsession.d/100custom_xrandr
xrandr --output DSI-1 --rotate right
EOF'
```

`sudo apt-get install tasksel`

`sudo cp -f /lib/firmware/brcm/brcmfmac43456-sdio.raspberrypi,400.txt /lib/firmware/brcm/brcmfmac43456-sdio.txt`

`sudo apt install wiringpi`  
`sudo ln -s /lib/arm-linux-gnueabihf/libwiringPi.so.2 /lib/arm-linux-gnueabihf/libwiringPi.so`  

### Outside chroot

dd img to sd card    
power on CM3   
config the network    

`sudo tasksel` to select desktop env to install 

