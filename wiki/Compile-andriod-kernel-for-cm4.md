# Download kernel source for andriod of rpi4

**sync-andriod-kernel.sh**
```
#!/bin/bash

BIN_DIR=$HOME/bin
REPO_PATH=$BIN_DIR/repo

if [ ! -d $BIN_DIR ]
then
    mkdir $BIN_DIR
    add_path_env $BIN_DIR
    curl https://storage.googleapis.com/git-repo-downloads/repo > $REPO_PATH
    chmod a+x $REPO_PATH
else
    echo "folder already exits. $BIN_DIR"
fi

export PATH=$PATH:$BIN_DIR

cd /data/andriod-kernel
repo init -u https://github.com/android-rpi/kernel_manifest -b arpi-5.10
repo sync


```

# Compile 
```
cd /data/andriod-kernel/
./build/build.sh
```

# Replace files

```
sudo losetup --show -f -P lineage-19.1-20220511-UNOFFICIAL-KonstaKANG-rpi4.img # assume /dev/loop0
sudo mount /dev/loop0p1 /mnt/p1

# Copy kernel binaries to boot partition
cp -rf /data/andriod-kernel/out/arpi-5.10/dist/Image to /mnt/p1
cp -rf /data/andriod-kernel/out/arpi-5.10/dist/bcm2711-rpi-*.dtb /mnt/p1
cp -rf /data/andriod-kernel/out/arpi-5.10/dist/vc4-kms-v3d-pi4.dtbo to /mnt/p1/overlays/

sudo umount /mnt/p1
sudo losetup -D /dev/loop0

```

#  Reference Url
* https://github.com/android-rpi/kernel_manifest
* https://github.com/android-rpi/device_arpi_rpi4