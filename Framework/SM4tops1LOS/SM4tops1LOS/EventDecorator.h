
#ifndef EVENTDECORATOR_H_
#define EVENTDECORATOR_H_

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"

namespace SM4tops1LOS {

  class EventDecorator:public top::EventSelectorBase {

  public:

    EventDecorator(std::string params, std::shared_ptr<top::TopConfig> config);
    ~EventDecorator() = default;

    // void initialise(const top::Event& event) const;
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
    void decorateMll(TTHbb::Event* tthevt) const;
    void decorateMtw(TTHbb::Event* tthevt) const;
    int  lepFlavChannel(TTHbb::Event* tthevt) const;

  };

}
#endif
