/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTbarReweightingTool.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTbarReweightingTool::TTbarReweightingTool(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
  m_params=params;

  /// add config here
  m_fractionReweighting = 0;
  m_fractionReweighting_pp6 = 0;
  m_ttbbNormReweighting = 0;
  m_ttbbNormReweighting_pp8=0;
  m_ttbbNormReweighting_new=0;
  m_ttbbShapeReweighting = 0;
  m_ttbbShapeReweighting_new = 0;
  m_ttcReweighting=0;
  m_ttbarNNLOReweighter=0;

  //Needed for Norm re-weighting
  m_NormRWTargets = {"410051", "410052_SherpaCSSKIN", "410053_SherpaMSTW", "410054_SherpaNNPDF", "410055_SherpaQCMMPS", "410056_SherpaX05", "410057_SherpaX2", "410058_SherpaGlosoft", "410184_SherpaMPIup", "410185_SherpaMPIdown" };
  m_NormRWNames = {"", "_SherpaCSSKIN", "_SherpaMSTW", "_SherpaNNPDF", "_SherpaQCMMPS", "_SherpaX05", "_SherpaX2", "_SherpaGlosoft", "_SherpaMPIup", "_SherpaMPIdown"};

  //Needed for Shape re-weighting  
  m_shapeRWTargets = {"SherpaNominal", "SherpaCSSKIN", "SherpaMSTW", "SherpaNNPDF", "SherpaQCMMPS", "SherpaX05", "SherpaX2", "SherpaGlosoft", "SherpaMPIup", "SherpaMPIdown"};
  m_shapeRWVarNames = {"truth_top_pt", "truth_ttbar_pt", "HFClassification_q1_pt", "HFClassification_qq_dr", "HFClassification_qq_pt"};

  if(m_config->isMC()){
    auto* esh = EventSaverHelper::get();

    esh->addVariableToBeSaved("weight_ttbar_FracRw", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    esh->addVariableToBeSaved("ttbb_MPIfactor_weight", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    esh->addVariableToBeSaved("ttcc_fraction_weight", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    esh->addVariableToBeSaved("weight_ttbb_Norm_pp8", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    esh->addVariableToBeSaved("NNLO_weight_toppt", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    esh->addVariableToBeSaved("ttbb4F_xs_weight", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 

    esh->addVariableToBeSaved("weight_ttbb_Norm_new", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    esh->addVariableToBeSaved("weight_ttbb_Shape_SherpaNewNominal", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 


    for( unsigned int iTarget = 0; iTarget < m_NormRWTargets.size(); ++iTarget){
      std::string thisVarToSave = "weight_ttbb_Norm"+m_NormRWNames.at(iTarget);
      esh->addVariableToBeSaved(thisVarToSave, TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    }  
    //  esh->addVariableToBeSaved("weight_ttbar_FracRw_pp6", TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 

    for( unsigned int iTarget = 0; iTarget < m_shapeRWTargets.size(); ++iTarget){
      std::string thisVarToSave = "weight_ttbb_Shape_"+m_shapeRWTargets.at(iTarget);
      esh->addVariableToBeSaved(thisVarToSave, TTHbb::DecorationType::Float,false, true ,"TTbarReweightingTool"); 
    }

  }

}

TTbarReweightingTool::~TTbarReweightingTool(){

}

bool TTbarReweightingTool::apply(const top::Event & event) const{

  if(!m_config->isMC()) return true;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  decorateFractionWeight(tthevt);
  decorateTtbbNormWeight(tthevt);
  decorateTtbbNormWeight_new(tthevt);
  decorateTtbbShapeWeight(tthevt);
  decorateTtbbShapeWeight_new(tthevt);
  decorateTtbbMPIWeight(tthevt);
  decorateTtccWeight(tthevt);
  decorateTtbbNormWeight_pp8(tthevt);
  decorateNNLOWeight(tthevt);
  decorateSherpaOLWeight(tthevt);

  return true;

}

bool TTbarReweightingTool::applyParticleLevel(const top::ParticleLevelEvent & plEvent) const{


  if(!m_config->isMC()) return true;

  if(!plEvent.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params  << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = plEvent.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  decorateFractionWeight(tthevt);
  decorateTtbbNormWeight(tthevt);
  decorateTtbbNormWeight_new(tthevt);
  decorateTtbbShapeWeight(tthevt);
  decorateTtbbShapeWeight_new(tthevt);
  decorateTtbbMPIWeight(tthevt);
  decorateTtccWeight(tthevt);
  decorateTtbbNormWeight_pp8(tthevt);
  decorateNNLOWeight(tthevt);
  decorateSherpaOLWeight(tthevt);

  return true;

}

void TTbarReweightingTool::decorateFractionWeight(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int HF_SimpleClassification = tthevt->intVariable("HF_SimpleClassification");
 
  if(!m_fractionReweighting)m_fractionReweighting = new FractionReweighting(dsid);
  //if(!m_fractionReweighting_pp6)m_fractionReweighting_pp6 = new FractionReweighting(dsid, 410000);

  tthevt->floatVariable("weight_ttbar_FracRw") = m_fractionReweighting->applyWeight(HF_SimpleClassification);
  ///tthevt->floatVariable("weight_ttbar_FracRw_pp6") = m_fractionReweighting_pp6->applyWeight(HF_SimpleClassification);
  

}


void TTbarReweightingTool::decorateTtbbNormWeight(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int HF_Classification = tthevt->intVariable("HF_Classification");

  if(!m_ttbbNormReweighting){
    m_ttbbNormReweighting = new ttbbNormReweighting(dsid, m_NormRWTargets);
  }

  std::vector<float> weights_norm = m_ttbbNormReweighting->applyWeight(HF_Classification);

  for( unsigned int iTarget = 0; iTarget < weights_norm.size(); ++iTarget){
    std::string thisVarToSave = "weight_ttbb_Norm"+m_NormRWNames.at(iTarget);
    tthevt->floatVariable(thisVarToSave) = weights_norm.at(iTarget);
  }

}

void TTbarReweightingTool::decorateTtbbNormWeight_pp8(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int HF_Classification = tthevt->intVariable("HF_Classification");

  if(!m_ttbbNormReweighting_pp8){
    m_ttbbNormReweighting_pp8 = new ttbbNormReweighting(dsid, {"410501"});
  }
  std::vector<float> weights_norm = m_ttbbNormReweighting_pp8->applyWeight(HF_Classification);
  tthevt->floatVariable("weight_ttbb_Norm_pp8")=weights_norm[0];

}

void TTbarReweightingTool::decorateTtbbNormWeight_new(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int HF_Classification = tthevt->intVariable("HF_Classification");

  if(!m_ttbbNormReweighting_new){
    m_ttbbNormReweighting_new = new ttbbNormReweighting(dsid, {"410323"});
  }
  std::vector<float> weights_norm = m_ttbbNormReweighting_new->applyWeight(HF_Classification);
  tthevt->floatVariable("weight_ttbb_Norm_new")=weights_norm[0];

}


void TTbarReweightingTool::decorateTtbbShapeWeight(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int HF_Classification = tthevt->intVariable("HF_Classification");

  if(!m_ttbbShapeReweighting){ 
    m_ttbbShapeReweighting = new ttbbShapeReweighting(dsid, m_shapeRWTargets, m_shapeRWVarNames);
  }

  std::vector<float> shapeRWVars; 
  for( unsigned int iVar = 0; iVar < m_shapeRWVarNames.size(); ++iVar){
    shapeRWVars.push_back( tthevt->floatVariable(m_shapeRWVarNames.at(iVar)) );
  }
  
  std::vector<float> weights_shape = m_ttbbShapeReweighting->applyWeight(HF_Classification, shapeRWVars);
  for( unsigned int iTarget = 0; iTarget < weights_shape.size(); ++iTarget){
    tthevt->floatVariable( ("weight_ttbb_Shape_"+m_shapeRWTargets.at(iTarget)).c_str() ) = weights_shape.at(iTarget);
  }

}


void TTbarReweightingTool::decorateTtbbShapeWeight_new(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int HF_Classification = tthevt->intVariable("HF_Classification");

  if(!m_ttbbShapeReweighting_new){ 
    m_ttbbShapeReweighting_new = new ttbbShapeReweighting(dsid, {"SherpaNew"}, m_shapeRWVarNames);
  }

  std::vector<float> shapeRWVars; 
  for( unsigned int iVar = 0; iVar < m_shapeRWVarNames.size(); ++iVar){
    shapeRWVars.push_back( tthevt->floatVariable(m_shapeRWVarNames.at(iVar)) );
  }
  
  std::vector<float> weights_shape = m_ttbbShapeReweighting_new->applyWeight(HF_Classification, shapeRWVars);
  tthevt->floatVariable( "weight_ttbb_Shape_SherpaNewNominal" ) = weights_shape[0];
  

}

void TTbarReweightingTool::decorateTtbbMPIWeight(std::shared_ptr<TTHbb::Event> tthevt) const{


  float ttbbmpi=1.;
  if(-1000==tthevt->intVariable("HF_Classification")){
    ttbbmpi = 1.5;
  }

  tthevt->floatVariable("ttbb_MPIfactor_weight")=ttbbmpi;

}

void TTbarReweightingTool::decorateTtccWeight(std::shared_ptr<TTHbb::Event> tthevt) const{

  int ttcdsid = tthevt->m_info->mcChannelNumber;
  if(ttcdsid == 410502) ttcdsid=410501;
  if(ttcdsid == 410503) ttcdsid=410501;
  if(ttcdsid == 410504) ttcdsid=410501;
  if(ttcdsid == 410505) ttcdsid=410501;
  if(ttcdsid == 410506) ttcdsid=410501;
  if(ttcdsid == 410507) ttcdsid=410501;
  if(ttcdsid == 410508) ttcdsid=410501;
  if(ttcdsid == 410509) ttcdsid=410501;
  if(ttcdsid == 410531) ttcdsid=410501;
  if(ttcdsid == 410532) ttcdsid=410501;
  if(ttcdsid == 410533) ttcdsid=410501;

  if(!m_ttcReweighting){
    m_ttcReweighting=new ttcReweighting(ttcdsid);
  }

  float ttcweight = m_ttcReweighting->applyWeight(tthevt->intVariable("HF_Classification"), 
						  tthevt->floatVariable("truth_top_pt"), 
						  tthevt->floatVariable("truth_ttbar_pt"), 
						  tthevt->floatVariable("HFClassification_q1_pt"), 
						  tthevt->floatVariable("HFClassification_qq_dr"), 
						  tthevt->floatVariable("HFClassification_qq_pt"));


  tthevt->floatVariable("ttcc_fraction_weight")=ttcweight;

}

void TTbarReweightingTool::decorateNNLOWeight(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  float nnlo_topptweight=1.;

  if(doNNLOReweighting(dsid)){
    if(!m_ttbarNNLOReweighter){
      m_ttbarNNLOReweighter=new NNLOReweighter(getNNLODSID(dsid));
      m_ttbarNNLOReweighter->Init();
    }

    nnlo_topptweight=m_ttbarNNLOReweighter->GetExtendedTopPtWeight(tthevt->floatVariable("truth_top_pt"));

  }

  tthevt->floatVariable("NNLO_weight_toppt")=nnlo_topptweight;

}


bool TTbarReweightingTool::doNNLOReweighting(int dsid) const{

 if( (dsid>=410000 && dsid<=410004) 
     || dsid==410009 
     || dsid==410120 
     || dsid==410121 
     || dsid==410500
     || dsid==410159) return true;

 if(dsid>=410501 && dsid<=410510) return true;
 if(dsid>=410531 && dsid<=410533) return true;
 if(dsid>=410511 && dsid<=410524) return true;
 if(dsid>=410568 && dsid<=410569) return true;

 if(dsid>=410525 && dsid<=410529) return true;
 if(dsid>=410225 && dsid<=410227) return true;
 if(dsid>=410274 && dsid<=410275) return true;
 if(dsid>=410249 && dsid<=410252) return true;
 if(dsid>=410281 && dsid<=410283) return true;


 return false;

}

int TTbarReweightingTool::getNNLODSID(int dsid) const{

  if (dsid>=410501 && dsid<=410510) return 410501;
  if (dsid>=410531 && dsid<=410533) return 410501;
  if (dsid>=410525 && dsid<=410529) return 410525;
  if (dsid>=410225 && dsid<=410227) return 410225;
  if (dsid>=410274 && dsid<=410275) return 410225;
  if (dsid>=410249 && dsid<=410252) return 410250;
  if (dsid>=410281 && dsid<=410283) return 410250;

  if(dsid>=410511 && dsid<=410524 && dsid%2==1) return 410511;
  if(dsid>=410511 && dsid<=410524 && dsid%2==0) return 410512;
  if(410568==dsid) return 410511;
  if(410569==dsid) return 410512;

  return dsid;

}

void TTbarReweightingTool::decorateSherpaOLWeight(std::shared_ptr<TTHbb::Event> tthevt) const{

  int dsid = tthevt->m_info->mcChannelNumber;
  int ttbb_categories = tthevt->intVariable("ttbb_categories");

  float ttbb4F_xs_weight=1.;
  
  if(dsid >= 410323 && dsid <= 410325){

    if(ttbb_categories>=1 && ttbb_categories<=4){
      ttbb4F_xs_weight=1.2854;
    }
   
  }
  
  tthevt->floatVariable("ttbb4F_xs_weight")=ttbb4F_xs_weight;

}



std::string TTbarReweightingTool::name() const{

  return "TTBARREWEIGHTING";

}



