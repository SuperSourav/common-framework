/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"

#include "TTHbbObjects/Event.h"

#include "TopEvent/EventTools.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHbbAsgToolHelper::TTHbbAsgToolHelper(std::string name):
  asg::AsgTool(name)
 {
 
}

TTHbbAsgToolHelper::~TTHbbAsgToolHelper(){

}

const xAOD::JetContainer* TTHbbAsgToolHelper::getJetContainer(std::string truthjetname) const{

  const xAOD::JetContainer* truthJets(nullptr);
  top::check(evtStore()->retrieve(truthJets, truthjetname),"Failed to retrieve truthJets "+truthjetname); 

  return truthJets;

}

const xAOD::TruthParticleContainer* TTHbbAsgToolHelper::getTruthParticleContainer(std::string name) const{

  const xAOD::TruthParticleContainer* truth(nullptr);
  top::check(evtStore()->retrieve(truth, name),"Failed to retrieve truth particle container "+name); 

  return truth;

}
