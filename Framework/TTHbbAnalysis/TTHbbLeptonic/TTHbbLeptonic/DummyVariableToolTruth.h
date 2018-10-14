/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DummyVariableToolTruth_H_
#define DummyVariableToolTruth_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

class DummyVariableToolTruth:public top::EventSelectorBase {

 public:

  DummyVariableToolTruth(std::string params, std::shared_ptr<top::TopConfig> config);
  ~DummyVariableToolTruth();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & ) const {return true;};
  bool applyParticleLevel(const top::ParticleLevelEvent & plEvent) const;
  std::string name() const override;

 private:

  mutable const top::ParticleLevelEvent * m_plEvent;
  std::shared_ptr<top::TopConfig> m_config;

  std::string m_params;

};

#endif
