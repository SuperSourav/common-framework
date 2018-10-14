/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/TTHBoostedSemilepReco.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbReconstruction/JetInCombinationDecorationTool.h"

#include "PathResolver/PathResolver.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHBoostedSemilepReco::TTHBoostedSemilepReco()
 {
 

   m_nJets=0;
   m_nbJets=0;
   m_leptonType=0;
   m_isMC=false;

   m_regionName="";
   m_regionVal=-1;

   initTool();
}

TTHBoostedSemilepReco::~TTHBoostedSemilepReco(){
  delete m_recoCombinationTool;

}


void TTHBoostedSemilepReco::initTool(){
  m_recoCombinationTool=new RecoCombinationsMakerTool();
  m_recoCombinationTool->m_combinationsDecorator.m_contBTagNames =  {"tagWeightBin"};
  
  m_recoCombinationTool->m_btagOrderVariableName = "";
  m_recoCombinationTool->m_btagIdName="isBtag";

}


TTHbb::CombinationContainer* TTHBoostedSemilepReco::buildCombinations(TTHbb::JetContainer* jets,
								      TTHbb::JetContainer* hadtopjets,
							       TTHbb::LeptonContainer* leptons,
							       TTHbb::NeutrinoContainer* neutrinos) const{
if (m_isMC)
    m_recoCombinationTool->m_isMC = true;
  m_recoCombinationTool->buildStrategyTTHBoostedSemiLep(jets);

  CompositeContainer *lepWContainer=0;
  CompositeContainer *hadWContainer=0;
  CompositeContainer *higgsContainer=0;
  CompositeContainer *incompleteHadTopContainer=0;
  TopCompositeContainer *lepTopContainer=0;
  TopCompositeContainer *hadTopContainer=0;
  CombinationContainer* combContainer=0;
 
  lepWContainer = m_recoCombinationTool->createLepWContainer(leptons, neutrinos, "lepWContainer");
  //std::cout << "lepW size = " << lepWContainer->size() << std::endl;
  if(!lepWContainer){
    std::cout << "WARNING " << name() << " could not create lepWContainer " << std::endl;
    return 0;
  }
  
  // pass only the subjets of the hadtop
  hadWContainer = m_recoCombinationTool->createBoostedHadWContainer(hadtopjets, "hadWContainer");
  //std::cout << "hadW size = " << hadWContainer->size() << std::endl;
  if(!hadWContainer){
    std::cout << "WARNING " << name() << " could not create hadWContainer " <<  std::endl;
    return 0;
  }
  

  higgsContainer = m_recoCombinationTool->createHiggsContainer(jets, "higgsContainer");
  //std::cout << "higgs size = " << higgsContainer->size() << std::endl;
  if(!higgsContainer){
    std::cout << "WARNING " << name() << " could not create higgsContainer " <<  std::endl;
    return 0;
  }

  lepTopContainer = m_recoCombinationTool->createLepTopContainer(lepWContainer, jets, "lepTopContainer");
  //std::cout << "leptop size = " << lepTopContainer->size() << std::endl;
  if(!lepTopContainer){
    std::cout << "WARNING " << name() << " could not create lepTopContainer " << std::endl;
    return 0;
  }

  // is the W Boosted?
  if ( !(hadWContainer[0][0]->child2 )){
    incompleteHadTopContainer = m_recoCombinationTool->createBoostedHadTopContainer(hadWContainer, hadtopjets, "hadTopContainer");
    //std::cout << "incompletehadtop size = " << incompleteHadTopContainer->size() << std::endl;
    if(!incompleteHadTopContainer){
      std::cout << "WARNING " << name() << " could not create incompleteHadTopContainer " <<  std::endl;
      return 0;
    }
      
    combContainer = m_recoCombinationTool->createIncompleteCombinations(lepTopContainer, incompleteHadTopContainer, higgsContainer, "combContainer");
    //std::cout << "comb size = " << combContainer->size() << std::endl;
    if(!combContainer){
      std::cout << "WARNING " << name() << " could not create combContainer " << std::endl;
      return 0;
    }
  }
  else{  // not a boosted W, so build normal top
    hadTopContainer = m_recoCombinationTool->createHadTopContainer(hadWContainer, hadtopjets, "hadTopContainer");
    //std::cout << "hadtop size = " << hadTopContainer->size() << std::endl;
    if(!hadTopContainer){
      std::cout << "WARNING " << name() << " could not create hadTopContainer " <<  std::endl;
      return 0;
    }
    
    combContainer = m_recoCombinationTool->createCombinations(lepTopContainer, hadTopContainer, higgsContainer, "combContainer");
    //std::cout << "comb size = " << combContainer->size() << std::endl;
    if(!combContainer){
      std::cout << "WARNING " << name() << " could not create combContainer " << std::endl;
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



std::string TTHBoostedSemilepReco::name() const{

  return "TTHBOOSTEDSEMILEPRECO";

}



