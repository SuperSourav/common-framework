/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

# include "TTHbbLeptonic/MCFakes.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TTHbbLeptonic/EventSaverHelper.h"

MCFakes::MCFakes(std::string params, std::shared_ptr<top::TopConfig> config) :
  m_config(config) {

  m_params=params;

  // m_fakesCalculator.reset( new TTHbb::MCFakesCalculator(stof(weightVec.at(0)), stof(weightVec.at(1))) );

  auto* esh = EventSaverHelper::get();
  esh->addVariableToBeSaved("MCFakes_hasFakeLepton", TTHbb::DecorationType::Int,false,"MCFakes");
}

MCFakes::~MCFakes() {
  
}

bool MCFakes::apply(const top::Event & event) const{
  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
  
  TTHbb::MCFakesAppender::appendEventVars(tthevt.get());

  return true;
}

std::string MCFakes::name() const{
  return "MCFAKES";
}
