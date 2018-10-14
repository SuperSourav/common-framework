/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/

#ifndef MllSelector_H_
#define MllSelector_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace SM4topsSS3L {
/**
 * @brief Put a cut on the invariant mass of two same flavour leptons.  If you
 * want to veto a window (say for the Z mass) you instead want MLLWindow.
 */
class MllSelector : public top::SignValueSelector {
public:
    explicit MllSelector(const std::string& params, const bool& useLooseLeptons=false);

    bool apply(const top::Event& event) const override;
    std::string name() const override;

private:
    bool m_useLooseLeptons;
  };
}
#endif
