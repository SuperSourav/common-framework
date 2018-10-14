/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbEDMConverter_H_
#define TTHbbEDMConverter_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "MVAVariables/JetOrderingTool.h"

#include <unordered_map>

class TTHbbEDMConverter:public top::EventSelectorBase {

 public:

  TTHbbEDMConverter(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHbbEDMConverter();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;
  ToolHandle<CP::IMuonSelectionTool> m_muonsel;
  mutable JetOrderingTool m_jetOrderingTool;

  void fillJets(const xAOD::JetContainer& orig, TTHbb::JetContainer& current) const;
  void fillElectrons(const xAOD::ElectronContainer& orig, TTHbb::LeptonContainer& current) const;
  void fillMuons(const xAOD::MuonContainer& orig, TTHbb::LeptonContainer& current) const;

  bool isPromptElec(int type, int orig) const;
  bool isBremElec(int type, int orig, int origBkg) const;
  bool isPromptMuon(int type, int orig) const;

  // Temporary solution to allow pseudo-continuous tag weight bins
  int calculateJetTagWeightBin(double tagweight) const;

  std::string m_params;

  mutable std::unordered_map<std::string,int> m_nBTagsOP;

  // We are defining simplified variables based on a single tagger, so let us be consistent
  std::string m_chosenBTagger; 
};

#endif
