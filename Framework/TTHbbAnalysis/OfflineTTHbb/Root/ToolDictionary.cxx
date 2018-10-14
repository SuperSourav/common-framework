/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "OfflineTTHbb/ToolDictionary.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbToolManager/OfflineMVAVariables.h"
#include "TTHbbToolManager/OfflineTtbarCorrections.h"
#include "TTHbbToolManager/OfflineFakes.h"
#include "TTHbbToolManager/OfflineTRFFakes.h"
#include "TTHbbToolManager/OfflineMCFakes.h"
#include "TTHbbToolManager/OfflineCPVariables.h"
#include "TTHbbToolManager/TtTRF.h"
#include "TTHbbToolManager/DictBase.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

#include <string>
#include <vector>
#include "TClass.h"
#include "TSystem.h"

namespace TTHbb{

  void ToolDictionary::buildDictionary(TTHbb::ToolManager* toolManager){
    /// Add each tool here using addToolToDict
    /// It is important to make sure everything is added in running order

    /*toolManager->addToolToDict("Test.runTool",
			       "MyToolsName",
			       []{ return new test("InternalToolName"); }
			       );*/

    toolManager->addToolToDict("TtTRF.TurnOn",
			       "TtTRF",
			       []{ return new TtTRF("TtTRF");}
			       );
    
    toolManager->addToolToDict("MVAVariables.TurnOn",/// This is the on/off setting in the config file
			       "MVAVariables",/// This is the name of the tool in the ToolManager dictionary (and what is given to the getter function)
			       []{ return new OfflineMVAVariables("MVAVariables");}/// This is the tool itself with its internal name
			       );

    toolManager->addToolToDict("OfflineFakes.TurnOn",
			       "OfflineFakes", 
			       []{ return new OfflineFakes("OfflineFakes");});
    
    toolManager->addToolToDict("OfflineTtbarCorrections.TurnOn",
			       "OfflineTtbarCorrections",
			       []{ return new OfflineTtbarCorrections("OfflineTtbarCorrections");});

    toolManager->addToolToDict("TRFFakes.TurnOn",
			       "TRFFakes",
			       []{ return new OfflineTRFFakes("OfflineTRFFakes");});

    toolManager->addToolToDict("MCFakes.TurnOn", "MCFakes", []{ return new OfflineMCFakes("OfflineMCFakes");});
    
    toolManager->addToolToDict("CPVariables.TurnOn",
			       "CPVariables",
			       []{ return new OfflineCPVariables("CPVariables");}
			       );

    /// This is where additional dictionaries from external packages can be loaded
    /// This means that the number of dependencies in OfflineTTHbb is reduced, and
    /// the user/developer needs not modify the OfflineTTHbb package to add in a tool
    /// Additional dictionaries are specified in the config file, giving their full name
    /// For example, TTHbb::ExtraDictionary
    auto* config = TTHbb::GlobalConfiguration::get();
    std::vector<std::string> extralibs = TTHbb::util::vectoriseString((*config)("Dictionaries"));

    
    for(auto lib : extralibs){
      std::cout << "INFO: Adding tools to the ToolManager from Dictionary in  " << lib << std::endl;
      gSystem->Load(("lib" + lib + ".so").c_str());
      TClass* c = TClass::GetClass(("TTHbb::" + lib + "Dictionary").c_str());
      if(c == 0){
	std::cout << "ERROR: Library for dictionary in " << lib << " not found! Check the naming is TTHbb::" << lib << "Dictionary." << std::endl;
	exit(18);
      }
      DictBase* dict = (DictBase*)c->New();

      dict->buildDictionary(toolManager);
      delete dict;
      delete c;
    }

    toolManager->dictionaryLoaded();
  }

  ToolDictionary::ToolDictionary(){
  }

  ToolDictionary::~ToolDictionary(){
  }

}
