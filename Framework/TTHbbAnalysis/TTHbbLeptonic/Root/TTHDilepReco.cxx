/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHDilepReco.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbReconstruction/JetInCombinationDecorationTool.h"
#include "PathResolver/PathResolver.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHDilepReco::TTHDilepReco(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
   m_params=params;

   m_xmlPath = PathResolverFindCalibDirectory("TTHbbLeptonic");

   m_nJets=0;
   m_nbJets=0;
   m_leptonType=0;
   do_nuweighting = false;

   m_configMap["Strategy"]="ICHEP";
   m_configMap["BTag"]="70";
   m_configMap["TRF"]="";
   m_configMap["Region"]="";
   m_configMap["Flag"]="TTHReco";
   m_configMap["nuW"]="false";

   initTool();
   initVariablesToCopy();
   initReaders();
   registerSaverVariables();
}

TTHDilepReco::~TTHDilepReco(){
  delete m_recoCombinationTool;
  delete m_recoBDTTool;
  delete m_recoMVATool;
}


void TTHDilepReco::initTool(){

  std::vector<std::string> options = TTHbb::util::vectoriseString(m_params," ");

  //std::cout << "RECO config" << std::endl;
  for(auto ss : options){

    std::vector<std::string> value = TTHbb::util::vectoriseString(ss,":");
    if(value.size() != 2){
      std::cout << "ERROR TTHDilepReco::initTool problem with option format " << ss << std::endl;
      abort();
    }
    std::unordered_map<std::string,std::string>::iterator iter = m_configMap.find(value[0]);
    if(iter == m_configMap.end()){
      std::cout << "ERROR TTHDilepReco::initTool unkown option " << value[0] << std::endl;
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
  m_recoStrategy=m_configMap["Strategy"];
  m_region=m_configMap["Region"];
  m_nuW=m_configMap["nuW"];
  if(m_nuW == "true") do_nuweighting=true;

  m_xmlPath+="/BDTweights_dilep/reconstruction";

  /// please validate this
  m_recoCombinationTool=new RecoCombinationsMakerTool();
  m_recoCombinationTool->m_isMC=m_config->isMC();
  m_recoCombinationTool->m_btagIdName="isbtagged_"+m_btagOP+m_trf;

  if("ICHEP"==m_recoStrategy){
    ///will set the needed number of bs and light jets (ie needed no of b)
    ///see how many are tagged, if more, then use all, if fewer use all other light jets
    m_recoCombinationTool->m_allowedOrderedWeightForBCut=5; /// the order starts at 1 (5th jets can be used)    /// default setup do that for now
    m_recoCombinationTool->m_forceTaggedJetsAsB=false;
    m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  }
  else if("4b"==m_recoStrategy  || "old"==m_recoStrategy|| "nu"==m_recoStrategy){
    m_recoCombinationTool->m_allowedOrderedWeightForBCut=4; /// the order starts at 1 (5th jets can be used)
    m_recoCombinationTool->m_allowedOrderedWeightForLightCut=-1; /// the order starts at 1 (the 4th can be used)
    m_recoCombinationTool->m_forceOneBJetsInHiggs=false;
    m_recoCombinationTool->m_forceTaggedJetsAsB=false;
    m_recoCombinationTool->m_btagOrderVariableName="tagWeightBin_order";
    m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  }
  else if("5b"==m_recoStrategy){
    m_recoCombinationTool->m_allowedOrderedWeightForBCut=5; /// the order starts at 1 (5th jets can be used)
    m_recoCombinationTool->m_allowedOrderedWeightForLightCut=-1; /// the order starts at 1 (the 4th can be used)
    m_recoCombinationTool->m_forceOneBJetsInHiggs=false;
    m_recoCombinationTool->m_forceTaggedJetsAsB=false;
    m_recoCombinationTool->m_btagOrderVariableName="tagWeightBin_order";
    m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  }
  else{
    std::cout << "ERROR: TTHDilepReco startegy " << m_recoStrategy << " not found " << std::endl;
    abort();
  }

}

void TTHDilepReco::initVariablesToCopy(){


  /// end guess what the best part is: I do not have to put them again in the saver!!! 
  m_copyFloatVariables.push_back(m_flag); /// this is the BDT output
  m_copyFloatVariables.push_back(m_flag+"_withH"); /// this is the BDT output with Higgs for the best combination without Higgs
  m_copyFloatVariables.push_back("Higgs_mass");
  m_copyFloatVariables.push_back("bbHiggs_dR");
  m_copyFloatVariables.push_back("Higgsleptop_dRmin");
  m_copyFloatVariables.push_back("Higgsleptop_dRmax");
  m_copyFloatVariables.push_back("Higgsbtop_dRmin");
  m_copyFloatVariables.push_back("Higgsbtop_dRmax");

  ///validation of input variables
  m_copyFloatVariables.push_back("diff_lepblepTop_mass");
  m_copyFloatVariables.push_back("leptop1_mass");
  m_copyFloatVariables.push_back("lepblepTop1_dR");
  m_copyFloatVariables.push_back("leptop2_mass");
  m_copyFloatVariables.push_back("lepblepTop2_dR");
  m_copyFloatVariables.push_back("blepTop1lepTop2_dPhi");
  m_copyFloatVariables.push_back("b1_pT");
  m_copyFloatVariables.push_back("b2_pT");
  m_copyFloatVariables.push_back("lepblepTop_diff");
  m_copyFloatVariables.push_back("topblep_dEtamin");
  
  if (do_nuweighting) {
    m_copyFloatVariables.push_back("m_ttbar_pT");
    m_copyFloatVariables.push_back("m_ttbar_y");
    m_copyFloatVariables.push_back("m_ttbar_m");
    m_copyFloatVariables.push_back("m_top_pT");
    m_copyFloatVariables.push_back("m_top_y");
    m_copyFloatVariables.push_back("m_tbar_pT");
    m_copyFloatVariables.push_back("m_tbar_y");
    m_copyFloatVariables.push_back("m_toptbar_dPhi");
    m_copyFloatVariables.push_back("m_toptbar_dEta");
    m_copyFloatVariables.push_back("m_toptbar_dy");
    m_copyFloatVariables.push_back("m_ttbarHiggs_dPhi");
    m_copyFloatVariables.push_back("m_ttbarHiggs_dEta");
    m_copyFloatVariables.push_back("weight_nu");

    m_copyFloatVariables.push_back("m_reco_cos_helicity_m");
    m_copyFloatVariables.push_back("m_reco_cos_helicity_p");
    m_copyFloatVariables.push_back("m_reco_cos_helicity");
    m_copyFloatVariables.push_back("m_reco_cos_raxis_m");
    m_copyFloatVariables.push_back("m_reco_cos_raxis_p");
    m_copyFloatVariables.push_back("m_reco_cos_raxis");
    m_copyFloatVariables.push_back("m_reco_cos_transverse_m");
    m_copyFloatVariables.push_back("m_reco_cos_transverse_p");
    m_copyFloatVariables.push_back("m_reco_cos_transverse");

    m_copyFloatVariables.push_back("m_top_pX"); 
    m_copyFloatVariables.push_back("m_top_pY"); 
    m_copyFloatVariables.push_back("m_top_pZ"); 
    m_copyFloatVariables.push_back("m_top_E"); 
    m_copyFloatVariables.push_back("m_tbar_pX"); 
    m_copyFloatVariables.push_back("m_tbar_pY"); 
    m_copyFloatVariables.push_back("m_tbar_pZ"); 
    m_copyFloatVariables.push_back("m_tbar_E"); 
    m_copyFloatVariables.push_back("m_lbH_pX"); 
    m_copyFloatVariables.push_back("m_lbH_pY"); 
    m_copyFloatVariables.push_back("m_lbH_pZ"); 
    m_copyFloatVariables.push_back("m_lbH_E"); 
    m_copyFloatVariables.push_back("m_slbH_pX"); 
    m_copyFloatVariables.push_back("m_slbH_pY"); 
    m_copyFloatVariables.push_back("m_slbH_pZ"); 
    m_copyFloatVariables.push_back("m_slbH_E"); 
    m_copyFloatVariables.push_back("m_ttbarHiggs_dR");
    m_copyFloatVariables.push_back("m_topHiggs_dR");
    m_copyFloatVariables.push_back("m_tbarHiggs_dR");
  }

  ///mv2 variables
  m_copyIntVariables.push_back("b1Higgs_tagWeightBin");
  m_copyIntVariables.push_back("b2Higgs_tagWeightBin");
  m_copyIntVariables.push_back("bbHiggs_tagWeightBin_sum");
  m_copyIntVariables.push_back("blepTop1_tagWeightBin");
  m_copyIntVariables.push_back("blepTop2_tagWeightBin");

  /// with higgs variables
  m_copyFloatVariablesH.push_back(m_flag);
  m_copyFloatVariablesH.push_back(m_flag+"_withH");
  m_copyFloatVariablesH.push_back("Higgs_mass");
  m_copyFloatVariablesH.push_back("bbHiggs_dR");
  m_copyFloatVariablesH.push_back("Higgsleptop_dRmin");
  m_copyFloatVariablesH.push_back("Higgsleptop_dRmax");
  m_copyFloatVariablesH.push_back("Higgsbtop_dRmin");
  m_copyFloatVariablesH.push_back("Higgsbtop_dRmax");

  ///validation of input variables
  m_copyFloatVariablesH.push_back("diff_lepblepTop_mass");
  m_copyFloatVariablesH.push_back("leptop1_mass");
  m_copyFloatVariablesH.push_back("lepblepTop1_dR");
  m_copyFloatVariablesH.push_back("leptop2_mass");
  m_copyFloatVariablesH.push_back("lepblepTop2_dR");
  m_copyFloatVariablesH.push_back("Higgs_mass");
  m_copyFloatVariablesH.push_back("Higgsttbar_dR");
  m_copyFloatVariablesH.push_back("bbHiggs_dR");
  m_copyFloatVariablesH.push_back("blepTop1lepTop2_dR");
  m_copyFloatVariablesH.push_back("Higgsbtop_dRmax");
  m_copyFloatVariablesH.push_back("b2Higgsmv2_pT");
  m_copyFloatVariablesH.push_back("blepTop1lepTop2_dPhi");
  m_copyFloatVariablesH.push_back("Higgsleptop_dRmin");  

  if (do_nuweighting) {
    m_copyFloatVariablesH.push_back("m_ttbar_pT");
    m_copyFloatVariablesH.push_back("m_ttbar_y");
    m_copyFloatVariablesH.push_back("m_ttbar_m");
    m_copyFloatVariablesH.push_back("m_top_pT");
    m_copyFloatVariablesH.push_back("m_top_y");
    m_copyFloatVariablesH.push_back("m_tbar_pT");
    m_copyFloatVariablesH.push_back("m_tbar_y");
    m_copyFloatVariablesH.push_back("m_toptbar_dPhi");
    m_copyFloatVariablesH.push_back("m_toptbar_dEta");
    m_copyFloatVariablesH.push_back("m_toptbar_dy");
    m_copyFloatVariablesH.push_back("m_ttbarHiggs_dPhi");
    m_copyFloatVariablesH.push_back("m_ttbarHiggs_dEta");
    m_copyFloatVariablesH.push_back("weight_nu");

    m_copyFloatVariablesH.push_back("m_reco_cos_helicity_m");
    m_copyFloatVariablesH.push_back("m_reco_cos_helicity_p");
    m_copyFloatVariablesH.push_back("m_reco_cos_helicity");
    m_copyFloatVariablesH.push_back("m_reco_cos_raxis_m");
    m_copyFloatVariablesH.push_back("m_reco_cos_raxis_p");
    m_copyFloatVariablesH.push_back("m_reco_cos_raxis");
    m_copyFloatVariablesH.push_back("m_reco_cos_transverse_m");
    m_copyFloatVariablesH.push_back("m_reco_cos_transverse_p");
    m_copyFloatVariablesH.push_back("m_reco_cos_transverse");

    m_copyFloatVariablesH.push_back("m_top_pX"); 
    m_copyFloatVariablesH.push_back("m_top_pY"); 
    m_copyFloatVariablesH.push_back("m_top_pZ"); 
    m_copyFloatVariablesH.push_back("m_top_E"); 
    m_copyFloatVariablesH.push_back("m_tbar_pX"); 
    m_copyFloatVariablesH.push_back("m_tbar_pY"); 
    m_copyFloatVariablesH.push_back("m_tbar_pZ"); 
    m_copyFloatVariablesH.push_back("m_tbar_E"); 
    m_copyFloatVariablesH.push_back("m_lbH_pX"); 
    m_copyFloatVariablesH.push_back("m_lbH_pY"); 
    m_copyFloatVariablesH.push_back("m_lbH_pZ"); 
    m_copyFloatVariablesH.push_back("m_lbH_E"); 
    m_copyFloatVariablesH.push_back("m_slbH_pX"); 
    m_copyFloatVariablesH.push_back("m_slbH_pY"); 
    m_copyFloatVariablesH.push_back("m_slbH_pZ"); 
    m_copyFloatVariablesH.push_back("m_slbH_E"); 
    m_copyFloatVariablesH.push_back("m_ttbarHiggs_dR");
    m_copyFloatVariablesH.push_back("m_topHiggs_dR");
    m_copyFloatVariablesH.push_back("m_tbarHiggs_dR");
  }
  ///mv2 variables
  m_copyIntVariablesH.push_back("b1Higgs_tagWeightBin");
  m_copyIntVariablesH.push_back("b2Higgs_tagWeightBin");
  m_copyIntVariablesH.push_back("bbHiggs_tagWeightBin_sum");
  m_copyIntVariablesH.push_back("blepTop1_tagWeightBin");
  m_copyIntVariablesH.push_back("blepTop2_tagWeightBin");

}

void TTHDilepReco::registerSaverVariables(){

  auto* esh = EventSaverHelper::get();

  registerSaverVariables(m_copyIntVariables, m_flag+"_best_", TTHbb::DecorationType::Int, name());
  registerSaverVariables(m_copyFloatVariables, m_flag+"_best_", TTHbb::DecorationType::Float, name());

  registerSaverVariables(m_copyIntVariablesH, m_flag+"_withH_best_", TTHbb::DecorationType::Int, name());
  registerSaverVariables(m_copyFloatVariablesH, m_flag+"_withH_best_", TTHbb::DecorationType::Float, name());
 
  esh->addVariableToBeSaved(m_flag+"_best_truthMatchPattern", TTHbb::DecorationType::Int, false, name());
  esh->addVariableToBeSaved(m_flag+"_withH_best_truthMatchPattern", TTHbb::DecorationType::Int, false, name());

  esh->addJetVariableToBeSaved(m_flag+"_best_Cand",TTHbb::DecorationType::Int,false, name());
  esh->addJetVariableToBeSaved(m_flag+"_withH_best_Cand",TTHbb::DecorationType::Int,false, name());

}

void TTHDilepReco::registerSaverVariables(std::vector<std::string> vect, std::string flag, TTHbb::DecorationType type, std::string description){

  auto* esh = EventSaverHelper::get();

  for(auto ss : vect){
    if(ss=="")continue;
    esh->addVariableToBeSaved(flag+ss, type, false, description);
  }

}


void TTHDilepReco::initReaders(){

  ///TMVAClassification_BDT5je4bi_defaultSig_btag60_modulo_2_rest_0.weights.xml
  ///TMVA_ReconstructionBDT_4ji4bi_77_modulo_2_rest_0.weights.xml

  EventSampling OddSampling;
  EventSampling EvenSampling;

  OddSampling.modulo=2;
  OddSampling.rest.push_back(1);
  OddSampling.patternName="modulo_2_rest_0"; /// the training is Even

  EvenSampling.modulo=2;
  EvenSampling.rest.push_back(0);
  EvenSampling.patternName="modulo_2_rest_1";/// the training is Odd

  m_recoBDTTool= new RecoBDTCalculatorTool();
  m_recoMVATool = new RecoMVAVariableTool();
  m_recoMVATool->m_isMC=m_config->isMC();

  m_njets_cuts=-1;
  m_nmaxjets_cuts=-1;
  m_nbjets_cuts=-1;
  m_nmaxbjets_cuts=-1;
  TTHbb::util::decodeRegionName(m_region, m_njets_cuts, m_nmaxjets_cuts, m_nbjets_cuts, m_nmaxbjets_cuts);

  std::vector<std::string> BDTVar;
  std::vector<std::string> BDTVarH;

  getBDTVars(BDTVar, BDTVarH);

  //std::string recoName = "Reco_"+m_recoStrategy+"_btag"+m_btagOP+m_trf;
  std::string recoName = m_flag;

  TMVAConfiguration recoConfig;
  recoConfig.samplings.push_back(OddSampling);
  recoConfig.samplings.push_back(EvenSampling);
  recoConfig.name=recoName;
  recoConfig.setCalibFilePattern(m_xmlPath+"/TMVA_ReconstructionBDT_"+m_region+"_noHiggs_"+m_btagOP+"_"+m_recoStrategy);
  recoConfig.setVariables(BDTVar);
  recoConfig.bookReader();
  /// also regions supports any cut on a decorated int or float var (can extend to char)
  /// region cuts redundant here since we are cutting before calling
  m_recoBDTTool->addRegion(recoConfig, m_njets_cuts, m_nmaxjets_cuts, m_nbjets_cuts, m_nmaxbjets_cuts, 0);
  m_recoMVATool->addCombinationVariables(recoName+"_best", m_copyFloatVariables, m_copyIntVariables);

  //std::string recoNameH = "Reco_withH_"+m_recoStrategy+"_btag"+m_btagOP+m_trf;
  std::string recoNameH = m_flag+"_withH";
  TMVAConfiguration recoConfig_withH;
  recoConfig_withH.samplings.push_back(OddSampling);
  recoConfig_withH.samplings.push_back(EvenSampling);
  recoConfig_withH.name=recoNameH;
  recoConfig_withH.setCalibFilePattern(m_xmlPath+"/TMVA_ReconstructionBDT_"+m_region+"_withHiggs_"+m_btagOP+"_"+m_recoStrategy);
  recoConfig_withH.setVariables(BDTVarH);
  recoConfig_withH.bookReader();

  m_recoBDTTool->addRegion(recoConfig_withH, m_njets_cuts, m_nmaxjets_cuts, m_nbjets_cuts, m_nmaxbjets_cuts, 0);
  m_recoMVATool->addCombinationVariables(recoNameH+"_best", m_copyFloatVariablesH, m_copyIntVariablesH);
  

}


void TTHDilepReco::defaultValues(TTHbb::Event* tthevt) const{

  /// need to check first since several instances
  /// is not strictly needed with the new saver
  for(auto ss : m_copyIntVariables){
    if(""==ss) continue;
    if(!tthevt->checkIntVariable(m_flag+"_best_"+ss)){
      tthevt->intVariable(m_flag+"_best_"+ss)=-9;
    }
  }
  for(auto ss : m_copyFloatVariables){
    if(""==ss) continue;
    if(!tthevt->checkFloatVariable(m_flag+"_best_"+ss)){
      tthevt->floatVariable(m_flag+"_best_"+ss)=-9;
    }
  }

  for(auto ss : m_copyIntVariablesH){
    if(""==ss) continue;
    if(!tthevt->checkIntVariable(m_flag+"_withH_best_"+ss)){
      tthevt->intVariable(m_flag+"_withH_best_"+ss)=-9;
    }
  }
  for(auto ss : m_copyFloatVariablesH){
    if(""==ss) continue;
    if(!tthevt->checkFloatVariable(m_flag+"_withH_best_"+ss)){
      tthevt->floatVariable(m_flag+"_withH_best_"+ss)=-9;
    }
  }

  if(!tthevt->checkIntVariable(m_flag+"_best_truthMatchPattern")){
    tthevt->intVariable(m_flag+"_best_truthMatchPattern")=0;
  }
  if(!tthevt->checkIntVariable(m_flag+"_withH_best_truthMatchPattern")){
    tthevt->intVariable(m_flag+"_withH_best_truthMatchPattern")=0;
  }

}

bool TTHDilepReco::apply(const top::Event & event) const{

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout << name() << ": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  TTHbb::JetContainer& jets = tthevt->m_jets;
  TTHbb::LeptonContainer& leptons = tthevt->m_leptons;
  //TTHbb::NeutrinoContainer& neutrinos = tthevt->m_neutrinos;

  m_nJets=jets.size();
  m_nbJets=tthevt->intVariable("nBTags_"+m_btagOP+m_trf);
  m_leptonType=tthevt->intVariable("leptonType");


  defaultValues(tthevt.get());
 
  if(leptons.size() != 2){
    return true;
  }
  if(m_nJets<4){ /// will never do it in 3j
    return true;
  }

  if(!passRegion(tthevt.get())) return true;


  unsigned long long eventNumber = tthevt->m_info->eventNumber;

  TTHbb::CombinationContainer* combinations;
  if (!do_nuweighting){
    combinations = buildCombinations(&jets, &leptons);
  } else {
    double met_met = tthevt->met_met;
    double met_phi = tthevt->met_phi;
    combinations = buildCombinations(&jets, &leptons, met_met, met_phi, eventNumber);
  }

  // TTHbb::CombinationContainer* combinations=buildCombinations(&jets, &leptons, met_met, met_phi);


  computeBDT(combinations, tthevt.get(), eventNumber);

  delete combinations;
  return true;

}

void TTHDilepReco::computeBDT(TTHbb::CombinationContainer* combinations, TTHbb::Event* tthevt,
				       unsigned long long eventNumber) const{


  if(!combinations) return;
  if(!combinations->size()) return;

  m_recoBDTTool->computeBDT(m_nJets, m_nbJets, m_leptonType, tthevt, combinations, eventNumber);
  /// not passing combinations for now not to compute additional variables from many combinations
  m_recoMVATool->fillRecoVariables(tthevt, 0); 

  JetInCombinationDecorationTool::fillRecoVariables(tthevt, m_flag+"_best", m_flag+"_best_Cand");
  JetInCombinationDecorationTool::fillRecoVariables(tthevt, m_flag+"_withH_best", m_flag+"_withH_best_Cand");

}


TTHbb::CombinationContainer* TTHDilepReco::buildCombinations(TTHbb::JetContainer* jets,
							     TTHbb::LeptonContainer* leptons) const{
  return buildCombinations(jets, leptons, -10e10, -10e10, -1);
}

TTHbb::CombinationContainer* TTHDilepReco::buildCombinations(TTHbb::JetContainer* jets,
							     TTHbb::LeptonContainer* leptons, double met_met, double met_phi, int eventNumber) const{

 
  m_recoCombinationTool->buildStrategyTTHDiLep(jets);


  CompositeContainer *lepTop1Container=0;
  CompositeContainer *lepTop2Container=0;
  CompositeContainer *higgsContainer=0;
  CombinationContainer* combContainer=0;

  /// whatever you do don't change these lines :-p
  auto lep1=(*leptons)[0];
  auto lep2=(*leptons)[1];

  /// convention for tops based on charge
  if(lep2->floatVariable("charge") > 0){
    lep1=(*leptons)[1];
    lep2=(*leptons)[0];
  }

  lepTop1Container = m_recoCombinationTool->createIncompLepTopContainer(lep1, jets, "lepTop1Container");
  if(!lepTop1Container){
    std::cout << "WARNING " << name() << " could not create lepTopContainer " << std::endl;
    return 0;
  }
  

  lepTop2Container = m_recoCombinationTool->createIncompLepTopContainer(lep2, jets, "lepTop2Container");
  if(!lepTop2Container){
    std::cout << "WARNING " << name() << " could not create lepTopContainer " << std::endl;
    return 0;
  }
  
  higgsContainer = m_recoCombinationTool->createHiggsContainer(jets, "higgsContainer");
  if(!higgsContainer){
    std::cout << "WARNING " << name() << " could not create higgsContainer " <<  std::endl;
    return 0;
  }


  combContainer = m_recoCombinationTool->createCombinations(lepTop1Container, lepTop2Container, higgsContainer, met_met, met_phi, eventNumber, "combContainer");
  if(!combContainer){
    std::cout << "WARNING " << name() << " could not create  combContainer "  << std::endl;
    return 0;
  }
  

  delete lepTop1Container;
  delete lepTop2Container;
  delete higgsContainer;
  
  return combContainer;

}



/// can change and read from text file
void TTHDilepReco::getBDTVars(std::vector<std::string> &BDTVar, 
			      std::vector<std::string> &BDTVarH){

  BDTVar.clear();
  BDTVarH.clear();

  /// fill depending on region startegy ...
  if("4ji4bi" == m_region || "4b" == m_recoStrategy || "5b" == m_recoStrategy){
    BDTVar.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVar.push_back("leptop1_mass:Mass_top1lepb");
    BDTVar.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVar.push_back("leptop2_mass:Mass_top2lepb");
    BDTVar.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVar.push_back("blepTop1lepTop2_dPhi:dPhi_top1btop2b");
    BDTVar.push_back("b1_pT:pT_top1b");
    BDTVar.push_back("b2_pT:pT_top2b");
    BDTVar.push_back("lepblepTop_diff:diff_dR_toplepb");
    BDTVar.push_back("topblep_dEtamin:mindEta_btop_l");

    BDTVarH.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVarH.push_back("leptop1_mass:Mass_top1lepb");
    BDTVarH.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVarH.push_back("leptop2_mass:Mass_top2lepb");
    BDTVarH.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVarH.push_back("Higgs_mass:Mass_Higgs");
    BDTVarH.push_back("Higgsttbar_dR:dR_HiggsTTbar");
    BDTVarH.push_back("bbHiggs_dR:dR_bbHiggs");
    BDTVarH.push_back("blepTop1lepTop2_dR:dR_top1btop2b");
    BDTVarH.push_back("Higgsbtop_dRmax:dRHiggsb_MaxdR");
  }
  else if("4ji3be" == m_region){
    BDTVar.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVar.push_back("leptop1_mass:Mass_top1lepb");
    BDTVar.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVar.push_back("leptop2_mass:Mass_top2lepb");
    BDTVar.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVar.push_back("blepTop1lepTop2_dPhi:dPhi_top1btop2b");
    BDTVar.push_back("b1_pT:pT_top1b");
    BDTVar.push_back("b2_pT:pT_top2b");
    BDTVar.push_back("lepblepTop_diff:diff_dR_toplepb");
    BDTVar.push_back("topblep_dEtamin:mindEta_btop_l");

    BDTVarH.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVarH.push_back("leptop1_mass:Mass_top1lepb");
    BDTVarH.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVarH.push_back("leptop2_mass:Mass_top2lepb");
    BDTVarH.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVarH.push_back("Higgs_mass:Mass_Higgs");

    BDTVarH.push_back("Higgsttbar_dPhi:dPhi_HiggsTTbar");
    BDTVarH.push_back("bbHiggs_dR:dR_bbHiggs");
    BDTVarH.push_back("b2Higgsmv2_pT:pT_slbHiggs");
    BDTVarH.push_back("blepTop1lepTop2_dPhi:dPhi_top1btop2b");
    BDTVarH.push_back("Higgsleptop_dRmin:mindR_bHiggs_l");
  }
  else if("nu" == m_recoStrategy && do_nuweighting){
    BDTVar.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVar.push_back("lepblepTop_diff:diff_dR_toplepb");
    BDTVar.push_back("leptop1_mass:Mass_top1lepb");
    BDTVar.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVar.push_back("leptop2_mass:Mass_top2lepb");
    BDTVar.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVar.push_back("blepTop1lepTop2_dPhi:dPhi_top1btop2b");
    BDTVar.push_back("b1_pT:pT_top1b");
    BDTVar.push_back("b2_pT:pT_top2b");
    BDTVar.push_back("topblep_dEtamin:mindEta_btop_l");
    BDTVar.push_back("m_ttbar_pT:m_pT_Ttbar");
    BDTVar.push_back("m_top_pT:m_pT_top");
    BDTVar.push_back("m_tbar_pT:m_pT_antitop");
    BDTVar.push_back("weight_nu:weight_nu");

    BDTVarH.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVarH.push_back("leptop1_mass:Mass_top1lepb");
    BDTVarH.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVarH.push_back("leptop2_mass:Mass_top2lepb");
    BDTVarH.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVarH.push_back("Higgs_mass:Mass_Higgs");
    BDTVarH.push_back("Higgsttbar_dR:dR_HiggsTTbar");
    BDTVarH.push_back("bbHiggs_dR:dR_bbHiggs");
    BDTVarH.push_back("blepTop1lepTop2_dR:dR_top1btop2b");
    BDTVarH.push_back("Higgsleptop_dRmax:dRHiggsb_MaxdR");
    BDTVarH.push_back("Higgsltop_dRmax:dRHiggsl_MaxdR");
    BDTVarH.push_back("bbHiggs_dEta:dEta_bbHiggs");
    BDTVarH.push_back("Higgsttbar_pT:pT_HiggsTTbar");
    BDTVarH.push_back("m_Higgsttbar_Mass:m_Mass_HiggsTTbar");
    BDTVarH.push_back("m_ttbar_pT:m_pT_Ttbar");
    BDTVarH.push_back("m_top_pT:m_pT_top");
    BDTVarH.push_back("m_tbar_pT:m_pT_antitop");
    BDTVarH.push_back("weight_nu:weight_nu");
  }
  else if("old" == m_recoStrategy){
    BDTVar.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVar.push_back("lepblepTop_diff:diff_dR_toplepb");
    BDTVar.push_back("leptop1_mass:Mass_top1lepb");
    BDTVar.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVar.push_back("leptop2_mass:Mass_top2lepb");
    BDTVar.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVar.push_back("blepTop1lepTop2_dPhi:dPhi_top1btop2b");
    BDTVar.push_back("b1_pT:pT_top1b");
    BDTVar.push_back("b2_pT:pT_top2b");
    BDTVar.push_back("topblep_dEtamin:mindEta_btop_l");

    BDTVarH.push_back("diff_lepblepTop_mass:diff_Mass_toplepb");
    BDTVarH.push_back("leptop1_mass:Mass_top1lepb");
    BDTVarH.push_back("lepblepTop1_dR:dR_top1lepb");
    BDTVarH.push_back("leptop2_mass:Mass_top2lepb");
    BDTVarH.push_back("lepblepTop2_dR:dR_top2lepb");
    BDTVarH.push_back("Higgs_mass:Mass_Higgs");
    BDTVarH.push_back("Higgsttbar_dR:dR_HiggsTTbar");
    BDTVarH.push_back("bbHiggs_dR:dR_bbHiggs");
    BDTVarH.push_back("blepTop1lepTop2_dR:dR_top1btop2b");
    BDTVarH.push_back("Higgsbtop_dRmax:dRHiggsb_MaxdR");
  }

}


bool TTHDilepReco::passRegion(TTHbb::Event* ) const{

  if(m_njets_cuts<=m_nmaxjets_cuts){
    if(m_nJets<m_njets_cuts && m_njets_cuts>=0) return false;
    if(m_nJets>m_nmaxjets_cuts && m_nmaxjets_cuts>=0) return false;
  }

  if(m_nbjets_cuts<=m_nmaxbjets_cuts){
    if(m_nbJets<m_nbjets_cuts && m_nbjets_cuts>=0) return false;
    if(m_nbJets>m_nmaxbjets_cuts && m_nmaxbjets_cuts>=0) return false;
  }

  return true;

}



std::string TTHDilepReco::name() const{

  return "RECO";

}



