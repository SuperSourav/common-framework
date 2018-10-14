/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/

#ifndef SSOSDilepSelector_H_
#define SSOSDilepSelector_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;

namespace SM4topsSS3L {

/**
 * @brief To accept an event based on a check of the charge of electrons and muons
 */
class SSOSDilepSelector : public top::EventSelectorBase
{
public:
    SSOSDilepSelector(const std::string& params, std::shared_ptr<top::TopConfig> config);
    ~SSOSDilepSelector() = default;
    /**
     * @brief Checks whether the event is SS or (OS):
     * requires two tight leptons within the leading three (or two) loose
     * leptons, and that the two tight leptpns are SS (OS)
     *
     *@params params: bool isOS, bool useLooseLeptons

     * @return True to keep the event, false otherwise.
     */
    bool apply(const top::Event& event) const override;

    std::string name() const override;

private:
    bool m_isOS;
    bool m_useLooseLeptons;

  };
}
#endif
