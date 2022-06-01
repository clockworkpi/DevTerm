# Fan control daemon for devterm A06()
 
## Install

**devterm is pre-installed this package, so devterm A06 users do not need to repeat the installation steps**

```
wget -O - https://raw.githubusercontent.com/clockworkpi/apt/main/debian/KEY.gpg | sudo apt-key add -
echo "deb https://raw.githubusercontent.com/clockworkpi/apt/main/debian/ stable main" | sudo tee -a /etc/apt/sources.list.d/clockworkpi.list

sudo apt update && apt install -y devterm-fan-daemon-cpi-a06 devterm-wiringpi-cpi
```

## Change the threshold temperature

Edit `/usr/local/bin/temp_fan_daemon_a06.py`

line starts with `MAX_TEMP=70000`

change the value of MAX_TEMP to be your target temperature value multiplied by 1000,eg 70000 for 70 degree,80000 for 80 degree  

for A06 recommended reference temperature range 70-80 

then restart systemd service to take effect

`sudo systemctl restart devterm-fan-temp-daemon-a06`


