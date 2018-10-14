/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHDilepClassBDT.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHDilepClassBDT::TTHDilepClassBDT(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
{
 
  m_params=params;
  

  m_nJets=0;
  m_nbJets=0;
  m_leptonType=0;
  m_regionName = "";
  m_regionVal = -1;

  m_xmlPath+="/BDTweights_dilep/classification";

  /// keep this before init tool
  m_configMap["RecoFlag"]="";
  initTool();
  //// keep this after init tool
  m_recoFlag=m_configMap["RecoFlag"];

  if(m_region.find("=")!=std::string::npos){
    TTHbb::util::decodeRegionName(m_region, m_regionName, m_regionVal);
  }

  // m_njets_cuts=-1;
  // m_nmaxjets_cuts=-1;
  // m_nbjets_cuts=-1;
  // m_nmaxbjets_cuts=-1;
  // TTHbb::util::decodeRegionName(m_region, m_njets_cuts, m_nmaxjets_cuts, m_nbjets_cuts, m_nmaxbjets_cuts);

  std::string xml = m_xmlPath+"/TMVA_Classification_";// + m_strategy;// + "_withReco_" + m_bTagOP;

  xml += (m_strategy);
  if(m_regionName == "DilepRegion_A3"  && m_strategy.find("A3") != std::string::npos){
    if(m_regionVal == 1){
      xml += "SR1";
    }
    else if(m_regionVal == 3){
      xml += "SR2";
    }
    else if(m_regionVal == 2){
      xml += "SR3";
    }
  }

  if(m_recoFlag != "") xml += "_withReco";

  if(m_strategy.find("A3") == std::string::npos) xml += ("_" + m_btagOP);

  initReaders(xml);
  registerSaverVariables();
   
}

TTHDilepClassBDT::~TTHDilepClassBDT(){
}


bool TTHDilepClassBDT::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");


  TTHbb::JetContainer& jets = tthevt->m_jets;
  TTHbb::LeptonContainer& leptons = tthevt->m_leptons;
  // TTHbb::NeutrinoContainer& neutrinos = tthevt->m_neutrinos;

  m_nJets=jets.size();
  m_nbJets=tthevt->intVariable("nBTags_"+m_btagOP+m_trf);
  m_leptonType=tthevt->intVariable("leptonType");


  defaultValues(tthevt.get());
 
  if(leptons.size() != 2){
    return true;
  }
  if(m_nJets<4){ /// will never do it in 4j
    return true;
  }

  if(!passRegion(tthevt.get())) return true;

  m_classBDTCalculatorTool.computeBDT(tthevt.get(), m_nJets, m_nbJets, m_leptonType, tthevt->m_info->eventNumber);

  return true;

}

void TTHDilepClassBDT::getBDTVars(std::vector<std::string> &BDTVar){

  BDTVar.clear();

  //std::string recos = m_strategy;
  //std::transform(recos.begin(), recos.end(), recos.begin(), ::tolower);
  //bool addreco = recos.find("Reco") != std::string::npos;
  bool addreco = (m_recoFlag != "");
  //bool b4 = recos.find("Sort4") != std::string::npos;
  //bool btagonly = recos.find("btagonly") != std::string::npos;
  std::string btagopVar=m_btagOP;
  //if(b4) btagopVar="Sort4";

  // if(btagonly){
  //   getBDTVarsBTag(BDTVar);
  //   return;
  // }

  if(m_strategy.find("A3") == std::string::npos){
    getBDTVarsKin(BDTVar, btagopVar);
    if(addreco){
      getBDTVarsReco(BDTVar);
    }
  }
  else{
    getBDTVarsPCBDT(BDTVar);
  }

}

/// can change and read from text file
void TTHDilepClassBDT::getBDTVarsKin(std::vector<std::string> &BDTVar, std::string& btagopVar){

  if( "twoAt77" == m_strategy || "fourAt85" == m_strategy){
    BDTVar.push_back("dEtabb_Avg_" + btagopVar);
    BDTVar.push_back("dRbb_MaxPt_" + btagopVar);
    BDTVar.push_back("Mbb_HiggsMass_" + btagopVar);
    BDTVar.push_back("dRbb_HiggsMass_" + btagopVar);
    BDTVar.push_back("nHiggsbb30_" + btagopVar + "/I");
    BDTVar.push_back("dEtajl_MaxdEta");
    BDTVar.push_back("H0_all");
    BDTVar.push_back("Mbb_MinM_" + btagopVar);
    BDTVar.push_back("Aplanarity_jets");
    BDTVar.push_back("Centrality_all");
    BDTVar.push_back("Mbb_MindR_" + btagopVar);
  }

  if( "threetag" == m_strategy ){
    BDTVar.push_back("dEtabb_Avg_" + btagopVar);
    BDTVar.push_back("dRbb_MaxPt_" + btagopVar);
    BDTVar.push_back("Mbb_HiggsMass_" + btagopVar);
    BDTVar.push_back("dRbb_HiggsMass_" + btagopVar);
    BDTVar.push_back("nHiggsbb30_" + btagopVar +"/I");
    BDTVar.push_back("dEtajl_MaxdEta");
    BDTVar.push_back("H0_all");
    BDTVar.push_back("Mbb_MinM_" + btagopVar);
    BDTVar.push_back("Aplanarity_jets");
    BDTVar.push_back("Centrality_all");
    BDTVar.push_back("Mbb_MindR_" + btagopVar);
  }

}

void TTHDilepClassBDT::getBDTVarsReco(std::vector<std::string> &BDTVar){

  if( "twoAt77" == m_strategy || "fourAt85" == m_strategy){
    BDTVar.push_back(m_recoFlag + "_withH_best_" + m_recoFlag + "_withH");
    BDTVar.push_back(m_recoFlag + "_best_Higgs_mass");
  }

  if( "threetag" == m_strategy ){
    BDTVar.push_back(m_recoFlag + "_withH_best_" + m_recoFlag + "_withH");
    BDTVar.push_back(m_recoFlag + "_best_Higgs_mass");
  }

}

void TTHDilepClassBDT::getBDTVarsPCBDT(std::vector<std::string> &BDTVar){

  if(m_strategy == "4j_A3"){
    if(m_regionVal == 1){//SR1
      BDTVar.push_back("TTHReco_4b_withH_best_TTHReco_4b_withH");
      BDTVar.push_back("dEtabb_Avg_70");
      BDTVar.push_back("Mbb_MinM_70");
      BDTVar.push_back("TTHReco_4b_withH_best_Higgsttbar_dR");
      BDTVar.push_back("nHiggsbb30_70/I");
      BDTVar.push_back("TTHReco_4b_best_Higgs_mass");
      BDTVar.push_back("TTHReco_4b_best_Higgsleptop_dRmin");
      BDTVar.push_back("Mbb_MindR_70");
      BDTVar.push_back("Mjj_MaxPt");
      BDTVar.push_back("Mbb_MaxPt_70");
      BDTVar.push_back("TTHReco_4b_best_TTHReco_4b_withH");
      BDTVar.push_back("TTHReco_4b_best_Higgsbtop_dRmin");
    }
    else if(m_regionVal == 3){//SR2
      BDTVar.push_back("dEtabb_Avg_85");
      BDTVar.push_back("TTHReco_4b_withH_best_TTHReco_4b_withH");
      BDTVar.push_back("Mbb_MinM_85");
      BDTVar.push_back("nHiggsbb30_77/I");
      BDTVar.push_back("Aplanarity_bjets_70");
      BDTVar.push_back("dRbb_MaxPt_85");
      BDTVar.push_back("TTHReco_4b_best_Higgsbtop_dRmin");
      BDTVar.push_back("TTHReco_4b_best_Higgsleptop_dRmin");
      BDTVar.push_back("nJets_Pt40/I");
      BDTVar.push_back("dEtajl_MaxdEta");
      BDTVar.push_back("TTHReco_4b_best_TTHReco_4b_withH");
      BDTVar.push_back("pTbb_MindR_77");
      BDTVar.push_back("TTHReco_4b_withH_best_Higgsttbar_dR");
      BDTVar.push_back("TTHReco_4b_best_bbHiggs_dR");
    }
    else if(m_regionVal == 2){//SR3
      BDTVar.push_back("dEtabb_Avg_60");
      BDTVar.push_back("TTHReco_ICHEP70_withH_best_TTHReco_ICHEP70_withH");
      BDTVar.push_back("Mbb_MindR_60");
      BDTVar.push_back("dRbb_MaxPt_60");
      BDTVar.push_back("Mbb_MaxM_60");
      BDTVar.push_back("dEtajl_MaxdEta");
      BDTVar.push_back("nJets_Pt40/I");
      BDTVar.push_back("TTHReco_ICHEP70_best_Higgs_mass");
      BDTVar.push_back("TTHReco_ICHEP70_best_Higgsleptop_dRmin");
      BDTVar.push_back("Mbb_MaxPt_60");
      BDTVar.push_back("HT_all");
    }
  }
  else if(m_strategy == "4j_bTagVars_A3"){
    if(m_regionVal == 3){
      BDTVar.push_back("dEtabb_Avg_85");
      BDTVar.push_back("TTHReco_4b_withH_best_TTHReco_4b_withH");
      BDTVar.push_back("Mbb_MinM_85");
      BDTVar.push_back("TTHReco_4b_best_bbHiggs_tagWeightBin_sum/I");
      BDTVar.push_back("nHiggsbb30_77/I");
      BDTVar.push_back("dRbb_MaxPt_85");
      BDTVar.push_back("TTHReco_4b_best_Higgsleptop_dRmin");
      BDTVar.push_back("TTHReco_4b_best_Higgsbtop_dRmin");
      BDTVar.push_back("TTHReco_4b_best_bbHiggs_dR");
      BDTVar.push_back("TTHReco_4b_best_Higgsbtop_dRmax");
      BDTVar.push_back("Aplanarity_bjets_70");
      BDTVar.push_back("TTHReco_4b_best_TTHReco_4b_withH");
      BDTVar.push_back("nJets_Pt40/I");
      BDTVar.push_back("dEtajl_MaxdEta");
    }
  }

}

bool TTHDilepClassBDT::passRegion(TTHbb::Event* eventvar) const{

  if(m_regionName!=""){
    if(!eventvar->checkIntVariable(m_regionName)){
      std::cout << "ERROR: TTHDilepClass::passRegion region not found " << m_regionName << std::endl;
      return false;
    }
    if(eventvar->intVariable(m_regionName) == m_regionVal) return true;
    else{
      return false;
    }
  }

  if("twoAt77" == m_strategy){
    if(eventvar->checkIntVariable("nBTags_77") && eventvar->checkIntVariable("nBTags_85")){
      return (eventvar->intVariable("nBTags_77") >= 2) && (eventvar->intVariable("nBTags_85") >= 4);
    }
    else{
      return false;
    }
  }

  if("fourAt85" == m_strategy){
    if(eventvar->checkIntVariable("nBTags_85")){
      return eventvar->intVariable("nBTags_85") >= 4;
    }
    else{
      return false;
    }
  }

  if("threetag" == m_strategy){
    if(eventvar->checkIntVariable("nBTags_60") && eventvar->checkIntVariable("nBTags_85")){
      return (eventvar->intVariable("nBTags_60") == 3) && (eventvar->intVariable("nBTags_85") == 3);
    }
    else{
      return false;
    }
  }

  

  return true;

}

std::string TTHDilepClassBDT::name() const{

  return "TTHDILEPCLASSBDT";

}
