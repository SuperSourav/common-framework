
#ifndef EVENTDECORATOR_H_
#define EVENTDECORATOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;

namespace SM4topsSS3L {

class EventDecorator:public top::EventSelectorBase {

 public:

  EventDecorator(std::string params, std::shared_ptr<top::TopConfig> config);
  ~EventDecorator() = default;
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

};

}
#endif
