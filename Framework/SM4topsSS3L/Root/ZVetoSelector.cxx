#include "SM4topsSS3L/ZVetoSelector.h"
#include "TopEvent/EventTools.h"

namespace SM4topsSS3L {
ZVetoSelector::ZVetoSelector(std::string& params) :
        m_cutvalue0(81000.),
        m_cutvalue1(101000.),
        m_useLooseLeptons(false),
        m_is_in_window(false)
{
    std::istringstream iss(params);
    iss >> m_cutvalue0 >> m_cutvalue1;

    bool use_loose_leptons;
    if (iss >> use_loose_leptons) m_useLooseLeptons = use_loose_leptons;

    bool is_in_window;
    if (iss >> is_in_window) m_is_in_window = is_in_window;

}

bool ZVetoSelector::apply(const top::Event& event) const
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

    std::vector<xAOD::IParticle*> electrons_sig, muons_sig;
    std::vector<double> electrons_sig_charge, muons_sig_charge;
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
                electrons_sig.push_back( (xAOD::IParticle*)event.m_electrons.at(lep.second.second) );
                //electrons_sig_charge.push_back(event.m_electrons.at(lep.second.second)->auxdataConst< double >("charge"));
                electrons_sig_charge.push_back(event.m_electrons.at(lep.second.second)->charge());
            }
        }
        else
        {
            if (m_useLooseLeptons ||
                event.m_muons.at(lep.second.second)->auxdataConst< char >("passPreORSelection") == 1)
            {
                muons_sig.push_back( (xAOD::IParticle*)event.m_muons.at(lep.second.second) );
                //muons_sig_charge.push_back(event.m_muons.at(lep.second.second)->auxdataConst< double >("charge"));
                muons_sig_charge.push_back(event.m_muons.at(lep.second.second)->charge());
            }
        }

        ++index_lep;
    }

    bool pairInWindow = false;
    unsigned int index_el_max = electrons_sig.size();
    unsigned int index_mu_max = muons_sig.size();

    if (index_el_max > 1) {
      for (unsigned int i = 0; i < index_el_max-1 ; i++) {
        for (unsigned int j = i+1; j < index_el_max; j++) {
          if (electrons_sig_charge[i]*electrons_sig_charge[j] == -1.) {
            const double mll = top::invariantMass(*electrons_sig[i], *electrons_sig[j]);
            if (mll >= m_cutvalue0 && mll <= m_cutvalue1) pairInWindow = true;
          }
        }
      }
    }

    if (index_mu_max > 1) {
      for (unsigned int i = 0; i < index_mu_max-1 ; i++) {
        for (unsigned int j = i+1; j < index_mu_max; j++) {
          if (muons_sig_charge[i]*muons_sig_charge[j] == -1.) {
            const double mll = top::invariantMass(*muons_sig[i], *muons_sig[j]);
            if (mll >= m_cutvalue0 && mll <= m_cutvalue1) pairInWindow = true;
          }
        }
      }
    }

    if (!m_is_in_window && !pairInWindow) return true;
    else if (m_is_in_window && pairInWindow) return true;
    else return false;

}

std::string ZVetoSelector::name() const {
    std::stringstream ss;
    ss << "SM4TOPSZVETO " << m_cutvalue0 << " " << m_cutvalue1
                     << " " << m_useLooseLeptons << " " << m_is_in_window;
    return ss.str();
  }
}
