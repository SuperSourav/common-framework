/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _OFFLINEMCFAKES_H_
#define _OFFLINEMCFAKES_H_

#include "TTHbbToolManager/ToolBase.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "MCFakes/MCFakesAppender.h"

namespace TTHbb {
  class OfflineMCFakes : public ToolBase {
  public:
    OfflineMCFakes(std::string name);
    ~OfflineMCFakes() {};

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();
  };
}
#endif //_OFFLINEMCFAKES_H_
