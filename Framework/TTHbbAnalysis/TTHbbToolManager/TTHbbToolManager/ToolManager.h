/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBBTOOLMANAGER_H_
#define _TTHBBTOOLMANAGER_H_

#include "TTHbbObjects/Event.h"
#include "TTHbbToolManager/ToolBase.h"

#include<map>
#include<unordered_map>
#include<string>
#include<utility>
#include<memory>

namespace TTHbb{

  class ToolManager{
  public:
    ~ToolManager();

    /// Add a tool to the manager dictionary
    /// A lambda function is used to return a tool that inherits from TTHbb::ToolBase
    /// @param settingKey The setting to look up in GlobalConfiguration to see if it is to be run
    /// @param name Provide a name for the tool
    /// @param func A Lambda function of the form []{ return new MyTool("ToolName"); }    
    void addToolToDict(std::string settingKey, std::string name, std::function<ToolBase*()> func);
    
    /// Opens the global configuration and checks which tools are set to run then adds them to toolMap
    void loadTools();

    /// Initialise all tools
    void initialiseAllTools();

    /// Apply all tools in the toolMap to the current event
    /// @param event The current event object
    void applyAllTools(TTHbb::Event* event);

    /// Apply the given tool to the current event
    /// @param name The provided name of the tool in the toolMap
    /// @param event The current event object
    void applyTool(std::string name, TTHbb::Event* event);

    /// Finalise all tools
    void finaliseAllTools();

    /// Set loaded bool to true
    void dictionaryLoaded();

    /// A function to check whether a tool exists and if the tool is gettable
    /// If it is gettable it returns the instance of the tool
    /// Otherwise it returns a null pointer
    /// @param toolName Name of the tool in the toolDict
    const ToolBase* getInstanceOfTool(std::string toolName);

    /// ToolManager is a singleton
    /// returns the instance of ToolManager
    static ToolManager* get();

  private:
    ToolManager();

    typedef std::pair<std::string, std::function<TTHbb::ToolBase*()>> tool;

    /// A map for all tools to be added to
    std::unordered_map<std::string, tool> toolDict;

    /// A vector to preserve the order in which tools are added
    std::vector< std::pair<std::string,std::string> > dictVector;

    /// A map of the tools which have been enabled in the config
    std::unordered_map<std::string, ToolBase*> toolMap;

    /// A vector to preserve the order in which the tools are to run
    std::vector<std::string> toolVector;

    /// bool to say whether dictionary has been loaded
    bool b_dictLoaded;

    /// singleton pointer
    static ToolManager* m_instance;

  };

}

#endif
