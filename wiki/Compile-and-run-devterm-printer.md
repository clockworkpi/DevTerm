# a06 a04
```
git clone https://github.com/clockworkpi/DevTerm.git
sudo systemctl stop devterm-socat devterm-printer

sudo apt remove devterm-wiringpi-cpi

cd DevTerm/Code/devterm_wiringpi_cpi/

sudo ./build
cd ~/DevTerm/Code/thermal_printer/
sudo apt install -y libfreetype-dev
make


