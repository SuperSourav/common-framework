/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DilepFakes/DilepFakesCalculator.h"

namespace TTHbb {
  DilepFakesCalculator::DilepFakesCalculator() : m_weightNorm_subleadElChan(1.), m_weightNorm_subleadMuChan(1.){
    
  }
  
  
  DilepFakesCalculator::DilepFakesCalculator(float weightNorm_subleadElChan, float weightNorm_subleadMuChan) : m_weightNorm_subleadElChan(weightNorm_subleadElChan), m_weightNorm_subleadMuChan(weightNorm_subleadMuChan) {

  }

  
  /* Generic lepton container functions */
  
  float DilepFakesCalculator::getFakeNormWeight(LeptonContainer& leptons) {
    if (MCFakesCalculator::hasFakeLeptons(leptons)) {
      if (isSubleadElChannel(leptons)) {
	return m_weightNorm_subleadElChan;
      }
  
      if (isSubleadMuChannel(leptons)) {
	return m_weightNorm_subleadMuChan;
      }
    }

    return 1.;
  }
  
  /**************************************/
  
  
  /* Electron and muon container functions */

  float DilepFakesCalculator::getFakeNormWeight(LeptonContainer& electrons, LeptonContainer& muons) {
    if (MCFakesCalculator::hasFakeLeptons(electrons, muons)) {
      if (isSubleadElChannel(electrons, muons)) {
	return m_weightNorm_subleadElChan;
      }
      if (isSubleadMuChannel(electrons, muons)) {
	return m_weightNorm_subleadMuChan;
      }
    }

    return 1.;
  }

  /*****************************************/


  bool DilepFakesCalculator::isSubleadChannel(LeptonContainer& leptons, leptonType leptonTypeA, leptonType leptonTypeB) {
    if (leptons.size() != 2) return false;

    if (leptons.at(0)->type() == leptonTypeB && leptons.at(1)->type() == leptonTypeB) {
      return false;
    }

    if (leptons.at(0)->type() == leptonTypeA && leptons.at(1)->type() == leptonTypeA) {
      return true;
    }

  if (   (leptons.at(0)->type() == leptonTypeA && leptons.at(0)->p4().Pt() < leptons.at(1)->p4().Pt())
      || (leptons.at(1)->type() == leptonTypeA && leptons.at(1)->p4().Pt() < leptons.at(0)->p4().Pt()) ) {
      return true;
    }

    return false;
  }
  
  
  // Check if current lepton collection of lepton typeA comes from an event where lepton typeA is subleading
  // (e.g. check if the current electron collection comes from a subleading electron event)
  bool DilepFakesCalculator::isSubleadChannel(LeptonContainer& leptons_typeA, LeptonContainer& leptons_typeB) {    
    if (leptons_typeA.size() + leptons_typeB.size() != 2 || leptons_typeA.size() == 0) {
      return false;
    }

    if (leptons_typeA.size() == 2) {
      return true;
    }
    else if (leptons_typeA.at(0)->p4().Pt() < leptons_typeB.at(0)->p4().Pt()) {
      return true;
    }

    return false;
  }
}
