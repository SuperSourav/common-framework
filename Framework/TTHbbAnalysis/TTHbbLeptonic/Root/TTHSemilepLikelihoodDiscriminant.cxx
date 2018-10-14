/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHSemilepLikelihoodDiscriminant.h"

//#include "TTHbbReconstruction/LikelihoodDiscriminantTool.h"
//
#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "MVAVariables/JetOrderingTool.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHSemilepLikelihoodDiscriminant::TTHSemilepLikelihoodDiscriminant(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
   m_params=params;
    
   //m_configMap["BTag"]="70";
   //m_configMap["TRF"]="";
   m_btagOP="85";
   m_trf="";
   if(m_trf != "") m_trf="_"+m_trf;

   
 auto* esh = EventSaverHelper::get();
 esh->addVariableToBeSaved("LHD_log10ProbSig_allmatched", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");
 esh->addVariableToBeSaved("LHD_log10ProbSig_missingparton", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");
 esh->addVariableToBeSaved("LHD_log10ProbSig", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");

 esh->addVariableToBeSaved("LHD_log10ProbBkg_allmatched", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");
 esh->addVariableToBeSaved("LHD_log10ProbBkg_missingparton", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");
 esh->addVariableToBeSaved("LHD_log10ProbBkg", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");

 esh->addVariableToBeSaved("LHD_Discriminant_allmatched", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");
 esh->addVariableToBeSaved("LHD_Discriminant_missingparton", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");
 esh->addVariableToBeSaved("LHD_Discriminant", TTHbb::DecorationType::Float,false,"TTHSemilepLikelihoodDiscriminant");

  m_discriminant = new LikelihoodDiscriminantTool();
   m_discriminant->Init();

 
 }

TTHSemilepLikelihoodDiscriminant::~TTHSemilepLikelihoodDiscriminant(){

}

bool TTHSemilepLikelihoodDiscriminant::apply(const top::Event & event) const{

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

  // m_btagOP=m_configMap["BTag"];
  // m_trf=m_configMap["TRF"];
  // if(m_trf != "") m_trf="_"+m_trf;

  
  TTHbb::JetContainer* jets = (&tthevt->m_jets);
  TTHbb::LeptonContainer* leptons = (&tthevt->m_leptons);

  // float met_met = tthevt->met_met;
  // float met_phi = tthevt->met_phi;
  

  int nJets=jets->size();
  int nbJets=tthevt->intVariable("nBTags_"+m_btagOP+m_trf);
  int nbJets_77=tthevt->intVariable("nBTags_77"+m_trf);
  int nbJets_60=tthevt->intVariable("nBTags_60"+m_trf);



  if(leptons->size() != 1){
    return true;
  }
  // if(m_nJets<5){ ///
  //   return true;
  // }

 JetOrderingTool::sortPt(*jets);
  
  m_discriminant->Clear();

  if(nJets>=5){
    if(nbJets>=4){
      m_discriminant->Run(nJets, nbJets, jets, leptons, tthevt->met_met, tthevt->met_phi);
    }
    else if(nbJets_77==3 && nJets>=6){
      m_discriminant->Run(nJets, 3, jets, leptons, tthevt->met_met, tthevt->met_phi);
    }
    else if(nbJets_60==3 && nJets==5){
      m_discriminant->Run(nJets, 4, jets, leptons, tthevt->met_met, tthevt->met_phi);
    }
  }

  tthevt->floatVariable("LHD_log10ProbSig_allmatched") = m_discriminant->m_log10ProbSig_6matched;
  tthevt->floatVariable("LHD_log10ProbSig_missingparton") = m_discriminant->m_log10ProbSig_5matched;
  tthevt->floatVariable("LHD_log10ProbSig") = m_discriminant->m_log10ProbSig_merged;
  
  tthevt->floatVariable("LHD_log10ProbBkg_allmatched") = m_discriminant->m_log10ProbBkg_6matched;
  tthevt->floatVariable("LHD_log10ProbBkg_missingparton") = m_discriminant->m_log10ProbBkg_5matched;
  tthevt->floatVariable("LHD_log10ProbBkg") = m_discriminant->m_log10ProbBkg_merged;
 
  tthevt->floatVariable("LHD_Discriminant_allmatched") = m_discriminant->m_Discriminant_6matched;
  tthevt->floatVariable("LHD_Discriminant_missingparton") = m_discriminant->m_Discriminant_5matched;
  tthevt->floatVariable("LHD_Discriminant") = m_discriminant->m_Discriminant_merged;
  
  return true;

}

std::string TTHSemilepLikelihoodDiscriminant::name() const{

  return "TTHSEMILEPLHD";

}



