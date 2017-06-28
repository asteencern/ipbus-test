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

class orm_event{
public:
  orm_event(int id) : evtID(id) {;}
  ~orm_event(){;}
  inline void addRawData(const char c){ buffer.push_back(c); }
  std::vector<char> &getRawData() { return buffer;}
private:
  std::vector<char> buffer;
  int evtID;
};

class master_orm_event{
public:
  master_orm_event(){;}
  ~master_orm_event(){;}
  std::vector< uint32_t > &getData(){ return data; }
  void addOrmEvent(orm_event oevt){
    if( orms.size()>=8 ){
      std::cout << "ERROR : master orm can not have more than 8 slave orms -> exit()" << std::endl;
      exit(0);
    }
    orms.push_back(oevt);
  }
  void prepareEvent()
  {
    if( orms.empty() ){
      std::cout << "ERROR : master orm should have AT LEAST one slave orms -> exit()" << std::endl;
      exit(0);
    }
    uint32_t N=orms[0].getRawData().size();
    // std::ostringstream os( std::ostringstream::ate ); //DEBUG print out
    // std::ostringstream so( std::ostringstream::ate ); //DEBUG print out
    for( uint32_t ii=0; ii<N; ii++ ){
      // os.str(""); os << "raw data =\t"; //DEBUG print out
      // so.str(""); so << "raw data without 8 =\t"; //DEBUG print out
      uint32_t word=0;
      for( uint32_t jj=0; jj<orms.size(); ++jj ){
	word |= ((orms[jj].getRawData().at(ii)&~0xffffff00)&0xf)<<jj*4;
	// os << std::hex << (orms[jj].getRawData().at(ii)&~0xffffff00) << "\t"; //DEBUG print out
	// so << std::hex << ((orms[jj].getRawData().at(ii)&~0xffffff00)&0xf) << "\t"; //DEBUG print out
      }
      // std::cout << os.str() << std::endl; //DEBUG print out
      // std::cout << so.str() << std::endl; //DEBUG print out
      // std::cout << "final word = " << std::hex << word << std::endl; //DEBUG print out
      // sleep(1); //DEBUG print out
      data.push_back(word);
    }
    calculate_crc();
    return;
  }
  uint32_t crc() const { return crc_; }
private:
  std::vector< orm_event > orms;
  std::vector<uint32_t> data;
  uint32_t crc_;
private:
  void calculate_crc()
  {
    uint32_t* dat=&data[0];
    boost::crc_32_type checksum;
    checksum.process_bytes( dat,(std::size_t)data.size() );
    crc_=(uint32_t)checksum.checksum();
  }
};

int main(int argc,char** argv)
{
  size_t N = 30784/4;
  if( argc<3 ){
    std::cout << "Wrong arguments: correct usage = bin/ormemulation file://path/to/connectionFile.xml deviceId" << std::endl;
    getchar();
    return 0;
  }
  std::string connectionFile=argv[1];
  std::string deviceName=argv[2];
  int nslave=1;
  if( argc>3 )
    nslave=atoi( argv[3] );
  std::vector<uint32_t> xx;
  std::ifstream file;
  if( argc>4 )
    file.open(argv[4],std::ios::in|std::ios::binary|std::ios::ate);
  else file.open("./RUN_170317_0912.raw.txt", std::ios::in|std::ios::binary|std::ios::ate);
  uint32_t evtId=0;
  std::vector<orm_event> events;
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
	orm_event evt(evtId);
	evt.addRawData( tmpff[0] );
	char* tmpdata = new char[N*4];
	file.read ( tmpdata, N*4);
	cursor+=N*4;
	for(unsigned int ii=0; ii<N*4; ii++)
	  evt.addRawData( tmpdata[ii] );
	file.seekg(cursor, std::ios::beg);
	char* tmp8f = new char[2];
	file.read ( tmp8f, 2);
	if( ((tmp8f[0]& ~0xFFFFFF00)==0x8f || (tmp8f[0]& ~0xFFFFFF00)==0xff) && 
	    ((tmp8f[1]& ~0xFFFFFF00)==0xff || (tmp8f[1]& ~0xFFFFFF00)==0x8f) ){
	  cursor+=2;
	  file.seekg(cursor, std::ios::beg);
	  evt.addRawData( tmp8f[0] );
	  evt.addRawData( tmp8f[1] );
	  char* tmpend = new char[1];
	  tmpend[0]=tmpff[0];
	  evt.addRawData( tmpend[0] );
	  evtId++;
	  events.push_back(evt);
	  std::cout << "event " << std::dec << evtId << " is ok; raw data size = " << evt.getRawData().size() << std::endl;
	}
	else{
	  std::cout << std::hex << (tmp8f[0]& ~0xFFFFFF00) << "\t" << (tmp8f[1]& ~0xFFFFFF00) << std::endl;
	}
      }
    }
  }
  else{
    std::cout << "file ./RUN_170317_0912.raw.txt is not in current directory -> exit" << std::endl;
    sleep(4);
    return 0;
  }
  

  std::cout << "events.size() " << events.size() << std::endl;

  uhal::ConnectionManager manager ( connectionFile );
  uhal::HwInterface sync_orm=manager.getDevice ( "SYNC_ORM" );
  uhal::HwInterface this_orm=manager.getDevice ( deviceName );

  
  std::vector<uint32_t> evtIds; evtIds.reserve(nslave);
  int evt_sent=0;
  std::ostringstream os( std::ostringstream::ate );
  while(1){
    evtIds.clear();
    uhal::ValWord<uint32_t> busy = sync_orm.getNode("SYNC.BUSY").read();
    sync_orm.dispatch();
    uhal::ValWord<uint32_t> alreadyWritten = this_orm.getNode("RDOUT.RDOUT_RDY").read();
    this_orm.dispatch();
    if( busy==1 && alreadyWritten==0 ){
      master_orm_event evt;
      os.str("");
      os << "we send the event " << evt_sent << " with the following events : ";
      for( int ii=0; ii<nslave; ii++ ){ 
	uint32_t id=std::rand()%500;
	evt.addOrmEvent( events.at(id) );
	os << id << "; ";
      }
      //os << "\n";
      evt.prepareEvent();
      this_orm.getNode("RDOUT.FIFO").writeBlock( evt.getData() );
      this_orm.getNode("RDOUT.CRC").write( evt.crc() );
      this_orm.getNode("RDOUT.RDOUT_RDY").write( (uint32_t)1 );
      this_orm.dispatch();
      evt_sent++;
      std::cout << os.str() << std::endl;
      boost::this_thread::sleep( boost::posix_time::microseconds(10) );
    }
    else{
      std::cout << "coucou" << std::endl;
      boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
    }
    // // uncomment the following for testing this code
    // boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
    // sync_orm.getNode("SYNC.BUSY").write(1);
    // sync_orm.dispatch();
    // this_orm.getNode("RDOUT.RDOUT_RDY").write(0);
    // this_orm.dispatch();
  }
    
  return 0;
}
