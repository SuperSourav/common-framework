/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/
#include "SM4topsSS3L/SSOSDilepSelector.h"
#include "TopConfiguration/TopConfig.h"

namespace SM4topsSS3L {
// #include "TestxAODFramework/QuickAnaSettings.h"

SSOSDilepSelector::SSOSDilepSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
        m_isOS(false),
        m_useLooseLeptons(false)

{
    std::istringstream iss(params);

    bool isOS;
    if (iss >> isOS) m_isOS = isOS;

    bool useLooseLeptons;
    if (iss >> useLooseLeptons) m_useLooseLeptons = useLooseLeptons;


  }

bool SSOSDilepSelector::apply(const top::Event& event) const
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

    std::vector<double> leptons_sig;
    unsigned int index_lep(0);
    unsigned int index_lep_max = (leptons.size() < 3) ? leptons.size() : 3;

    for (const auto lep : leptons)
    {
        if (index_lep >= index_lep_max) break;

        if (lep.second.first==0)
        {
            if (m_useLooseLeptons ||
                event.m_electrons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1) // This variable stores the information whether or not, the object also passes the tight (nominal) conditions.
            {
                leptons_sig.push_back(event.m_electrons.at(lep.second.second)->charge());
            }
        }
        else
        {
            if (m_useLooseLeptons ||
                event.m_muons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                leptons_sig.push_back(event.m_muons.at(lep.second.second)->charge());
            }
        }

        ++index_lep;
    }

    if (leptons_sig.size() != 2) return false;

    return m_isOS ? leptons_sig[0]!=leptons_sig[1] : leptons_sig[0]==leptons_sig[1];
}

std::string SSOSDilepSelector::name() const {
    std::stringstream ss;
    ss << "SSOSDilepSelector " << m_useLooseLeptons << " " << m_isOS;
    return ss.str();
  }
}
