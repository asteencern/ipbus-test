import os
from optparse import OptionParser

def mycallback(option,opt,value,parser):
    setattr(parser.values, option.dest, value.split(','))

parser = OptionParser()

parser.add_option("-d","--deviceIds",type='string',action='callback',
                  callback=mycallback, dest='deviceIds')

parser.add_option("-N","--nSlaves",type='int',dest='nSlaves',default=1)

parser.add_option("-f","--inputfiles",type='string',action='callback',
                  callback=mycallback, dest='inputfiles')

(options, args) = parser.parse_args()
print options

if len(options.deviceIds)!=len(options.inputfiles):
	print 'WARNING deviceIds and inputfiles parameter options don\'t have the same size -> inputfiles take default run as input'
	options.inputfiles=[]
	for i in range(0,len(options.deviceIds)):
		options.inputfiles.append('RUN_170317_0912.raw.txt')

os.system("killall ormemulation")

for i in range(0,len(options.deviceIds)):
    start="\"source scripts/env.sh; ./bin/ormemulation file://./etc/orm-connection.xml "+options.deviceIds[i]+" "+str(options.nSlaves)+" "+options.inputfiles[i]+"\""
    cmd="\'bash -c "+start+" \' &"
    cmd="gnome-terminal -e "+cmd
    print cmd
    os.system(cmd)


#check port are ope: 
#netstat -lnup | grep DummyHardware
