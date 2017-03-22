#include <iostream>
#include <vector>
#include <uhal/uhal.hpp>
#include <fstream>
#include <iomanip>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <ctime>

int main(int argc, char** argv)
{
  std::string connectionFile="file:///home/asteen/daq-dev/ipbus-test/etc/skirocconnection.xml";
  std::string deviceName="controlhub2";
  uint32_t blockSize=962;//corresponds to 3848 bytes

  uhal::ConnectionManager manager ( connectionFile );
  uhal::HwInterface hw=manager.getDevice ( deviceName );
  
  uint32_t nevent=0;

  while(1){
    if( nevent>10 ) break;
    uhal::ValWord<uint32_t> test=hw.getNode("SKI0.READENABLE").read();
    hw.dispatch();
    if( test!=1 )continue;

    uhal::ValWord<uint32_t> crc = hw.getNode("SKI0.CRC").read();
    uhal::ValWord<uint32_t> nbytes = hw.getNode("SKI0.NBYTES").read();
    uhal::ValVector<uint32_t> x0 = hw.getNode("SKI0.FIFO").readBlock(blockSize);
    hw.dispatch();
    
    std::vector<uint32_t> tmp;
    tmp.insert( tmp.end(), x0.begin(), x0.end() );

    uint32_t *data=&tmp[0];
    boost::crc_32_type checksum;
    checksum.process_bytes(data,(std::size_t)tmp.size());
    
    if( crc==checksum.checksum() ){
      std::cout << "Event number = " << nevent << " : \t success to transfer " << nbytes << " bytes of data \t checksum = " << crc << std::endl;
      hw.getNode("SKI0.READENABLE").write( static_cast<uint32_t>(0) );
      hw.getNode("SKI0.WRITEENABLE").write( static_cast<uint32_t>(1) );
      hw.dispatch();
    }
    else{
      std::cout << "Event number = " << nevent << " :\t fail to transfer " << nbytes << " bytes of data -> try again" << std::endl;
      continue;
    }
    nevent++;
  }
  return 0;
}
