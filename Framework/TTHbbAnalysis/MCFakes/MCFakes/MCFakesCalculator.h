/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MCFAKESCALCULATOR_H_
#define _MCFAKESCALCULATOR_H_

#include "TTHbbObjects/Event.h"

namespace TTHbb {
  class MCFakesCalculator {
  public:
    // Default constructor.
    // Currently all functions are static, so object creation not needed, but could be useful in future for storing settings
    MCFakesCalculator() {};
    ~MCFakesCalculator() {};

    
    // Test if a specific lepton is considered fake, using variables from lepton
    // (these functions contain actual current prompt lepton definitions)
    static bool isFakeElectron(int el_true_type, int el_true_origin, int el_true_originbkg);
    static bool isFakeMuon(int mu_true_type, int mu_true_origin, int mu_true_originbkg);

    // Test specific lepton by TTHbbEDM pointer to lepton
    static inline bool isFakeElectron(std::shared_ptr<Lepton> el) { return MCFakesCalculator::isFakeElectron(el->intVariable("el_true_type"), el->intVariable("el_true_origin"), el->intVariable("el_true_originbkg")); };
    static inline bool isFakeMuon(std::shared_ptr<Lepton> mu) { return MCFakesCalculator::isFakeMuon(mu->intVariable("mu_true_type"), mu->intVariable("mu_true_origin"), mu->intVariable("mu_true_originbkg")); };


    /* Generic lepton container functions */

    // Check if any of your leptons are categorised as fake, returning true if so
    static bool hasFakeLeptons(LeptonContainer& leptons);

    /**************************************/

    
    /* Electron and muon container functions */

    // Check if any of your leptons are categorised as fake, returning true if so
    static bool hasFakeLeptons(LeptonContainer& electrons, LeptonContainer& muons);

    /*****************************************/
  };
}

#endif // _DILEPFAKESCALCULATOR_H_
