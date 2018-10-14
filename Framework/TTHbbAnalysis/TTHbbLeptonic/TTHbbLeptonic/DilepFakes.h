/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _DILEPFAKES_H_
#define _DILEPFAKES_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

#include "DilepFakes/DilepFakesCalculator.h"
#include "DilepFakes/DilepFakesAppender.h"
#include "TTHbbObjects/Event.h"

#include <string>

class DilepFakes : public top::EventSelectorBase {

 public:

  DilepFakes(std::string params, std::shared_ptr<top::TopConfig> config);
  ~DilepFakes();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  float m_weightNorm_subleadElChan;
  float m_weightNorm_subleadMuChan;

  std::shared_ptr<TTHbb::DilepFakesCalculator> m_fakesCalculator;
};

#endif // _DILEPFAKES_H_
