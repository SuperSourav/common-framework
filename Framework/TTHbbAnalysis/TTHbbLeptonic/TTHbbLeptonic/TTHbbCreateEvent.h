/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbCreateEvent_H_
#define TTHbbCreateEvent_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"


class TTHbbCreateEvent:public top::EventSelectorBase {

 public:

  TTHbbCreateEvent(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHbbCreateEvent();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  bool applyParticleLevel(const top::ParticleLevelEvent & event) const override;
  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;


  std::string m_params;


};

#endif
