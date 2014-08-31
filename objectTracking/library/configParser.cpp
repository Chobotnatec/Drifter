#include <fstream>
#include <iostream>
#include "configParser.h"

using namespace std;
namespace po=boost::program_options;

namespace tracking{

  string configFileHelp =
    "\nObjectTracking configuration file help";

  ConfigParser::ConfigParser():mainDesc(""),
			       configFileDesc(configFileHelp){
    init();
  }
  
  ConfigParser::ConfigParser( std::string helpMsg ):mainDesc(helpMsg),
						    configFileDesc(configFileHelp){
    init();
  }

  void ConfigParser::init(){
    // initialize main descriptor 
    mainDesc.add_options()
      ("help,h", "Pring help msg")
      ("configFile,c", po::value<string>()->default_value("tracking.cfg"), "Configuration file.");

    // initialize config file descriptor
    configFileDesc.add_options()
      ("dataFolder", po::value<string>(), "ObjectTracking data folder.");
  }

  ConfigParser::Status ConfigParser::parse(int argc, char **argv){

    Status stat;

    stat = parseMain(argc, argv);

    if(stat!=SUCCESS){
      return stat;
    }

    stat = parseConfigFile( vm["configFile"].as<string>() );

    return stat;
  }
  
  ConfigParser::Status ConfigParser::parseMain(int argc, char **argv){
    po::store( po::parse_command_line(argc,argv,mainDesc), vm);
    po::notify(vm);

    if(vm.count("help")){
      printHelp();
      return EXIT;
    }

    return SUCCESS;
  }

  ConfigParser::Status ConfigParser::parseConfigFile(std::string configFileName){
    
    ifstream ifs( configFileName );
    if (!ifs){
      cout << "Can not open config file: " << configFileName << endl;
      return ERROR;
    }
    else{
      po::store(parse_config_file(ifs, configFileDesc), vm);
      po::notify(vm);

      // now chceck that necessary options are set

      if(vm.count("dataFolder")==0){
	cout <<  "Please specify location of object tracking data folder!" << endl;
	return ERROR;
      }
    }
    
    return SUCCESS;
  }

  void ConfigParser::printHelp(){
    cout << mainDesc << configFileDesc << endl;
  }
    

}

