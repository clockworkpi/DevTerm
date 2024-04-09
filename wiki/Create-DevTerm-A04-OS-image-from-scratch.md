**This wiki maybe outdated since armbian build system is always updating , so don't panic**



# Generate the prototype os image
```
cd ~
git clone https://github.com/armbian/build.git
git clone https://github.com/clockworkpi/DevTerm.git

cd build

git reset --hard 5fa022603c0948cc59688ba782b3711f980a0be3

cp -rf ~/DevTerm/Code/patch/armbian_build_a04/userpatches/* userpatches/
cp -rf ~/DevTerm/Code/patch/armbian_build_a04/config/boards/* config/boards/
cp -rf ~/DevTerm/Code/patch/armbian_build_a04/config/kernel/*  config/kernel/

#Then exec ./compile.sh under armbian build
cd ~/build && ./compile.sh 
```
after image done  
uncompress the  
`linux-dtb-current-sunxi64_21.11.0-trunk_arm64.deb`  
`linux-image-current-sunxi64_21.11.0-trunk_arm64.deb`  
 
and then combine all files ,all the postinst, preinst,prerm,postrm  
to be one `devterm-kernel-current-cpi-a04.deb`    
the reason is if not doing this , `apt-get upgrade` will replace the linux-dto,linux-image* in future, which will cause boot failed  
so to keep a04 linux kernel in safe , I made devterm-kernel-current-cpi-a04     

#  Prepare to chroot into the image 
```
sudo apt install -y qemu-user-static  
sudo losetup -f # find the avaiable loop device number,eg:loop11
sudo losetup -P /dev/loop11 ~/build/output/images/Armbian_21.11.0-trunk_Clockworkpi-a04_hirsute_current_5.10.75_xfce_desktop.img 
sudo mount /dev/loop11p1 /mnt/p1/

cd /mnt/p1

sudo mount --bind /dev dev/
sudo mount --bind /sys sys/
sudo mount --bind /proc proc/
sudo mount --bind /dev/pts dev/pts

sudo chroot .
```
## Inside chroot
(inside)
### import clockworpi apt 
```
curl https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | tee /etc/apt/trusted.gpg.d/clockworkpi.asc
echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | tee  /etc/apt/sources.list.d/clockworkpi.list  
```

### Preset cpi username and password
```
sh /etc/profile.d/armbian-check-first-login.sh 
```
### Install kernel
```
sudo apt update
sudo apt install -y devterm-kernel-current-cpi-a04
apt install -y xfce4-power-manager
touch /home/cpi/.first_start
chown cpi:cpi /home/cpi/.first_start
```
 
### Allow sudo cpi without password prompt

```
echo -e  "cpi\tALL=(ALL:ALL) NOPASSWD:ALL" | sudo tee  /etc/sudoers.d/cpi

```
### Config xrandr 
```
sudo bash -c 'cat << EOF > /etc/X11/Xsession.d/100custom_xrandr
xrandr --output DSI-1 --rotate right
xrandr --output None-1 --rotate right
EOF'
```

### Config lightdm
#### autologin 

```
sudo bash -c 'cat <<EOF > /etc/lightdm/lightdm.conf.d/12-autologin.conf 
[Seat:*]
autologin-user=cpi
autologin-user-timeout=0
EOF'
```

```
sudo bash -c 'cat <<EOF > /etc/lightdm/lightdm.conf.d/13-rotate-dsi.conf
[Seat:*]
greeter-setup-script=/etc/lightdm/setup.sh
EOF'
```
```
sudo bash -c 'cat <<EOF >/etc/lightdm/setup.sh
#!/bin/bash
xrandr --output DSI-1 --rotate right
xrandr --output None-1 --rotate right
exit 0
EOF'

sudo chmod +x /etc/lightdm/setup.sh
```

#### config lightdm background
```
sudo sed  -i '/background/c\background = #202020' /etc/lightdm/slick-greeter.conf

```

### Disable app crash report dialog
```
sudo sed -i '/enabled=1/c\enabled=0'  /etc/default/apport
```

### Hide suspend button 
cd /home/cpi/.config/xfce4/xfconf/xfce-perchannel-xml
```
sudo sed -i 's/\(name\=\".*ShowSuspend.*\"\s\)value="\(true\|false\)"/\1value=\"false\"/gi' xfce4-session.xml

sudo sed -i 's/\(type\=\".*string.*\"\s\)value="\(+suspend\)"/\1value=\"-suspend\"/gi' xfce4-panel.xml

```

### Add framebuffer console rotate /boot/armbianEnv.txt
```
extraargs=fbcon=rotate:1
```

### Change ssh login banner
In `/etc/update-motd.d/10-armbian-header`
```
TERM=linux toilet -f standard -F metal $(echo $BOARD_NAME | sed 's/Orange Pi/OPi/' | sed 's/NanoPi/NPi/' | sed 's/Banana Pi/BPi/' | sed 's/Clockworkpi A04/Clockworkpi/')
```

### Install devterm packages
```
sudo apt update
sudo apt install -y devterm-wiringpi-cpi-a04 devterm-thermal-printer devterm-thermal-printer-cups  devterm-first-start-a04 devterm-fan-daemon-cpi-a04 devterm-audio-patch

```
### Disable sshd
```
sudo touch /etc/ssh/sshd_not_to_be_run

```
### Install ibus input method for CJK

`sudo apt install ibus ibus-pinyin ibus-gtk ibus-gtk3 -y`
```
cat <<EOF >/home/cpi/.config/autostart/ibus.desktop
[Desktop Entry]
Exec=ibus-daemon -drxR
GenericName=IBus
Name[zh_CN]=IBus
Name=IBus
Name[en_US]=IBus
StartupNotify=true
Terminal=false
Type=Application
EOF
```

```
sudo bash -c 'cat <<EOF >/etc/profile.d/ibus.sh
#!/bin/bash

export XIM_PROGRAM=ibus
export XIM=ibus
export XMODIFIERS=@im=ibus
export GTK_IM_MODULE=ibus
export QT_IM_MODULE=ibus
EOF'
```
### GDM3 
due to the panfrost screen flashing issue with lightdm,use gdm3 instead of lightdm  
` sudo apt install gdm3`
```
sudo bash -c 'cat <<EOF > /usr/share/gdm/greeter/autostart/01_rotate.desktop
[Desktop Entry]
Version=1.0
Type=Application
Name=Rotate Screen
Exec=/etc/lightdm/setup.sh
Terminal=false
NoDisplay=true
EOF'
```
#### gdm3 background and hide poweroff,suspend
In `/etc/gdm3/greeter.dconf-defaults `
```
...
[org/gnome/desktop/background]
picture-options='none'
primary-color='#222222'
...

[org/gnome/login-screen]
disable-restart-buttons=true
```
#### config gdm3 autologin 
/etc/gdm3/custom.conf  

```
[daemon]
...
AutomaticLoginEnable = true
AutomaticLogin = cpi
```

## libreoffice fix for panfrost issue
` sudo apt install libreoffice-gtk3 `

# Exit from chroot
```
exit
(out)
sudo umount /mnt/p1/dev/pts
sudo umount /mnt/p1/dev
sudo umount /mnt/p1/proc
sudo umount /mnt/p1/sys
##clear bash 
sudo rm -rf root/.bash_history
cd -
sudo umount /mnt/p1 
sudo losetup -D /dev/loop11
```
