/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/RecoObjects.h"
#include <iostream>

namespace TTHbb{


  Composite::Composite(){

    m_objectType= PhysObjectType::CompositeType;
    clear();

  }

  Composite::~Composite(){
  }


  void Composite::clear(){


    child1.reset();
    child2.reset();
  
    isMatch=false;
    isOneMatch=false;

  }


  Composite* Composite::clone() const{
    return new Composite(*this);
  }

  Composite* Composite::deepClone() const{

    Composite *n = new Composite(*this);

    n->child1 = (child1)?std::make_shared<Particle>(*child1):0;
    n->child2 = (child2)?std::make_shared<Particle>(*child2):0;

    return n;

  }

  void Composite::dump() const{

    std::cout << "composite type=" << (int)m_objectType << " name="<< name 
	      << " p4=pTEtaPhiM(" << Pt()<<","<<  Eta()<<","<<  Phi()<<","<< M() <<")" 
	      << " formed of:" << std::endl;
    if(child1){
      std::cout << " --->"; child1->dump();
    }
    if(child2){
      std::cout << " --->"; child2->dump();
    }
    std::cout << "--- end " << name <<" ---" << std::endl;

  }

  //// TopComposite

  TopComposite::TopComposite(){
    m_objectType=PhysObjectType::TopCompositeType;
    clear();
  }

  TopComposite::~TopComposite(){
  }


  void TopComposite::clear(){

    childw.reset();
    childb.reset();
    isMatch=false;
    isWMatch=false;
    isBMatch=false;

  }


  TopComposite* TopComposite::clone() const{
    return new TopComposite(*this);
  }

  TopComposite* TopComposite::deepClone() const{

    TopComposite* n = new TopComposite(*this);

    n->childw = (childw)?std::shared_ptr<Composite>(childw->deepClone()):0;
    n->childb = (childb)?std::make_shared<Particle>(*childb):0;
  
    return n;

  }

  void TopComposite::dump() const{

    std::cout << "top composite type=" << (int)m_objectType << " name="<< name 
	      << " p4=pTEtaPhiM(" << Pt()<<","<<  Eta()<<","<<  Phi()<<","<< M() <<")"
	      << " formed of:" << std::endl;
    if(childw){
      std::cout << " --->"; childw->dump();
    }
    if(childb){
      std::cout << " --->"; childb->dump();
    }
    std::cout << "--- end " << name <<" ---" << std::endl;
  }

  /// ParticleCluster

  ParticleCluster::ParticleCluster(){
    m_objectType=PhysObjectType::ParticleClusterType;
    clear();
  }

  ParticleCluster::~ParticleCluster(){
  }

  void ParticleCluster::clear(){
    jets.clear();
    lepton.reset();
    neutrino.reset();
  }

  ParticleCluster* ParticleCluster::clone() const{
    return new ParticleCluster(*this);
  }

  ParticleCluster* ParticleCluster::deepClone() const{

    ParticleCluster* n = new ParticleCluster(*this);

    n->lepton = (lepton)?std::make_shared<Lepton>(*lepton):0;
    n->neutrino = (neutrino)?std::make_shared<Neutrino>(*neutrino):0;

    n->jets.clear();
    for(auto child: jets){
      n->jets.push_back(std::make_shared<Jet>(*child));
    }

    return n;

  }

  void ParticleCluster::dump() const{

    std::cout << "ParticleCluster dump implementation in progress..." << std::endl;

  }


  /// Combination
  Combination::Combination(){
    clear();
  }

  Combination::~Combination(){
  }


  void Combination::clear(){

    hadTop.reset();
    lepTop.reset();
    lepTop1.reset();
    lepTop2.reset();
    higgs.reset();
    incompleteHadTop.reset();
    isMatch=false;
    truthMatchPattern=0;

    m_intDynVars.clear();
    m_charDynVars.clear();
    m_floatDynVars.clear();
  }


  Combination* Combination::clone() const{
    return new Combination(*this);
  }


  Combination* Combination::deepClone() const{

    Combination *n = new Combination(*this);

    n->hadTop=(hadTop)?std::shared_ptr<TopComposite>(hadTop->deepClone()):0;
    n->lepTop=(lepTop)?std::shared_ptr<TopComposite>(lepTop->deepClone()):0;
    n->higgs=(higgs)?std::shared_ptr<Composite>(higgs->deepClone()):0;
    n->incompleteHadTop=(incompleteHadTop)?std::shared_ptr<Composite>(incompleteHadTop->deepClone()):0;

    return n;
  }


  void Combination::dump() const{

    std::cout << "new Combination containing:" << std::endl;
    if(hadTop){
      std::cout << " --->"; hadTop->dump();
    }
    if(lepTop){
      std::cout << " --->"; lepTop->dump();
    }
    if(higgs){
      std::cout << " --->"; higgs->dump();
    }
    if(incompleteHadTop){
      std::cout << " --->"; incompleteHadTop->dump();
    }
  }


  RecoEvent::RecoEvent(){
    clear();
  }

  RecoEvent::~RecoEvent(){
    ///..
  }



  std::shared_ptr<Combination>& RecoEvent::combinationVariable(std::string s){
    return m_combinationDynVars[s];
  }

  void RecoEvent::clear(){
    nJets=-1;
    nBJets=-1;
    leptonType=0;
    nLeptons=-1;
    nAllLeptons=-1;
    nNeutrinos=-1;
    //m_btaggingCut=-9999;
    m_btagIdName="";
    m_floatDynVars.clear();
    m_intDynVars.clear();
    m_charDynVars.clear();
    m_combinationDynVars.clear();
  }



} /// namespace
