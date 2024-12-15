# Upgrade bootloader and firmware

for os image 69 and later

https://forum.rvspace.org/t/visionfive-2-debian-image-december-released/1097/43?page=2  

https://github.com/starfive-tech/VisionFive2/releases/  download latest sdcard.img, dd to sd card, then hdmi or serial port wait for the system to start

## Flash files
```
flashcp -v u-boot-spl.bin.normal.out /dev/mtd0
flashcp -v visionfive2_fw_payload.img /dev/mtd1
```

## 7110 full os large mirror address
https://debian.starfivetech.com/