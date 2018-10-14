
#ifndef ElectronDecorator_H_
#define ElectronDecorator_H_

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEventSelectionTools/EventSelectorBase.h"

#include <AsgTools/MessageCheck.h>
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"

using namespace asg::msgUserCode;

namespace SM4topsCommon {

class ElectronDecorator : public top::EventSelectorBase {
 public:
  ElectronDecorator(const std::string &selectorName, const std::string &params,
                    std::shared_ptr<top::TopConfig> config);
  ~ElectronDecorator() = default;
  // void initialise(const top::Event& event) const;
  virtual bool apply(const top::Event &event) const override;

  std::string name() const override;

 private:
  std::vector<std::string> m_electronTriggerNames;
  std::vector<std::string> m_electronSLTriggerNames;
  std::vector<std::string> m_electronDLTriggerNames;

  AsgElectronChargeIDSelectorTool m_electronChargeIDLoose;
  AsgElectronChargeIDSelectorTool m_electronChargeIDMedium;
  AsgElectronChargeIDSelectorTool m_electronChargeIDTight;

};
}  // namespace SM4topsCommon
#endif
