/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TMVAClassificationInterface.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TTHbbLeptonic/EventSaverHelper.h"

#include "PathResolver/PathResolver.h"

TMVAClassificationInterface::TMVAClassificationInterface(){

  m_xmlPath = PathResolverFindCalibDirectory("TTHbbLeptonic");

  m_configMap["Strategy"]="ICHEP";
  m_configMap["BTag"]="70";
  m_configMap["TRF"]="";
  m_configMap["Region"]="";
  m_configMap["Flag"]="TTHReco";
  m_configMap["XMLFlag"]="xmlNoSet";
  m_configMap["VarOption"]="";

}

TMVAClassificationInterface::~TMVAClassificationInterface(){
}


void TMVAClassificationInterface::initTool(){

  std::vector<std::string> options = TTHbb::util::vectoriseString(m_params," ");

  //std::cout << "RECO config" << std::endl;
  for(auto ss : options){

    std::vector<std::string> value = TTHbb::util::vectoriseString(ss,":");
    if(value.size() != 2){
      std::cout << "ERROR TMVAClassificationInterface::initTool problem with option format " << ss << std::endl;
      abort();
    }
    std::unordered_map<std::string,std::string>::iterator iter = m_configMap.find(value[0]);
    if(iter == m_configMap.end()){
      std::cout << "ERROR TMVAClassificationInterface::initTool unkown option " << value[0] << std::endl;
      abort();
    }
    iter->second = value[1];
    //std::cout << value[0] <<"=" <<value[1] << std::endl;
  }

  /// faster to set main options to variables
  m_btagOP=m_configMap["BTag"];
  m_trf=m_configMap["TRF"];
  if(m_trf != "") m_trf="_"+m_trf;
  m_flag=m_configMap["Flag"];
  m_strategy=m_configMap["Strategy"];
  m_region=m_configMap["Region"];
  m_xmlFlag = m_configMap["XMLFlag"];
  m_varOption = m_configMap["VarOption"];
 

}


void TMVAClassificationInterface::registerSaverVariables(){

  auto* esh = EventSaverHelper::get();
  esh->addVariableToBeSaved(m_flag, TTHbb::DecorationType::Float, false, "TMVAClassificationInterface"+m_flag);

}


void TMVAClassificationInterface::initReaders(std::string xml){

  EventSampling OddSampling;
  EventSampling EvenSampling;

  OddSampling.modulo=2;
  OddSampling.rest.push_back(1);
  OddSampling.patternName="modulo_2_rest_0"; /// the training is Even

  EvenSampling.modulo=2;
  EvenSampling.rest.push_back(0);
  EvenSampling.patternName="modulo_2_rest_1";/// the training is Odd

  std::vector<std::string> classVar;
  getBDTVars(classVar);

  TMVAConfiguration classConfig;
  classConfig.samplings.push_back(OddSampling);
  classConfig.samplings.push_back(EvenSampling);
  classConfig.name=m_flag;
  classConfig.setCalibFilePattern(xml);
  classConfig.setVariables(classVar);
  classConfig.bookReader();

  RegionInterface::RegionDefinition region;
  //// can add region cuts into RegionDefinition
  region.tmvaConfiguration=classConfig;

  m_classBDTCalculatorTool.addRegion(region);

 
}


void TMVAClassificationInterface::defaultValues(TTHbb::Event* tthevt) const{

  if(!tthevt->checkFloatVariable(m_flag)){
    tthevt->floatVariable(m_flag)=-9;
  }

}
