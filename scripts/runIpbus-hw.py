import os
from optparse import OptionParser

def mycallback(option,opt,value,parser):
    setattr(parser.values, option.dest, value.split(','))

parser = OptionParser()

parser.add_option("-p","--portUdp",type='string',action='callback',
                  callback=mycallback, dest='portUdp')

parser.add_option("-t","--portTcp",type='string',action='callback',
                  callback=mycallback, dest='portTcp')

(options, args) = parser.parse_args()
print options

print "I start to kill all DummyHardwareUdp.exe and DummyHardwareTcp.exe"
os.system("killall DummyHardwareUdp.exe")
os.system("killall DummyHardwareTcp.exe")
if options.portUdp:
    for i in options.portUdp:
        startudponport="/opt/cactus/bin/uhal/tests/DummyHardwareUdp.exe -p "+i+" -v 2 &"
        print startudponport
        os.system(startudponport)

if options.portTcp:
    for i in options.portTcp:
        startudponport="/opt/cactus/bin/uhal/tests/DummyHardwareTcp.exe -p "+i+" -v 2 &"
        print startudponport
        os.system(startudponport)

#check port are ope: 
#netstat -lnup | grep DummyHardware
