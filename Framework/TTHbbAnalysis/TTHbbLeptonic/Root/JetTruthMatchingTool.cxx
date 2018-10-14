/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/JetTruthMatchingTool.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbTruthHelper/ParticleIDHelper.h"
#include "TTHbbTruthHelper/GeneralTruthHelper.h"
#include "TTHbbTruthHelper/TruthMatchHelper.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

JetTruthMatchingTool::JetTruthMatchingTool(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
{
 
  m_params=params;
  std::string jetcoll_str; 


  if(!m_config->isMC()) return;

  std::stringstream ss( params );
  if( !(ss >> jetcoll_str >> m_drCut) ){
    std::cout << " ERROR in JetTruthMatchingTool config" << std::endl;
    std::cout << "Use JetTruthMatchingTool in the form\n\tJetTruthMatching JetCollection deltaRvalue"
              << "\ne.g. JETTRUTHMATCHING SMALLJETS 0.3\nSetting to default SMALLJETS 0.3" << std::endl;
    abort();
  }
  

  if(jetcoll_str == "SMALLJETS"){
    std::cout << "--- Matching small jets as follows: dR < " << m_drCut << std::endl;
    m_useLargeJets=false;
  }
  else if(jetcoll_str == "LARGEJETS"){
    std::cout << "--- Matching large jets as follows: dR < " << m_drCut << std::endl;
    m_useLargeJets=true;
  }
  else{
    std::cout << "--- JetTruthMatching set up not recognised" << std::endl;
    abort();
  }

  /// register the jet decorations that you want to dump
  auto* esh = EventSaverHelper::get();
  if(m_useLargeJets){
    esh->addLJetVariableToBeSaved("truthmatch", TTHbb::DecorationType::Int,true,"JetTruthMatchingTool");
    esh->addLJetVariableToBeSaved("truthmatch_init", TTHbb::DecorationType::Int,true,"JetTruthMatchingTool");
  }
  else{
    esh->addJetVariableToBeSaved("truthmatch", TTHbb::DecorationType::Int,true,"JetTruthMatchingTool");
    esh->addJetVariableToBeSaved("truthmatch_init", TTHbb::DecorationType::Int,true,"JetTruthMatchingTool");
  }

}

JetTruthMatchingTool::~JetTruthMatchingTool(){

}

bool JetTruthMatchingTool::apply(const top::Event & event) const{

  //// 

  if(!m_config->isMC()) return true;
  if( !(m_config->nominalHashValue() == event.m_hashValue )) return true; /// nominal only

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");


  if(m_useLargeJets){
    m_jetMatchingTool.inclusiveDeltaRMatch(tthevt->m_largeJets, tthevt->m_partons, m_drCut, "JetPartonMatching");
    computeMatchingFlags(tthevt->m_largeJets);
  }
  else{
    m_jetMatchingTool.inclusiveDeltaRMatch(tthevt->m_jets, tthevt->m_partons, m_drCut, "JetPartonMatching");
    computeMatchingFlags(tthevt->m_jets);
  }

  
  return true;

}

std::string JetTruthMatchingTool::name() const{

  return "JETTRUTHMATCHING";

}

template <class T>
void JetTruthMatchingTool::computeMatchingFlags(T Jets) const{



  for(auto jet : Jets){
    int finalflag=0;
    int initFlag=0;
    for(auto a : jet->particleMatch("JetPartonMatching") ){
      auto pp = a.lock();
      if(!pp){
	std::cout << "WARNING JetTruthMatchingTool::computeMatchingFlags: particle-jet association expired" << std::endl;
	continue;
      }

      if(TTHbb::PhysObjectType::BasicTruthParticleType != pp->objectType()){
	std::cout << "Warning computeMatchingFlags pointer is not truth particle "
		  << (int)pp->objectType() << std::endl;
	continue;
      }
      std::shared_ptr<TTHbb::TruthParticle> part = std::static_pointer_cast<TTHbb::TruthParticle>(pp);

      long long info = part->historyInfo();
      int flag=0;
      flag|=decorateHiggsMatching(part);
      flag|=decorateTopMatching(part);
      flag|=decorateBSMHiggsMatching(part);
      flag|=decorateChargedHiggsMatching(part);
      if(TTHbbTruth::isInitialState(info)){
	initFlag|=flag;
      }
      if(TTHbbTruth::isFinalState(info)){
	finalflag|=flag;
      }
    }
    
    jet->intVariable("truthmatch")=finalflag;
    jet->intVariable("truthmatch_init")=initFlag;

  }

}


int JetTruthMatchingTool::decorateHiggsMatching(std::shared_ptr<TTHbb::TruthParticle> part) const{

  long long info = part->historyInfo();
  int pdg=part->intVariable("pdgid");

  if(!TTHbbTruth::isFromHiggs(info)) return 0;
  if(!ParticleIDHelper::isbq(pdg)) return 0;

  if( TTHbbTruth::isLeadingFromHiggs(info) ) return 1<<TTHbbTruth::JetMatchLeadingBHiggs;
  if( TTHbbTruth::isSubleadingFromHiggs(info) ) return 1<<TTHbbTruth::JetMatchSubLeadingBHiggs;
  
  std::cout <<"WARNING: JetTruthMatchingTool b from higgs but not leading or subleading " << std::endl;
  return 0;

}

int JetTruthMatchingTool::decorateTopMatching(std::shared_ptr<TTHbb::TruthParticle> part) const{

    
  long long info = part->historyInfo();
  int pdg=part->intVariable("pdgid"); 
  if(!ParticleIDHelper::isQuark(pdg)) return 0;
  if(ParticleIDHelper::isTop(pdg)) return 0;
 
  int flag=0;
  if(ParticleIDHelper::isbq(pdg) && TTHbbTruth::isDirectlyFromTop(info)){

    if(TTHbbTruth::isFromHadTop(info)) flag|= 1<<TTHbbTruth::JetMatchBHadTop;
    else if(TTHbbTruth::isFromLepTop(info)) flag|= 1<<TTHbbTruth::JetMatchBLepTop;
    else std::cout <<"WARNING: decorateTopMatching b from top but not had or lep " << std::endl;
    if(pdg == 5) flag|= 1<<TTHbbTruth::JetMatchBTop;
    if(pdg == -5) flag|= 1<<TTHbbTruth::JetMatchBAntiTop;

  }
  else if(!TTHbbTruth::isDirectlyFromTop(info) && TTHbbTruth::isFromHadTop(info)){

    if( TTHbbTruth::isLeadingFromWTop(info) ) flag|= 1<<TTHbbTruth::JetMatchLeadingJetW;
    else if( TTHbbTruth::isSubleadingFromWTop(info) ) flag|= 1<<TTHbbTruth::JetMatchSubLeadingJetW;
    else std::cout <<"WARNING: decorateTopMatching q from W but not leading or subleading " 
		   << pdg << "  " << info << std::endl;

    if( TTHbbTruth::isFromTop(info) ) flag|= 1<<TTHbbTruth::JetMatchJetWplus;
    else if( TTHbbTruth::isFromAntiTop(info))  flag|= 1<<TTHbbTruth::JetMatchJetWminus;
    else std::cout <<"WARNING: decorateTopMatching q from W but not leading or subleading " 
		   << pdg << "  " << info << std::endl;
  }
      
  return flag;
}


int JetTruthMatchingTool::decorateBSMHiggsMatching(std::shared_ptr<TTHbb::TruthParticle> part) const{
  
  int shift = -1;
  long long info = part->historyInfo();

  if(TTHbbTruth::isLeadingChildFromLeadingA(info)) shift=TTHbbTruth::JetMatchLeadingChildFromLeadingA;
  if(TTHbbTruth::isSubleadingChildFromLeadingA(info)) shift=TTHbbTruth::JetMatchSubleadingChildFromLeadingA;
  if(TTHbbTruth::isLeadingChildFromSubleadingA(info)) shift=TTHbbTruth::JetMatchLeadingChildFromSubleadingA;
  if(TTHbbTruth::isSubleadingChildFromSubleadingA(info)) shift=TTHbbTruth::JetMatchSubleadingChildFromSubleadingA;

  if(shift<0) return 0;
  return 1<<shift;


}


int JetTruthMatchingTool::decorateChargedHiggsMatching(std::shared_ptr<TTHbb::TruthParticle> part) const{
  
  int shift = -1;  
  long long info = part->historyInfo();
  int pdg=part->intVariable("pdgid"); 
  if(!ParticleIDHelper::isbq(pdg)) return 0;

  if(TTHbbTruth::isAssociatedBwithChargedHiggs(info)) shift=TTHbbTruth::JetMatchAssociatedB;
  if(TTHbbTruth::isFromChargedHiggs(info)) shift=TTHbbTruth::JetMatchBChargedHiggs;

  if(shift<0) return 0;
  return 1<<shift;


}

