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


#ifndef H4bAnalysisVariables_H_
#define H4bAnalysisVariables_H_

class H4bAnalysisVariables:public top::EventSelectorBase {
  public:

    H4bAnalysisVariables(std::string params, std::shared_ptr<top::TopConfig> config);
    ~H4bAnalysisVariables();

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
    void DecoratePrimaryVertex() const;
    void DecorateEventInfo() const;
    void DecorateTruthInfo() const;
};
#endif
