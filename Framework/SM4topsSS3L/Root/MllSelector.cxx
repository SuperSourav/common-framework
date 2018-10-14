/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/

#include "SM4topsSS3L/MllSelector.h"
#include "TopEvent/EventTools.h"

namespace SM4topsSS3L {
MllSelector::MllSelector(const std::string& params, const bool& useLooseLeptons) :
        SignValueSelector(useLooseLeptons ? "MLL_USELOOSELEPTONS" : "MLL", params),
        m_useLooseLeptons(useLooseLeptons) {}

bool MllSelector::apply(const top::Event& event) const
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
            if (m_useLooseLeptons ||
                event.m_electrons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                leptons_sig.push_back( (xAOD::IParticle*) event.m_electrons.at(lep.second.second) );
            }
        }
        else
        {
            if (m_useLooseLeptons ||
                event.m_muons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                leptons_sig.push_back( (xAOD::IParticle*) event.m_muons.at(lep.second.second) );
            }
        }

        ++index_lep;
    }

    if (leptons_sig.size() != 2) return false;
    //if (leptons_sig.size() < 2) return false;

    const double mll = top::invariantMass(*leptons_sig[0], *leptons_sig[1]);
    return checkFloat(mll, value()); // https://gitlab.cern.ch/atlas/athena/blob/master/PhysicsAnalysis/TopPhys/xAOD/TopEventSelectionTools/TopEventSelectionTools/SignValueSelector.h#L131
}

std::string MllSelector::name() const {
    std::stringstream ss;
    ss << "SM4topsSS3LMllSelector " << m_useLooseLeptons;
    return ss.str();
  }
}
