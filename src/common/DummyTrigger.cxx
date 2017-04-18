#include <iostream>
#include <sstream>
#include <algorithm>

#include <uhal/uhal.hpp>
#include <boost/thread/thread.hpp>
#include <boost/timer/timer.hpp>

class controlInitializer{
public:
  controlInitializer(){;}
  void init(uhal::HwInterface controlHw,size_t nhexaboard)
  {
    std::ostringstream node( std::ostringstream::ate );
    for( size_t ii=0; ii<nhexaboard; ii++ ){
      node.str(""); node << "CONTROL.HEXABOARD" << ii << ".WRITTEN";
      controlHw.getNode( node.str().c_str() ).write( (uint32_t)0);
    }
    controlHw.getNode( "CONTROL.TRIGGER" ).write( (uint32_t)0);
    controlHw.getNode( "CONTROL.BUSY" ).write( (uint32_t)0);
    controlHw.getNode( "CONTROL.READ" ).write( (uint32_t)0);
    controlHw.dispatch();
  }
};

int main(int argc,char** argv)
{
  if( argc<3 ){
    std::cout << "Wrong arguments: correct usage = bin/DummyTrigger file://path/to/connectionFile.xml nhexaboards" << std::endl;
    return 0;
  }
  std::string connectionFile=argv[1];
  size_t nhexaboard=atoi(argv[2]);
  bool waitForDataReadOut=false;
  if( argc>3 ){
    std::stringstream ss(argv[3]);
    if(!(ss >> std::boolalpha >> waitForDataReadOut )){
      std::cout << "some unexpected problem" << std::endl;
    }
  }

  uhal::ConnectionManager manager ( connectionFile );  
  uhal::HwInterface controlHw=manager.getDevice ( "DummyControl" );

  std::ostringstream node( std::ostringstream::ate );
  controlInitializer t; t.init(controlHw,nhexaboard);
  std::vector< uhal::ValWord<uint32_t> > boardstatusVec;

  double meanTimeToRead=0;
  double evt=0;
  while(1){
    
    uhal::ValWord<uint32_t> busy = controlHw.getNode( "CONTROL.BUSY" ).read( );
    controlHw.dispatch();
    //std::cout << "busy = " << busy << std::endl;
    //boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
    if( busy!=1 ){// continue;
      uint32_t a=static_cast<uint32_t>( std::rand() )%10;
      if( a>=0 ){
	std::cout << "received new trigger" << std::endl;
	controlHw.getNode( "CONTROL.TRIGGER" ).write( (uint32_t)1 );
	controlHw.getNode( "CONTROL.BUSY" ).write( (uint32_t)1 );
	controlHw.dispatch();
      }
      else{
	//boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
	continue;
      }
    }
    
    boardstatusVec.clear();
    for( size_t ii=0; ii<nhexaboard; ii++ ){
      node.str(""); node << "CONTROL.HEXABOARD" << ii << ".WRITTEN";
      uhal::ValWord<uint32_t> w=controlHw.getNode( node.str().c_str() ).read();
      boardstatusVec.push_back(w);
      controlHw.dispatch();
      //std::cout <<  node.str().c_str()  << " = " <<  w  << std::endl;
    }
    controlHw.dispatch();
    if( std::find(boardstatusVec.begin(), boardstatusVec.end(), (uint32_t)0)==boardstatusVec.end() ){
      controlHw.getNode( "CONTROL.READ" ).write( (uint32_t)1 );
      controlHw.dispatch();
      uhal::ValWord<uint32_t> readStatus;
      boost::timer::cpu_timer t;
      while(1){
	if( waitForDataReadOut ){
	  //wait central daq read the data
	  readStatus=controlHw.getNode( "CONTROL.READ" ).read();
	  controlHw.dispatch();
	  if( readStatus==0 ) break;
	}
	else{
	  //relaxe busy after some time
	  boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
	  break;
	}
      }
      uint16_t ttime=t.elapsed().wall;
      meanTimeToRead+=ttime/1e9;      
      evt++;
      std::cout << "time needed to read = " << ttime/1e9 << "\t average time to read = " << meanTimeToRead/evt << std::endl;
      std::cout << "finished to read data -> relaxe trigger and busy flag" << std::endl;
      controlHw.getNode( "CONTROL.TRIGGER" ).write( (uint32_t)0 );
      controlHw.getNode( "CONTROL.BUSY" ).write( (uint32_t)0 );
      for( size_t ii=0; ii<nhexaboard; ii++ ){
	node.str(""); node << "CONTROL.HEXABOARD" << ii << ".WRITTEN";
	controlHw.getNode( node.str().c_str() ).write( (uint32_t)0 );
      }
      controlHw.dispatch();
    }
  }
  
  return 0;
}
