# Prepare
Before everything
We assume that you have a Devterm with stock os running  
and a little bit of Linux compiling experience   
know how to install packages by package manager like apt    

## WiringPi 
### CM3 or CM4 
```
sudo apt install -y wiringpi libwiringpi-dev libcups2-dev 
```

### A06 A04
```
git clone https://github.com/clockworkpi/DevTerm.git
cd DevTerm/Code/devterm_wiringpi_cpi/
sudo ./build
#chose 0 or 1 depend which board you have
```

### R01
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

# Compile thermal printer driver code
```
git clone https://github.com/clockworkpi/DevTerm.git
cd DevTerm/Code/thermal_printer
make  
sudo systemctl stop devterm-printer   
sudo cp -rf devterm_thermal_printer.elf /usr/local/bin  
sudo systemctl start devterm-printer 
```
## Debug or run it from manually
1. setup socat socket for data receving for thermal printer
```
   sudo systemctl stop devterm-printer #stop service
   sudo systemctl stop devterm-socat   #stop service
   sudo socat -d -d pty,link=/tmp/DEVTERM_PRINTER_OUT,raw,echo=0 pty,link=/tmp/DEVTERM_PRINTER_IN,raw,echo=0
   sudo ./usr/local/bin/devterm_socat.sh # <- this .sh file is in DevTerm/Code/thermal_printer/usr/local/bin/
```
1. run devterm_thermal_printer.elf
```
   sudo ./devterm_thermal_printer.elf
```

now you can test the printer by cat or echo something into `/tmp/DEVTERM_PRINTER_IN`   
eg: `echo "hello world\n\n\n\n\n\n\n\n\n\n" > /tmp/DEVTERM_PRINTER_IN

we recommend use [tmux](https://github.com/tmux/tmux/wiki) to do this job  
so that you can run and see all the commands output in a single terminal window

## For CUPS
just install `devterm-thermal-printer-cups`  
``` sudo apt install devterm-thermal-printer-cups```

this package will add a CUPS serial printer adapter,a CUPS filter for the DevTerm's thermal printer  
and will let other programs to  print content through image printing using DevTerm's thermal printer  
source code is in https://github.com/clockworkpi/DevTerm/tree/main/Code/devterm_thermal_printer_cups

all devterm related deb packages available at 

https://github.com/clockworkpi/apt