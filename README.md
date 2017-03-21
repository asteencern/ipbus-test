# ipbus-test 
Ipbus installation and documentation : https://svnweb.cern.ch/trac/cactus

The two c++ codes (in src/common) show examples of writting and reading some data with ipbus.

Compile the code: 

`$ source script/env.sh`

`$ make`

`$ export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH`

Start ipbus hardware emulation : 

`$ /opt/cactus/bin/uhal/tests/DummyHardwareUdp.exe -p 60001 -v 2`

Or use the python script :

`$ python scripts/runIpbus-hw.py --port=60001`

Possible to emulate several ipbus hardware:

`$ python scripts/runIpbus-hw.py --port=60001,60002,60003`

Write data: 

`$ ./bin/writedata file://etc/hwconnection.xml dummy.controlhub2.1 4096 98`

Read data

`$ ./bin/readdata file://etc/hwconnection.xml dummy.controlhub2.1 4096`
