/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/
#ifndef TrigMatch_H_
#define TrigMatch_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include <memory>
#include "TopConfiguration/TopConfig.h"

namespace SM4topsSS3L {
  // // Forward declare
  // class Event;
  // class TopConfig;

  /**
  * @brief Match offline object to trigger.
  *
  */
  class TrigMatch : public top::EventSelectorBase {
    public:

      TrigMatch(const std::string& selectorName, std::shared_ptr<top::TopConfig> config);

      virtual bool apply(const top::Event& event) const override;

      std::string name() const override;

    private:
      ///List of triggers to 'or' together for each event. If any one passes, the event passes
      std::vector<std::string> m_allTriggers_Loose;
      std::vector<std::string> m_electronTriggers_Loose;
      std::vector<std::string> m_muonTriggers_Loose;
      std::vector<std::string> m_allTriggers_Tight;
      std::vector<std::string> m_electronTriggers_Tight;
      std::vector<std::string> m_muonTriggers_Tight;
  };

}
#endif /* TrigMatch_H_ */
