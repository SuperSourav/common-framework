/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DilepFakes/DilepFakesAppender.h"

namespace TTHbb {
  void DilepFakesAppender::appendVars(TTHbb::Event* event, std::shared_ptr<TTHbb::DilepFakesCalculator> fakesCalculator) {
    int isSubleadElChan = 0;
    int isSubleadMuChan = 0;
    float weight_dilepFakesNorm = 1.;

    if (event->m_leptons.size() == 2) {

      // Here getFakeNormWeight will perform hasFakesLeptons and isSubleadLepChannel checks and return appropriate weight
      // If I change structure of DilepFakesCalculator, I could avoid repeating checks and just return the bools I'm setting myself here all at the same time, or at least set them simultaneously
      if (MCFakesCalculator::hasFakeLeptons(event->m_leptons)) {
	weight_dilepFakesNorm = fakesCalculator->getFakeNormWeight(event->m_leptons);
      }

      if (fakesCalculator->isSubleadElChannel(event->m_leptons)) {
	  isSubleadElChan = 1;
      }

      if (fakesCalculator->isSubleadMuChannel(event->m_leptons)) {
	  isSubleadMuChan = 1;
      }
    }
    
    event->charVariable("isSubleadElChan") = isSubleadElChan;
    event->charVariable("isSubleadMuChan") = isSubleadMuChan;
    event->floatVariable("weight_dilepFakesNorm") = weight_dilepFakesNorm;
  }
}
