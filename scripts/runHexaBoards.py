import os
from optparse import OptionParser

def mycallback(option,opt,value,parser):
    setattr(parser.values, option.dest, value.split(','))

parser = OptionParser()

parser.add_option("-d","--deviceIds",type='string',action='callback',
                  callback=mycallback, dest='deviceIds')

parser.add_option("-p","--prepareData",dest='prepareData',choices=["true","false"], default="false")

(options, args) = parser.parse_args()
print options

os.system("killall hexaboardemulation")

for i in options.deviceIds:
    start="\"./bin/hexaboardemulation file://./etc/connection.xml "+i+" "+options.prepareData+"\""
    cmd="\'bash -c "+start+" \' &"
    cmd="gnome-terminal -e "+cmd
    print cmd
    os.system(cmd)


#check port are ope: 
#netstat -lnup | grep DummyHardware
