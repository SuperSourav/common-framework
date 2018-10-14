#include "SM4topsCommon/EventDecorator.h"

#include <sstream>
#include <algorithm>
#include <numeric>
#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "../TTHbbAnalysis/TTHbbLeptonic/TTHbbLeptonic/EventSaverHelper.h"
#include "../TTHbbAnalysis/TTHbbObjects/TTHbbObjects/Event.h"

namespace SM4topsCommon {

  EventDecorator::EventDecorator(std::string params,std::shared_ptr<top::TopConfig> config) 
  {

    // Event decoration:
    auto* ehs = EventSaverHelper::get();
    ehs->addVariableToBeSaved("HT_all", TTHbb::DecorationType::Float, false, "EventDecorator");
    ehs->addVariableToBeSaved("HT_jets", TTHbb::DecorationType::Float, false, "EventDecorator");
    ehs->addVariableToBeSaved("nRCJets", TTHbb::DecorationType::Int, false, "EventDecorator");
    ehs->addVariableToBeSaved("mJSum", TTHbb::DecorationType::Float, false, "EventDecorator");
  }

  bool EventDecorator::apply(const top::Event & event) const{

    // Check if TTHbb::Event exists
    if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
      std::cout << "EventDecorator: TTHbbEventVariables (TTHbb::Event*) object not found" 
		<< std::endl;
      std::cout << "------> aborting :-( " << std::endl;
      abort();
    }

    // Extract TTHbb::Event
    std::shared_ptr<TTHbb::Event> tthevt = 
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

    // Event decoration:
    if(!tthevt->floatVariable("HT_all")) { // if variables not already filled
      tthevt->floatVariable("HT_all") = top::ht(event);
      tthevt->floatVariable("HT_jets") = std::accumulate(event.m_jets.begin(), event.m_jets.end(), 0., [](float init, const xAOD::Jet *jet) {return init + jet->pt();});
      decorateRCJets(tthevt);
    }

    return true;
  }

  std::string EventDecorator::name() const{
    return "SM4SM4topsCommonEventDecorator";
  }

  void EventDecorator::decorateRCJets(std::shared_ptr<TTHbb::Event> tthevt) const {

    // Extract the RC jets
    auto rcjets = tthevt->m_customObj["rcjet"];

    int nRCJets=0, iJ=0;
    float mJSum=0;

    for (auto &rcjet: rcjets){      
      if( rcjet->Pt() < 200000 || fabs(rcjet->Eta()) > 2.0 ) continue;
      if( rcjet->M()>100000 ) nRCJets++;
      if( iJ<4 ) mJSum += rcjet->M();
      iJ++;
    }
    
    tthevt->intVariable("nRCJets") = nRCJets;
    tthevt->floatVariable("mJSum") = mJSum;
  }

}
