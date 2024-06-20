# How to compile devterm cm3 kernel

## Clone kernel source code 
```
git clone https://github.com/raspberrypi/linux.git  
cd linux
git checkout remotes/origin/rpi-4.19.y
```

## Get cross compile tools
```
git clone https://github.com/raspberrypi/tools.git
```

## Compiling process
```
#must use the rpi arm-bcm2708 cross compiler tools

cd linux
git apply devterm-4.19_v0.11.patch #get patch from https://github.com/clockworkpi/DevTerm/tree/main/Code/patch/cm3

export PATH=/data/github/raspberrypi/tools/arm-bcm2708/arm-linux-gnueabihf/bin/:$PATH ## change the arm-bcm2708 tools location for yourself

KERNEL=kernel7l make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
KERNEL=kernel7l make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j3
INSTALL_MOD_PATH=./modules 
rm -rf $INSTALL_MOD_PATH
make modules_install

rm modules/lib/modules/*/build
rm modules/lib/modules/*/source

```

## /boot/config.txt

In config.txt,I renamed kernel7.img to devterm-kernel7.img

```
ignore_lcd=1
dtoverlay=vc4-kms-v3d,audio=0,cma-384
dtoverlay=devterm-pmu
dtoverlay=devterm-panel
dtoverlay=devterm-wifi
dtoverlay=devterm-bt
dtoverlay=devterm-misc
gpio=5=op,dh
gpio=9=op,dh
gpio=10=ip,np
gpio=11=op,dh

enable_uart=1
dtparam=spi=on
dtoverlay=spi-gpio35-39

dtparam=audio=on
kernel=devterm-kernel7.img
```

