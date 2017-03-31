# ipbus-test 
Ipbus installation and documentation : https://svnweb.cern.ch/trac/cactus

The two c++ codes (in src/common) show examples of writting and reading some data with ipbus.

Compile the code: 

`source scripts/env.sh`

`make`

`export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH`

Start ipbus hardware emulation : 

`/opt/cactus/bin/uhal/tests/DummyHardwareUdp.exe -p 60001 -v 2`

Or use the python script :

`python scripts/runIpbus-hw.py --port=60001`

Possible to emulate several ipbus hardware:

`python scripts/runIpbus-hw.py --port=60001,60002,60003`

Write data: 

`./bin/writedata file://etc/hwconnection.xml controlhub2.1 4096 98`

Read data

`./bin/readdata file://etc/hwconnection.xml controlhub2.1 4096`

Or start skiroc emulation : 

`./bin/skirocemulation 65` 

The data can be readout by daqreaderemulation.cxx (to check everything is ok):

`./bin/daqreaderemulation`

To read the data within eudaq, see https://github.com/HGCDAQ/eudaq (branch tb2017 producers/ipbusTest).

Spy what is happening with the controlhub :

`watch -n 1 controlhub_stats`
