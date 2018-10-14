/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetTruthMatchingTool_H_
#define JetTruthMatchingTool_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

#include "TTHbbMatchingTools/JetMatchingTool.h"
#include "TTHbbObjects/Event.h"


class JetTruthMatchingTool:public top::EventSelectorBase {

 public:

  JetTruthMatchingTool(std::string params, std::shared_ptr<top::TopConfig> config);
  ~JetTruthMatchingTool();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:


  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  template <class T>
  void computeMatchingFlags(T jets) const;
  int decorateHiggsMatching(std::shared_ptr<TTHbb::TruthParticle> part) const;
  int decorateTopMatching(std::shared_ptr<TTHbb::TruthParticle> part) const;
  int decorateBSMHiggsMatching(std::shared_ptr<TTHbb::TruthParticle> part) const;
  int decorateChargedHiggsMatching(std::shared_ptr<TTHbb::TruthParticle> part) const;


  mutable TTHbb::JetMatchingTool m_jetMatchingTool;
  bool m_useLargeJets;
  float m_drCut;

};

#endif
