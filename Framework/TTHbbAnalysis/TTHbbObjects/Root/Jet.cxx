/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/Jet.h"

namespace TTHbb{

  Jet::Jet(){
    m_objectType = PhysObjectType::JetType;
  }
  
  Jet::Jet(float pt, float eta, float phi, float e){
    this->SetPtEtaPhiE(pt,eta,phi,e);
    m_objectType = PhysObjectType::JetType;
  }

  Jet::~Jet(){
  }

  Jet* Jet::clone() const{
    return new Jet(*this);
  }

  Jet* Jet::deepClone() const{
    return new Jet(*this);
  }

}
