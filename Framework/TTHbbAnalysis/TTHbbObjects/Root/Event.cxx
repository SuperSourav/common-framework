/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/Jet.h"

namespace TTHbb{

  Event::Event(){
    met_met=0;
    met_phi=0;
    m_reco=false;
    m_truth=false;
  }

  Event::~Event(){
  }

  std::shared_ptr<Jet> Event::addJet(float pt, float eta, float phi, float e, float mv2, float jvt){
    std::shared_ptr<Jet> jet = std::make_shared<Jet>(pt, eta, phi, e);
    jet->floatVariable("mv2")=mv2;
    jet->floatVariable("jvt")=jvt;
    m_jets.push_back(jet);
    return jet;
  }

  void Event::addJet(std::shared_ptr<Jet> j){
    m_jets.push_back(j);
  }

  std::shared_ptr<Lepton> Event::addLepton(float pt, float eta, float phi, float e){
    m_leptons.push_back(std::make_shared<Lepton>(pt, eta, phi, e));
    return m_leptons.back();
  }

  void Event::addLepton(std::shared_ptr<Lepton> l){
    m_leptons.push_back(l);
  }

  void Event::addElectron(std::shared_ptr<Lepton> l){
    m_electrons.push_back(l);
  }

  void Event::addMuon(std::shared_ptr<Lepton> l){
    m_muons.push_back(l);
  }

  void Event::addBucket(Bucket b){
    m_BucketCont.push_back(b);
  }
  std::shared_ptr<LargeJet> Event::addLargeJet(float pt, float eta, float phi, float e, int topTag, float D2, float tau32_wta){
    m_largeJets.push_back(std::make_shared<LargeJet>(pt, eta, phi, e, topTag, D2, tau32_wta));
    return m_largeJets.back();
  }

  void Event::addLargeJet(std::shared_ptr<LargeJet> j){
    m_largeJets.push_back(j);
  }
  
  // Michele
  std::shared_ptr<Particle> Event::addCustomObject(std::string name){
    m_customObj[name].push_back(std::make_shared<Particle>());
    m_customObj[name].back()->objectType(PhysObjectType::CustomObjectType);
    return m_customObj[name].back();
  }

  // Michele
  void Event::addCustomObject(std::string name,std::shared_ptr<Particle> o){
    m_customObj[name].push_back(o);
  }

  void Event::clearAll(){
    clearReco();
    clearTruth();
  }

  void Event::clearReco(){
    m_jets.clear();
    m_electrons.clear();
    m_muons.clear();
    m_leptons.clear();
    m_largeJets.clear();
    m_neutrinos.clear();
    m_combinationDynVars.clear();
    m_customObj.clear();
    clearDecorations();
    m_reco=false;
  }

  void Event::clearTruth(){
    m_partons.clear();
    m_HFHadrons.clear();
    m_truthJets.clear();
    m_truth=false;
  }

  void Event::clearDecorations(){
    /// not clearing the pointer decorations
    /// not sure who isthe owner
    m_intDynVars.clear();
    m_floatDynVars.clear();
    m_charDynVars.clear();

  }

  std::shared_ptr<Combination>& Event::combinationVariable(std::string name){
    return m_combinationDynVars[name];
  }

}
