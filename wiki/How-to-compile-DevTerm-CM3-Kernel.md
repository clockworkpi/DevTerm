## Env 
VirtualBox with ubuntu 20.04

## Toolchain

https://github.com/raspberrypi/tools/tree/master/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin


## Get kernel
1. git clone https://github.com/raspberrypi/linux.git
1. cd linux
1. git checkout remotes/origin/rpi-4.19.y
1. git reset --hard cc39f1c9f82f6fe5a437836811d906c709e0661c
1. git apply [devterm-4.19_v0.1.patch](https://raw.githubusercontent.com/clockworkpi/DevTerm/main/Code/kernel/devterm-4.19_v0.1.patch)


## Compile
```
KERNEL=kernel7 make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
KERNEL=kernel7 make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j3
export INSTALL_MOD_PATH=./modules
rm -rf $INSTALL_MOD_PATH
make modules_install
rm $INSTALL_MOD_PATH/lib/modules/*/build
rm $INSTALL_MOD_PATH/lib/modules/*/source
```

## copy kernel to SD card
$1 is the sd card location (mount point)
```
export INSTALL_MOD_PATH=./modules
sudo cp -r $INSTALL_MOD_PATH/lib/modules $1/rootfs/lib/
cat config_a >> $1/boot/config.txt
cp arch/arm/boot/zImage $1/boot/kernel7.img
cp arch/arm/boot/dts/bcm2710-rpi-cm3.dtb $1/boot/bcm2710-rpi-cm3.dtb
cp arch/arm/boot/dts/overlays/*.dtbo $1/boot/overlays/
```

## config_a
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
```