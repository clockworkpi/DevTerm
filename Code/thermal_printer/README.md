# DevTerm Thermal printer 

### Console commands example


`echo "Hello DevTerm" > /tmp/DEVTERM_PRINTER_IN`

`echo -e "Hello DevTerm\n\n\n\n\n\n" > /tmp/DEVTERM_PRINTER_IN`

`cat file.txt > /tmp/DEVTERM_PRINTER_IN`

`ncal -hb | tee > /tmp/DEVTERM_PRINTER_IN`

### Eos/Pos commands example


#### ESC ! n  
set printer font index,n:0-4  
`echo -en "\x1b\x21\x0" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1b\x21\x1" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1b\x21\x2" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1b\x21\x3" > /tmp/DEVTERM_PRINTER_IN`  
`echo -en "\x1b\x21\x4" > /tmp/DEVTERM_PRINTER_IN`  

https://github.com/clockworkpi/DevTerm/blob/81addc7f4ba1eb4acb2f59fb1fef70386dbe1f0d/Code/thermal_printer/devterm_thermal_printer.c#L381

#### DC2 # n   
n:0-F, set printer printing density 

`echo -en "\x12\x23\x8" > /tmp/DEVTERM_PRINTER_IN`

#### DC2 T  
print the test page  
`echo -en "\x12\x54" >  /tmp/DEVTERM_PRINTER_IN`


### How to run it from source

* make  
* sudo systemctl stop devterm-printer   
* sudo cp -rf devterm_thermal_printer.elf /usr/local/bin  
* sudo systemctl start devterm-printer  
