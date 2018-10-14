/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbTruthEDMConverter_H_
#define TTHbbTruthEDMConverter_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "AsgTools/AsgTool.h"
#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/TruthHistoryHelper.h"
#include "TTHbbLeptonic/PartonTruthFinder.h"
#include "TTHbbMatchingTools/JetMatchingTool.h"
#include "TTHFClassificationTools/ClassifyAndCalculateHF.h"

class TTHbbTruthEDMConverter:public top::EventSelectorBase{

 public:

  TTHbbTruthEDMConverter(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHbbTruthEDMConverter();

  bool apply(const top::Event & event) const override;
  bool applyParticleLevel(const top::ParticleLevelEvent & plEvent) const override;

  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;
  TTHbbAsgToolHelper* m_asgHelper;
  std::string m_params;

  void fillTruthParticles(std::shared_ptr<TTHbb::Event> tthevt, 
			  const xAOD::TruthParticleContainer* truthpart) const;
  void decorateTTHbbEvent(std::shared_ptr<TTHbb::Event> tthevt) const;
  void fillTruthJets(const xAOD::JetContainer& orig, TTHbb::JetContainer& current) const;
  void fillHFHadrons(const xAOD::TruthParticle* part, TTHbb::TruthParticleContainer& container) const;
  void fillPartons(const xAOD::TruthParticleContainer* truthcontainer, 
		   TTHbb::TruthParticleContainer& container) const;
  void fillTruthMatching(TTHbb::TruthParticleContainer& truthHF, 
			 TTHbb::JetContainer& jets)const;
  void copyEventDecorations(const xAOD::EventInfo* info, std::shared_ptr<TTHbb::Event> tthevt) const;

  TruthHistoryHelper* m_truthHistoryHelper;
  PartonTruthFinder* m_partonTruthFinder;
  TTHbb::JetMatchingTool* m_jetMatchingTool;
  ClassifyAndCalculateHF* m_ttHFClassification;

};

#endif
