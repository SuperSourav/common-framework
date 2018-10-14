/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DummyVariableTool_H_
#define DummyVariableTool_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

class DummyVariableTool:public top::EventSelectorBase {

 public:

  DummyVariableTool(std::string params, std::shared_ptr<top::TopConfig> config);
  ~DummyVariableTool();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;

  std::string m_params;

};

#endif
