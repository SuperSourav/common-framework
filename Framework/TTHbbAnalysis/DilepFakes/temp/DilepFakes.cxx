/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

# include "TTHbbLeptonic/DilepFakes.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TTHbbLeptonic/EventSaverHelper.h"

DilepFakes::DilepFakes(std::string params, std::shared_ptr<top::TopConfig> config) :
  m_config(config) {

  m_params=params;
  std::vector<std::string> weightVec = TTHbb::util::vectoriseString(params," ");

  m_fakesCalculator.reset( new TTHbb::DilepFakesCalculator(stof(weightVec.at(0)), stof(weightVec.at(1))) );

  auto* esh = EventSaverHelper::get();
  esh->addVariableToBeSaved("isSubleadElChan", TTHbb::DecorationType::Int,false,"DilepFakes");
  esh->addVariableToBeSaved("isSubleadMuChan", TTHbb::DecorationType::Int,false,"DilepFakes");
  esh->addVariableToBeSaved("weight_dilepFakesNorm", TTHbb::DecorationType::Float,false,"DilepFakes");
}

DilepFakes::~DilepFakes() {
  
}

bool DilepFakes::apply(const top::Event & event) const{
  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
  
  TTHbb::DilepFakesAppender::appendVars(tthevt.get(), m_fakesCalculator);

  return true;
}

std::string DilepFakes::name() const{
  return "DILEPFAKES";
}
