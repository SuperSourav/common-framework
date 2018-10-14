/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ClassifyAndCalculateHF_HH
#define ClassifyAndCalculateHF_HH


#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/TruthParticle.h"
#include "TTHbbObjects/Jet.h"

#include <unordered_map>

class ClassifyAndCalculateHF{

public:

  ClassifyAndCalculateHF();
  ~ClassifyAndCalculateHF();
  

  inline void jetPtCut(float a){m_jetPtCut=a;}
  inline void jetEtaCut(float a){m_jetEtaCut=a;}
  inline void leadingHadronPtCut(float a){m_leadingHadronPtCut=a;}
  inline void leadingHadronPtRatioCut(float a){m_leadingHadronPtRatioCut=a;}


  void flagJets(TTHbb::JetContainer& jets, std::string hfmatchname, std::string hfDecorationName="HFClassification");
  int computeHFClassification(TTHbb::JetContainer& jets, std::string hfDecorationName="HFClassification");
  int getSimpleClassification(int hfclassif);
  void getReweightingVariables(TTHbb::JetContainer& jets, int hfclassif, TTHbb::BaseObject* event, std::string hfDecorationName="HFClassification");






private:

  float m_jetPtCut;
  float m_jetEtaCut;
  float m_leadingHadronPtCut;
  float m_leadingHadronPtRatioCut;

};


#endif
