/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/DummyVariableToolTruth.h"

#include "TTHbbObjects/Event.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

DummyVariableToolTruth::DummyVariableToolTruth(std::string params,std::shared_ptr<top::TopConfig> config):
  m_plEvent(0),
  m_config(config)
 {
 
   m_params=params;
}

DummyVariableToolTruth::~DummyVariableToolTruth(){

}

bool DummyVariableToolTruth::applyParticleLevel(const top::ParticleLevelEvent & plEvent) const{

  m_plEvent = &plEvent;

  if(!plEvent.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params <<std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = plEvent.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

 /// compute some variables
 /// can have naming comvention to easilly add/remove variables from dumper
 /// like ttHbb_blockname_*

 tthevt->intVariable("ttHbb_myNJets") = tthevt->m_jets.size();
 
 /// not always created
 if(tthevt->m_jets.size()>=6){
   tthevt->intVariable("ttHbb_nPotatoes") = tthevt->m_jets.size();
 }

 tthevt->intVariable("ttHbb_theVariableWithNoName") = 666;

 for(auto jet : tthevt->m_jets){

   jet->floatVariable("theTomatos") = jet->p4().Pt();

 }

  return true;

}

std::string DummyVariableToolTruth::name() const{

  return "DUMMY_TRUTH";

}



