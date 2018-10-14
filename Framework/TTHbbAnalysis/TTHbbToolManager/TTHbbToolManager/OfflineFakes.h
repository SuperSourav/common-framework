/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBB_OFFLINEFAKES_H_
#define _TTHBB_OFFLINEFAKES_H_

#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbToolManager/ToolBase.h"
#include "TopFakes/FakesWeights.h"

#include <iostream>
#include <vector>

#include "TLorentzVector.h"
#include "TMath.h"

namespace TTHbb {

  class OfflineFakes : public ToolBase{

  public:
    OfflineFakes(std::string name);
    ~OfflineFakes(){};

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();
    
  private:
    FakesWeights *m_weight_ejets_2015;
    FakesWeights *m_weight_ejets_2016;
    FakesWeights *m_weight_mujets_2015;
    FakesWeights *m_weight_mujets_2016;

    bool m_use2016Prescaled; 
    bool m_applyPrescales;

    float m_weight_mm;
    
  };

  // utility functions

  float deltaPhi(float phi1, float phi2);
  float minDeltaRljet(std::shared_ptr<Lepton> lep,std::vector<std::shared_ptr<Jet> > jets);
  
}

#endif 
