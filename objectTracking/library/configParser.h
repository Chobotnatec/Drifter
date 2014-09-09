#ifndef _CONFIGPARSER_H_
#define _CONFIGPARSER_H_

#include <string>
#include <exception>

#include <boost/program_options.hpp>

namespace tracking{

  /**
   * @brief Class for parsing object tracking configuration file
   *
   * @todo How to use this class.
   * @todo Add description of object tracking configuration file.
   *
   */
  class ConfigParser{
  public:
    class Exception : public std::exception{
    public:
      Exception( std::string const & _msg ) : msg(_msg){}
       const char* what() const throw(){
	return msg.c_str();
      }
    private:
      std::string msg;
    };

    ConfigParser();
    ConfigParser( std::string const & helpMsg );
  private:
    void init();
  public:

    /**
     * @brief Parse program arguments.
     * For predefined arguments see %parseMain %parseConfigFile
     * @return True - everything went ok. False - you should stop the program, something bad happend or help message was shown.
     * @throw %Exception
     */
    bool parse( int argc, char *argv[] );

    /**
     * @brief Parse only main arguments.
     * Parse only main arguments.
     * There are two predefined main arguments.
     * 'help' - prints help message
     * 'configFile' - location of object tracking configuration file
     * @return True - everything went ok. False - you should stop the program, something bad happend or help message was shown.
     * @throw %Exception
     * @todo Consider validity check of the configuration file
     */
    bool parseMain( int argc, char *argv[] );

    /**
     * @brief Parse configuration file.
     * Parse configuration file and checks that 'dataFolder' is parsed.
     * 'dataFolder' - location of object tracking configuration file
     * @return True - everything went ok. False - you should stop the program, something bad happend.
     * @throw %Exception
     * @todo add new arguments to this documentation
     */ 
    bool parseConfigFile( std::string configFileName );

    template<typename T>
    T get(std::string const & argName){
      try{
	return vm[argName].as<T>();
      }catch( boost::bad_any_cast const & exc ){
	throw Exception( "Failed while getting \'" + argName + "\' argument." );
      }
    }

    template<typename T>
    T get(std::string const & argName, int i){
      try{
	return vm[argName].as<std::vector<T> >()[i];
      }catch( boost::bad_any_cast const & exc ){
	throw Exception( "Failed while getting " + std::to_string(i) + "th \'" + argName + "\' argument" );
      }
    }

    template<typename T>
    int count(std::string const & argName){
      if( vm.count(argName) ){
	try{
	  return vm[argName].as<std::vector<T> >().size();
	}catch( boost::bad_any_cast const & exc ){
	  return 1;
	}
      }else{
	return 0;
      }
    }



    int count(std::string const & argName){
      return vm.count(argName);
    }

    /**
     * @brief Prints description of arguments.
     */
    void printHelp();

  public:
    boost::program_options::options_description configFileDesc;
    boost::program_options::options_description mainDesc;
    boost::program_options::variables_map vm; /*< Variable map used to access parsed arguments. */
    static const std::string configFileHelp;
  };
}

#endif /* _CONFIGPARSER_H_ */
  
