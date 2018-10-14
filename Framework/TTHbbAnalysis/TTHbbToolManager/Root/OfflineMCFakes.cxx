/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/OfflineMCFakes.h"
	
namespace TTHbb {
  OfflineMCFakes::OfflineMCFakes(std::string name) : ToolBase(true){
    m_name = name;
  }
  
  void OfflineMCFakes::initialise() {
  }
	
  void OfflineMCFakes::apply(TTHbb::Event* event) {
    MCFakesAppender::appendEventVars(event);
  }
	
	 
  void OfflineMCFakes::finalise() {
  }
}
