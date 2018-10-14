/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/

#include "SM4topsSS3L/MllWindowSelector.h"
#include "TopEvent/EventTools.h"

namespace SM4topsSS3L {

MllWindowSelector::MllWindowSelector(std::string& params) :
        m_cutvalue0(0.),
        m_cutvalue1(0.),
        m_use_loose_leptons(false),
        m_is_in_window(false)
{
    std::istringstream iss(params);
    iss >> m_cutvalue0 >> m_cutvalue1;

    bool use_loose_leptons;
    if (iss >> use_loose_leptons) m_use_loose_leptons = use_loose_leptons;

    bool is_in_window;
    if (iss >> is_in_window) m_is_in_window = is_in_window;

}

bool MllWindowSelector::apply(const top::Event& event) const
{
    std::map<double, std::pair<int, unsigned int>, std::greater<double>> leptons;
    unsigned int index_el(0), index_mu(0);

    for (const auto* const el : event.m_electrons)
    {
        leptons[el->pt()] = std::make_pair( 0, index_el);
        ++index_el;
    }
    for (const auto* const mu : event.m_muons)
    {
        leptons[mu->pt()] = std::make_pair( 1, index_mu);
        ++index_mu;
    }

    if (leptons.size() < 2) return false;

    std::vector<xAOD::IParticle*> leptons_sig;
    unsigned int index_lep(0);
    unsigned int index_lep_max = (leptons.size() < 3) ? leptons.size() : 3;

    for (const auto lep : leptons)
    {
        if (index_lep >= index_lep_max) break;

        if (lep.second.first==0)
        {
            if (m_use_loose_leptons ||
                event.m_electrons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                leptons_sig.push_back( (xAOD::IParticle*)event.m_electrons.at(lep.second.second) );
            }
        }
        else
        {
            if (m_use_loose_leptons ||
                event.m_muons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                leptons_sig.push_back( (xAOD::IParticle*)event.m_muons.at(lep.second.second) );
            }
        }

        ++index_lep;
    }

    if (leptons_sig.size() != 2) return false;
    //if (leptons_sig.size() < 2) return false;

    const double mll = top::invariantMass(*leptons_sig[0], *leptons_sig[1]);
    if (m_is_in_window) return mll >= m_cutvalue0 && mll <= m_cutvalue1;
    else return mll < m_cutvalue0 || mll > m_cutvalue1;
}

std::string MllWindowSelector::name() const {
    std::stringstream ss;
    ss << "SM4TOPSSS3LMLLWINDOW " << m_cutvalue0 << " " << m_cutvalue1
                     << " " << m_use_loose_leptons << " " << m_is_in_window;
    return ss.str();
  }
}
