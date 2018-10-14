/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <utility>
#include <memory>
#include "TLorentzVector.h"
//#include "TopEvent/Event.h"
#include "TTHbbObjects/Particle.h"

#ifndef PAIREDSYSTEM_H_
#define PAIREDSYSTEM_H_

class PairedSystem : public TLorentzVector{

 public:

  PairedSystem();
  PairedSystem(TLorentzVector v1, TLorentzVector v2);
  PairedSystem(std::shared_ptr<TTHbb::Particle> p1, std::shared_ptr<TTHbb::Particle> p2);
  PairedSystem(const std::vector<PairedSystem>& psv);
  PairedSystem(PairedSystem* ps);
  virtual ~PairedSystem();

  TLorentzVector p4() const{ return *this; };

  float DeltaEta() const;
  float DeltaR() const;
  float DeltaPhi() const;
  float SumP() const;
  float SumPt() const;
  float DiffP() const;

  //float DeltaPhiMet(const top::Event& event);
  
 private:
  
  float m_DeltaEta;
  float m_DeltaR;
  float m_DeltaPhi;
  float m_SumP;
  float m_SumPt;
  float m_DiffP; 
  
};

#endif
