#include <iostream>
#include <vector>
#include <uhal/uhal.hpp>
#include <fstream>
#include <iomanip>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>

using namespace uhal;

int main(int argc, char** argv)
{

  std::string connectionFile="file:///home/asteen/ipbus-example/etc/hwconnection.xml";
  std::string deviceName="dummy.controlhub2.1";
  uint32_t blockSize=4096;//number of uint32_t in on block, WARNING: check address file
  uint32_t seed=0;
  if( argc>1 )
    deviceName=argv[1];
  if( argc>2 )
    deviceName=argv[2];
  if( argc>3 )
    blockSize=atoi(argv[3]);
  if( argc>4 )
    seed=atoi(argv[4]);

  ConnectionManager manager ( connectionFile );
  HwInterface hw=manager.getDevice ( deviceName );
 
  std::vector<uint32_t> x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc,xd,xe;

  std::srand( seed );
  for( uint32_t i=0; i<blockSize; i++ ){
    x0.push_back( static_cast<uint32_t>( std::rand() ) );
    x1.push_back( static_cast<uint32_t>( std::rand() ) );
    x2.push_back( static_cast<uint32_t>( std::rand() ) );
    x3.push_back( static_cast<uint32_t>( std::rand() ) );
    x4.push_back( static_cast<uint32_t>( std::rand() ) );
    x5.push_back( static_cast<uint32_t>( std::rand() ) );
    x6.push_back( static_cast<uint32_t>( std::rand() ) );
    x7.push_back( static_cast<uint32_t>( std::rand() ) );
    x8.push_back( static_cast<uint32_t>( std::rand() ) );
    x9.push_back( static_cast<uint32_t>( std::rand() ) );
    xa.push_back( static_cast<uint32_t>( std::rand() ) );
    xb.push_back( static_cast<uint32_t>( std::rand() ) );
    xc.push_back( static_cast<uint32_t>( std::rand() ) );
    xd.push_back( static_cast<uint32_t>( std::rand() ) );
    xe.push_back( static_cast<uint32_t>( std::rand() ) );
  }
  std::cout << "we have emulated "<< sizeof(x0[0])*x0.size()*15 << " bytes of data" << std::endl;

  hw.getNode("SWITCH_EMULATOR").getNode("MEM0").writeBlock(x0);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM1").writeBlock(x1);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM2").writeBlock(x2);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM3").writeBlock(x3);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM4").writeBlock(x4);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM5").writeBlock(x5);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM6").writeBlock(x6);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM7").writeBlock(x7);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM8").writeBlock(x8);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM9").writeBlock(x9);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM10").writeBlock(xa);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM11").writeBlock(xb);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM12").writeBlock(xc);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM13").writeBlock(xd);
  hw.getNode("SWITCH_EMULATOR").getNode("MEM14").writeBlock(xe);

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

  uint32_t* dat=&tmp[0];
  boost::crc_32_type checksum;
  checksum.process_bytes( dat,(std::size_t)tmp.size() );
  
  hw.getNode("SWITCH_EMULATOR").getNode("CRC").write((uint32_t)checksum.checksum());
  hw.getNode("SWITCH_EMULATOR").getNode("NBYTES").write(sizeof(tmp[0])*tmp.size());
  
  std::cout << "start sending data" << std::endl;
  hw.dispatch();
  std::cout << "finish sending data" << std::endl;
  return 0;
}
