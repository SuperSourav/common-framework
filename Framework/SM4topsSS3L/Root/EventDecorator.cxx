#include "SM4topsSS3L/EventDecorator.h"

#include <sstream>
#include <algorithm>

#include "TopEvent/EventTools.h"


#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "../TTHbbAnalysis/TTHbbLeptonic/TTHbbLeptonic/EventSaverHelper.h"
#include "../TTHbbAnalysis/TTHbbObjects/TTHbbObjects/Event.h"

namespace SM4topsSS3L {

EventDecorator::EventDecorator(std::string params,std::shared_ptr<top::TopConfig> config) {
  
}

bool EventDecorator::apply(const top::Event & event) const{
  return true;
}

std::string EventDecorator::name() const{
  return "SM4topsSS3LEventDecorator";
}

}
