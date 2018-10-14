/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCFakes/MCFakesCalculator.h"

namespace TTHbb {

  // Note: if a truth flag of 0 is ever considered prompt electron, then events without electrons will be considered prompt as intVariable will return 0 if the variable doesn't exist.
  // Fine for now but I could do a simple if (checkIntVariable) to be sure.
  bool MCFakesCalculator::isFakeElectron(int el_true_type, int el_true_origin, int el_true_originbkg) {
    bool prompt    = (el_true_type == 2 &&
		       (el_true_origin == 10 || el_true_origin == 12 || el_true_origin == 13 || el_true_origin == 14 || el_true_origin == 43) ); // 43 is "diboson" origin, but is needed due to buggy origin flags in Sherpa ttbar
    bool recovered = (el_true_type == 4 && el_true_origin == 5 &&
		       (el_true_originbkg == 10 || el_true_originbkg == 12 || el_true_originbkg == 13 || el_true_originbkg == 14 || el_true_originbkg == 43) ); // 43 is "diboson" origin, but is needed due to buggy origin flags in Sherpa ttbar

    return !(prompt || recovered);
  }


  bool MCFakesCalculator::isFakeMuon(int mu_true_type, int mu_true_origin, int /*mu_true_originbkg*/) {
    bool prompt = (mu_true_type == 6 &&
		    (mu_true_origin == 10 || mu_true_origin == 12 || mu_true_origin == 13 || mu_true_origin == 14 || mu_true_origin == 43) ); // 43 is "diboson" origin, but is needed due to buggy origin flags in Sherpa ttbar
    
    return !prompt;
  }


  /* Generic lepton container functions */

  bool MCFakesCalculator::hasFakeLeptons(LeptonContainer& leptons) {
    bool hasFakeLep = false;
    
    for (std::shared_ptr<Lepton> lep : leptons) {
      if (lep->checkCharVariable("isPrompt")) { // Use variable dressed by TTHbbEDMConverter if available
	if (lep->charVariable("isPrompt") == 0) {
	  hasFakeLep = true;
	  break;
	}
      }
      else if (lep->type() == leptonType::muon) { // Currently, fake muon check will be quicker to do than fake electron check, so check muons first
	if (MCFakesCalculator::isFakeMuon(lep)) {
	  hasFakeLep = true;
	  break;
	}
      }
      else if (lep->type() == leptonType::electron) {
	if (MCFakesCalculator::isFakeElectron(lep)) {
	  hasFakeLep = true;
	  break;
	}
      }
    }

    return hasFakeLep;
  }
  
  /**************************************/


  /* Electron and muon container functions */

  bool MCFakesCalculator::hasFakeLeptons(LeptonContainer& electrons, LeptonContainer& muons) {
    bool hasFakeLep = false;
    
  for (std::shared_ptr<Lepton> mu : muons) { // Currently, fake muon check will be quicker to do than fake electron check, so check muons first
      if (MCFakesCalculator::isFakeMuon(mu)) {
	hasFakeLep = true;
	break;
      }
    }

    if (!hasFakeLep) { // No need to check electron if you've already found a fake muon
      for (std::shared_ptr<Lepton> el : electrons) {
	if (MCFakesCalculator::isFakeElectron(el)) {
	  hasFakeLep = true;
	  break;
	}
      }
    }
    
    return hasFakeLep;
  }

  /*****************************************/
}
