# System
ubuntu 22.04 gcc8.4.0
```
sudo apt install gcc-8 gcc-8-aarch64-linux-gnu gcc-8-arm-linux-gnueabihf
```

# Download patch
```
git clone https://github.com/clockworkpi/DevTerm.git
```

# Download kernel and patch it
```
git clone https://github.com/raspberrypi/linux
cd linux
git checkout 3a33f11c48572b9dd0fecac164b3990fc9234da8
cp ~/DevTerm/Code/patch/cm4/cm4_kernel_0704.patch .
git apply cm4_kernel_0704.patch

#Strongly recommend to use gcc 8.4.0 as the cross compiler
KERNEL=kernel7l make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
KERNEL=kernel7l make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j1
INSTALL_MOD_PATH=./modules make modules_install
rm modules/lib/modules/*/build
rm modules/lib/modules/*/source

mkdir output
mkdir output/boot
mkdir output/lib/modules -p
mkdir output/boot/overlays

cp -rf modules/lib/modules/5.10.17-v7l+ output/lib/modules/
cp arch/arm/boot/dts/overlays/*.dtbo output/boot/overlays/
cp arch/arm/boot/dts/bcm2711-rpi-cm4.dtb output/boot/
cp arch/arm/boot/zImage output/boot/kernel7l.img

```

output is the folder contains all kernel stuff  

# config.txt
```
disable_overscan=1
dtparam=audio=on
[pi4]
dtoverlay=vc4-fkms-v3d
max_framebuffers=2

[all]
dtoverlay=dwc2,dr_mode=host
dtoverlay=vc4-kms-v3d-pi4,cma-384
dtoverlay=devterm-pmu
dtoverlay=devterm-panel
dtoverlay=devterm-misc
dtoverlay=audremap,pins_12_13

dtparam=spi=on
gpio=10=ip,np
```
