/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAVariables/PairedSystem.h"

PairedSystem::PairedSystem() :
  TLorentzVector(0,0,0,0), m_DeltaEta(-1), m_DeltaR(0), m_DeltaPhi(-1), m_SumP(-1), m_SumPt(-1),
  m_DiffP(-1)
{
}


PairedSystem::PairedSystem(TLorentzVector v1, TLorentzVector v2) : TLorentzVector()
{
  TLorentzVector::operator=(v1+v2);
  m_DeltaEta = fabs(v1.Eta() - v2.Eta());
  m_DeltaPhi = fabs(v1.DeltaPhi(v2));
  m_DeltaR = v1.DeltaR(v2);
  m_SumP = v1.P() + v2.P();
  m_SumPt = v1.Pt() + v2.Pt();
  m_DiffP = fabs(v1.P() - v2.P());
}

PairedSystem::PairedSystem(std::shared_ptr<TTHbb::Particle> p1, std::shared_ptr<TTHbb::Particle> p2) : PairedSystem(p1->p4(), p2->p4())
{
}

PairedSystem::PairedSystem(const std::vector<PairedSystem>& psv) : TLorentzVector()
{
  double invsize = (psv.size()>0)?1.0/(psv.size()):1.;

  m_DeltaEta=0;
  m_DeltaPhi=0;
  m_DeltaR=0;
  m_SumP=0;
  m_SumPt=0;
  m_DiffP=0;

  for(auto ps : psv){
    TLorentzVector::operator+=(ps);
    m_DeltaEta += ps.DeltaEta();
    m_DeltaPhi += ps.DeltaPhi();
    m_DeltaR += ps.DeltaR();
    m_SumP += ps.SumP();
    m_SumPt += ps.SumPt();
    m_DiffP += ps.DiffP();
  }
  TLorentzVector::operator*=(invsize);
  m_DeltaEta *= invsize;
  m_DeltaPhi *= invsize;
  m_DeltaR *= invsize;
  m_SumP *= invsize;
  m_SumPt *= invsize;
  m_DiffP *= invsize;
}

PairedSystem::PairedSystem(PairedSystem* ps) : TLorentzVector()
{
  TLorentzVector::operator=(*ps);
  m_DeltaEta = ps->DeltaEta();
  m_DeltaPhi = ps->DeltaPhi();
  m_DeltaR = ps->DeltaR();
  m_SumP = ps->SumP();
  m_SumPt = ps->SumPt();
  m_DiffP = ps->DiffP();
}



PairedSystem::~PairedSystem(){
}

//--------//

float PairedSystem::DeltaEta() const{
  return m_DeltaEta;
}

float PairedSystem::DeltaR() const {
  return m_DeltaR;
}

float PairedSystem::DeltaPhi() const{
  return m_DeltaPhi;
}

float PairedSystem::SumP() const{
  return m_SumP;
}

float PairedSystem::SumPt() const{
  return m_SumPt;
}

float PairedSystem::DiffP() const{
  return m_DiffP;
}

/*float PairedSystem::DeltaPhiMet(const top::Event& event){
  return fabs(event.m_met->phi() - this->Phi());
  }*/
