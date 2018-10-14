/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/NeutrinoReconstruction.h"

#include "TTHbbObjects/Event.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

NeutrinoReconstruction::NeutrinoReconstruction(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
   m_params=params;
   m_neuRecoTool=new TTHbb::NeutrinoReconstructionTool(); /// default for now
}

NeutrinoReconstruction::~NeutrinoReconstruction(){
  delete m_neuRecoTool;
}

bool NeutrinoReconstruction::apply(const top::Event & event) const{

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

 TTHbb::LeptonContainer* sleptons = &(tthevt->m_leptons);
 
  if( sleptons->size() != 1){
    std::cout << "WARNING: NeutrinoReconstruction lepton size != 1 == " << sleptons->size() << std::endl;
    return true;
  }

  TTHbb::NeutrinoContainer *neutrinos = m_neuRecoTool->compute(tthevt->met_met, tthevt->met_phi, sleptons);

  tthevt->m_neutrinos = *neutrinos;

  delete neutrinos; // yes I know this is stupid but this what happens when we mix pointers and references
  

  return true;

}

std::string NeutrinoReconstruction::name() const{

  return "NEUTRINO";

}



