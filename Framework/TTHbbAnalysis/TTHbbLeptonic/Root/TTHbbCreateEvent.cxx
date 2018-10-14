/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbCreateEvent.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

#include "TTHbbObjects/TTHbbUtils.h"
#include "TopEvent/EventTools.h"


TTHbbCreateEvent::TTHbbCreateEvent(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
   m_params=params;
}

TTHbbCreateEvent::~TTHbbCreateEvent(){

}

bool TTHbbCreateEvent::apply(const top::Event & event) const{

  std::shared_ptr<TTHbb::Event> tthevt;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    tthevt = std::make_shared<TTHbb::Event>();
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables") = tthevt;

    tthevt->m_info = std::make_shared<TTHbb::EventData>();
    tthevt->m_info->eventNumber=event.m_info->eventNumber();
    if(m_config->isMC()){
      tthevt->m_info->mcChannelNumber=event.m_info->mcChannelNumber();
    }
    else{
      tthevt->m_info->mcChannelNumber=0;
    }
    tthevt->m_info->runNumber=event.m_info->runNumber();
    tthevt->m_info->isData= !m_config->isMC();

  }
  else{
    tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
  }

  /// nothing to be done for now here  
  /// will be cleaned in EventSaver

  return true;

}



bool TTHbbCreateEvent::applyParticleLevel(const top::ParticleLevelEvent & plEvent) const{

  std::shared_ptr<TTHbb::Event> tthevt;

  if(!plEvent.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    tthevt = std::make_shared<TTHbb::Event>();
    plEvent.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables") = tthevt;

    tthevt->m_info = std::make_shared<TTHbb::EventData>();
    tthevt->m_info->eventNumber=plEvent.m_info->eventNumber();
    tthevt->m_info->mcChannelNumber=plEvent.m_info->mcChannelNumber();
    tthevt->m_info->runNumber=plEvent.m_info->runNumber();
    tthevt->m_info->isData= !m_config->isMC();

  }
  else{
    tthevt = plEvent.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
  }

  /// nothing to be done for now here  
  /// will be cleaned in EventSaver

  return true;

}



std::string TTHbbCreateEvent::name() const{
  return "TTHBBCREATEEVENT";
}



