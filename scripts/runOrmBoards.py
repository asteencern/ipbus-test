import os
from optparse import OptionParser

def mycallback(option,opt,value,parser):
    setattr(parser.values, option.dest, value.split(','))

parser = OptionParser()

parser.add_option("-d","--deviceIds",type='string',action='callback',
                  callback=mycallback, dest='deviceIds')

parser.add_option("-N","--nSlaves",type='int',dest='nSlaves',default=1)

(options, args) = parser.parse_args()
print options

os.system("killall ormemulation")

for i in options.deviceIds:
    start="\"./bin/ormemulation file://./etc/orm-connection.xml "+i+" "+str(options.nSlaves)+"\""
    cmd="\'bash -c "+start+" \' &"
    cmd="gnome-terminal -e "+cmd
    print cmd
    os.system(cmd)


#check port are ope: 
#netstat -lnup | grep DummyHardware
