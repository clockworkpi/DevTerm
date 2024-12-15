# Intro
DevTerm A06 os image based on Armbian with current kernel branch, XFCE desktop   
and it is better to run a Ubuntu 21.04 in a VirtualBox with a good bandwidth network to do all the jobs   
If the condition of the network is not so well, 
the Armbian build process will fail on some packages downloading  or mirrors connecting  

The entire operation requires a certain experience in linux  
be careful  

**This wiki maybe outdated since armbian build system is always updating , so don't panic**

# Build armbian image for A06
```
cd ~
git clone https://github.com/armbian/build.git
git clone https://github.com/clockworkpi/DevTerm.git

cd build
git reset --hard 43d179914ae9e1ebb5d72315d9f9f68f5fb3e330

mkdir -p userpatches/kernel/rockchip64-current/
mkdir -p userpatches/u-boot/u-boot-rockchip64-mainline/

git apply ~/DevTerm/Code/patch/armbian_build_a06/patch/armbian.patch
cp ~/DevTerm/Code/patch/armbian_build_a06/patch/kernel*.patch userpatches/kernel/rockchip64-current/
cp ~/DevTerm/Code/patch/armbian_build_a06/patch/uboot*.patch userpatches/u-boot/u-boot-rockchip64-mainline/
cp -f ~/DevTerm/Code/patch/armbian_build_a06/patch/lib.config userpatches/
cp ~/DevTerm/Code/patch/armbian_build_a06/patch/clockworkpi-a06.conf config/boards/

#Then exec ./compile.sh under armbian build
cd ~/build && sudo ./compile.sh  BOARD=clockworkpi-a06 BRANCH=current BUILD_MINIMAL=no BUILD_DESKTOP=no KERNEL_ONLY=yes KERNEL_CONFIGURE=no 
``` 

after image done  
uncompress the  
`linux-dtb-current-rockchip64_21.08.0-trunk_arm64.deb`  
`linux-image-current-rockchip64_21.08.0-trunk_arm64.deb`  

and then combine all files ,all the postinst, preinst,prerm,postrm  
to be one `devterm-kernel-current-cpi-a06.deb`    
the reason is if not doing this , `apt-get upgrade` will replace the linux-dto,linux-image* in future, which will cause boot failed  
so to keep a06 linux kernel in safe , I made devterm-kernel-current-cpi-a06    

# Chroot image
```
sudo losetup -P /dev/loop0  Armbian_21.08.0-trunk_Clockworkpi-a06_focal_current_5.10.55_xfce_desktop.img
sudo mount /dev/loop0p1 /mnt/p1

cd /mnt/p1
sudo mount --bind /dev dev/
sudo mount --bind /sys sys/
sudo mount --bind /proc proc/
sudo mount --bind /dev/pts dev/pts
sudo mv etc/ld.so.preload etc/ld_so_preload
sudo chroot .
```

## Inside Chroot
### Run password wizard
```
sh /etc/profile.d/armbian-check-first-login.sh
```
set **root** password,create default user **cpi** with password **cpi**  
```
touch /home/cpi/.first_start
chown cpi:cpi /home/cpi/.first_start
```
### Install devterm software  
```
sudo apt update
sudo apt install curl wget -y

curl https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo tee /etc/apt/trusted.gpg.d/clockworkpi.asc
echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee  /etc/apt/sources.list.d/clockworkpi.list  

sudo apt update
sudo apt remove linux-image*
sudo apt install devterm-kernel-current-cpi-a06 devterm-fan-daemon-cpi-a06 devterm-thermal-printer devterm-thermal-printer-cups devterm-wiringpi-cpi  devterm-first-start-a06 devterm-audio-patch
```

### /boot/ArmbianEnv.txt
```
bootlogo=false
extraargs= fbcon=rotate:1
...
```


### autologin
```
sudo bash -c 'cat <<EOF > /etc/lightdm/lightdm.conf.d/12-autologin.conf

[Seat:*]
autologin-user=cpi
autologin-user-timeout=0
EOF'
```

### config lightdm
```
sudo bash -c 'cat << EOF > /etc/lightdm/lightdm.conf.d/13-rotate-dsi.conf
[Seat:*]
greeter-setup-script=/etc/lightdm/setup.sh
EOF'

sudo bash -c 'cat <<EOF >/etc/lightdm/setup.sh
#!/bin/bash
xrandr --output DSI-1 --rotate right
exit 0
EOF'

sudo chmod +x /etc/lightdm/setup.sh
```

### config xrandr 
```
sudo bash -c 'cat << EOF > /etc/X11/Xsession.d/100custom_xrandr
xrandr --output DSI-1 --rotate right
EOF'
```

### additional software
```
sudo apt remove celluloid mpv 
sudo apt install -y arandr chromium-browser vlc cpupower-gui xfce4-power-manager
```
### /etc/default/apport 
`enable=0`

### ssh banner fix 
/etc/update-motd.d/10-armbian-header
```
.... sed 's/Banana Pi/BPi/' | sed 's/Clockworkpi A06/Clockworkpi/')

```
### Disable Display power management in xfce4 power manager 
display power management must be disabled ,once the display turn off, you need to reboot to get it back
~/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-power-manager.xml  

```
<?xml version="1.0" encoding="UTF-8"?>

<channel name="xfce4-power-manager" version="1.0">
  <property name="xfce4-power-manager" type="empty">
    <property name="power-button-action" type="empty"/>
    <property name="brightness-switch-restore-on-exit" type="int" value="0"/>
    <property name="brightness-switch" type="int" value="0"/>
    <property name="show-tray-icon" type="int" value="1"/>
    <property name="lid-action-on-battery" type="uint" value="1"/>
    <property name="logind-handle-lid-switch" type="bool" value="false"/>
    <property name="lid-action-on-ac" type="uint" value="0"/>
    <property name="inactivity-sleep-mode-on-battery" type="uint" value="1"/>
    <property name="sleep-button-action" type="uint" value="1"/>
    <property name="hibernate-button-action" type="uint" value="1"/>
    <property name="lock-screen-suspend-hibernate" type="bool" value="false"/>
    <property name="critical-power-action" type="uint" value="4"/>
    <property name="critical-power-level" type="uint" value="5"/>
    <property name="dpms-enabled" type="bool" value="false"/>
    <property name="blank-on-ac" type="int" value="0"/>
    <property name="brightness-on-battery" type="uint" value="120"/>
    <property name="blank-on-battery" type="int" value="0"/>
    <property name="brightness-on-ac" type="uint" value="120"/>
    <property name="brightness-level-on-battery" type="uint" value="40"/>
    <property name="brightness-level-on-ac" type="uint" value="42"/>
  </property>
</channel>
```
### Default wallpaper 
```
/usr/share/backgrounds/xfce/xfce-verticals.png
```  
# Quit Chroot
```
exit
sudo mv etc/ld_so_preload etc/ld.so.preload

sudo umount /mnt/p1/dev/pts
sudo umount /mnt/p1/dev
sudo umount /mnt/p1/proc
sudo umount /mnt/p1/sys
##clear bash 
sudo rm -rf root/.bash_history
sudo rm usr/bin/qemu-arm-static

cd -
sudo umount /mnt/p1
```

umount may failed at /mnt/p1/dev  
just use `ps aux | grep cupsd` to see if there a process named like `/usr/bin/qemu-aarch64-static /usr/sbin/cupsd -C /etc/cups/cupsd.conf -s /etc/cups/cups-files.conf`  

find the pid ,use `sudo kill -9 ${pid}` with that pid  
then umount again   



# Flash the image to SD card
* Linux  
`sudo dd if=Armbian_21.08.0-trunk_Clockworkpi-a06_focal_current_5.10.55_xfce_desktop.img  of=/dev/sdX bs=8M status=progress`


# Ubuntu 2104 Hirsute went offline 
fix:
https://forum.clockworkpi.com/t/hirsute-went-offline-late-july-2022-how-to-manage/8770/3

new os image: 26f52bfde573479960d8696f407d19b9 http://dl.clockworkpi.com/DevTerm_A06_v0.2h.img.bz2   
switched to ubuntu jammy ,LTS up to 2037  

# 2023 06 30 new kernel patch:
https://github.com/clockworkpi/uConsole/tree/master/Code/patch/a06/20230630


