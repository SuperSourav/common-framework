/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/NLargeJetSelector.h"


NLargeJetSelector::NLargeJetSelector(const std::string& params) :
  SignValueSelector("LJET_N", params, true) {
  checkMultiplicityIsInteger();
}
 
bool NLargeJetSelector::apply(const top::Event& event) const {
  auto func = [&](const xAOD::Jet* jetPtr){return jetPtr->pt() > value();};
  auto count = std::count_if(event.m_largeJets.begin(), event.m_largeJets.end(), func);
  return checkInt(count, multiplicity());
}
