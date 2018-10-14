/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/LargeJet.h"

namespace TTHbb{

  LargeJet::LargeJet(){
    m_objectType=PhysObjectType::LargeJetType;
  }
  
  LargeJet::LargeJet(float pt, float eta, float phi, float e){
    this->SetPtEtaPhiE(pt,eta,phi,e);
    m_objectType=PhysObjectType::LargeJetType;
  }

  LargeJet::LargeJet(float pt, float eta, float phi, float e, int topTag, float D2, float tau32_wta){
    this->SetPtEtaPhiE(pt,eta,phi,e);
    m_objectType=PhysObjectType::LargeJetType;
    intVariable("topTag")=topTag;
    floatVariable("D2")=D2;
    floatVariable("tau32_wta")=tau32_wta;
  }


  LargeJet::~LargeJet(){
  }

  LargeJet* LargeJet::clone() const{
    return new LargeJet(*this);
  }

  LargeJet* LargeJet::deepClone() const{
    return new LargeJet(*this);
  }

}
