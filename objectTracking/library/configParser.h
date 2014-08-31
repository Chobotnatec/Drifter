#ifndef _CONFIGPARSER_H_
#define _CONFIGPARSER_H_

#include <string>

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
    /**
     * @brief Return types of parse function.
     */
    enum Status{
      SUCCESS, /*< When all necessary arguments were parsed. */
      EXIT, /*< When you should exit program after argument parsing. For example when help message is shown. */
      ERROR /*< Something bad happened and you should exit. */
    };

    ConfigParser();
    ConfigParser( std::string helpMsg );
  private:
    void init();
  public:

    /**
     * @brief Parse program arguments.
     * For predefined arguments see %parseMain %parseConfigFile
     * @return EXIT - when program should be terminated, for example when help message has been shown. ERROR - when something bad happend. Otherwise return SUCCESS.
     */
    Status parse( int argc, char *argv[] );

    /**
     * @brief Parse only main arguments.
     * Parse only main arguments.
     * There are two predefined main arguments.
     * 'help' - prints help message
     * 'configFile' - location of object tracking configuration file
     * @return EXIT - when help message has been shown. ERROR - when configuration file has not been specified(There is no chcecking if the file name is valid). Otherwise return SUCCESS.
     * @todo Consider validity check of the configuration file
     */
    Status parseMain( int argc, char *argv[] );

    /**
     * @brief Parse configuration file.
     * Parse configuration file and checks that 'dataFolder' is parsed.
     * 'dataFolder' - location of object tracking configuration file
     * @return ERROR - when 'dataFolder' is not specified (There is no validity check of the folder). Otherwise return SUCCESS.
     */
    Status parseConfigFile( std::string configFileName );

    /**
     * @brief Prints description of arguments.
     */
    void printHelp();

    
  public:
    boost::program_options::options_description configFileDesc;
    boost::program_options::options_description mainDesc;
    boost::program_options::variables_map vm; /*< Variable map used to access parsed arguments. */
  };
}

#endif /* _CONFIGPARSER_H_ */
