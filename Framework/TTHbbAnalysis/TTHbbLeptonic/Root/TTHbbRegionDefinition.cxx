/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbRegionDefinition.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "PathResolver/PathResolver.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHbbRegionDefinition::TTHbbRegionDefinition(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config), m_params(params), m_dilepPCRegionTool(0), m_dilepFilePath("")
 {
   // Generate the full path with PathResolver
   m_dilepFilePath = PathResolverFindCalibFile("TTHbbLeptonic/DileptonRegions.root");

   //m_params=params;
   m_semilepOptions = {"signalRegions", "baseline", "optionA", "optionB", "optionC", "optionD", "optionE", "optionF", "optionG", "optionH"};

   

   m_dilepPCRegionTool = new TTHbb::DileptonSimplePCRegionTool(m_dilepFilePath.c_str());

   /// add config here
 
   auto* esh = EventSaverHelper::get();

   for(auto a : m_semilepOptions){
     esh->addVariableToBeSaved("SemilepRegions_"+a, TTHbb::DecorationType::Int,false, false,"TTHbbRegionDefinition"); 
   }
   for(auto reg : m_dilepPCRegionTool->getRegionList()){
     esh->addVariableToBeSaved("DilepRegion_" + reg, TTHbb::DecorationType::Int,false,false,"TTHbbRegionDefinition");
   }
   esh->addVariableToBeSaved("SemilepRegions_PCBdiscr", TTHbb::DecorationType::Int,false,false,"TTHbbRegionDefinition");


}

TTHbbRegionDefinition::~TTHbbRegionDefinition(){
  if(m_dilepPCRegionTool){
    delete m_dilepPCRegionTool;
    m_dilepPCRegionTool = 0;
  }
}

bool TTHbbRegionDefinition::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  if(tthevt->m_leptons.size()==1){
    decorateSemilepRegions(tthevt);
  }

  if(tthevt->m_leptons.size() == 2){
    for(auto reg : m_dilepPCRegionTool->getEventRegions(tthevt->m_jets)){
      tthevt->intVariable(reg.first) =  reg.second;
    }
  }

  return true;

}


void TTHbbRegionDefinition::decorateSemilepRegions(std::shared_ptr<TTHbb::Event> tthevt) const{

  int nbtag85 = tthevt->intVariable("nBTags_85");
  int nbtag77 = tthevt->intVariable("nBTags_77");
  int nbtag70 = tthevt->intVariable("nBTags_70");
  int nbtag60 = tthevt->intVariable("nBTags_60");

  int njets = tthevt->m_jets.size();

  int PCBdiscr = m_semilepPCRegionTool.SemilepPCRegionDiscri(nbtag60, nbtag70, nbtag77, nbtag85);
  tthevt->intVariable("SemilepRegions_PCBdiscr")=PCBdiscr;


  for(auto a : m_semilepOptions){

    int region = m_semilepPCRegionTool.SemilepPCRegionsNumber(njets, PCBdiscr, a);
    tthevt->intVariable("SemilepRegions_"+a)=region;

  }


}


std::string TTHbbRegionDefinition::name() const{

  return "TTHBBREGIONS";

}



