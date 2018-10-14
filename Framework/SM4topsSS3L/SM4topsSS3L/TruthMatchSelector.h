#ifndef TRUTHMATCHSELECTOR_H_
#define TRUTHMATCHSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

namespace SM4topsSS3L {
  // Forward declare

/**
 * @brief To accept an event based on a check of electrons and muons.
 */
class TruthMatchSelector : public top::EventSelectorBase {
public:
    /**
     * @brief Select events based on truth matching.
     */
    explicit TruthMatchSelector(const std::string& params, std::shared_ptr<top::TopConfig> config);

    /**
     * @brief Checks whether the selected leptons are truth matched
     *
     * @return True to keep the event, false otherwise.
     */
    bool apply(const top::Event& event) const override;

    std::string name() const override;

private:
    enum SSbjetsChannel
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
        NSSBJETSCH = 10
        //DILEPORTRILEP = 10,
        //NSSBJETSCH = 11 };
    };

    int m_channel;
    bool m_useLooseLeptons;
    bool m_isMC;
  };

}

#endif
