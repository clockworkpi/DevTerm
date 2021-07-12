# DevTerm Thermal printer 



### Eos/Pos commands example


#### ESC ! n  
set printer font index,n:0-4  
`echo -en "\x1b\x0" > /tmp/DEVTERM_PRINTER_IN`


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
