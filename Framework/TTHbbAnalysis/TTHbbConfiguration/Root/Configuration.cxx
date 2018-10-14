/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbConfiguration/Configuration.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace TTHbb{

  Configuration::Configuration() : configured(false), CLparamsSet(false)
  {
    m_name = "baseConfig";
    //List of settings to be added here with registerSetting(name, short description, default value) 
    /// Add the CustomSettings setting by default, so that users can always define their own settings on the fly
    registerSetting("CustomSettings", "List of custom settings to add to the configuration which will be defined in the config file", "");
  }

  Configuration::~Configuration(){
  }

  void Configuration::loadConfig(std::string configFile){
    
    /// Check to see if a config file has already been loaded
    if(configured){
      std::cout << "WARNING in TTHbb::Configuration (" << m_name 
		<< ")\n\tConfig already loaded" << std::endl;
    }

    std::string line;
    std::ifstream infile;
    infile.open(configFile);
    
    while(std::getline(infile,line))
      {
	//# will mark the beginning of a comment
	size_t hash = line.find("#");
	if (hash != std::string::npos){
	  line = line.substr(0, hash);
	}
	if(line.empty()) continue;
	//remove whitespace from the beginning and end of line
	const auto strBgn = line.find_first_not_of(" \t");
	const auto strEnd = line.find_last_not_of(" \t");
	line = line.substr(strBgn, strEnd+1);

	//check line isn't empty!
	if(line.empty()) continue;
	
	std::istringstream iss(line);
	std::string name, setting;
	
	iss >> name;
	if((name.size() + 1) >= iss.str().size()){
	  setting = "";
	}
	else{
	  setting = iss.str().substr(name.size() + 1);
	}

	if(setting.size() == 0){
	  std::cout << "WARNING in TTHbb::Configuration (" << m_name 
		    << ")\n\tNo parameters given for setting:\t" << name << std::endl;
	  continue;
	}

	if(configMap.count(name)){
	  configMap[name] = setting;
	  /// Check to see if the setting is the custom settings, and if so, add the comma delimited settings to the config map
	  if(TTHbb::util::iequals("CustomSettings", name)){
	    std::vector<std::string> customSettingVec = TTHbb::util::vectoriseString(setting);
	    for(auto set : customSettingVec){
	      registerSetting(set, "User added custom setting.","");
	    }
	  }
	}
	else{
	  std::cout << "WARNING in TTHbb::Configuration (" << m_name 
		    << ")\n\tUnknown setting in config file:\t" << name << std::endl;
	}
      }
    infile.close();

    configured = true;
  }
  

  //Be flexible, let the user pass settings in the command line, collected as two vectors
  void Configuration::parseCLOpts(std::vector<std::string> settingVec, std::vector<std::string> paramVec){
        /// Check to see if a config file has already been loaded
    if(CLparamsSet){
      std::cout << "WARNING in TTHbb::Configuration (" << m_name 
		<< ")\n\tCommand line settings already changed!" << std::endl;
    }

    std::string name, setting;
    
    for(unsigned int i = 0; i < settingVec.size(); ++i){
      name = settingVec.at(i);
      setting = paramVec.at(i);

      if(configMap.count(name)){
	configMap[name] = setting;
	/// Check to see if the setting is the custom settings, and if so, add the comma delimited settings to the config map
	if(TTHbb::util::iequals("CustomSettings", name)){
	  std::vector<std::string> customSettingVec = TTHbb::util::vectoriseString(setting);
	  for(auto set : customSettingVec){
	    registerSetting(set, "User added custom setting.","");
	  }
	}
      }
      else{
	std::cout << "WARNING in TTHbb::Configuration (" << m_name 
		  << ")\n\tUnknown setting in command line:\t" << name << std::endl;
      }
    }
    CLparamsSet = true;
  }

  void Configuration::printConfig(){
    std::cout << "\nConfiguration Setting for TTHbbConfiguration (" << m_name 
	      << "):"<< std::endl;
    for(auto setting : configMap){
      std::cout << std::setw(30) << setting.first << ":\t" 
		<< setting.second << std::endl;
      std::cout << std::string(30,' ') << "\t(" 
		<< helpMap.at(setting.first) << ")" << std::endl;
    }
  }

  void Configuration::printHelp(){
    std::cout << "\nConfiguration Options for TTHbbConfiguration (" << m_name 
	      << ") config file:" <<  std::endl;
    for(auto setting : configMap){
      std::cout << std::setw(30) << setting.first << ":\t" 
		<< helpMap.at(setting.first) << "\n"<< std::endl;
    }
  }

  const std::string Configuration::Setting(std::string s) const{
    if(configMap.count(s)){
      return configMap.at(s);
    }
    else{
      std::cout << "ERROR in TTHbb::Configuration (" << m_name 
		<< ")\n\tAccessing unknown setting:\t" << s << std::endl;
      exit(2);
    }
  }

  //friendly override, can use configuration("setting name") when using the class
  const std::string Configuration::operator()(std::string s) const{
    return this->Setting(s);
  }

  void Configuration::registerSetting(std::string name, std::string help, std::string defVal){
    configMap[name] = defVal;
    helpMap[name]   = help;
  }

}
