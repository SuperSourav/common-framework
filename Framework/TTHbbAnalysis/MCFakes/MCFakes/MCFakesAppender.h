/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MCFAKESAPPENDER_H_
#define _MCFAKESAPPENDER_H_

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "MCFakes/MCFakesCalculator.h"

namespace TTHbb {
  class MCFakesAppender {
  public:
    MCFakesAppender() {};
    ~MCFakesAppender() {};

    // Make variable appending generically available i.e. to perform the same variable appending in online code without duplicating code
    static void appendEventVars(TTHbb::Event* event);

    static inline void appendLeptonVars(TTHbb::Event* event) { MCFakesAppender::appendLeptonVars(event->m_leptons); };
    static void appendLeptonVars(TTHbb::LeptonContainer& leptons);
  };
}
#endif //_MCFAKESAPPENDER_H_
