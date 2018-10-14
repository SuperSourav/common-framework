/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _HPLUSDISCRIMINANT_H_
#define _HPLUSIDSCRIMINANT_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

#include "HplusDiscriminant/HplusDiscriminantCalculator.h"

#include <string>
#include <map>
#include <utility>
#include <memory>

class HplusDiscriminant : public top::EventSelectorBase{

 public:
  
  HplusDiscriminant(std::string params, std::shared_ptr<top::TopConfig> config);
  ~HplusDiscriminant();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  TTHbb::HplusDiscriminantCalculator* m_calculator;
  std::vector<int> m_masspoints; // in GeV
  bool m_store_apmmj;
  bool m_store_likelihood;

};

#endif
