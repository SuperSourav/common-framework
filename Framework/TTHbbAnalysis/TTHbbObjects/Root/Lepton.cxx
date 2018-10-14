/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/Lepton.h"

namespace TTHbb{

  Lepton::Lepton(){
    m_objectType = PhysObjectType::LeptonType;
  }

  Lepton::Lepton(float pt, float eta, float phi, float e){
    this->SetPtEtaPhiE(pt,eta,phi,e);
    m_objectType = PhysObjectType::LeptonType;
  }

  Lepton::~Lepton(){
  }

  Lepton* Lepton::clone() const{
    return new Lepton(*this);
  }

  Lepton* Lepton::deepClone() const{
    return new Lepton(*this);
  }

}
