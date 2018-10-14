/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/
#ifndef LeptonChannelSelector_H_
#define LeptonChannelSelector_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

namespace SM4topsSS3L {

class LeptonChannelSelector : public top::EventSelectorBase
{
public:
    /**
     * @brief Select events based on multiplicity, which should be an integer.
     */
    explicit LeptonChannelSelector(const std::string& params, std::shared_ptr<top::TopConfig> config);

    /**
     * @brief Checks whether the event passes the leptonic channel selection:
     * for ee channel, requires two tight leptons within the leading three (or
     * two) loose leptons, and that both are electrons; and so on
     *
     * @return True to keep the event, false otherwise.
     */
    bool apply(const top::Event& event) const override;

    std::string name() const override;

private:
    enum SM4topsSS3LChannel
    {
        UNDEFINED = 0,
        EE = 1,
        MM = 2,
        EM = 3,
        DILEP = 4,
        EEE = 5,
        EEM = 6,
        EMM = 7,
        MMM = 8,
        TRILEP = 9,
        DILEPORTRILEP = 10,
        SM4topsSS3LChannelMAX = 11
        //NSSBJETSCH = 10
    };

    int m_channel;
    bool m_use_loose_leptons;
};

}

#endif
