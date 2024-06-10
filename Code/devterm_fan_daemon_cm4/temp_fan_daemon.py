###devterm raspberry pi fan control daemon
import os
import time


def isDigit(x):
    try:
        float(x)
        return True
    except ValueError:
        return False

def measure_temp():
    temp = os.popen("/usr/bin/vcgencmd measure_temp").readline()
    temp2 = temp.replace("temp=","")
    temp3 = temp2.replace("'C","").strip()
    #print(temp3)
    if isDigit(temp3):
        return float(temp3)
    else:
        return 0


def init_fan_gpio():
    os.popen("gpio mode 17 out")

def fan_on():
    init_fan_gpio()
    os.popen("gpio write 17 1")

def fan_off():
    init_fan_gpio()
    os.popen("gpio write 17 0")


MAX_TEMP=80

init_fan_gpio()

while True:
    temp =  measure_temp()
    if(temp > MAX_TEMP):
        fan_on()
    else:
        fan_off()
    
    time.sleep(5)




