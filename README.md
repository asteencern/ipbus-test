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

<<<<<<< HEAD
`$ ./bin/skirocemulation file://./etc/skirocconnection.xml controlhub2 65` 

The data can be readout by daqreaderemulation.cxx (to check everything is ok):

`$ ./bin/daqreaderemulation file://./etc/skirocconnection.xml controlhub2`
=======
`./bin/skirocemulation 65` 

The data can be readout by daqreaderemulation.cxx (to check everything is ok):

`./bin/daqreaderemulation`
>>>>>>> 5e910f7c7f778f258d596d6ce0c8794c5f977a0a

To read the data within eudaq, see https://github.com/HGCDAQ/eudaq (branch tb2017 producers/ipbusTest).

Spy what is happening with the controlhub :

<<<<<<< HEAD
`$ watch -n 1 controlhub_stats`

Start more realistic data transfer using RUN_170317_0912.raw.txt (https://cms-docdb.cern.ch/cgi-bin/DocDB/ShowDocument?docid=13285) :

      Open a terminal :
      
      `$ source script/env.sh`

      `$ python scripts/runIpbus-hw.py --port=50001,60001,60002,60003,60004,60005`
      
      `$./binDummyTrigger file://file./connection.xml 5`, the "5" corresponds to the number of fake boards sending data

      Open another terminal :
      
      `$ source script/env.sh`

      Launch boards emulations :

      `$ python scripts/runHexaBoards.py -d HEXABOARD0,HEXABOARD1,HEXABOARD2,HEXABOARD3,HEXABOARD4`, the HEXABOARDi correspond to the names of the 5 boards which are sending data

      Read out the data :

      `$./bin/hexaboardReader file://./connection.xml HEXABOARD0 HEXABOARD1 HEXABOARD2 HEXABOARD3 HEXABOARD4`
=======
`watch -n 1 controlhub_stats`
>>>>>>> 5e910f7c7f778f258d596d6ce0c8794c5f977a0a
