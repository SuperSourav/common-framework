/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbMatchingTools/JetMatchingTool.h"



namespace TTHbb{


  JetMatchingTool::JetMatchingTool(){
    m_jetPtCut=15000.;
    m_jetEtaCut=2.5;
    m_drCut=0.4;
  }
    
  JetMatchingTool::~JetMatchingTool(){
  }



  void JetMatchingTool::matchHadronsToJets(TruthParticleContainer& hadrons, JetContainer& jets, std::string matchname){

    //// the matching is uniq for hadrons but not for jets
    //// need to cut on jets before matching, not for hadrons where we can do it later
    //// however the current HFClassification is matching hadrons to the closest jet before the cuts
    //// and then cut on jets later - was this studied

    JetContainer seljets;  
    if(m_jetPtCut<=0 && m_jetEtaCut<0){
      seljets= jets;
    }
    else{
      for(auto jet : jets){
	if(jet->p4().Pt()<m_jetPtCut) continue;
	if(fabs(jet->p4().Eta())>m_jetEtaCut) continue;
	seljets.push_back(jet);
      }
    }
    simpleDeltaRMatch(hadrons, seljets, m_drCut, matchname);

  }










}

