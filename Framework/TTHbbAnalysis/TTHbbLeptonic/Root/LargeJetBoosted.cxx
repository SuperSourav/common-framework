/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/LargeJetBoosted.h"
#include "TTHbbLeptonic/BoostedTools.h"

LargeJetBoosted::LargeJetBoosted(double ptcut, double etamax) :
  m_ptcut(ptcut),
  m_etamax(etamax) {

  m_bvariables = new BoostedTools;


  }
 
LargeJetBoosted::~LargeJetBoosted() {
  delete m_bvariables;
}

bool LargeJetBoosted::passSelection(const xAOD::Jet& jet) {
  if (jet.pt() < m_ptcut)
    return false;

  if (std::fabs(jet.eta()) > m_etamax)
    return false;

  m_bvariables->initialise(jet);
  
  jet.auxdecor<float>("tau21_wta")     = m_bvariables->Tau21_wta();
  jet.auxdecor<float>("tau32_wta")     = m_bvariables->Tau32_wta();
  jet.auxdecor<float>("D2")            = m_bvariables->D2();
  jet.auxdecor<float>("C2")            = m_bvariables->C2();
//  jet.auxdecor<bool>("topTag")         = m_bvariables->TopTag();
  jet.auxdecor<bool>("bosonTag")       = m_bvariables->BosonTag();
//  jet.auxdecor<bool>("topTag_loose")   = m_bvariables->TopTag_loose();
  jet.auxdecor<bool>("bosonTag_loose") = m_bvariables->BosonTag_loose();
  jet.auxdecor<char>("good")           = 1;
  

  return true;
}
   
void LargeJetBoosted::print(std::ostream& os) const {
  os << "LargeJetBoosted\n"
     << "    * pT > " << m_ptcut << "\n"
     << "    * |eta| < " << m_etamax << "\n";

}
