#!/usr/bin/python3

import glob
import os
import sys,getopt
import subprocess
import time

cpus = []
mid_freq = 0
max_freq = 0

#Start the fan above this temperature
MAX_TEMP=60000
#Cool additionally this far past MAX_TEMP before turning the fan off
TEMP_THRESH=2000
ONCE_TIME=30

lastTemp = {}

gpiopath="/sys/class/gpio"
gpiopin=96
def init_fan_gpio():
    if not os.path.exists("%s/gpio%i" % (gpiopath,gpiopin)):
        open("%s/export" % (gpiopath),"w").write(str(gpiopin))
    open("%s/gpio%i/direction" % (gpiopath,gpiopin),"w").write("out")

def fan_on():
    init_fan_gpio()
    open("%s/gpio%i/value" % (gpiopath,gpiopin),"w").write("1")
    time.sleep(ONCE_TIME)

def fan_off():
    init_fan_gpio()
    open("%s/gpio%i/value" % (gpiopath,gpiopin),"w").write("0")

def  fan_state():
    return int(open("%s/gpio%i/value" % (gpiopath,gpiopin),"r").read()) == 1

def isDigit(x):
    try:
        float(x)
        return True
    except ValueError:
        return False


def cpu_infos():
    global cpus
    global mid_freq
    global max_freq

    cpus = glob.glob('/sys/devices/system/cpu/cpu[0-9]')
    cpus.sort()
#/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
#/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

    scaling_available_freq = open(os.path.join(cpus[0],"cpufreq/scaling_available_frequencies"),"r").read()
    scaling_available_freq = scaling_available_freq.strip("\n")
    scaling_available_freqs = scaling_available_freq.split(" ")

    for var in scaling_available_freqs:
        if isDigit(var):
            if(int(var) > 1000000):
                if(mid_freq == 0):
                    mid_freq = int(var)
            max_freq = int(var)


def set_gov(gov):
    global cpus
    for var in cpus:
        gov_f = os.path.join(var,"cpufreq/scaling_governor")
        #print(gov_f)
        subprocess.run( "echo %s | sudo tee  %s" %(gov,gov_f),shell=True)
    
def set_performance(scale):
    global cpus
    global mid_freq
    global max_freq

    freq = mid_freq
    if scale =="mid":
        freq = mid_freq
    elif scale =="max":
        freq = max_freq
  
    for var in cpus:
        _f = os.path.join(var,"cpufreq/scaling_max_freq")
        #print(_f)
        subprocess.run( "echo %s | sudo tee  %s" %(freq,_f),shell=True)
  
def fan_loop():
    global lastTemp
    statechange = False
    hotcount = 0
    coolcount = 0
    while True:
        temps = glob.glob('/sys/class/thermal/thermal_zone[0-9]/')
        temps.sort()
        hotcount = 0
        coolcount = 0
        for var in temps:
            #Initial check
            if not var in lastTemp:
                sys.stderr.write("Found zone: " + var + "\n")
                lastTemp[var] = 0
            _f = os.path.join(var,"temp")
            #print( open(_f).read().strip("\n") )
            _t = open(_f).read().strip("\n")
            if isDigit(_t):
                if int(_t) > MAX_TEMP:
                    if lastTemp[var] <= MAX_TEMP:
                        sys.stderr.write("Temp(" + var +"): " + _t + "; hot.\n")
                    hotcount += 1
                else:
                    #Don't turn it off right at the threshold
                    if (int(_t) + TEMP_THRESH) < MAX_TEMP:
                        if (lastTemp[var] +  TEMP_THRESH) >= MAX_TEMP:
                            sys.stderr.write("Temp(" + var + ": " + _t + "; cool.\n")
                        coolcount += 1
                lastTemp[var] = int(_t)
        if hotcount > 0:
            if not fan_state():
                #sys.stderr.write("Temps: " + str(lastTemp) +"\n")
                sys.stderr.write("Fan on, "  +  str(hotcount) + " zones hot.\n")
            fan_on()
        else:
            if coolcount >  0:
                if fan_state():
                    #sys.stderr.write("Temps: " + str(lastTemp) +"\n")
                    sys.stderr.write("Fan off, " + str(hotcount) + " zones hot.\n")
                fan_off()
        time.sleep(5)

def main(argv):
    global cpus
    scale = 'mid'
    gov   = 'powersave'
    try:
        opts, args = getopt.getopt(argv,"hs:g:",["scale=","governor="])
    except getopt.GetoptError:
        print ('test.py -s <scale> -g <governor>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('test.py -s <scale>')
            sys.exit()
        elif opt in ("-s", "--scale"):
            scale = arg
        elif opt in ("-g","--governor"):
            gov = arg

    print ('Scale is ', scale,"Gov is ",gov)

    init_fan_gpio()
    cpu_infos()
    set_gov(gov)
    set_performance(scale)
    fan_loop()


if __name__ == "__main__":
   main(sys.argv[1:])

