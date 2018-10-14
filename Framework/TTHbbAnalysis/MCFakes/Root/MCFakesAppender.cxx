/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCFakes/MCFakesAppender.h"

namespace TTHbb {  
  void MCFakesAppender::appendEventVars(TTHbb::Event* event) {
    int hasFakeLepton = 0;

    if (MCFakesCalculator::hasFakeLeptons(event->m_leptons)) {
      hasFakeLepton = 1;
    }

    event->intVariable("MCFakes_hasFakeLepton") = hasFakeLepton;
  }

  
  void MCFakesAppender::appendLeptonVars(TTHbb::LeptonContainer& leptons) {
    for (std::shared_ptr<Lepton> lepton : leptons) {
      int isFake = 0;
      
      if (lepton->type() == leptonType::electron) {
	isFake = MCFakesCalculator::isFakeElectron(lepton);
      }
      else if (lepton->type() == leptonType::muon) {
	isFake = MCFakesCalculator::isFakeMuon(lepton);
      }

      lepton->charVariable("MCFakes_isFakeLepton") = isFake;
    }
  }
}
