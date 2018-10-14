/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "TLorentzVector.h"
#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

#include "TrigDecisionTool/TrigDecisionTool.h"


#ifndef TriggerPrescales_H_
#define TriggerPrescales_H_

class TriggerPrescales:public top::EventSelectorBase {
  public:

    TriggerPrescales(std::string params, std::shared_ptr<top::TopConfig> config);
    ~TriggerPrescales();

//    void initialise(const top::Event& event) const;
    bool apply(const top::Event & event) const override;
    std::string name() const override;

  private:
    bool m_debug;

    //event
    mutable const top::Event* m_event;
    std::shared_ptr<top::TopConfig> m_config;

    std::string m_params;

    //      //functions
    void DecorateTriggerPrescales() const;
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;// for trigger prescales
};
#endif
