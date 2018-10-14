/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include "TTHbbLeptonic/TriggerPrescales.h"
#include <cstdlib>
#include "TMVA/Types.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"

//______________________________________________________________________________
TriggerPrescales::TriggerPrescales(std::string params,std::shared_ptr<top::TopConfig> config):
  m_event(0),
  m_config(config),
  m_trigDecisionTool("Trig::TrigDecisionTool")
{
  m_params=params;
  m_debug = false;

  /// register the event decorations that you want to dump
 auto* esh = EventSaverHelper::get();
 //                           name,                              type,                          nominalOnly, origin
 esh->addVariableToBeSaved("PS_HLT_e24_lhmedium_nod0_L1EM18VH", TTHbb::DecorationType::Float  , false,       "TriggerPrescales");
 esh->addVariableToBeSaved("PS_HLT_mu24"                      , TTHbb::DecorationType::Float  , false,       "TriggerPrescales");
}


//______________________________________________________________________________
TriggerPrescales::~TriggerPrescales(){
}


//______________________________________________________________________________
bool TriggerPrescales::apply(const top::Event & event) const{

  m_event = &event;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout << "DummyVariableTool: TTHbbEventVariables (TTHbb::Event*) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params
      << "> <systname: " << m_config->systematicName( event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
  }

  if (!m_config->isMC()) {
    this->DecorateTriggerPrescales();
  }

  return true;
}


//______________________________________________________________________________
std::string TriggerPrescales::name() const{
  return "TRIGGERPRESCALES";
}


//______________________________________________________________________________
void TriggerPrescales::DecorateTriggerPrescales() const
{
  //--- Get the TTHbb event pointer
  std::shared_ptr<TTHbb::Event> tthevt = m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  // adding trigger prescale branches on data
  tthevt->floatVariable("PS_HLT_e24_lhmedium_nod0_L1EM18VH") = m_trigDecisionTool->getChainGroup("HLT_e24_lhmedium_nod0_L1EM18VH")->getPrescale();
  tthevt->floatVariable("PS_HLT_mu24") = m_trigDecisionTool->getChainGroup("HLT_mu24")->getPrescale();
}
