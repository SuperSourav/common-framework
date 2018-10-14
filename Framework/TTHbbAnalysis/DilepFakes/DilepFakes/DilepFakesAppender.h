/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _DILEPFAKESAPPENDER_H_
#define _DILEPFAKESAPPENDER_H_

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "MCFakes/MCFakesCalculator.h"

#include "DilepFakes/DilepFakesCalculator.h"


namespace TTHbb {
  class DilepFakesAppender {
  public:
    DilepFakesAppender() {};
    ~DilepFakesAppender() {};

    // Make variable appending generically available i.e. to perform the same variable appending in online code without duplicating code
    static void appendVars(TTHbb::Event* event, std::shared_ptr<TTHbb::DilepFakesCalculator> fakesCalculator);
  };
}

#endif  //_DILEPFAKESAPPENDER_H_
