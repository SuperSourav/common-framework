
#ifndef LeptonVectorDecorator_H_
#define LeptonVectorDecorator_H_

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEventSelectionTools/EventSelectorBase.h"

#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;

namespace SM4topsSS3L {

class LeptonVectorDecorator : public top::EventSelectorBase {
 public:
  LeptonVectorDecorator(std::string params, std::shared_ptr<top::TopConfig> config);
  ~LeptonVectorDecorator() = default;
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event& event) const override;
  std::string name() const override;

  struct Lepton {
    float pt;
    float eta;
    float phi;
    float e;
    float charge;
    int pdgId;
    char isTight;
    unsigned int origIndex;
  };

 private:
  std::vector<unsigned int> m_indices;
  std::string getVarName(const unsigned int& index, const std::string& varName) const;
};

}  // namespace SM4topsSS3L
#endif
