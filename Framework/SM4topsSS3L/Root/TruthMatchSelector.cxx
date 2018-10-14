#include "SM4topsSS3L/TruthMatchSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

namespace SM4topsSS3L {

TruthMatchSelector::TruthMatchSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
        m_channel(UNDEFINED),
        m_useLooseLeptons(false),
        m_isMC(true)
{
    std::istringstream iss(params);

    iss >> m_channel;
    if (m_channel < UNDEFINED || m_channel >= NSSBJETSCH) m_channel=UNDEFINED;

    bool useLooseLeptons;
    if (iss >> useLooseLeptons) m_useLooseLeptons = useLooseLeptons;

    m_isMC = config->isMC();

}

bool TruthMatchSelector::apply(const top::Event& event) const
{
    if (!m_isMC) return true;

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

    int truthMatched_lep_n_in_lll(0);
    int truthMatched_el_n_in_lll(0);
    int truthMatched_mu_n_in_lll(0);

    unsigned int index_lep(0);
    unsigned int index_lep_max = (leptons.size() < 3) ? leptons.size() : 3;

    for (const auto lep : leptons)
    {
        if (index_lep >= index_lep_max) break;

        if (lep.second.first==0)
        {
            if (event.m_electrons.at(lep.second.second)->auxdataConst< int >("truthType") == 2 &&
                (event.m_electrons.at(lep.second.second)->charge()*event.m_electrons.at(lep.second.second)->auxdataConst< int >("firstEgMotherPdgId")) < 0)
            {
              if (m_useLooseLeptons || event.m_electrons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1) {
                ++truthMatched_lep_n_in_lll;
                ++truthMatched_el_n_in_lll;
              }
            }
        }
        else
        {
            if (event.m_muons.at(lep.second.second)->auxdataConst< int >("truthType") == 6)
            {
              if (m_useLooseLeptons || event.m_muons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1) {
                ++truthMatched_lep_n_in_lll;
                ++truthMatched_mu_n_in_lll;
              }
            }
        }

        ++index_lep;
    }

    switch ( SSbjetsChannel(m_channel) )
    {
        case EE:
            return truthMatched_lep_n_in_lll==2 && truthMatched_el_n_in_lll==2;
            break;
        case MM:
            return truthMatched_lep_n_in_lll==2 && truthMatched_el_n_in_lll==0;
            break;
        case EM:
            return truthMatched_lep_n_in_lll==2 && truthMatched_el_n_in_lll==1;
            break;
        case DILEP:
            return truthMatched_lep_n_in_lll==2;
            break;
        case EEE:
            return truthMatched_lep_n_in_lll==3 && truthMatched_el_n_in_lll==3;
            break;
        case EEM:
            return truthMatched_lep_n_in_lll==3 && truthMatched_el_n_in_lll==2;
            break;
        case EMM:
            return truthMatched_lep_n_in_lll==3 && truthMatched_el_n_in_lll==1;
            break;
        case MMM:
            return truthMatched_lep_n_in_lll==3 && truthMatched_el_n_in_lll==0;
            break;
        case TRILEP:
            return truthMatched_lep_n_in_lll==3;
            break;
        default:
            std::cout << "WARNING: Unknown channel passed to TruthMatchSelector" << std::endl;
    }

    return false;
}

std::string  TruthMatchSelector::name() const {
    std::stringstream ss;
    ss << "SM4TOPS_TRUTHMATCH == " << m_channel << " " << m_useLooseLeptons;
    return ss.str();
}

}
