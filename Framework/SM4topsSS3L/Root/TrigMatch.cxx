/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/
#include "SM4topsSS3L/TrigMatch.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>
#include <regex>

using namespace std::regex_constants;

namespace SM4topsSS3L {

  TrigMatch::TrigMatch(const std::string& selectorName,std::shared_ptr<top::TopConfig> config) {


    m_electronTriggers_Loose = config->electronTriggers_Loose(selectorName);
    m_muonTriggers_Loose = config->muonTriggers_Loose(selectorName);
    m_allTriggers_Loose = config->allTriggers_Loose(selectorName);
    m_electronTriggers_Tight = config->electronTriggers_Tight(selectorName);
    m_muonTriggers_Tight = config->muonTriggers_Tight(selectorName);
    m_allTriggers_Tight = config->allTriggers_Tight(selectorName);

    std::cout<<"Triggers Matching for selector = "<<selectorName<<std::endl;
    for (auto s : m_allTriggers_Loose) {
      std::cout << "--All loose trigger = " << s << "--" << std::endl;
    }
    for (auto s : m_electronTriggers_Loose) {
      std::cout << "--Loose electron Trigger = " << s << "--" << std::endl;
    }
    for (auto s : m_muonTriggers_Loose) {
      std::cout << "--Loose muon Trigger = " << s << "--" << std::endl;
    }
    for (auto s : m_allTriggers_Tight) {
      std::cout << "--All tight trigger = " << s << "--" << std::endl;
    }
    for (auto s : m_electronTriggers_Tight) {
      std::cout << "--Tight electron Trigger = " << s << "--" << std::endl;
    }
    for (auto s : m_muonTriggers_Tight) {
      std::cout << "--Tight muon Trigger = " << s << "--" << std::endl;
    }
  }

  bool TrigMatch::apply(const top::Event& event) const
  {

    // different treatment depending if it's a loose or tight top::Event
    bool loose = event.m_isLoose;

    // if no lepton trigger menu is associated to this selection, return true
    // no effect of TRIGMATCH if TRIGDEC wasn't used
    if (!loose) {
      if (m_electronTriggers_Tight.size()+m_muonTriggers_Tight.size() == 0) return true;
    }
    else {
      if (m_electronTriggers_Loose.size()+m_muonTriggers_Loose.size() == 0) return true;
    }

    bool trigMatch(false);

    //build lepton list ordered in pT
    //we will check only the first three leading leptons
    std::map<double, std::pair<int, unsigned int>, std::greater<double>> leptons;
    unsigned int index_el(0), index_mu(0);

    for (const auto* const el : event.m_electrons) {
      leptons[el->pt()] = std::make_pair(0, index_el);
      ++index_el;
    }
    for (const auto* const mu : event.m_muons) {
      leptons[mu->pt()] = std::make_pair(1, index_mu);
      ++index_mu;
    }

    if (leptons.size() < 2) return false;
    unsigned int index_lep_max = (leptons.size() < 3) ? leptons.size() : 3;

    //loop on all triggers
    for (const auto& trigger : loose?m_allTriggers_Loose:m_allTriggers_Tight) {

      //number of leptons in trigger: electrons, muons, taus
      int N_leptons[3] = {0, 0, 0};

      //analyze trigger name and count number of leptons
      std::string trigger_str = trigger;
      std::smatch m;
      while(std::regex_search (trigger_str, m, std::regex("_([0-9]?)(e|mu|tau)"))) {
        if (m.size() < 3) continue;
        int nlep = 1;
        if (m[1] != "") nlep = stoi(m[1]);
        if (m[2] == "e") N_leptons[0] += nlep;
        else if (m[2] == "mu") N_leptons[1] += nlep;
        else if (m[2] == "tau") N_leptons[2] += nlep;
        trigger_str = m.suffix().str();
      }

      //now check how many leptons are matched
      int N_matchedleptons[3] = {0, 0, 0};

      unsigned int index_lep(0);
      for (const auto lep : leptons) {
        if (index_lep >= index_lep_max) break;

        std::string trig = "TRIGMATCH_" + trigger;

        if (lep.second.first==0) {
          if (event.m_electrons.at(lep.second.second)->isAvailable<char>(trig)) {
            if (event.m_electrons.at(lep.second.second)->auxdataConst<char>(trig) == 1) N_matchedleptons[0]++;
          }//decoration isAvailable
        }//if electron
        else {
          if (event.m_muons.at(lep.second.second)->isAvailable<char>(trig)) {
            if (event.m_muons.at(lep.second.second)->auxdataConst<char>(trig) == 1) N_matchedleptons[1]++;
          } // decoration isAvailable
        }//if muon

        ++index_lep;
      }//loop over leptons


      //compare the two arrays
      //nb of matched leptons has to be greater or equal
      //to the nb of leptons of each type in the trigger name
      for (int index = 0; index < 3; index++) {
        if (N_matchedleptons[index] < N_leptons[index]) break;
        if (index == 2) { trigMatch = true; return trigMatch; }
      }

    }//loop on triggers

    return trigMatch;

  }

  std::string TrigMatch::name() const {
    std::string name = "TrigMatch";
    return name;
  }
}
