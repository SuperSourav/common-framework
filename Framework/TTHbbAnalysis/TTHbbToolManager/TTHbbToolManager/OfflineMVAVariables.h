/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _OFFLINEMVAVARIABLES_H_
#define _OFFLINEMVAVARIABLES_H_

#include "TTHbbToolManager/ToolBase.h"
#include "MVAVariables/MVAVariables.h"

#include <memory>

namespace TTHbb{
  
  class OfflineMVAVariables : public ToolBase{
    /**
     * Implementation of MVAVariables which is accessible whilst running in all tools
     * to keep down the number of calculations
     */
  public:
    OfflineMVAVariables(std::string name);
    ~OfflineMVAVariables();
    
    void initialise();
    
    void apply(TTHbb::Event* event);

    void finalise();

    std::shared_ptr<MVAVariables> getMVAVariables(std::string name);

  private:
    std::map<std::string, std::shared_ptr<MVAVariables> > mapOfMVAVars;
  };

}
#endif
