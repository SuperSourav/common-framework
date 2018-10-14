/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/BoostedSelection.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

BoostedSelection::BoostedSelection(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
   std::cout <<name() <<": BoostedSelection::BoostedSelection" << std::endl;
   // std::cout <<name() <<": params: " << params << std::endl;

   m_params=params;

     auto* esh = EventSaverHelper::get();

     esh->addVariableToBeSaved("boosted_passes3211", TTHbb::DecorationType::Int, false, true ,"BoostedSelection"); 
     esh->addVariableToBeSaved("boosted_passes3311", TTHbb::DecorationType::Int, false, true ,"BoostedSelection"); 
     esh->addVariableToBeSaved("boosted_passes3311_1b1nb", TTHbb::DecorationType::Int, false, true ,"BoostedSelection"); 
     esh->addVariableToBeSaved("boosted_passes1T1H", TTHbb::DecorationType::Int, false, true ,"BoostedSelection"); 
     esh->addVariableToBeSaved("boosted_passesRc1T1H", TTHbb::DecorationType::Int, false, true ,"BoostedSelection"); 
     esh->addVariableToBeSaved("boosted_passesAny", TTHbb::DecorationType::Int, false, true ,"BoostedSelection"); 


     esh->addVariableToBeSaved("btag_sum_all", TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("dR_HT",        TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("btag_sum_out_TH", TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("btag_sum_out_TH_over_all", TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("Top_d12",      TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("dRbH_out_TH",  TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("dRbT_out_TH",  TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("dR_lepH",      TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     esh->addVariableToBeSaved("rc_higgs_m",   TTHbb::DecorationType::Float, false, true ,"BoostedSelection");
     


   m_boostedVariables.initialise();
}

BoostedSelection::~BoostedSelection(){
  m_boostedVariables.finalise();
}

bool BoostedSelection::apply(const top::Event & event) const{

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  if (tthevt->charVariable("BoostedVariablesFilled") == 0) {
    // only run this once
    // TTHbb::BoostedVariables boosted;
    const_cast<TTHbb::BoostedVariables*>(&m_boostedVariables)->apply(tthevt.get());
    tthevt->charVariable("BoostedVariablesFilled") = 1;
  }

  // bool passes3211 = tthevt->charVariable("3211_passesAll");
  // return passes3211;
  return true;
}


std::string BoostedSelection::name() const{

  return "BOOSTEDSELECTION";

}
