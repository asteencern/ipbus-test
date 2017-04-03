#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <uhal/uhal.hpp>
#include <fstream>
#include <iomanip>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread/thread.hpp>

class event{
public:
  event(int id) : evtID(id) {;}
  ~event(){;}
  inline void addRawData(const char c){ buffer.push_back(c); }
  std::vector<char> &getRawData() { return buffer; }
  std::vector<uint32_t> &getData() { return data; }
  uint32_t checkSum() const {return crc;}
  void prepareDataBeforeSending()
  {
    int N=buffer.size()/4;
    for(int ii=0; ii<N; ii++){
      uint32_t word=(buffer[ii*4+2]& ~0xFFFFFF00)<<6*4 | (buffer[ii*4+3]& ~0xFFFFFF00)<<4*4 | (buffer[ii*4]& ~0xFFFFFF00) << 2*4 | (buffer[ii*4+1]& ~0xFFFFFF00);
      data.push_back(word);
    }
    std::cout << "data size = " << data.size() << std::endl;

    uint32_t* dat=&data[0];
    boost::crc_32_type checksum;
    checksum.process_bytes( dat,(std::size_t)data.size() );
    crc=(uint32_t)checksum.checksum();
  }
  
protected:
  std::vector<char> buffer;
  std::vector<uint32_t> data;
  int evtID;
  uint32_t crc;
};

int main(int argc,char** argv)
{
  size_t N = 30784/4;
  std::vector<uint32_t> xx;
  std::ifstream file ("./RUN_170317_0912.raw.txt", std::ios::in|std::ios::binary|std::ios::ate);
  uint32_t evtId=0;
  std::vector<event> events;
  if (file.is_open()){    
    int cursor=0;
    file.seekg(cursor, std::ios::beg);
    while(1){
      if( !file.good() ) break;
      char* tmpff = new char[1];
      file.read ( tmpff, 1);
      if( (tmpff[0]& ~0xFFFFFF00)==0xff ){
	cursor++;
	file.seekg(cursor, std::ios::beg);	
	event evt(evtId);
	evt.addRawData( tmpff[0] );
	char* tmpdata = new char[N*4];
	file.read ( tmpdata, N*4);
	cursor+=N*4;
	for(unsigned int ii=0; ii<N*4; ii++)
	  evt.addRawData( tmpdata[ii] );
	file.seekg(cursor, std::ios::beg);
	char* tmp8f = new char[2];
	file.read ( tmp8f, 2);
	if( (tmp8f[0]& ~0xFFFFFF00)==0x8f && (tmp8f[1]& ~0xFFFFFF00)==0x8f){
	  cursor+=2;
	  file.seekg(cursor, std::ios::beg);
	  evt.addRawData( tmp8f[0] );
	  evt.addRawData( tmp8f[1] );
	  char* tmpend = new char[1];
	  tmpend[0]=tmpff[0];
	  evt.addRawData( tmpend[0] );
	  evtId++;
	  evt.prepareDataBeforeSending();
	  events.push_back(evt);
	  std::cout << "event " << std::dec << evtId << " is ok; raw data size = " << evt.getRawData().size() << std::endl;
	}
      }
    }
  }

  std::cout << "events.size() " << events.size() << std::endl;

  if( argc<3 ){
    std::cout << "Wrong arguments: correct usage = bin/hexaboardemulation file://path/to/connectionFile.xml deviceId" << std::endl;
    getchar();
    return 0;
  }
  std::string connectionFile=argv[1];
  std::string deviceName=argv[2];

  uhal::ConnectionManager manager ( connectionFile );
  uhal::HwInterface hw=manager.getDevice ( deviceName );

  
  uhal::HwInterface controlHw=manager.getDevice ( "DummyControl" );

  evtId=0;
  std::ostringstream node( std::ostringstream::ate );
  node.str(""); node << "CONTROL." << deviceName << ".WRITTEN";
  while(1){
    uhal::ValWord<uint32_t> trigger = controlHw.getNode("CONTROL.TRIGGER").read();
    uhal::ValWord<uint32_t> alreadyWritten = controlHw.getNode(node.str().c_str()).read();
    controlHw.dispatch();
    if( trigger!=1 || alreadyWritten==1 )
      continue;
    
    if( evtId>events.size()-1 ){ std::cout << "end" << std::endl; getchar();break;}
    std::cout << "we send event " << evtId << " with an unsigned int vector with a size of " << events.at(evtId).getData().size() << std::endl;
    hw.getNode("HEXA.FIFO").writeBlock( events.at(evtId).getData() );
    hw.getNode("HEXA.CRC").write( events.at(evtId).checkSum() );
    hw.dispatch();
    controlHw.getNode(node.str().c_str()).write( (uint32_t)1 );
    controlHw.dispatch();
    evtId++;
  }
    
  return 0;
}
