/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MCFAKES_H_
#define _MCFAKES_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

#include "MCFakes/MCFakesCalculator.h"
#include "MCFakes/MCFakesAppender.h"
#include "TTHbbObjects/Event.h"

#include <string>

class MCFakes : public top::EventSelectorBase {

 public:

  MCFakes(std::string params, std::shared_ptr<top::TopConfig> config);
  ~MCFakes();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  // std::shared_ptr<TTHbb::MCFakesCalculator> m_fakesCalculator;
};

#endif // _MCFAKES_H_
