/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/

#ifndef MllWindowSelector_H_
#define MllWindowSelector_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace SM4topsSS3L {
/**
 * @brief Reject events in the invariant mass (of two charged leptons) window.
 */
class MllWindowSelector : public top::EventSelectorBase {
public:
    explicit MllWindowSelector(std::string& params);

    bool apply(const top::Event& event) const override;

    std::string name() const override;

private:
    double m_cutvalue0;
    double m_cutvalue1;

    bool m_use_loose_leptons;
    bool m_is_in_window;
  };
}
#endif
