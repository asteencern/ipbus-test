#include <iostream>
#include <vector>
#include <uhal/uhal.hpp>
#include <fstream>
#include <iomanip>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <ctime>

using namespace uhal;

int main(int argc, char** argv)
{

  if( argc<3 ){
    std::cout << "Wrong arguments: correct usage = bin/skirocemulation file://path/to/connectionFile.xml deviceId seed " << std::endl;
    return 0;
  }
  std::string connectionFile=argv[1];
  std::string deviceName=argv[2];
  uint32_t blockSize=4096;//number of uint32_t in on block, WARNING: check address file
  if( argc>3 )
    blockSize=atoi(argv[3]);

  ConnectionManager manager ( connectionFile );
  HwInterface hw=manager.getDevice ( deviceName );
  
  ValWord<uint32_t> crc = hw.getNode("SWITCH_EMULATOR").getNode("CRC").read();
  ValWord<uint32_t> nbytes = hw.getNode("SWITCH_EMULATOR").getNode("NBYTES").read();

  ValVector<uint32_t> x0 = hw.getNode("SWITCH_EMULATOR").getNode("MEM0").readBlock(blockSize);
  ValVector<uint32_t> x1 = hw.getNode("SWITCH_EMULATOR").getNode("MEM1").readBlock(blockSize);
  ValVector<uint32_t> x2 = hw.getNode("SWITCH_EMULATOR").getNode("MEM2").readBlock(blockSize);
  ValVector<uint32_t> x3 = hw.getNode("SWITCH_EMULATOR").getNode("MEM3").readBlock(blockSize);
  ValVector<uint32_t> x4 = hw.getNode("SWITCH_EMULATOR").getNode("MEM4").readBlock(blockSize);
  ValVector<uint32_t> x5 = hw.getNode("SWITCH_EMULATOR").getNode("MEM5").readBlock(blockSize);
  ValVector<uint32_t> x6 = hw.getNode("SWITCH_EMULATOR").getNode("MEM6").readBlock(blockSize);
  ValVector<uint32_t> x7 = hw.getNode("SWITCH_EMULATOR").getNode("MEM7").readBlock(blockSize);
  ValVector<uint32_t> x8 = hw.getNode("SWITCH_EMULATOR").getNode("MEM8").readBlock(blockSize);
  ValVector<uint32_t> x9 = hw.getNode("SWITCH_EMULATOR").getNode("MEM9").readBlock(blockSize);
  ValVector<uint32_t> xa = hw.getNode("SWITCH_EMULATOR").getNode("MEM10").readBlock(blockSize);
  ValVector<uint32_t> xb = hw.getNode("SWITCH_EMULATOR").getNode("MEM11").readBlock(blockSize);
  ValVector<uint32_t> xc = hw.getNode("SWITCH_EMULATOR").getNode("MEM12").readBlock(blockSize);
  ValVector<uint32_t> xd = hw.getNode("SWITCH_EMULATOR").getNode("MEM13").readBlock(blockSize);
  ValVector<uint32_t> xe = hw.getNode("SWITCH_EMULATOR").getNode("MEM14").readBlock(blockSize);
  
  std::cout << "start reading data" << std::endl;
  clock_t start,end;
  start=clock();
  hw.dispatch();
  end=clock();
  std::cout << "finish reading "<< nbytes << " bytes of data in " << (end-start)/(CLOCKS_PER_SEC/1000) << " ms" << std::endl;

  std::vector<uint32_t> tmp;
  tmp.insert(tmp.end(),x0.begin(),x0.end());
  tmp.insert(tmp.end(),x1.begin(),x1.end());
  tmp.insert(tmp.end(),x2.begin(),x2.end());
  tmp.insert(tmp.end(),x3.begin(),x3.end());
  tmp.insert(tmp.end(),x4.begin(),x4.end());
  tmp.insert(tmp.end(),x5.begin(),x5.end());
  tmp.insert(tmp.end(),x6.begin(),x6.end());
  tmp.insert(tmp.end(),x7.begin(),x7.end());
  tmp.insert(tmp.end(),x8.begin(),x8.end());
  tmp.insert(tmp.end(),x9.begin(),x9.end());
  tmp.insert(tmp.end(),xa.begin(),xa.end());
  tmp.insert(tmp.end(),xb.begin(),xb.end());
  tmp.insert(tmp.end(),xc.begin(),xc.end());
  tmp.insert(tmp.end(),xd.begin(),xd.end());
  tmp.insert(tmp.end(),xe.begin(),xe.end());
  uint32_t *data=&tmp[0];
  boost::crc_32_type checksum;
  checksum.process_bytes(data,(std::size_t)tmp.size());
  
  std::cout << checksum.checksum() << "\t" << crc << std::endl;

  return 0;
}
