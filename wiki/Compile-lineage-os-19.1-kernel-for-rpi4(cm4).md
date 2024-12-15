# Prepare
```
mkdir -p ~/data/andriod
mkdir -p ~/data/github/lineage-rpi
cd ~/data/github/lineage-rpi

git clone https://github.com/lineage-rpi/android_kernel_brcm_rpi -b lineage-19.1
git clone https://github.com/lineage-rpi/proprietary_vendor_brcm -b lineage-19.1

cd ~/data/andriod

git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9 -b pie-release
```
# Fix firmware path

edit `~/data/github/lineage-rpi/android_kernel_brcm_rpi/arch/arm64/configs/lineageos_rpi4_defconfig` line 1537  
to be  
`CONFIG_EXTRA_FIRMWARE_DIR="/home/cpi/data/github/lineage-rpi/proprietary_vendor_brcm/rpi4/proprietary/vendor/firmware"`  

# Compile

```
cd ~/data/github/lineage-rpi/android_kernel_brcm_rpi
```

create **m.sh** with content like:

```
#!/bin/bash

export PATH=/home/cpi/data/andriod/aarch64-linux-android-4.9/bin:$PATH

ARCH=arm64 CROSS_COMPILE=aarch64-linux-androidkernel- make lineageos_rpi4_defconfig
ARCH=arm64 CROSS_COMPILE=aarch64-linux-androidkernel- make Image dtbs -j1
```

```
chmod +x m.sh
./m.sh # start kernel compile
```

# Replace files
copy 
* bcm2711-rpi-400.dtb
* bcm2711-rpi-4-b.dtb
* bcm2711-rpi-cm4.dtb
* bcm2711-rpi-cm4s.dtb  


from `~/data/github/lineage-rpi/android_kernel_brcm_rpi/arch/arm64/boot/dts/broadcom/` to lineage os image boot partition

copy `~/data/github/lineage-rpi/android_kernel_brcm_rpi/arch/arm64/boot/Image` to to lineage os image boot partition

copy `~/data/github/lineage-rpi/android_kernel_brcm_rpi/arch/arm64/boot/dts/overlays/vc4-kms-v3d.dtbo` to to lineage os image boot partition/overlays

# Kernel config
https://gist.github.com/cuu/92bfa28a9b6de421834e5f9f408a12a4
