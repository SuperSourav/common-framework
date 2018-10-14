#ifndef ZVetoSelector_H_
#define ZVetoSelector_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace SM4topsSS3L {
/**
 * @brief Reject events if the invariant mass of any pair of leptons is in the mass window of the Z. This applies to pairs of same-sign, same-flavour leptons in the three leading leptons.
 */
class ZVetoSelector : public top::EventSelectorBase {
public:
    explicit ZVetoSelector(std::string& params);

    bool apply(const top::Event& event) const override;

    std::string name() const override;

private:
    double m_cutvalue0;
    double m_cutvalue1;

    bool m_useLooseLeptons;
    bool m_is_in_window;
  };
}
#endif
