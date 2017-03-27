#include <iostream>
#include <vector>
#include <uhal/uhal.hpp>
#include <fstream>
#include <iomanip>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread/thread.hpp>

int main(int argc, char** argv)
{
  if( argc<3 ){
    std::cout << "Wrong arguments: correct usage = bin/skirocemulation file://path/to/connectionFile.xml deviceId seed " << std::endl;
    return 0;
  }
  std::string connectionFile=argv[1];
  std::string deviceName=argv[2];
  uint32_t blockSize=962;//corresponds to 3848 bytes
  uint32_t seed=0;
  if( argc>3 )
    seed=atoi(argv[3]);

  uhal::ConnectionManager manager ( connectionFile );
  uhal::HwInterface hw=manager.getDevice ( deviceName );
  
  uint32_t nevent=0;

  std::cout << "hw.getNodes().size() = " << hw.getNodes().size() << std::endl;
  for( unsigned int i=0; i<hw.getNodes().size(); i++ )
    std::cout << hw.getNodes()[i] << std::endl;

  hw.getNode("SKI0.WRITEENABLE").write( static_cast<uint32_t>( 1 ) );
  hw.getNode("SKI0.READENABLE").write( static_cast<uint32_t>( 0 ) );
  hw.dispatch();
  std::cout << "here i am" << std::endl;
  std::srand( seed );
  while(1){
    uhal::ValWord<uint32_t> test=hw.getNode("SKI0.WRITEENABLE").read();
    hw.dispatch();
    if( test!=1 ) continue;
    std::vector<uint32_t> x0;
    for( uint32_t i=0; i<blockSize; i++ )
      x0.push_back( static_cast<uint32_t>( std::rand() ) );
  
    std::cout << "we have emulated "<< sizeof(x0[0])*x0.size() << " bytes of data\t x0[0] = " << x0[0] << std::endl;
  
    hw.getNode("SKI0.FIFO").writeBlock(x0);
  
    uint32_t* dat=&x0[0];
    boost::crc_32_type checksum;
    checksum.process_bytes( dat,(std::size_t)x0.size() );
  
    hw.getNode("SKI0.CRC").write((uint32_t)checksum.checksum());
    hw.getNode("SKI0.NBYTES").write(sizeof(x0[0])*x0.size());
    hw.getNode("SKI0.WRITEENABLE").write( static_cast<uint32_t>( 0 ) );
    hw.getNode("SKI0.READENABLE").write( static_cast<uint32_t>( 1 ) );
    hw.dispatch();
    boost::this_thread::sleep( boost::posix_time::milliseconds(1000) );
    nevent++;
  }
  return 0;
}
