4G modem
# Packages

```
sudo apt install -y modemmmanager  network-manager  pppoe
```


# Power ON 4G ext(uconsole)
```
echo "pulling up gpio 34"
sudo gpio mode 34 out
sudo gpio write 34 1

echo "pulling up 33 to reset "
sudo gpio mode 33 out
sudo gpio write 33 1

sleep 5
sudo gpio write 33 0
echo "pulling back 33"
sleep 10
echo "done"

```
## cm4 version
```
sudo gpio mode 24 out 
sudo gpio write 24 1 

sudo gpio mode 15 out 
sudo gpio write 15 1 

sleep 5
sudo gpio write 15 0
```

## devterm 4G module
POWER 42  
RESET 43

# Power OFF 4G ext(uconsole)

```
echo "Power off 4G module"
sudo gpio mode 34 out 
sudo gpio write 34 0 

sudo gpio write 34 1
sleep 3
sudo gpio write 34 0

sleep 10
echo "Done"
```

## cm4 version
```
echo "Power off 4G module"
sudo gpio mode 24 out 
sudo gpio write 24 0 

sudo gpio write 24 1
sleep 3
sudo gpio write 24 0

sleep 10
echo "Done"
```

## List modem
```
mmcli -L
```

/org/freedesktop/ModemManager1/Modem/0 [QUALCOMM INCORPORATED] SIMCOM_SIM7600G-H

## Enable modem
```
sudo mmcli -m 0 - e
```

## How to use nmcli to create a NetworkManager 4G connection 

```
sudo nmcli c add type gsm ifname cdc-wdm0 con-name 4GNet apn yourapn gsm.username gsmusername gsm.password gsmpassword
```

replace **yourapn** and **gsmusername** with your carrier service

then we can see a `4GNet` in NetworkManager (like KDE's network manager UI)

I got the cdc-wdm0 from with **qmi_wwan** driver
```
mmcli -m 0 | grep "primary port"
```
it will be **ttyUSB2** if use **qcdm** driver 


Now we can use **nmtui** to connect 4GNet  
or
```
sudo nmcli connection up 4GNet  
sudo nmcli connection down 4GNet
```
if everything is right ,you will have your **ppp0**  in `sudo ifconfig`

Here is reference arguments I've used for nmcli

```
connection.id:                          Movistar
connection.uuid:                        acab2207-347a-424c-b366-b2c4ef4e4c75
connection.stable-id:                   --
connection.type:                        gsm
connection.interface-name:              ttyAMA0
connection.autoconnect:                 no
connection.autoconnect-priority:        0
connection.autoconnect-retries:         -1 (default)
connection.multi-connect:               0 (default)
connection.auth-retries:                -1
connection.timestamp:                   0
connection.read-only:                   no
connection.permissions:                 --
connection.zone:                        --
connection.master:                      --
connection.slave-type:                  --
connection.autoconnect-slaves:          -1 (default)
connection.secondaries:                 --
connection.gateway-ping-timeout:        0
connection.metered:                     unknown
connection.lldp:                        default
connection.mdns:                        -1 (default)
connection.llmnr:                       -1 (default)
ipv4.method:                            auto
ipv4.dns:                               --
ipv4.dns-search:                        --
ipv4.dns-options:                       ""
ipv4.dns-priority:                      0
ipv4.addresses:                         --
ipv4.gateway:                           --
ipv4.routes:                            --
ipv4.route-metric:                      -1
ipv4.route-table:                       0 (unspec)
ipv4.ignore-auto-routes:                no
ipv4.ignore-auto-dns:                   no
ipv4.dhcp-client-id:                    --
ipv4.dhcp-timeout:                      0 (default)
ipv4.dhcp-send-hostname:                yes
ipv4.dhcp-hostname:                     --
ipv4.dhcp-fqdn:                         --
ipv4.never-default:                     no
ipv4.may-fail:                          yes
ipv4.dad-timeout:                       -1 (default)
ipv6.method:                            auto
ipv6.dns:                               --
ipv6.dns-search:                        --
ipv6.dns-options:                       ""
ipv6.dns-priority:                      0
ipv6.addresses:                         --
ipv6.gateway:                           --
ipv6.routes:                            --
ipv6.route-metric:                      -1
ipv6.route-table:                       0 (unspec)
ipv6.ignore-auto-routes:                no
ipv6.ignore-auto-dns:                   no
ipv6.never-default:                     no
ipv6.may-fail:                          yes
ipv6.ip6-privacy:                       -1 (unknown)
ipv6.addr-gen-mode:                     stable-privacy
ipv6.dhcp-duid:                         --
ipv6.dhcp-send-hostname:                yes
ipv6.dhcp-hostname:                     --
ipv6.token:                             --
serial.baud:                            115200
serial.bits:                            8
serial.parity:                          none
serial.stopbits:                        1
serial.send-delay:                      0
ppp.noauth:                             yes
ppp.refuse-eap:                         no
ppp.refuse-pap:                         no
ppp.refuse-chap:                        no
ppp.refuse-mschap:                      no
ppp.refuse-mschapv2:                    no
ppp.nobsdcomp:                          no
ppp.nodeflate:                          no
ppp.no-vj-comp:                         no
ppp.require-mppe:                       no
ppp.require-mppe-128:                   no
ppp.mppe-stateful:                      no
ppp.crtscts:                            no
ppp.baud:                               115200
ppp.mru:                                0
ppp.mtu:                                auto
ppp.lcp-echo-failure:                   0
ppp.lcp-echo-interval:                  0
gsm.number:                             *99#
gsm.username:                           --
gsm.password:                           <hidden>
gsm.password-flags:                     0 (none)
gsm.apn:                                internet.movistar.com.co
gsm.network-id:                         --
gsm.pin:                                <hidden>
gsm.pin-flags:                          0 (none)
gsm.home-only:                          no
gsm.device-id:                          --
gsm.sim-id:                             --
gsm.sim-operator-id:                    --
gsm.mtu:                                auto
proxy.method:                           none
proxy.browser-only:                     no
proxy.pac-url:                          --
proxy.pac-script:                       --
```

if nmcli can not start the gsm connection with errors like Ipv4 stack ,dual-stack addressing not supported by the modem  
that means we need to re-compile the kernel   

requires all ppp driver in kernel   
**Devices drivers ->Network device support**  
and compile as *, not module  

```
Linux Kernel Configuration
└─> Device Drivers
   └─> USB support
      └─> USB Wireless Device Management support
as module
```

```
Linux Kernel Configuration
└─> Device Drivers
   └─> Network device support
     └─> USB Network Adapters
        └─> QMI WWAN driver for Qualcomm MSM based 3G and LTE modems

as module
```

on A06 
```
sudo apt install pppoe
```

## In order to use SIMCOM_SIM7600G-H to call or receive calls
we have to blacklist some kernel modules

```
$ cat /etc/modprobe.d/blacklist-qmi.conf
blacklist qmi_wwan
blacklist cdc_wdm
```

```
mmcli -m 0 --messaging-list-sms

mmcli -m 0  --voice-list-calls

mmcli -m 0 --voice-create-call='number=xxxxxxxxxxxx'

mmcli -m 0  --voice-list-calls

mmcli -m 0 --start -o 0

mmcli -m 0 --accept -o 1
```

# Enable ModemManager debug  
change `/etc/systemd/system/dbus-org.freedesktop.ModemManager1.service`  
add --debug after `/usr/sbin/ModemManager`  
```bash
...
[Service]
Type=dbus
BusName=org.freedesktop.ModemManager1
ExecStart=/usr/sbin/ModemManager --debug
StandardError=null
Restart=on-abort
...
```
Then  

```
sudo systemctl daemon-reload
sudo systemctl restart  ModemManager.service  
```

## show audio levels
```
sudo mmcli -m 0 --command "AT+CLVL=?"
response: '+CLVL: (0-5)'
```
## show current levels
```
sudo mmcli -m 0 --command "AT+CLVL?"
response: '+CLVL: 4'
```
```
sudo mmcli -m 0 --command "AT+CLVL=5"
response: ''
```
```
sudo mmcli -m 0 --command "AT+CLVL=10"
error: command failed: 'GDBus.Error:org.freedesktop.ModemManager1.Error.MobileEquipment.Unknown: Unknown error'
```

## mute mic
```
sudo mmcli -m 0 --command "AT+CMUT=?"
response: '+CMUT: (0-1)'
```
```
sudo mmcli -m 0 --command "AT+CMUT?"
response: '+CMUT: 0'
```
```
Mute (but may need to be used during a call)
sudo mmcli -m 0 --command "AT+CMUT=1"
```

no ring volume 

## Test signal 
```
sudo mmcli -m any --signal-setup=10 # 10secs to refresh   
sudo mmcli -m any # to see signal quality
```