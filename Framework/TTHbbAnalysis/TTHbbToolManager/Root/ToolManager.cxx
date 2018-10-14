/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/ToolManager.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include <iostream>

namespace TTHbb{

  ToolManager* ToolManager::m_instance = 0;

  ToolManager::~ToolManager(){
    for(auto myTool : toolMap){
      delete myTool.second;
    }
    toolMap.clear();
  }

  void ToolManager::addToolToDict(std::string settingKey, std::string name, std::function<ToolBase*()> func){
    if(b_dictLoaded){
      std::cout << "WARNING in TTHbb::ToolManager::addToolToDict:\t"
		<< "Cannot add " << name << " to ToolManager. Dictionary already loaded" << std::endl;
      return;
    }
    toolDict[settingKey] = tool(name, func);
    dictVector.push_back(std::make_pair(name,settingKey));
  }

  void ToolManager::loadTools(){
    auto config = GlobalConfiguration::get();
  
    if(!config->isConfigured()){
      std::cout << "ERROR in TTHbb::ToolManager:\tGlobalConfiguration has not been configured, cannot load tools." << std::endl;
      exit(11);
    }

    for(auto tool : dictVector){
      if(util::iequals((*config)(tool.second),"true")){
	toolVector.push_back(tool.first);
      }
    }

    for(auto myTool : toolDict){
      if(util::iequals((*config)(myTool.first),"true")){
	toolMap[myTool.second.first] = myTool.second.second();
      }
    }
  }

  void ToolManager::initialiseAllTools(){
    for(auto tool : toolVector){
      (toolMap[tool])->initialise();
    }
  }

  void ToolManager::applyAllTools(TTHbb::Event* event){
    for(auto tool : toolVector){
      toolMap[tool]->apply(event);
    }
  }

  void ToolManager::applyTool(std::string name, TTHbb::Event* event){
    if(toolMap.count(name)){
      toolMap.at(name)->apply(event);
    }
    else{
      std::cout << "WARNING in TTHbb::ToolManager::applyTool:\tTool does not exist in the toolMap: " 
		<< name << "\n\t\t\tCheck correct name given. Skipping tool." << std::endl;
    }
  }

  void ToolManager::finaliseAllTools(){
    for(auto tool : toolVector){
      toolMap[tool]->finalise();
    }
  }

  void ToolManager::dictionaryLoaded(){
    b_dictLoaded = true;
  }

  const ToolBase* ToolManager::getInstanceOfTool(std::string toolName){
    if(toolMap.count(toolName)){
      if(toolMap.at(toolName)->isGettable()){
	return toolMap[toolName];
      }
      else{
	std::cout << "WARNING in TTHbb::ToolManager::getInstanceOfTool:\tTrying to access a none-gettable tool "
		  << toolName << std::endl;
	return 0x0;
      }
    }
    else{
	std::cout << "WARNING in TTHbb::ToolManager::getInstanceOfTool:\tTrying to a tool not in the toolMap:"
		  << toolName << "\n\t\t\tCheck tool is activated in config file." << std::endl;
      return 0x0;
    }
  }

  TTHbb::ToolManager* ToolManager::get(){
    if(m_instance == 0){
      m_instance = new TTHbb::ToolManager();
    }
    return m_instance;
  }

  ToolManager::ToolManager() : b_dictLoaded(false){
  }
}
