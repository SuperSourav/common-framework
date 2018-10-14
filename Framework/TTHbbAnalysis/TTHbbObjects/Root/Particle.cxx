/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/Particle.h"

#include <iostream>
#include <bitset>

namespace TTHbb{
  
  Particle::Particle(){ 
    m_objectType=PhysObjectType::BasicParticleType;
    m_truthMatch=0;
  }

  Particle::~Particle(){
  }

  Particle* Particle::clone() const{
    return new Particle(*this);
  }

  Particle* Particle::deepClone() const{
    return new Particle(*this);
  }

  void Particle::dump() const{

    std::cout << "particle type=" << (int)m_objectType 
	      << " name="<< name 
	      << " p4=pTEtaPhiM(" << Pt()<<","<<  Eta()<<","<<  Phi()<<","<< M() <<")" 
	      << " truthmatch=" <<m_truthMatch<<"("<<std::bitset<64>(m_truthMatch)<<")"
	      << std::endl;

  }

  std::vector<std::weak_ptr<Particle> >& Particle::particleMatch(std::string n){
    return m_particleMatchs[n];
  }

  bool Particle::checkParticleMatch(std::string n){
    return m_particleMatchs.count(n);
  }


}
