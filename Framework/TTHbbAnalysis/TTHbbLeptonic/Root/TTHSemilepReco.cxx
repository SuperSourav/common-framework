/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHSemilepReco.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbReconstruction/JetInCombinationDecorationTool.h"

#include "PathResolver/PathResolver.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHSemilepReco::TTHSemilepReco(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
   m_params=params;

   m_xmlPath = PathResolverFindCalibDirectory("TTHbbLeptonic");

   m_nJets=0;
   m_nbJets=0;
   m_leptonType=0;

   m_configMap["Strategy"]="ICHEP";
   m_configMap["BTag"]="70"; /// used only in njnb regions (a la ICHEP)
   m_configMap["TRF"]="";
   m_configMap["Region"]="";
   m_configMap["Flag"]="TTHReco";
   m_configMap["XMLFlag"]="xmlNoSet";
   m_configMap["VarOption"]="";


   m_njets_cuts=-1;
   m_nmaxjets_cuts=-1;
   m_nbjets_cuts=-1;
   m_nmaxbjets_cuts=-1;
   m_regionName="";
   m_regionVal=-1;

   initTool();
   initVariablesToCopy();
   initReaders();
   registerSaverVariables();
}

TTHSemilepReco::~TTHSemilepReco(){
  delete m_recoCombinationTool;
  delete m_recoBDTTool;
  delete m_recoMVATool;
}


void TTHSemilepReco::initTool(){

  std::vector<std::string> options = TTHbb::util::vectoriseString(m_params," ");

  //std::cout << "RECO config" << std::endl;
  for(auto ss : options){

    std::vector<std::string> value = TTHbb::util::vectoriseString(ss,":");
    if(value.size() != 2){
      std::cout << "ERROR TTHSemilepReco::initTool problem with option format " << ss << std::endl;
      abort();
    }
    std::unordered_map<std::string,std::string>::iterator iter = m_configMap.find(value[0]);
    if(iter == m_configMap.end()){
      std::cout << "ERROR TTHSemilepReco::initTool unkown option " << value[0] << std::endl;
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
  m_xmlFlag = m_configMap["XMLFlag"];
  m_varOption = m_configMap["VarOption"];

  std::cout << "TTHbbSemilepReco with options:"<< std::endl;
  for(auto a : m_configMap){
    std::cout << " "<< a.first<<":"<<a.second;
  }
  std::cout << std::endl;

  if(m_region.find("=")!=std::string::npos){
    TTHbb::util::decodeRegionName(m_region, m_regionName, m_regionVal);
  }
  else{
    TTHbb::util::decodeRegionName(m_region, m_njets_cuts, m_nmaxjets_cuts, m_nbjets_cuts, m_nmaxbjets_cuts);
  }

  m_xmlPath+="/BDTweights_semilep/reconstruction";
 

  m_recoCombinationTool=new RecoCombinationsMakerTool();
  m_recoCombinationTool->m_isMC=m_config->isMC();
  m_recoCombinationTool->m_btagIdName="isbtagged_"+m_btagOP+m_trf;

  if("ICHEP"==m_recoStrategy){
    /// default setup do that for now 
    m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  }
  else if("b4l4"==m_recoStrategy){
    m_recoCombinationTool->m_allowedOrderedWeightForBCut=4; /// the order starts at 1 (4th jets can be used)
    m_recoCombinationTool->m_allowedOrderedWeightForLightCut=5; /// the order starts at 1 (the 5th can be used)
    m_recoCombinationTool->m_forceOneBJetsInHiggs=false;
    m_recoCombinationTool->m_forceTaggedJetsAsB=false;
    m_recoCombinationTool->m_btagOrderVariableName="tagWeightBin_order";
    m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  }
  else if("b5l3"==m_recoStrategy){
    m_recoCombinationTool->m_allowedOrderedWeightForBCut=5; /// the order starts at 1 (4th jets can be used)
    m_recoCombinationTool->m_allowedOrderedWeightForLightCut=4; /// the order starts at 1 (the 5th can be used)
    m_recoCombinationTool->m_forceOneBJetsInHiggs=false;
    m_recoCombinationTool->m_forceTaggedJetsAsB=false;
    m_recoCombinationTool->m_btagOrderVariableName="tagWeightBin_order";
    m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  }
  else{
    std::cout << "ERROR: TTHSemilepReco startegy " << m_recoStrategy << " not found " << std::endl;
    abort();
  }


}

void TTHSemilepReco::initVariablesToCopy(){


  /// end guess what the best part is: I do not have to put them again in the saver!!! 
  m_copyFloatVariables.push_back(m_flag); /// this is the BDT output
  m_copyFloatVariables.push_back(m_flag+"_withH"); /// this is the BDT output with Higgs for the best combination without Higgs
  m_copyFloatVariables.push_back("b1Higgsbhadtop_dR");
  m_copyFloatVariables.push_back("bbHiggs_dR");
  m_copyFloatVariables.push_back("hadWb1Higgs_mass");
  m_copyFloatVariables.push_back("Higgs_mass");
  m_copyFloatVariables.push_back("Higgsbleptop_mass");
  m_copyFloatVariables.push_back("Higgslep_dR");
  m_copyFloatVariables.push_back("Higgsleptop_dR");
  m_copyFloatVariables.push_back("Higgsq1hadW_mass");
  m_copyFloatVariables.push_back("lepTophadTop_dR");
  m_copyFloatVariables.push_back("Higgshadtop_dR");

  m_copyFloatVariables.push_back("leptop_mass");
  m_copyFloatVariables.push_back("hadtop_mass");
  m_copyFloatVariables.push_back("hadW_mass");
  m_copyFloatVariables.push_back("hadWblepTop_mass");
  m_copyFloatVariables.push_back("lepWbhadTop_mass");
  m_copyFloatVariables.push_back("hadWbhadTop_dR");
  m_copyFloatVariables.push_back("hadWblepTop_dR");
  m_copyFloatVariables.push_back("lepblepTop_dR");
  m_copyFloatVariables.push_back("lepbhadTop_dR");
  m_copyFloatVariables.push_back("qqhadW_dR");
  m_copyFloatVariables.push_back("blepTopbhadTop_dR");
  m_copyFloatVariables.push_back("bhadTopq1hadW_dR");
  m_copyFloatVariables.push_back("bhadTopq2hadW_dR");
  m_copyFloatVariables.push_back("minbhadTopqhadW_dR");
  m_copyFloatVariables.push_back("diff_mindRbhadTopqhadW_dRlepblepTop");
  m_copyFloatVariables.push_back("lepb1Higgs_dR");
  m_copyFloatVariables.push_back("b1Higgsbleptop_dR");

  m_copyFloatVariables.push_back("nu_pt");
  m_copyFloatVariables.push_back("nu_eta");
  m_copyFloatVariables.push_back("nu_phi");

  m_copyFloatVariables.push_back("leptop_pt");
  m_copyFloatVariables.push_back("hadtop_pt");
  m_copyFloatVariables.push_back("Higgs_pt");


  m_copyFloatVariablesH.push_back(m_flag);
  m_copyFloatVariablesH.push_back(m_flag+"_withH");
  m_copyFloatVariablesH.push_back("ttH_Ht");
  m_copyFloatVariablesH.push_back("Higgsbhadtop_dR");
  m_copyFloatVariablesH.push_back("Higgsbleptop_dR");
  m_copyFloatVariablesH.push_back("Higgsttbar_dR");
 
  m_copyFloatVariablesH.push_back("b1Higgsbhadtop_dR");
  m_copyFloatVariablesH.push_back("bbHiggs_dR");
  m_copyFloatVariablesH.push_back("hadWb1Higgs_mass");
  m_copyFloatVariablesH.push_back("Higgs_mass");
  m_copyFloatVariablesH.push_back("Higgsbleptop_mass");
  m_copyFloatVariablesH.push_back("Higgslep_dR");
  m_copyFloatVariablesH.push_back("Higgsleptop_dR");
  m_copyFloatVariablesH.push_back("Higgsq1hadW_mass");
  m_copyFloatVariablesH.push_back("lepTophadTop_dR");


  m_copyFloatVariablesH.push_back("leptop_mass");
  m_copyFloatVariablesH.push_back("hadtop_mass");
  m_copyFloatVariablesH.push_back("hadW_mass");
  m_copyFloatVariablesH.push_back("hadWblepTop_mass");
  m_copyFloatVariablesH.push_back("lepWbhadTop_mass");
  m_copyFloatVariablesH.push_back("hadWbhadTop_dR");
  m_copyFloatVariablesH.push_back("hadWblepTop_dR");
  m_copyFloatVariablesH.push_back("lepblepTop_dR");
  m_copyFloatVariablesH.push_back("lepbhadTop_dR");
  m_copyFloatVariablesH.push_back("qqhadW_dR");
  m_copyFloatVariablesH.push_back("blepTopbhadTop_dR");
  m_copyFloatVariablesH.push_back("bhadTopq1hadW_dR");
  m_copyFloatVariablesH.push_back("bhadTopq2hadW_dR");
  m_copyFloatVariablesH.push_back("minbhadTopqhadW_dR");
  m_copyFloatVariablesH.push_back("diff_mindRbhadTopqhadW_dRlepblepTop");
  m_copyFloatVariablesH.push_back("lepb1Higgs_dR");
  m_copyFloatVariablesH.push_back("b1Higgsbleptop_dR");

  m_copyFloatVariablesH.push_back("nu_pt");
  m_copyFloatVariablesH.push_back("nu_eta");
  m_copyFloatVariablesH.push_back("nu_phi");

  m_copyFloatVariablesH.push_back("leptop_pt");
  m_copyFloatVariablesH.push_back("hadtop_pt");
  m_copyFloatVariablesH.push_back("Higgs_pt");

  //m_copyIntVariables = {""}; /// none
  m_copyIntVariables.push_back("b1Higgs_tagWeightBin");
  m_copyIntVariables.push_back("b2Higgs_tagWeightBin");
  m_copyIntVariables.push_back("bbHiggs_tagWeightBin_sum");
  m_copyIntVariables.push_back("bbTop_bbHiggs_tagWeightBin_diff");
  m_copyIntVariablesH.push_back("bhadTop_tagWeightBin");

}

void TTHSemilepReco::registerSaverVariables(){

  auto* esh = EventSaverHelper::get();

  registerSaverVariables(m_copyIntVariables, m_flag+"_best_", TTHbb::DecorationType::Int, name());
  registerSaverVariables(m_copyFloatVariables, m_flag+"_best_", TTHbb::DecorationType::Float, name());

  registerSaverVariables(m_copyIntVariablesH, m_flag+"_withH_best_", TTHbb::DecorationType::Int, name());
  registerSaverVariables(m_copyFloatVariablesH, m_flag+"_withH_best_", TTHbb::DecorationType::Float, name());
 
  esh->addVariableToBeSaved(m_flag+"_best_truthMatchPattern", TTHbb::DecorationType::Int, false, name());
  esh->addVariableToBeSaved(m_flag+"_withH_best_truthMatchPattern", TTHbb::DecorationType::Int, false, name());

  esh->addVariableToBeSaved(m_flag+"_best_nuApprox", TTHbb::DecorationType::Int, false, name());
  esh->addVariableToBeSaved(m_flag+"_withH_best_nuApprox", TTHbb::DecorationType::Int, false, name());


  esh->addJetVariableToBeSaved(m_flag+"_best_Cand",TTHbb::DecorationType::Int,false, name());
  esh->addJetVariableToBeSaved(m_flag+"_withH_best_Cand",TTHbb::DecorationType::Int,false, name());


}

void TTHSemilepReco::registerSaverVariables(std::vector<std::string> vect, std::string flag, TTHbb::DecorationType type, std::string description){

  auto* esh = EventSaverHelper::get();

  for(auto ss : vect){
    if(ss=="")continue;
    esh->addVariableToBeSaved(flag+ss, type, false, description);
  }

}


void TTHSemilepReco::initReaders(){

  ///TMVAClassification_BDT5je4bi_defaultSig_btag60_modulo_2_rest_0.weights.xml

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

  std::vector<std::string> BDTVar;
  std::vector<std::string> BDTVarH;

  getBDTVars(BDTVar, BDTVarH);

  //std::string recoName = "Reco_"+m_recoStrategy+"_btag"+m_btagOP+m_trf;
  std::string recoName = m_flag;

  TMVAConfiguration recoConfig;
  recoConfig.samplings.push_back(OddSampling);
  recoConfig.samplings.push_back(EvenSampling);
  recoConfig.name=recoName;
  recoConfig.setCalibFilePattern(m_xmlPath+"/TMVAClassification_BDT"+m_xmlFlag);
  recoConfig.setVariables(BDTVar);
  recoConfig.bookReader();
  /// also regions supports any cut on a decorated int or float var (can extend to char)
  /// region cuts redundant here since we are cutting before calling
  m_recoBDTTool->addRegion(recoConfig, 0,10000.,0,10000, 0);
  m_recoMVATool->addCombinationVariables(recoName+"_best", m_copyFloatVariables, m_copyIntVariables);

  //std::string recoNameH = "Reco_withH_"+m_recoStrategy+"_btag"+m_btagOP+m_trf;
  std::string recoNameH = m_flag+"_withH";
  TMVAConfiguration recoConfig_withH;
  recoConfig_withH.samplings.push_back(OddSampling);
  recoConfig_withH.samplings.push_back(EvenSampling);
  recoConfig_withH.name=recoNameH;
  recoConfig_withH.setCalibFilePattern(m_xmlPath+"/TMVAClassification_BDT"+m_xmlFlag+"_withHiggs");
  recoConfig_withH.setVariables(BDTVarH);
  recoConfig_withH.bookReader();

  m_recoBDTTool->addRegion(recoConfig_withH, 0,10000.,0,10000, 0); /// no cuts already selected
  m_recoMVATool->addCombinationVariables(recoNameH+"_best", m_copyFloatVariablesH, m_copyIntVariablesH);
  
}


void TTHSemilepReco::defaultValues(TTHbb::Event* tthevt) const{

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

  if(!tthevt->checkIntVariable(m_flag+"_best_nuApprox")){
    tthevt->intVariable(m_flag+"_best_nuApprox")=-9;
  }
  if(!tthevt->checkIntVariable(m_flag+"_withH_best_nuApprox")){
    tthevt->intVariable(m_flag+"_withH_best_nuApprox")=-9;
  }


}

bool TTHSemilepReco::apply(const top::Event & event) const{


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
  TTHbb::NeutrinoContainer& neutrinos = tthevt->m_neutrinos;

  m_nJets=jets.size();
  m_nbJets=tthevt->intVariable("nBTags_"+m_btagOP+m_trf);
  m_leptonType=tthevt->intVariable("leptonType");


  defaultValues(tthevt.get());
 
  if(leptons.size() != 1){
    return true;
  }
  if(m_nJets<5){ /// will never do it in 4j
    return true;
  }

  if(!passRegion(tthevt.get())) return true;

  TTHbb::CombinationContainer* combinations=buildCombinations(&jets, &leptons, &neutrinos);

  unsigned long long eventNumber = tthevt->m_info->eventNumber;

  computeBDT(combinations, tthevt.get(), eventNumber);

  delete combinations;
  return true;

}

void TTHSemilepReco::computeBDT(TTHbb::CombinationContainer* combinations, TTHbb::Event* tthevt,
				       unsigned long long eventNumber) const{


  if(!combinations) return;
  if(!combinations->size()) return;

  m_recoBDTTool->computeBDT(m_nJets, m_nbJets, m_leptonType, tthevt, combinations, eventNumber);
  /// not passing combinations for now not to compute additional variables from many combinations
  m_recoMVATool->fillRecoVariables(tthevt, 0); 

  JetInCombinationDecorationTool::fillRecoVariables(tthevt, m_flag+"_best", m_flag+"_best_Cand");
  JetInCombinationDecorationTool::fillRecoVariables(tthevt, m_flag+"_withH_best", m_flag+"_withH_best_Cand");

  decorateNeutrinoSolution(tthevt, m_flag+"_best");
  decorateNeutrinoSolution(tthevt, m_flag+"_withH_best");


}

void TTHSemilepReco::decorateNeutrinoSolution(TTHbb::Event* tthevt, std::string combname) const{

  std::shared_ptr<TTHbb::Combination> comb = tthevt->combinationVariable(combname);

  if(!comb){
    std::cout << "ERROR: decorateNeutrinoSolution could not find combination " << combname  << std::endl;
    return;
  }
 if(!comb->lepTop){
    std::cout << "ERROR: decorateNeutrinoSolution no leptop in combination " << combname  << std::endl;
    return;
  }
 
 tthevt->intVariable(combname+"_nuApprox") = comb->lepTop->childw->child2->intVariable("nuapprox");
 tthevt->floatVariable(combname+"_nu_pt") = comb->lepTop->childw->child2->p4().Pt();
 tthevt->floatVariable(combname+"_nu_eta") = comb->lepTop->childw->child2->p4().Eta();
 tthevt->floatVariable(combname+"_nu_phi") = comb->lepTop->childw->child2->p4().Phi();
  

}

TTHbb::CombinationContainer* TTHSemilepReco::buildCombinations(TTHbb::JetContainer* jets,
							       TTHbb::LeptonContainer* leptons,
							       TTHbb::NeutrinoContainer* neutrinos) const{

  m_recoCombinationTool->buildStrategyTTHSemiLep(jets);

  CompositeContainer *lepWContainer=0;
  CompositeContainer *hadWContainer=0;
  CompositeContainer *higgsContainer=0;
  CompositeContainer *incompleteHadTopContainer=0;
  TopCompositeContainer *lepTopContainer=0;
  TopCompositeContainer *hadTopContainer=0;
  CombinationContainer* combContainer=0;
 
  lepWContainer = m_recoCombinationTool->createLepWContainer(leptons, neutrinos, "lepWContainer");
  if(!lepWContainer){
    std::cout << "WARNING " << name() << " could not create lepWContainer " << std::endl;
    return 0;
  }
  
  if(m_nJets>=6){
    hadWContainer = m_recoCombinationTool->createHadWContainer(jets, "hadWContainer");
    if(!hadWContainer){
      std::cout << "WARNING " << name() << " could not create hadWContainer " <<  std::endl;
      return 0;
    }
  }

  higgsContainer = m_recoCombinationTool->createHiggsContainer(jets, "higgsContainer");
  if(!higgsContainer){
    std::cout << "WARNING " << name() << " could not create higgsContainer " <<  std::endl;
    return 0;
  }

  lepTopContainer = m_recoCombinationTool->createLepTopContainer(lepWContainer, jets, "lepTopContainer");
  if(!lepTopContainer){
    std::cout << "WARNING " << name() << " could not create lepTopContainer " << std::endl;
    return 0;
  }

  if(m_nJets>=6){
    hadTopContainer = m_recoCombinationTool->createHadTopContainer(hadWContainer, jets, "hadTopContainer");
    if(!hadTopContainer){
      std::cout << "WARNING " << name() << " could not create hadTopContainer " <<  std::endl;
      return 0;
    }
 
    combContainer = m_recoCombinationTool->createCombinations(lepTopContainer, hadTopContainer, higgsContainer, "combContainer");
    if(!combContainer){
      std::cout << "WARNING " << name() << " could not create combContainer " << std::endl;
      return 0;
    }
  }

  if(m_nJets==5){
    incompleteHadTopContainer = m_recoCombinationTool->createIncompleteHadTopContainer(jets, "incompleteHadTopContainer");
    if(!incompleteHadTopContainer){
      std::cout << "WARNING " << name() << " could not create incompleteHadTopContainer " << std::endl;
	return 0;
    }
   
    combContainer = m_recoCombinationTool->createIncompleteCombinations(lepTopContainer, incompleteHadTopContainer, higgsContainer, "combContainer");
    if(!combContainer){
      std::cout << "WARNING " << name() << " could not create  combContainer " << std::endl;
      return 0;
    }
  }
  
  delete lepWContainer;
  delete hadWContainer;
  delete higgsContainer;
  delete incompleteHadTopContainer;
  delete lepTopContainer;
  delete hadTopContainer;
  
  return combContainer;

}



/// can change and read from text file
void TTHSemilepReco::getBDTVars(std::vector<std::string> &BDTVar, 
				       std::vector<std::string> &BDTVarH){

  BDTVar.clear();
  BDTVarH.clear();

  if("6ji4bi" == m_varOption || "6je4bi" == m_varOption || "7ji4bi" == m_varOption ){

    BDTVar.push_back("leptop_mass");
    BDTVar.push_back("hadtop_mass");
    BDTVar.push_back("hadW_mass");
    BDTVar.push_back("hadWblepTop_mass");
    BDTVar.push_back("lepWbhadTop_mass");
    BDTVar.push_back("hadWbhadTop_dR");
    BDTVar.push_back("hadWblepTop_dR");
    BDTVar.push_back("lepblepTop_dR");
    BDTVar.push_back("lepbhadTop_dR");
    BDTVar.push_back("qqhadW_dR");
    BDTVar.push_back("blepTopbhadTop_dR");
    BDTVar.push_back("bhadTopq1hadW_dR");
    BDTVar.push_back("bhadTopq2hadW_dR");
    BDTVar.push_back("minbhadTopqhadW_dR");
    BDTVar.push_back("diff_mindRbhadTopqhadW_dRlepblepTop");

    BDTVarH=BDTVar;
    BDTVarH.push_back("Higgs_mass");
    BDTVarH.push_back("bbHiggs_dR");
    BDTVarH.push_back("lepb1Higgs_dR");
    BDTVarH.push_back("Higgsq1hadW_mass");

    return;
  }

  if("6ji3be" == m_varOption ){

    BDTVar.push_back("leptop_mass");
    BDTVar.push_back("hadtop_mass");
    BDTVar.push_back("hadW_mass");
    BDTVar.push_back("hadWblepTop_mass");
    BDTVar.push_back("lepWbhadTop_mass");
    BDTVar.push_back("hadWbhadTop_dR");
    BDTVar.push_back("hadWblepTop_dR");
    BDTVar.push_back("lepblepTop_dR");
    BDTVar.push_back("lepbhadTop_dR");
    BDTVar.push_back("qqhadW_dR");
    BDTVar.push_back("blepTopbhadTop_dR");
    BDTVar.push_back("bhadTopq1hadW_dR");
    BDTVar.push_back("bhadTopq2hadW_dR");
    BDTVar.push_back("minbhadTopqhadW_dR");
    BDTVar.push_back("diff_mindRbhadTopqhadW_dRlepblepTop");

    BDTVarH = BDTVar;
    BDTVarH.push_back("Higgs_mass");
    BDTVarH.push_back("bbHiggs_dR");
    BDTVarH.push_back("b1Higgsbleptop_dR");
    BDTVarH.push_back("lepb1Higgs_dR");
    BDTVarH.push_back("b1Higgsbhadtop_dR");
    BDTVarH.push_back("Higgsq1hadW_mass");

    return;
  }

  if("5je4bi" == m_varOption ){

    BDTVar.push_back("leptop_mass");
    BDTVar.push_back("hadtop_mass");
    BDTVar.push_back("hadWblepTop_mass");
    BDTVar.push_back("lepWbhadTop_mass");
    BDTVar.push_back("hadWbhadTop_dR");
    BDTVar.push_back("hadWblepTop_dR");
    BDTVar.push_back("lepblepTop_dR");
    BDTVar.push_back("lepbhadTop_dR");
    BDTVar.push_back("blepTopbhadTop_dR");
    BDTVar.push_back("diff_mindRbhadTopqhadW_dRlepblepTop");

    BDTVarH = BDTVar;
    BDTVarH.push_back("Higgs_mass");
    BDTVarH.push_back("bbHiggs_dR");
    BDTVarH.push_back("b1Higgsbleptop_dR");
    BDTVarH.push_back("lepb1Higgs_dR");
    BDTVarH.push_back("b1Higgsbhadtop_dR");
    BDTVarH.push_back("Higgsq1hadW_mass");

    return;
  }


}


bool TTHSemilepReco::passRegion(TTHbb::Event* evt) const{


  if(m_regionName!=""){
    if(!evt->checkIntVariable(m_regionName)){
      std::cout << "ERROR: TTHSemilepReco::passRegion region not found " << m_regionName << std::endl;
      return false;
    }
    if(evt->intVariable(m_regionName)==m_regionVal)return true;
    return false;
  }


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



std::string TTHSemilepReco::name() const{

  return "TTHSEMILEPRECO";

}



