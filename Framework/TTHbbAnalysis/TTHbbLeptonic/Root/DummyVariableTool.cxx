/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/DummyVariableTool.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

DummyVariableTool::DummyVariableTool(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
   m_params=params;

   /// register the event decorations that you want to dump
 auto* esh = EventSaverHelper::get();
 esh->addVariableToBeSaved("ttHbb_myNJets", TTHbb::DecorationType::Int,false,"DummyVariableTool");
 esh->addVariableToBeSaved("ttHbb_theVariableWithNoName", TTHbb::DecorationType::Int,false,"DummyVariableTool");
 esh->addVariableToBeSaved("ttHbb_myNJets", TTHbb::DecorationType::Int,false,"DummyVariableTool");
}

DummyVariableTool::~DummyVariableTool(){

}

bool DummyVariableTool::apply(const top::Event & event) const{

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

   // std::cout << "  " << name() << " para " << m_params 
   // 	    << " syst " << m_config->systematicName(event.m_hashValue)
   // 	    << " toolptr " << this << " topptr " << &event
   // 	    << " tthptr " << tthevt.get() 
   // 	    << " truth " << tthevt->truthAvailable() 
   // 	    << " reco " << tthevt->recoAvailable() 
   // 	    << std::endl;

 // old style
 event.m_info->auxdecor<int>("nJets")= tthevt->m_jets.size();

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

std::string DummyVariableTool::name() const{

  return "DUMMY";

}



