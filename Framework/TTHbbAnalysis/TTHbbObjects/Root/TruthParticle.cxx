/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/TruthParticle.h"

#include <iostream>
#include <bitset>

namespace TTHbb{
  
  TruthParticle::TruthParticle(){ 
    m_objectType=PhysObjectType::BasicTruthParticleType;
    m_historyInfo=0;
  }

  TruthParticle::TruthParticle(double pt, double eta, double phi, double e){ 
    m_objectType=PhysObjectType::BasicTruthParticleType;
    m_historyInfo=0;
    this->SetPtEtaPhiE(pt, eta, phi, e);
  }

  TruthParticle::~TruthParticle(){
  }

  TruthParticle* TruthParticle::clone() const{
    return new TruthParticle(*this);
  }

  TruthParticle* TruthParticle::deepClone() const{
    return new TruthParticle(*this);
  }


}
