/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTED_BOOSTEDSELECTION_H
#define BOOSTED_BOOSTEDSELECTION_H

// TODO: actually link BoostedVariables tool

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

// namespace TTHbb {

#include "BoostedVariables/BoostedVariables.h"

class BoostedSelection:public top::EventSelectorBase {

 public:

  BoostedSelection(std::string params, std::shared_ptr<top::TopConfig> config);
  ~BoostedSelection();
  bool apply(const top::Event & event) const override;

  std::string name() const override;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;
  TTHbb::BoostedVariables m_boostedVariables;
};

// } // namespace TTHbb

#endif//BOOSTED_BOOSTEDSELECTION_H