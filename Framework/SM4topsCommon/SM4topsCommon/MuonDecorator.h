
#ifndef MuonDecorator_H_
#define MuonDecorator_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "xAODTracking/VertexContainer.h"
#include "IsolationSelection/IsolationSelectionTool.h"
#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;

namespace SM4topsCommon {
class MuonDecorator:public top::EventSelectorBase {

 public:

  MuonDecorator(const std::string &selectorName, const std::string &params, std::shared_ptr<top::TopConfig> config);
  ~MuonDecorator() = default;
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

  std::vector<std::string> m_muonTriggerNames;
  std::vector<std::string> m_muonSLTriggerNames;
  std::vector<std::string> m_muonDLTriggerNames;
  };
}

#endif
