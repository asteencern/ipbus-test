#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <uhal/uhal.hpp>
#include <fstream>
#include <iomanip>
#include <pthread.h>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread/thread.hpp>

#define NUM_THREADS 100

class readout{
public:
  readout(uhal::HwInterface hw) : m_hw(hw)
  {}
  ~readout(){;}
  void read()
  {
    m_data.clear();
    while(1){
      uhal::ValVector<uint32_t> data=m_hw.getNode("HEXA.FIFO").readBlock( m_hw.getNode("HEXA.FIFO").getSize() );
      uhal::ValWord<uint32_t> crc=m_hw.getNode("HEXA.CRC").read( );
      m_hw.dispatch();
      for( uhal::ValVector<uint32_t>::const_iterator it=data.begin(); it!=data.end(); ++it )
	m_data.push_back(*it);
      uint32_t *dat=&m_data[0];
      boost::crc_32_type checksum;
      checksum.process_bytes(dat,(std::size_t)m_data.size());
      if( crc==checksum.checksum() ){
	m_crc=crc;
	std::cout << m_hw.id() << " complete its readout" << std::endl;
	break;
      }
      else
	std::cout << m_hw.id() << " crc problem : crc received = " << crc << "\t crc calculated = " << checksum.checksum() << "\t -> we try another readout " << std::endl;
    }
  }

  uint32_t checksum() const {return m_crc;}
  std::vector<uint32_t> & data() {return m_data;}
protected:
  std::vector<uint32_t> m_data;
  uhal::HwInterface m_hw;
  uint32_t m_crc;
};

class readoutEvent{
public:
  readoutEvent() : m_eid(0)
  {}
  ~readoutEvent(){;}
  void addData( std::vector<uint32_t> data)
  {
    m_data.insert(m_data.end(), data.begin(), data.end());
  }
  std::vector<uint32_t> & data() {return m_data;}
  void writeDataOnDisk() {std::cout << "j'ai ecrit l'evenement " << m_eid << " sur le disk" << std::endl;}
  void reset(){ m_data.clear(); m_eid++; }
protected:
  int m_eid;
  std::vector<uint32_t> m_data;
};

struct thread_data{
  uint32_t id;
  std::string deviceId;
  readout *rd;
  readoutEvent *rdev;
};

void *threadFunc( void *threadArg )
{
  struct thread_data *m_arg;
  m_arg = (struct thread_data*)threadArg;
  std::cout << "I am running thread id " << m_arg->id << std::endl;
  m_arg->rd->read();
  m_arg->rdev->addData( m_arg->rd->data() );
  pthread_exit(NULL);
}
  
int main(int argc,char** argv)
{
  if( argc<2 ){
    std::cout << "Wrong arguments: correct usage = bin/hexaboardemulation file://path/to/connectionFile.xml deviceId1 deviceId2 deviceId3 ..." << std::endl;
    return 0;
  }
  std::string connectionFile=argv[1];
  uhal::ConnectionManager manager ( connectionFile );
  std::map<std::string,uhal::HwInterface> hwmap;
  for( int ii=0; ii<argc-2; ii++ ){
    uhal::HwInterface hw=manager.getDevice ( argv[2+ii] );
    hwmap.insert( std::pair<std::string,uhal::HwInterface>(argv[2+ii],hw) );
  }
  uhal::HwInterface *controlHw=new uhal::HwInterface( manager.getDevice ( "DummyControl" ) );

  std::cout << "number of hw = " << hwmap.size() << std::endl;
  for( std::map<std::string,uhal::HwInterface>::iterator it=hwmap.begin(); it!=hwmap.end(); ++it ){
    std::cout << it->first << "\t" << it->second.getNodes().size() << std::endl;
  }

  std::ostringstream node( std::ostringstream::ate );
  readoutEvent *event=new readoutEvent();    
  pthread_t thread[NUM_THREADS];
  struct thread_data td[NUM_THREADS];
  while(1){
    uhal::ValWord<uint32_t> globalread = controlHw->getNode("CONTROL.READ").read();
    controlHw->dispatch();
    if( globalread==0 )
      continue;
    size_t expectedDataSize=hwmap.begin()->second.getNode("HEXA.FIFO").getSize()*hwmap.size();
    std::cout << "on essaye" << std::endl;
    // uint32_t ii=0;
    // for( std::map<std::string,uhal::HwInterface>::iterator it=hwmap.begin(); it!=hwmap.end(); ++it ){
    //   td[ii].id = ii;
    //   //std::cout <<"main() : creating thread, " << ii << std::endl;
    //   td[ii].deviceId = it->first;
    //   td[ii].rd = new readout( it->second );
    //   td[ii].rdev = event;
    //   int rc=pthread_create(&thread[ii], NULL, threadFunc, (void *)&td[ii]);
    //   if(rc){
    // 	std::cout << "Error:unable to create thred," << rc << std::endl;
    // 	exit(-1);
    //   }
    //   ii++;
    // }
    for( std::map<std::string,uhal::HwInterface>::iterator it=hwmap.begin(); it!=hwmap.end(); ++it ){
      readout reader( it->second );
      reader.read();
      event->addData( reader.data() );
    }
    std::cout << "on attend" << std::endl;
    boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
    if(event->data().size()==expectedDataSize){
      std::cout << "on a reussi" << std::endl;
      event->writeDataOnDisk();
      event->reset();
      controlHw->getNode("CONTROL.READ").write((uint32_t)0);
      controlHw->dispatch();
    }
    else{
      std::cout << event->data().size() << std::endl;
      std::cout << "olala c'est le bordel -> break" <<  std::endl;
      break;
    }
  }
  pthread_exit(NULL);
}
