![scr](scr.png?raw=true "Screenshot")

Devterm R01

`d1_twm.tar.bz2`  devterm r01 stock os image twm configs  
```
.
..
.Xdefaults
.Xresources
.bash_profile
.bashrc
.gkrellm2
.twm
.twmrc
.xinitrc
readme
```

`tar xpjfv d1_twm.tar.bz2 -C /home/cpi`  
 
## Expand R01 rootfs partition size
```
wget https://github.com/clockworkpi/DevTerm/raw/main/Code/R01/expand_devterm_d1_root.sh
chmod +x expand_devterm_d1_root.sh
sudo ./expand_devterm_d1_root.sh
```
