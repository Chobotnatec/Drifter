#include <fstream>
#include <iostream>
#include "configParser.h"

using namespace std;
namespace po=boost::program_options;

namespace tracking{

  // I had to define CONFIG_FILE_HELP with #define. When I define it as const string than I get wierd segmentation fault when I instantiate ConfigParser in global scope. No idea why :(
  #define CONFIG_FILE_HELP "\nObjectTracking configuration file help"


  ConfigParser::ConfigParser():mainDesc(""),
			       configFileDesc(CONFIG_FILE_HELP){
    init();
  }
  

  ConfigParser::ConfigParser( std::string const & helpMsg ) :mainDesc(helpMsg),
							     configFileDesc(CONFIG_FILE_HELP){
    init();
  }

  void ConfigParser::init(){
    // initialize main descriptor 
    mainDesc.add_options()
      ("help,h", "Pring help msg")
      ("configFile,c", po::value<string>()->default_value("tracking.cfg"), "Configuration file.");

    // initialize config file descriptor
    configFileDesc.add_options()
      ("dataFolder", po::value<string>(), "ObjectTracking data folder.")
      ("camID", po::value<int>()->default_value(0), "ID of camera")
      ("camWidth", po::value<int>()->default_value(640), "Width resolution of camera in pixels.")
      ("camHeight",po::value<int>()->default_value(480), "Height resolution of camera in pixels")
      ("distanceMatrix",po::value<string>()->default_value("distanceMatrix"), "Name of distance matrix file");
       ;
  }
  
  bool ConfigParser::parse(int argc, char **argv){

    bool stat;

    if(!parseMain(argc, argv))
      return false;

    if(!parseConfigFile( get<string>("configFile") ))
      return false;

    return true;
  }
  
  bool ConfigParser::parseMain(int argc, char **argv){
    po::store( po::parse_command_line(argc,argv,mainDesc), vm);
    po::notify(vm);

    if(vm.count("help")){
      printHelp();
      return false;
    }
    return true;
  }

  bool ConfigParser::parseConfigFile(std::string configFileName){
    
    ifstream ifs( configFileName );
    if (!ifs){
      throw Exception( "Could not open configuration file: \"" + configFileName + "\"");
    }
    else{
      po::store(parse_config_file(ifs, configFileDesc), vm);
      po::notify(vm);

      // now chceck that necessary options are set

      if(vm.count("dataFolder")==0){
	throw Exception( "Location of object tracking data folder is not specified. See argument 'dataFolder'.");
      }
    }
    
    return true;
  }

  void ConfigParser::printHelp(){
    cout << mainDesc << configFileDesc << endl;
  }
    

}

