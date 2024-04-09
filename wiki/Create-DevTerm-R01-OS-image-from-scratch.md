
# Before start 
Our system image is modified based on fedora wiki  
Mainly replace the kernel and rootfs  
Some linux experience is required to handle every possible situation  
there are some discrepancies with the final system image  
So this document is more suitable for the guidance of the R-01 system image  


# Prepare

a ubuntu linux system, like ubuntu 21.04

install riscv64 gcc
```
sudo apt install gcc-11-riscv64-linux-gnu binutils-riscv64-linux-gnu
sudo apt install git
sudo apt install make
sudo apt install build-essential
sudo apt install libfontconfig1
sudo apt install ncurses-devel
sudo apt install libncurses-dev
sudo apt install build-essential
sudo apt install flex bison
sudo apt install python3-distutils
sudo apt install swig
sudo apt install python3-dev
sudo apt install openssl
sudo apt install libssl-dev

```

install qemu static for chroot

`sudo apt install qemu-user-static`


manually fix default compiler link:

```
sudo ln -s /usr/bin/riscv64-linux-gnu-cpp-11 /usr/bin/riscv64-linux-gnu-cpp
sudo ln -s /usr/bin/riscv64-linux-gnu-gcc-nm-11 /usr/bin/riscv64-linux-gnu-gcc-nm
sudo ln -s /usr/bin/riscv64-linux-gnu-gcov-dump-11 /usr/bin/riscv64-linux-gnu-gcov-dump
sudo ln -s /usr/bin/riscv64-linux-gnu-gcc-11 /usr/bin/riscv64-linux-gnu-gcc
sudo ln -s /usr/bin/riscv64-linux-gnu-gcc-ranlib-11 /usr/bin/riscv64-linux-gnu-gcc-ranlib
sudo ln -s /usr/bin/riscv64-linux-gnu-gcov-tool-11 /usr/bin/riscv64-linux-gnu-gcov-tool
sudo ln -s /usr/bin/riscv64-linux-gnu-gcc-ar-11 /usr/bin/riscv64-linux-gnu-gcc-ar
sudo ln -s /usr/bin/riscv64-linux-gnu-gcov-11 /usr/bin/riscv64-linux-gnu-gcov
sudo ln -s /usr/bin/riscv64-linux-gnu-lto-dump-11 /usr/bin/riscv64-linux-gnu-lto-dump
```


# Bootloader

### reference doc:  
https://fedoraproject.org/wiki/Architectures/RISC-V/Allwinner


```
git clone https://github.com/smaeul/sun20i_d1_spl
pushd sun20i_d1_spl
git checkout origin/mainline
make CROSS_COMPILE=riscv64-linux-gnu- p=sun20iw1p1 mmc
popd
```

```
pushd sun20i_d1_spl
sudo dd if=nboot/boot0_sdcard_sun20iw1p1.bin of=/dev/sdX bs=512 seek=16
```

git clone https://github.com/tekkamanninja/opensbi -b allwinner_d1  
pushd opensbi  
CROSS_COMPILE=riscv64-linux-gnu- PLATFORM=generic FW_PIC=y BUILD_INFO=y make  
popd  

```
git clone https://github.com/tekkamanninja/u-boot -b allwinner_d1  
pushd u-boot  
make CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv nezha_defconfig  
make CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv menuconfig  
make CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv u-boot.bin u-boot.dtb  
popd  
```
**in menuconfig, there is an import option must be selected:**

 ![menuconfig](https://raw.githubusercontent.com/clockworkpi/DevTerm/main/Pictures/Image%202022-04-11%20112248.png)

otherwises, the boot will hang 


## u-boot.toc1

**toc1.cfg**

```
[opensbi]
file = fw_dynamic.bin
addr = 0x40000000
[dtb]
file = u-boot.dtb
addr = 0x44000000
[u-boot]
file = u-boot.bin
addr = 0x4a000000
```
```
pushd u-boot
cp ${PATH_TO_TOC1_CFG}/toc1.cfg ${PATH_TO_OPENSBI}/fw_dynamic.bin .
tools/mkimage -T sunxi_toc1 -d toc1.cfg  u-boot.toc1
popd
```

pushd u-boot  
sudo dd if=u-boot.toc1 of=/dev/sdX bs=512 seek=32800
![boot ](https://raw.githubusercontent.com/clockworkpi/DevTerm/main/Pictures/Image%202022-04-10%20192110.png)


# Disk partition prepare

## Original fedora image fdisk partition
```
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0xda568ce2

Device                                                                            Boot   Start      End  Sectors  Size Id Type
fedora-riscv64-d1-developer-xfce-rawhide-Rawhide-20211124-132603.n.0-sda.raw.img2        69632   319487   249856  122M  c W95 FAT32 (LBA)
fedora-riscv64-d1-developer-xfce-rawhide-Rawhide-20211124-132603.n.0-sda.raw.img3 *     319488  1320959  1001472  489M 83 Linux
fedora-riscv64-d1-developer-xfce-rawhide-Rawhide-20211124-132603.n.0-sda.raw.img4      1320960 25319423 23998464 11.5G 83 Linux
```


## Create disk image file

### create image file with dd
```
MEGA=16777216

dd if=/dev/zero bs=$MEGA count=1024 of=disk.img
echo -e "o\nn\np\n2\n69632\n319487\nn\np\n3\n319488\n1320959\nn\np\n4\n1320960\n\n\nt\n2\nc\na\n3\n\nw" | fdisk disk.img
fdisk -l disk.img
```
```
sudo losetup --show -f -P disk.img #/dev/loop1
sudo mkfs.vfat /dev/loop1p2
sudo mkfs.ext4 /dev/loop1p3
sudo mkfs.ext4 /dev/loop1p4
```

# Download ubuntu riscv image for rootfs

https://wiki.ubuntu.com/RISC-V

https://cdimage.ubuntu.com/releases/21.04/release/

https://cdimage.ubuntu.com/releases/21.04/release/ubuntu-21.04-preinstalled-server-riscv64+unmatched.img.xz

## Take rootfs of ubuntu out
```
sudo losetup --show -f -P ubuntu-21.04-preinstalled-server-riscv64+unmatched.img # eg: /dev/loop1,depends on your linux system

sudo mount /dev/loop1p1 /mnt/p1
sudo tar cpjfv ubuntu-21.04-preinstalled-server-riscv64+unmatched_rootfs.tar.bz2 -C /mnt/p1/ .
sudo umount /mnt/p1
sudo losetup -D /dev/loop1 #dettach,release /dev/loop1
```
download link:
https://mega.nz/file/IQFlWZKL#_ERlz3GXoUgxsIoaLHK8oQ2A6SNFcHhZpS2tF3N6RaQ  
https://mega.nz/file/IIlAHKST#PP8feOr6tHxdbD-Zoyj64G4TfG7HG_1s3VHasisP6h0



## Prebuilt files

### boot partition 


#### ubuntu_2104_devterm_d1_boot.tar.bz2  
```
https://mega.nz/file/NEsV1SRQ#FexV7wJhiUQKLcfBol4bB5m8UsX1O5qWet8geONclY4
https://mega.nz/file/YJUjiKKb#nrxH6BDvyGjpNSyZEzbRqbHk9ed9RE7jcuOPbTdximM
```

*included files*

```
├── board.dtb  
├── **boot**  
│ └── uEnv.txt  
├── config-5.4.61  
├── **efi**  
├── **extlinux**  
│ ├── extlinux.conf  
│ └── extlinux.conf.cpi.bak  
├── **grub2**  
│ └── **themes**  
│ └── **system**  
│ └── **background.png**  
├── grub.cfg  
├── System.map-5.4.61  
├── **uEnv.txt** -\> boot/uEnv.txt  
└── vmlinuz-5.4.61  
```

#### ubuntu_2104_devterm_d1_EFI.tar.bz2  

https://mega.nz/file/kNUBRBIB#NsPzKTDWbfBSTth7-eVz9gPjeaEdYnP7PfuYGz28cOk

**extlinux.conf is the file actually do the boot configuration**
 
# How to compile kernel

## get kernel source 
https://github.com/cuu/last_linux-5.4.git

it is a mirror kernel code from tina_d1_h  of all winner with all patched for devterm R-01
## Official toolchain
https://github.com/cuu/toolchain-thead-glibc #It's official allwinner toolchain
download toolchain

## Steps for compiling kernel

extract and put toolchain in any folder you like

then edit **m.sh** in `last_linux-5.4`

set correct path of toolchain for PATH

```
export PATH=/data/tina_d1_h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/bin/:$PATH

```
to be 
```
export PATH=/wherever_you_put_toolchain/riscv64-glibc-gcc-thead_20200702/bin/:$PATH
```

then simply run ./m.sh to compile kernel

we just simple use the prebuit toolchain from all-winner to compile the patched kernel code based on all-winner tina for devterm_r01

and we have to ,because allwinner prebuilt toolchain has the neccessary custom opcode for the kernel of R-01

kernel patch url: https://github.com/clockworkpi/DevTerm/tree/main/Code/patch/d1

to get original tina code from allwinner ,visit https://open.allwinnertech.com/

## Mainline toolchain
https://github.com/riscv/riscv-gnu-toolchain  
all the same as Official toolchain except need to disable Vector in kernel `make menuconfig`  
  
![d1_disable_vector_kernel](https://raw.githubusercontent.com/clockworkpi/DevTerm/main/Pictures/d1_disable_vector_kernel.jpg)  

# Make alpha os image 

required file list:
* ubuntu_2104_devterm_d1_EFI.tar.bz2   
* ubuntu_2104_devterm_d1_boot.tar.bz2  
* ubuntu-21.04-preinstalled-server-riscv64+unmatched_rootfs.tar.bz2  
* boot0_sdcard_sun20iw1p1.bin
* u-boot.toc1 
* disk.img with all partitions formatted


```
sudo losetup --show -f -P disk.img  ## assume we will have /dev/loop1
sudo dd if=boot0_sdcard_sun20iw1p1.bin of=/dev/loop1 bs=512 seek=16
sudo dd if=u-boot.toc1 of=/dev/loop1 bs=512 seek=32800

sudo mount /dev/loop1p2 /mnt/p2
sudo mount /dev/loop1p3 /mnt/p3
sudo mount /dev/loop1p4 /mnt/p4
sudo tar xpjfv  ubuntu_2104_devterm_d1_EFI.tar.bz2 -C /mnt/p2
sudo tar xpjfv  ubuntu_2104_devterm_d1_boot.tar.bz2  -C /mnt/p3
sudo tar xpjfv ubuntu-21.04-preinstalled-server-riscv64+unmatched_rootfs.tar.bz2  -C /mnt/p4

```

## Chroot part
```
sudo mount /dev/loop1p3 /mnt/p4/boot
cd /mnt/p4

sudo mount --bind /dev dev/
sudo mount --bind /sys sys/
sudo mount --bind /proc proc/
sudo mount --bind /dev/pts dev/pts
sudo chroot .
```
### upgrade to devel 
now we are in chroot  
```
touch /etc/cloud/cloud-init.disabled
apt remove update-notifier-common  
apt-get update && apt-get purge needrestart
```
then edit `/etc/apt/source.list`, replace all hirsute to devel 
then
```
apt update
apt-get dist-upgrade
```
wait a long time until upgrade done(or retry if failed,it is neccessary for having a newer xorg/mesa in riscv to display)

after upgrade
```
apt update 
apt install  mesa-utils libgl1-mesa-glx libglx-mesa0 libgles2-mesa libegl-mesa0 
apt install -y  xterm cmus alsamixergui qutebrowser elinks vim emacs tilde bc gnuplot dosbox chocolate-doom gimp xfig xpdf xaos x11-apps gkrellm  imagemagick inkscape
apt remove multipath-tools
```

### add cpi user
```
useradd -m  cpi

#add into groups
usermod -a -G adm,dialout,cdrom,floppy,sudo,audio,dip,video,plugdev,netdev,lxd,pulse,pulse-access cpi
```
### Deploy the TWM window manager env.

The R-01 uses twm as default window manager for low resource usage

```
wget https://github.com/clockworkpi/DevTerm/blob/main/Code/R01/d1_twm.tar.bz2?raw=true

tar xpjfv d1_twm.tar.bz2 -C /home/cpi

chown -R cpi:cpi /home/cpi

```

### xorg fbdev config
rotate Xorg 
```
ubuntu@ubuntu:~$ cat /etc/X11/xorg.conf.d/10-d1.conf  
Section "Device" 
        Identifier "FBDEV" 
        Driver "fbdev" 
        Option "fbdev" "/dev/fb0" 
        Option "Rotate" "cw" 
        Option          "SwapbuffersWait" "true" 
EndSection 
 
Section "Screen" 
        Identifier "Screen0" 
        Device "FBDEV" 
        DefaultDepth 24 
 
        Subsection "Display" 
                Depth 24 
                Modes "1280x480" "480x1280" 
        EndSubsection 
 
EndSection
```

### Optmise time of boot
```
sudo systemctl mask apt-daily.service apt-daily-upgrade.service
sudo systemctl disable apt-daily.service apt-daily-upgrade.service
sudo systemctl disable apt-daily.timer apt-daily-upgrade.timer

sudo systemctl disable NetworkManager-wait-online.service 
sudo systemctl disable systemd-random-seed.service

sudo systemctl disable  e2scrub_reap.service
sudo systemctl disable systemd-networkd-wait-online.service

sudo systemctl disable lvm2-monitor.service

sudo systemctl disable snapd.seeded.service 
sudo systemctl disable pppd-dns.service

sudo systemctl disable avahi-daemon.service
sudo systemctl disable ModemManager.service

sudo systemctl disable apport.service
sudo systemctl mask apport.service
sudo systemctl disable accounts-daemon.service
sudo systemctl mask accounts-daemon.service
sudo systemctl disable udisks2.service   
```

### faster login
```
All scripts in /etc/update-motd.d/ transferred to other place or just deleted them all
```

### Install devterm related package
```
curl https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo tee /etc/apt/trusted.gpg.d/clockworkpi.asc
echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee  /etc/apt/sources.list.d/clockworkpi.list  

apt update

apt install devterm-audio-patch devterm-backlight-cpi devterm-keyboard-firmware devterm-thermal-printer devterm-thermal-printer-cups devterm-wiringpi-cpi
```

## Get out chroot

```
exit
cd 

sudo umount /mnt/p4/dev/pts
sudo umount /mnt/p4/dev/
sudo umount /mnt/p4/proc
sudo umount /mnt/p4/sys
sudo umount /mnt/p4/boot

sudo umount /mnt/p3
sudo umount /mnt/p2
sudo umount /mnt/p4

sudo losetup -D /dev/loop1

```

# Final part

now we have a chroot edited os image file disk.img in 16GB with ubuntu devel branch root fs

you can still use 
```
sudo losetup --show -f -P disk.img  
sudo gparted /dev/loop1
``` 
to shrink the disk size to be like 8G or less to fit your sd card

then dd it into sd card


# Compiling other stuff

## WiringPI
```
git clone https://github.com/clockworkpi/DevTerm.git
wget https://github.com/WiringPi/WiringPi/archive/refs/tags/final_official_2.50.tar.gz
tar zxvf final_official_2.50.tar.gz 
cd WiringPi-final_official_2.50/
cp ../DevTerm/Code/patch/d1/wiringCP0329.patch .
git apply wiringCP0329.patch
sudo ./build
#Choice: 2

```

# For apt upgrade

apt upgrade on R01 os will mess up the content of extlinux.conf  
lead the boot failed  

so mount the sd card(with R01 os)  on a PC (like Linux )

then go into the **boot partition** of the sd card

you will see 
```
extlinux.conf  
extlinux.conf.cpi.bak  
```
replace extlinux.conf  with extlinux.conf.cpi.bak

the **extlinux.conf.cpi.bak** is for this kind of situation

# Hints
https://forum.clockworkpi.com/t/r01-os-early-hints/8636  