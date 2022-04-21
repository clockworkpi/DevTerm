# DevTerm Thermal printer 

### Console commands example


`echo "Hello DevTerm" > /tmp/DEVTERM_PRINTER_IN`

`echo -e "Hello DevTerm\n\n\n\n\n\n" > /tmp/DEVTERM_PRINTER_IN`

`cat file.txt > /tmp/DEVTERM_PRINTER_IN`

`ncal -hb | tee > /tmp/DEVTERM_PRINTER_IN`

### Eos/Pos commands example


#### ESC ! n  
set printer font index,n:0-4  
`echo -en "\x1d\x21\x0" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1d\x21\x1" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1d\x21\x2" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1d\x21\x3" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1d\x21\x4" > /tmp/DEVTERM_PRINTER_IN`  

##### ascii font size 
1. 0 = 8x16
1. 1 = 5x7
1. 2 = 6x12
1. 3 = 7x14
1. 4 = 8x16

##### unicode font size  

1. 0 = 12x12
1. 1 = 14x14
1. 2 = 16x16
1. 3 = 18x18
1. 4 = 20x20



https://github.com/clockworkpi/DevTerm/blob/81addc7f4ba1eb4acb2f59fb1fef70386dbe1f0d/Code/thermal_printer/devterm_thermal_printer.c#L381

#### DC2 # n   
n:0-F, set printer printing density 

`echo -en "\x12\x23\x8" > /tmp/DEVTERM_PRINTER_IN`

#### DC2 T  
print the test page  
`echo -en "\x12\x54" >  /tmp/DEVTERM_PRINTER_IN`


### UNICODE
Edit `/usr/local/etc/devterm-printer` ,point to an existed ttf file,eg: 
`TTF=/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc`

then restart devterm-printer.service with
```
sudo systemctl restart devterm-printer
```
devterm-printer daemon will auto detect the ttf font file
 
#### switch to UNICODE MODE  
```
echo -en "\x1b\x21\x1" > /tmp/DEVTERM_PRINTER_IN  
```
#### switch back to default ASCII mode  
```
echo -en "\x1b\x21\x0" > /tmp/DEVTERM_PRINTER_IN  
```

then

```
echo "日月火水木金土 ΕΙΝΑΙ Ο ΘΕΟΣ ΓΕΩΜΕΤΡΗΣ" > /tmp/DEVTERM_PRINTER_IN
```
to print unicode characters

#### ESC V n rotation command,unicode mode only

`echo -en "\x1b\x56\x0" >/tmp/DEVTERM_PRINTER_IN`  disable rotation  

`echo -en "\x1b\x56\x1" >/tmp/DEVTERM_PRINTER_IN`  90 degree   
`echo -en "\x1b\x56\x2" >/tmp/DEVTERM_PRINTER_IN`  180 degree  
`echo -en "\x1b\x56\x3" >/tmp/DEVTERM_PRINTER_IN`  270 degree  

### How to run it from source

* make  
* sudo systemctl stop devterm-printer   
* sudo cp -rf devterm_thermal_printer.elf /usr/local/bin  
* sudo systemctl start devterm-printer  
