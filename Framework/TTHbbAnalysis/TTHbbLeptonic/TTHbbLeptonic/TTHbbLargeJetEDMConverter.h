/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbLargeJetEDMConverter_H_
#define TTHbbLargeJetEDMConverter_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "AsgTools/AsgTool.h"
#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"
#include "TopObjectSelectionTools/RCJetMC15.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "MVAVariables/JetOrderingTool.h"

class TTHbbLargeJetEDMConverter:public top::EventSelectorBase {

 public:

  TTHbbLargeJetEDMConverter(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHbbLargeJetEDMConverter();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;
  TTHbbAsgToolHelper* m_asgHelper;

  void fillReclusteredJets(const top::Event& event, TTHbb::Event& tthevt) const;
  void fillLargeJets(const xAOD::JetContainer& orig, TTHbb::Event& event) const;

  std::string m_params;
  mutable RCJetMC15* m_rc;

};

#endif
