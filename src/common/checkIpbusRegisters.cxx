#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <uhal/uhal.hpp>
#include <uhal/definitions.hpp>
#include <fstream>
#include <iomanip>
#include <boost/program_options.hpp>

#define RDOUT_DONE_MAGIC 0xABCD4321

int main(int argc,char** argv)
{
  std::string m_connectionfile,m_hwname;
  bool m_sendRDOUTDONE;
  try { 
    /** Define and parse the program options 
     */ 
    namespace po = boost::program_options; 
    po::options_description desc("Options"); 
    desc.add_options() 
      ("help,h", "Print help messages") 
      ("connectionfile",po::value<std::string>(&m_connectionfile)->default_value("file://./etc/connection.xml"),"name of the connection file")
      ("hwname",po::value<std::string>(&m_hwname)->default_value("RDOUT_ORM0"),"ipbus hardware name")
      ("send_RDOUT_DONE",po::value<bool>(&m_sendRDOUTDONE)->default_value(false),"set to true if a RDOUT_DONE_MAGIC should be sent at the end");
    
    po::variables_map vm; 
    try { 
      po::store(po::parse_command_line(argc, argv, desc),  vm); 
      if ( vm.count("help")  ) { 
        std::cout << desc << std::endl; 
        return 0; 
      } 
      po::notify(vm);
    }
    catch(po::error& e) { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      std::cerr << desc << std::endl; 
      return 1; 
    }
    
    if( vm.count("connectionfile") ) std::cout << "m_connectionfile = " << m_connectionfile << std::endl;
  }
  catch(std::exception& e) { 
    std::cerr << "Unhandled Exception reached the top of main: " 
              << e.what() << ", application will now exit" << std::endl; 
    return 2; 
  } 

  uhal::ConnectionManager manager ( m_connectionfile );
  uhal::HwInterface orm=manager.getDevice ( m_hwname );
  std::vector<std::string> nodes=orm.getNodes();
  orm.dispatch();
  if(m_sendRDOUTDONE)
    orm.getNode("RDOUT_DONE").write(RDOUT_DONE_MAGIC);
  for( std::vector<std::string>::iterator it=nodes.begin(); it!=nodes.end(); ++it ){
    uint32_t size=orm.getNode(*it).getSize();
    orm.dispatch();
    if( size==1 ){
      uhal::ValWord<uint32_t> value = orm.getNode(*it).read();
      uhal::defs::NodePermission perm=orm.getNode(*it).getPermission();
      orm.dispatch();
      std::cout << (*it) << ": permissions (READ=0x1,WRITE=0x2,READWRITE=0x3) = \t" << std::hex << perm << ";\t value = " << std::setw(8) << std::setfill('0') << value << std::endl;
      
    }
  }
  
  return 0;
}
