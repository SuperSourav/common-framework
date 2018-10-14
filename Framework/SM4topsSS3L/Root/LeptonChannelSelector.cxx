/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/
#include "SM4topsSS3L/LeptonChannelSelector.h"
#include <cmath>

namespace SM4topsSS3L {

LeptonChannelSelector::LeptonChannelSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
        m_channel(UNDEFINED),
        m_use_loose_leptons(false)
{
    std::istringstream iss(params);

    iss >> m_channel;
    if (m_channel < UNDEFINED || m_channel >= SM4topsSS3LChannelMAX) m_channel=UNDEFINED;

    bool use_loose_leptons;
    if (iss >> use_loose_leptons) m_use_loose_leptons = use_loose_leptons;

}

bool LeptonChannelSelector::apply(const top::Event& event) const
{
    std::map<double, std::pair<int, unsigned int>, std::greater<double>> leptons;
    unsigned int index_el(0), index_mu(0);

    for (const auto* const el : event.m_electrons)
    {
        leptons[el->pt()] = std::make_pair(0, index_el);
        ++index_el;
    }
    for (const auto* const mu : event.m_muons)
    {
        leptons[mu->pt()] = std::make_pair(1, index_mu);
        ++index_mu;
    }

    if (leptons.size() < 2) return false;

    int tight_lep_n_in_lll(0);
    int tight_el_n_in_lll(0);
    int tight_centralel_n_in_lll(0);
    int tight_mu_n_in_lll(0);

    unsigned int index_lep(0);
    unsigned int index_lep_max = (leptons.size() < 3) ? leptons.size() : 3;

    for (const auto lep : leptons)
    {
        if (index_lep >= index_lep_max) break;

        if (lep.second.first==0)
        {
            if (m_use_loose_leptons ||
                event.m_electrons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)// This variable stores the information whether or not, the object also passes the tight (nominal) conditions.
            {
                ++tight_lep_n_in_lll;
                ++tight_el_n_in_lll;

                if (std::abs(event.m_electrons.at(lep.second.second)->eta()) < 1.37) ++tight_centralel_n_in_lll;
            }
        }
        else
        {
            if (m_use_loose_leptons ||
                event.m_muons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                ++tight_lep_n_in_lll;
                ++tight_mu_n_in_lll;
            }
        }

        ++index_lep;
    }

    switch ( SM4topsSS3LChannel(m_channel) )
    {
        case EE:
            return tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==2;
            break;
        case MM:
            return tight_lep_n_in_lll==2 && tight_el_n_in_lll==0;
            break;
        case EM:
            return tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==1 && tight_mu_n_in_lll==1;
            break;
        case DILEP:
            return (tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==2) || (tight_lep_n_in_lll==2 && tight_el_n_in_lll==0) || (tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==1 && tight_mu_n_in_lll==1);
            break;
        case EEE:
            return tight_lep_n_in_lll==3 && tight_el_n_in_lll==3;
            break;
        case EEM:
            return tight_lep_n_in_lll==3 && tight_el_n_in_lll==2;
            break;
        case EMM:
            return tight_lep_n_in_lll==3 && tight_el_n_in_lll==1;
            break;
        case MMM:
            return tight_lep_n_in_lll==3 && tight_el_n_in_lll==0;
            break;
        case TRILEP:
            return tight_lep_n_in_lll==3;
            break;
        case DILEPORTRILEP:
            return tight_lep_n_in_lll==2 || tight_lep_n_in_lll==3;
            break;
        default:
            std::cout << "WARNING: Unknown channel passed to LeptonChannelSelector" << std::endl;
    }

    return false;
}

std::string  LeptonChannelSelector::name() const {
    std::stringstream ss;
    ss << "SM4TOPSSS3LCHANNEL == " << m_channel << " " << m_use_loose_leptons;
    return ss.str();
}

}
