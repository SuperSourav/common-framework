/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HPLUSDISCRIMINANTDATA_H
#define HPLUSDISCRIMINANTDATA_H
#include <TLorentzVector.h>
#include <vector>

namespace TTHbb {

  class HplusDiscriminantData {

  public:
  HplusDiscriminantData() : m_useonlycorrectcombination(false),
      m_bl_JetIndex(-1),
      m_q1_JetIndex(-1),
      m_q2_JetIndex(-1),
      m_bh_JetIndex(-1),
      m_b2h_JetIndex(-1){ };
    ~HplusDiscriminantData(){};
    int m_eventNumber;
    bool m_ejets;
    bool m_mujets;
    int m_njets;
    int m_nbjets;
    TLorentzVector m_lepton;
    std::vector<TLorentzVector> m_jets;
    std::vector<float> m_jets_mv2c10;
    std::vector<float> m_jets_truthflav;
    TLorentzVector m_neutrino;

    bool m_useonlycorrectcombination;
    int m_bl_JetIndex;
    int m_q1_JetIndex;
    int m_q2_JetIndex;
    int m_bh_JetIndex;
    int m_b2h_JetIndex;
  };
}
#endif // HPLUSDISCRIMINANTDATA_H
