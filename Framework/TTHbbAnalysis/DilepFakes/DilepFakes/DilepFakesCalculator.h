/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _DILEPFAKESCALCULATOR_H_
#define _DILEPFAKESCALCULATOR_H_

#include "TTHbbObjects/Event.h"

#include "MCFakes/MCFakesCalculator.h"


namespace TTHbb {
  class DilepFakesCalculator {
  public:
    // Default constructor, fake normalisation weights are set to 1.
    DilepFakesCalculator();

    // Constructor for specifying fake normalisation weights to use
    DilepFakesCalculator(float weightNorm_subleadElChan, float weightNorm_subleadMuChan);
    ~DilepFakesCalculator() {};
    

    /* Generic lepton container functions */

    // Check if your leptons come from an event categorised as subleading electron or subleading muon channel
    // (these are the channels currently used in the fake estimate method)
    inline bool isSubleadElChannel(LeptonContainer& leptons) { return isSubleadChannel(leptons, leptonType::electron, leptonType::muon); };
    inline bool isSubleadMuChannel(LeptonContainer& leptons) { return isSubleadChannel(leptons, leptonType::muon, leptonType::electron); };

    // Get fake normalisation weight appropriate for the current event
    // (returns 1. if no fake leptons are present)
    float getFakeNormWeight(LeptonContainer& leptons);
    
    /**************************************/

    
    /* Electron and muon container functions */

    // Check if your leptons come from an event categorised as subleading electron or subleading muon channel
    // (these are the channels currently used in the fake estimate method)
    inline bool isSubleadElChannel(LeptonContainer& electrons, LeptonContainer& muons) { return isSubleadChannel(electrons, muons); };
    inline bool isSubleadMuChannel(LeptonContainer& electrons, LeptonContainer& muons) { return isSubleadChannel(muons, electrons); };

    // Get fake normalisation weight appropriate for the current event
    // (returns 1. if no fake leptons are present)
    float getFakeNormWeight(LeptonContainer& electrons, LeptonContainer& muons);
    
    /*****************************************/
    
  private:
    float m_weightNorm_subleadElChan;
    float m_weightNorm_subleadMuChan;

    bool isSubleadChannel(LeptonContainer& leptons, leptonType leptonTypeA, leptonType leptonTypeB);
    bool isSubleadChannel(LeptonContainer& leptons_typeA, LeptonContainer& leptons_typeB);
  };
}

#endif  //_DILEPFAKESCALCULATOR_H_
