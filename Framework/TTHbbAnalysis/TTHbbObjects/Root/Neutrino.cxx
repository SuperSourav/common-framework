/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/Neutrino.h"

namespace TTHbb{

  Neutrino::Neutrino(){
    m_objectType = PhysObjectType::NeutrinoType;
  }

  Neutrino::Neutrino(float pt, float eta, float phi, float e){
    this->SetPtEtaPhiE(pt,eta,phi,e);
    m_objectType = PhysObjectType::NeutrinoType;
  }

  Neutrino::~Neutrino(){
  }

  Neutrino* Neutrino::clone() const{
    return new Neutrino(*this);
  }

  Neutrino* Neutrino::deepClone() const{
    return new Neutrino(*this);
  }
}
