/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRFCALC_H_
#define TRFCALC_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbTRFCalc/TTHbbTRFCalc.h"
#include "TopConfiguration/TopConfig.h"

class TRFCalc:public top::EventSelectorBase {

 public:

  TRFCalc(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TRFCalc();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

  bool Decorate(std::shared_ptr<TTHbb::Event> tthevt, CombinedTRF members) const;
  mutable TTHbbTRFCalc* calc;

 private:
  std::string m_params;
  std::shared_ptr<top::TopConfig> m_config;
  unsigned int m_nbtags;
  std::map<std::string, std::string> m_trf_config;
  std::string wp_lab;

};

#endif
