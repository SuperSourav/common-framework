/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <string>
#include <vector>
#include <map>

namespace TTHbb{
  
  class Configuration{
    /**
     * @brief A class for reading a text config file in the form "SETTING parameter"
     * Stores the configuration in a map with each setting accessible with brackets
     */
    
  public:

    Configuration();
    virtual ~Configuration();
    
    /// Load the configuratin from a text file
    void loadConfig(std::string configFile);

    /// By passing two vectors of command line arguments can change settings from what is specified in the config file
    /// @param settingVec a vector of strings, where each entry is the name of the setting to be changed
    /// @param paramVec a vector of strings, where each entry is the parameter for the corresponding setting in settingVec
    void parseCLOpts(std::vector<std::string> settingVec, std::vector<std::string> paramVec);

    /// Print out current configuration, with help text
    void printConfig();

    /// Print out configuration help text
    void printHelp();

    /// Check if config has been loaded
    bool isConfigured(){ return configured; };
    
    /// Get the value for a setting in the config
    /// @param s Name of the setting to get the value for
    const std::string Setting(std::string s) const;

    /// Override to access settings
    /// @param s Name of the setting to get the value for
    /// Use is configName("setting")
    const std::string operator()(std::string s) const;

  protected:
    /// All the settings should be defined in the constructor using this function
    /// @param name The name of the setting in the config file
    /// @param help A short string explaining the setting
    /// @param defVal The default value for the setting, by default empty
    void registerSetting(std::string name, std::string help, std::string defVal="");

    /// Name of configuration file, in case of inheritance
    std::string m_name;

    /// Check to see if already configured
    bool configured;
    bool CLparamsSet;

    std::map<std::string,std::string> configMap;
    std::map<std::string,std::string> helpMap;
  };

}

#endif
