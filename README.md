# ipbus-test : hgcal branch

In this branch, the ormemulation binary is reading raw data from a txt file (which should be copy from anywhere in this directory), creating bit stream according to the expected data format and publishing the data as vector of 32 bits unsigned int. Then, these data should be readout by another program: see HGCDAQ eudaq framework https://github.com/HGCDAQ/eudaq .

Ipbus installation and documentation : https://svnweb.cern.ch/trac/cactus .

Compile the code: 

`source scripts/env.sh`

`make`

Start ipbus hardware emulation with the python script :

`python scripts/runIpbus-hw.py --portUDP=50001,60001,60002`

This initiates 3 udp ipbus hardware emulation. As it can be seen in etc/orm-connection.xml, port 50001 is expected for the emulation of the sync board. The other ports are for read-out board emulations.
Start data publication in these ports: 

`python scripts/runOrmBoards.py -d RDOUT_ORM0,RDOUT_ORM1 -N 8

The "8" means that each read-out board contains data from 8 slave boards. The maximum number of slave boards is 8. Each 32 bits word corresponds to 4 bits per slave board. If we run with less than 8 slave boards (eg. 3), the words are filled with zeros (eg. 3*4 bits contain data, the remaining 20 bits are zeros).

If in src/common/ormemulation.cxx, lines between 175 and 180 can be uncommented for checking/debugging without the eudaq readout part.