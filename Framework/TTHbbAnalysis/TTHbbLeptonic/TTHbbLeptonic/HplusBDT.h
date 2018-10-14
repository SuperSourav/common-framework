/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBB_HplusBDT_H_
#define _TTHBB_HplusBDT_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"


#include <vector>
#include <string>
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include <map>
#include <utility>
#include <memory>

class HplusBDT : public top::EventSelectorBase{

public:
    HplusBDT(std::string params, std::shared_ptr<top::TopConfig> config);
    ~HplusBDT();

    bool apply(const top::Event& event) const override;
    std::string name() const override;

private:
    std::vector<std::string> grabVariableList(std::string filename);
private:
    std::shared_ptr<top::TopConfig> m_config;
    std::string m_params;
    std::string m_Selection;
    float m_mv2WP;
    bool m_isTRF;
    std::string m_mv2Algo;
    mutable unsigned int m_nbjets;
    mutable unsigned int m_njets;
    mutable std::shared_ptr<TTHbb::Event> m_event;

    mutable std::map<std::string, TMVA::Reader*> m_tmvaReaders;
    std::set<std::string> m_variableSet;
    std::set<std::string> m_dilepVariableSet;
    std::set<std::string> m_semilepVariableSet;
    mutable std::map<std::string, float> m_variableMap;
    std::vector<std::string> m_masspoints;
    std::vector<std::string> m_regions;
    mutable std::map<std::pair<unsigned int, unsigned int>,std::vector<std::string>> m_BDT;
    mutable std::map<unsigned int,std::vector<std::string>> m_TRFBDT;
    mutable std::vector<std::string> m_TRFTypes;
};


#endif
