/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbTruthEDMConverter.h"
#include "TTHbbTruthHelper/ParticleIDHelper.h"
#include "TTHbbLeptonic/TruthxAODPartHelper.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "MVAVariables/JetOrderingTool.h"

#include "TopEvent/EventTools.h"
#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHbbTruthEDMConverter::TTHbbTruthEDMConverter(std::string params, std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {

   if ( asg::ToolStore::contains<TTHbbAsgToolHelper>("TTHbbAsgHelper") ) {
     m_asgHelper = asg::ToolStore::get<TTHbbAsgToolHelper>("TTHbbAsgHelper");
   } 
   else {
     m_asgHelper = new TTHbbAsgToolHelper("TTHbbAsgHelper");
     top::check( m_asgHelper->initialize() , "Failed to initialize TTHbbAsgHelper" );
   }
   
   m_params=params;

   m_truthHistoryHelper=new TruthHistoryHelper();
   m_truthHistoryHelper->avoidSherpaLoop(true);

   m_partonTruthFinder = new PartonTruthFinder();
   m_partonTruthFinder->setTruthHistoryHelper(m_truthHistoryHelper);

   m_jetMatchingTool= new TTHbb::JetMatchingTool();
   m_ttHFClassification=new ClassifyAndCalculateHF();


   if(m_config->isMC()){

     auto* esh = EventSaverHelper::get();

     esh->addVariableToBeSaved("truth_HDecay", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_higgs_eta", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_higgs_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_nHiggs", TTHbb::DecorationType::Int,true, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_nLepTop", TTHbb::DecorationType::Int,true, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_nTop", TTHbb::DecorationType::Int,true, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_nVectorBoson", TTHbb::DecorationType::Int,true, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_top_dilep_filter", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_tbar_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_top_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_hadtop_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("truth_ttbar_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");


     esh->addVariableToBeSaved("HFClassification_q1_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter"); 
     esh->addVariableToBeSaved("HFClassification_q1_eta", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("HFClassification_q1_phi", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("HFClassification_q1_m", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");  
     esh->addVariableToBeSaved("HFClassification_q2_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter"); 
     esh->addVariableToBeSaved("HFClassification_q2_eta", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("HFClassification_q2_phi", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("HFClassification_q2_m", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");  
  
     esh->addVariableToBeSaved("HFClassification_qq_pt", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter"); 
     esh->addVariableToBeSaved("HFClassification_qq_ht", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter"); 
     esh->addVariableToBeSaved("HFClassification_qq_dr", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter"); 
     esh->addVariableToBeSaved("HFClassification_qq_m", TTHbb::DecorationType::Float,false, true ,"TTHbbTruthEDMConverter");  

     esh->addVariableToBeSaved("HF_Classification", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");  
     esh->addVariableToBeSaved("HF_SimpleClassification", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");  

     esh->addVariableToBeSaved("nPDFFlavor", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");  
     esh->addVariableToBeSaved("ttbb_categories", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");  


     esh->addVariableToBeSaved("TopHeavyFlavorFilterFlag", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");  
     esh->addVariableToBeSaved("HadTop200", TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");
     esh->addVariableToBeSaved("TTbar150",  TTHbb::DecorationType::Int,false, true ,"TTHbbTruthEDMConverter");


   }
 
}

TTHbbTruthEDMConverter::~TTHbbTruthEDMConverter(){
  delete m_truthHistoryHelper;
  delete m_partonTruthFinder;
  delete m_jetMatchingTool;
}

bool TTHbbTruthEDMConverter::apply(const top::Event & event) const{

  if(!m_config->isMC()) return true;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  std::string truthjetname = m_config->sgKeyTruthJets();
  const xAOD::JetContainer* truthJets = m_asgHelper->getJetContainer(truthjetname);
  const xAOD::TruthParticleContainer* truthpart = event.m_truth;

  //// create truth once per event, they will not change per systematics or selection
  //// the truth will be automatically cleared by AT (top::Event cleared) at each event
  //// one exception for decoration which are cleared with reco
  //// need to recreate them but hopefully this is not a lot of CPU


  if(!tthevt->truthAvailable()){
    fillTruthJets(*truthJets, tthevt->m_truthJets);
    fillTruthParticles(tthevt, truthpart);
    fillTruthMatching(tthevt->m_HFHadrons, tthevt->m_truthJets);
    tthevt->truthAvailable(true);
  }

  decorateTTHbbEvent(tthevt);
  copyEventDecorations(event.m_info, tthevt);

  return true;
}


bool TTHbbTruthEDMConverter::applyParticleLevel(const top::ParticleLevelEvent & plEvent) const{


  if(!m_config->isMC()) return true;

  if(!plEvent.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params  << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = plEvent.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  std::string truthjetname = m_config->sgKeyTruthJets();
  std::string truthpartname = m_config->sgKeyMCParticle();
  const xAOD::JetContainer* truthJets = m_asgHelper->getJetContainer(truthjetname);
  const xAOD::TruthParticleContainer* truthpart = m_asgHelper->getTruthParticleContainer(truthpartname);

  if(!tthevt->truthAvailable()){
    fillTruthJets(*truthJets, tthevt->m_truthJets);
    fillTruthParticles(tthevt, truthpart);
    fillTruthMatching(tthevt->m_HFHadrons, tthevt->m_truthJets);
    tthevt->truthAvailable(true);
  }

  decorateTTHbbEvent(tthevt);
  copyEventDecorations(plEvent.m_info, tthevt);
  /// nothing to be done for now here  
  /// will be cleaned in EventSaver

  return true;

}

std::string TTHbbTruthEDMConverter::name() const{

  return "TTHBBTRUTHEDM";

}

void TTHbbTruthEDMConverter::copyEventDecorations(const xAOD::EventInfo* info, std::shared_ptr<TTHbb::Event> tthevt) const{

  if(info->isAvailable<int>("TopHeavyFlavorFilterFlag")){
    tthevt->intVariable("TopHeavyFlavorFilterFlag") = info->auxdata<int>("TopHeavyFlavorFilterFlag");
  }
  else{
    tthevt->intVariable("TopHeavyFlavorFilterFlag") =-1;
  }

  if(info->isAvailable<int>("HadTop200")){
    tthevt->intVariable("HadTop200") = info->auxdata<int>("HadTop200");
  }
  else{
    tthevt->intVariable("HadTop200") =-1;
  }

  if(info->isAvailable<int>("TTbar150")){
    tthevt->intVariable("TTbar150") = info->auxdata<int>("TTbar150");
  }
  else{
    tthevt->intVariable("TTbar150") =-1;
  }


}


void TTHbbTruthEDMConverter::fillTruthParticles(std::shared_ptr<TTHbb::Event> tthevt, 
						const xAOD::TruthParticleContainer* truthpart) const{


  for(const xAOD::TruthParticle*  part : *truthpart) {//loop truth
    if(ParticleIDHelper::isGeant(part->barcode())) break; /// we are at geant and we don't want those
    fillHFHadrons(part, tthevt->m_HFHadrons);
  }
    /// for this one we need to reloop
  fillPartons(truthpart, tthevt->m_partons);

}


void TTHbbTruthEDMConverter::decorateTTHbbEvent(std::shared_ptr<TTHbb::Event> tthevt) const{

 
  /// recreate event truth decoration here

  int HDecay=-9;
  float higgs_eta=-9;
  float higgs_pt=-9;

  int nHiggs=0;
  int nLepTop=0;
  int nVectorBoson=0;

  int top_dilep_filter=0;

  float tbar_pt=-1;
  float top_pt=-1;
  float hadtop_pt=-1;
  float ttbar_pt=-1;

  TLorentzVector topp4;
  TLorentzVector antitopp4;
  int nt=0;
  int ntbar=0;

  for(auto part : tthevt->m_partons){
    long long info = part->historyInfo();
    /// we were using FS for all these variables
    if(!TTHbbTruth::isFinalState(info)) continue;

    int pdg = part->intVariable("pdgid");
    
    if(TTHbbTruth::isHadTop(pdg, info) && TTHbbTruth::isFinalState(info) ){
      hadtop_pt=part->Pt();
    }

    if(TTHbbTruth::isHiggs(pdg, info)){
      ++nHiggs;
      higgs_eta=part->p4().Eta();
      higgs_pt=part->p4().Pt();
      HDecay=part->intVariable("HDecay");
    }
    if(TTHbbTruth::isLepTop(pdg, info)){
      ++nLepTop;
    }
    if(6==pdg){
      topp4=part->p4();
      ++nt;
    }
    if(-6==pdg){
      antitopp4=part->p4();
      ++ntbar;
    }
    if(TTHbbTruth::isVectorBoson(pdg, info)){
      ++nVectorBoson;
    }
  }
  

  if(nHiggs!=1){
    higgs_eta=-8;
    higgs_pt=-8;
  }

  int nTop=nt+ntbar;
  bool isTTbar=false;

  if(1==nt && 1==ntbar){
    tbar_pt=antitopp4.Pt();
    top_pt=topp4.Pt();
    ttbar_pt=(antitopp4+topp4).Pt();
    isTTbar=true;
  }

  /// using nLepTop since the cut on lepton pT is 0 (and no filter but exclusive decay in new samples)
  top_dilep_filter=isTTbar && nLepTop==2;

  tthevt->intVariable("truth_HDecay")=HDecay;
  tthevt->floatVariable("truth_higgs_eta")=higgs_eta;
  tthevt->floatVariable("truth_higgs_pt")=higgs_pt;
  tthevt->intVariable("truth_nHiggs")=nHiggs;
  tthevt->intVariable("truth_nLepTop")=nLepTop;
  tthevt->intVariable("truth_nTop")=nTop;
  tthevt->intVariable("truth_nVectorBoson")=nVectorBoson;
  tthevt->intVariable("truth_top_dilep_filter")=top_dilep_filter;
  tthevt->floatVariable("truth_tbar_pt")=tbar_pt;
  tthevt->floatVariable("truth_top_pt")=top_pt;
  tthevt->floatVariable("truth_hadtop_pt")=hadtop_pt;
  tthevt->floatVariable("truth_ttbar_pt")=ttbar_pt;


  //// lets redecorate HF stuff

  int hfclassif = m_ttHFClassification->computeHFClassification(tthevt->m_truthJets);
  int simpleclassif= m_ttHFClassification->getSimpleClassification(hfclassif);
  m_ttHFClassification->getReweightingVariables(tthevt->m_truthJets, hfclassif, tthevt.get());

  tthevt->intVariable("HF_Classification")=hfclassif;
  tthevt->intVariable("HF_SimpleClassification")=simpleclassif;

  int ttbb_categories=0;
  if(simpleclassif ==1){
    ttbb_categories=5;
    if((hfclassif >= 1000) && (hfclassif < 1100)){
      ttbb_categories  = 1;//tt+b
    }
    else if((hfclassif >= 2000) && (hfclassif < 2100)){
      ttbb_categories  = 2; //tt+bb
    }
    else if ((hfclassif >= 100) && (hfclassif < 200)){
      ttbb_categories  = 3;//tt+B
    }
    else if (((hfclassif >= 1100) && (hfclassif < 2000)) || 
	     ((hfclassif >= 200) && (hfclassif < 1000)) 
	     || (hfclassif >= 2100)){
      ttbb_categories  = 4;//tt+>=3b
    }
  }
  tthevt->intVariable("ttbb_categories")=ttbb_categories;

  int nPDFFlavor=5;
  int dsid = tthevt->m_info->mcChannelNumber;
  if(dsid >= 410323 && dsid <= 410325)nPDFFlavor=4;

  tthevt->intVariable("nPDFFlavor")=nPDFFlavor;

}


void TTHbbTruthEDMConverter::fillTruthJets(const xAOD::JetContainer& orig, TTHbb::JetContainer& current) const{
  
  for(auto jet : orig){
    auto tthjet = std::make_shared<TTHbb::Jet>(jet->pt(), jet->eta(), jet->phi(), jet->e());
    // if(jet->pt()<10000.) continue;
    // if(fabs(jet->eta())>3.) continue;
    tthjet->id = current.size();
    current.push_back(tthjet);
  }
  JetOrderingTool::sortPt(current);

}

void TTHbbTruthEDMConverter::fillHFHadrons(const xAOD::TruthParticle* part, TTHbb::TruthParticleContainer& container) const{

  int OriginFlag=6;
  if(part->isAvailable<int>("TopHadronOriginFlag")){
    OriginFlag=part->auxdata<int>("TopHadronOriginFlag");
  }

  /// lets keep all HF hadrons (initial and final), will see later what we need
  if(OriginFlag==6 && !ParticleIDHelper::isHFHadron(part->pdgId()) ) return;

  std::shared_ptr<TTHbb::TruthParticle> tthpart = TruthxAODPartHelper::createTruthPart(part);
  tthpart->intVariable("TopHadronOriginFlag") = OriginFlag;
  int type=0;
  if(ParticleIDHelper::isCHadron(part->pdgId())) type=4;
  if(ParticleIDHelper::isBHadron(part->pdgId())) type=5;

  tthpart->charVariable("hadronType") = type;

  tthpart->charVariable("isInitial") = m_truthHistoryHelper->isHFHadronInitialState(part, type);
  tthpart->charVariable("isFinal") = m_truthHistoryHelper->isHFHadronFinalState(part, type);

  container.push_back(tthpart);

}

void TTHbbTruthEDMConverter::fillPartons(const xAOD::TruthParticleContainer* truthcontainer, TTHbb::TruthParticleContainer& container) const{

  std::set<const xAOD::TruthParticle*> partons;

  m_partonTruthFinder->fillPartons(truthcontainer, partons);

  for(auto part : partons){

    std::shared_ptr<TTHbb::TruthParticle> tthpart = TruthxAODPartHelper::createTruthPart(part);
    long long info=0;
    if(part->isAvailable<long long>("TTHbbGeneralTruthParticleLabel")){
      info=part->auxdecor<long long>("TTHbbGeneralTruthParticleLabel");
    }
    tthpart->historyInfo(info);
    int hdecay=-1;
    if(part->isAvailable<int>("TTHbbHiggsDecayFlag")){
      hdecay=part->auxdecor<int>("TTHbbHiggsDecayFlag");
    }
    tthpart->intVariable("HDecay")=hdecay;
    //if(!TTHbbTruth::isFinalState(info)) continue;
    container.push_back(tthpart);
  }

}


void TTHbbTruthEDMConverter::fillTruthMatching(TTHbb::TruthParticleContainer& truthHF, 
					       TTHbb::JetContainer& jets) const{


  /// lets do object level stuff once per event here
  m_jetMatchingTool->jetPtCut(-1);
  m_jetMatchingTool->jetEtaCut(-1);
  m_jetMatchingTool->drCut(0.4);

  m_jetMatchingTool->matchHadronsToJets(truthHF, jets, "HFHadronJetsAssoc");
  m_ttHFClassification->flagJets(jets, "HFHadronJetsAssoc");

}
