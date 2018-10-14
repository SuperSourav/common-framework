/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Added by MLB (AZ) to the SM4tops-common-framework, 2018/08/21
  Decorates jets with info related to q/g discrimination
*/

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"

#ifndef QGVar_H_
#define QGVar_H_

namespace SM4topsCommon {
class QGVar:public top::EventSelectorBase {
  public:

    QGVar(std::string params, std::shared_ptr<top::TopConfig> config);
    ~QGVar();

    //    void initialise(const top::Event& event) const;
    bool apply(const top::Event & event) const override;
    std::string name() const override;

  private:
    bool m_debug;

    TTHbbAsgToolHelper* m_asgHelper;

    //event
    mutable const top::Event* m_event;
    std::shared_ptr<top::TopConfig> m_config;

    std::string m_params;

    //functions
    const xAOD::Jet* FindxAODJets(std::shared_ptr<TTHbb::Jet>& jet,
				  const xAOD::JetContainer& xAODJets) const;

    void DecorateJetsWithQGVars() const;
    int FindHSVertexIdx() const;

  };
}
#endif
