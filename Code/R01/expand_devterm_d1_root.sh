#!/bin/bash

GREEN='\033[1;32m'
NC='\033[0m'

printf "${GREEN}Installing dependencies...${NC}\n"

# Install growpart util
export DEBIAN_FRONTEND=noninteractive
sudo apt-get -y install cloud-guest-utils

printf "${GREEN}Growing partition...${NC}\n"

# Grow root partition
sudo growpart /dev/mmcblk0 4

printf "${GREEN}Resizing file system...${NC}\n"

# Resize file system
sudo resize2fs /dev/mmcblk0p4

printf "${GREEN}Cleaning up...${NC}\n"

# Uninstall growpart package again
#sudo apt-get -y remove cloud-guest-utils

printf "\n${GREEN}Done! 😊 ${NC}\n\n"

#rm -rf  /etc/init.d/expand_devterm_d1_root.sh
#unlink /etc/rc3.d/S01expand_devterm_d1_root.sh	

