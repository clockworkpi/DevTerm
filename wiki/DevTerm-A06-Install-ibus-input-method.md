`sudo apt install ibus ibus-pinyin -y`
```
cat <<EOF >~/.config/autostart/ibus.desktop
[Desktop Entry]
Exec=ibus-daemon -drxR
GenericName=IBus
Name[zh_CN]=IBus
Name=IBus
Name[en_US]=IBus
StartupNotify=true
Terminal=false
Type=Application
EOF
```

```
sudo bash -c 'cat <<EOF >/etc/profile.d/ibus.sh
#!/bin/bash

export XIM_PROGRAM=ibus
export XIM=ibus
export XMODIFIERS=@im=ibus
export GTK_IM_MODULE=ibus
export QT_IM_MODULE=ibus
EOF'
```

`sudo reboot `

Then right-click the ibus icon in the upper right corner of the top menu  
select preferences  
add (Add) **pinyin** input method, the default is super+space to enable the input method  
you can change the shortcut in **iBus Preferences**
