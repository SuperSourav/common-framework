/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHBoostedBDT.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHBoostedBDT::TTHBoostedBDT(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
{
 
  m_params=params;


  m_nJets=0;
  m_nbJets=0;
  m_leptonType=0;

  m_xmlPath+="/BDTweights_boosted/";

  /// keep this before init tool
  m_configMap["RecoFlag"]="";
  initTool();
  //// keep this after init tool
  m_recoFlag=m_configMap["RecoFlag"];

  // TMVAClassification_BDT_modulo_2_rest_0.weights.xml
  std::string xml = m_xmlPath+"/TMVAClassification_BDT"+m_xmlFlag;
  
  initReaders(xml);
  registerSaverVariables();
   
}

TTHBoostedBDT::~TTHBoostedBDT(){
}


bool TTHBoostedBDT::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");


  TTHbb::JetContainer& jets = tthevt->m_jets;
  //  TTHbb::LeptonContainer& leptons = tthevt->m_leptons;
  // TTHbb::NeutrinoContainer& neutrinos = tthevt->m_neutrinos;

  m_nJets=jets.size();
  m_nbJets=tthevt->intVariable("nBTags_"+m_btagOP+m_trf);
  m_leptonType=tthevt->intVariable("leptonType");

  
  defaultValues(tthevt.get());
 
  if(!passRegion(tthevt.get())) return true;

  m_classBDTCalculatorTool.computeBDT(tthevt.get(), m_nJets, m_nbJets, m_leptonType, tthevt->m_info->eventNumber);

  return true;

}



/// can change and read from text file
void TTHBoostedBDT::getBDTVars(std::vector<std::string> &BDTVar){

  BDTVar.push_back("rc_higgs_m");  // in xml: rename Higgs_m to higgs_m
  BDTVar.push_back("btag_sum_out_TH_over_all"); // same name
  BDTVar.push_back("dRbH_out_TH"); // same name
  BDTVar.push_back("Top_d12");  // same name
  BDTVar.push_back("dRbT_out_TH"); // same name
  BDTVar.push_back("dR_HT"); // same name
  BDTVar.push_back("btag_sum_all");
  BDTVar.push_back("dR_lepH"); // same name



  /*    BDTVar.push_back("H_d12");
    BDTVar.push_back("dRbb_in_H");
    BDTVar.push_back("dRbH_out_TH");
    BDTVar.push_back("dR_lepH"); // dR_LepH
    BDTVar.push_back("Top_d12");
    BDTVar.push_back("dRbT_out_TH");
    BDTVar.push_back("dR_HT");
    BDTVar.push_back("rcjet0_eta");
    BDTVar.push_back("btag_sum_all");
    BDTVar.push_back("dRbb_MaxPt_recal"); //dRbb_Max_Pt_recal
    BDTVar.push_back("btag_sum_out_TH_over_all"); 
  */

    

}


bool TTHBoostedBDT::passRegion(TTHbb::Event* eventvar) const{


  if("RCJETS" == m_strategy){
    if(!eventvar->intVariable("boosted_passesRc1T1H")){
      return false;
    }
  }

  return true;

}

std::string TTHBoostedBDT::name() const{

  return "TTHBOOSTEDBDT";

}
