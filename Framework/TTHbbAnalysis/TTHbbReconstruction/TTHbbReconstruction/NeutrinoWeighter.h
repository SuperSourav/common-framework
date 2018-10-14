#ifndef NeutrinoWeighter_h
#define NeutrinoWeighter_h
#include <iostream>

#include "TObject.h"
#include "TRandom3.h"
#include "TLorentzVector.h"
#include "TH1F.h"

class NeutrinoWeighter {
  ClassDef(NeutrinoWeighter,1);
private:

  std::vector< double > m_nu_eta;
  std::vector< double > m_nu_sinh;
  std::vector< double > m_nu_cosh;
  std::vector< double > m_nubar_eta;
  std::vector< double > m_nubar_sinh;
  std::vector< double > m_nubar_cosh;
  std::vector< double > m_top_smears;
  std::vector< double > m_W_smears;
  TLorentzVector m_top;
  TLorentzVector m_tbar;
  TLorentzVector m_ttbar;
  TLorentzVector m_b;
  TLorentzVector m_bbar;
  TLorentzVector m_nu;
  TLorentzVector m_nubar;
  double m_weight_max;
  TRandom3 m_random;

  bool do_jetsmearing;
  bool m_do_both_pairings;
  bool m_do_chi2;
  bool m_do_crystalball_smearing;
  bool m_include_x;
  bool m_include_y;
  bool m_include_t;
  bool m_include_phi;
  bool m_include_mWp;
  bool m_include_mWn;
  bool m_include_mtop;
  bool m_include_mtbar;
  bool m_include_b_jvc;
  bool m_include_bbar_jvc;
  bool m_scale_top_mass;
  bool m_scale_b_mass;
  bool m_scale_l_mass;


  TH1F pt_25_50;
  TH1F pt_50_100;
  TH1F pt_100_200;
  TH1F pt_200_inf;

public:

  std::vector< double > GetNuEta(){ return m_nu_eta; };
  std::vector< double > GetNubarEta(){ return m_nubar_eta; };
  void SetNuEta(std::vector< double > etas)     { m_nu_eta = etas; m_nubar_eta = etas;};
  void SetNuSinhEta(std::vector< double > etas) { m_nu_sinh = etas; m_nubar_sinh = etas;};
  void SetNuCoshEta(std::vector< double > etas) { m_nu_cosh = etas; m_nubar_cosh = etas;};
  void SetTopMass(std::vector< double > top_mass){ m_top_smears = top_mass;};
  void SetWMass(std::vector< double > W_mass){ m_W_smears = W_mass;};

  NeutrinoWeighter(int setting, int event_number);
  ~NeutrinoWeighter();
  double Reconstruct(TLorentzVector lepton_pos, TLorentzVector lepton_neg, TLorentzVector jet_1, TLorentzVector jet_2, double met_ex, double met_ey, double met_phi, double b_jvc, double bbar_jvc);
  void calculateWeight(TLorentzVector lepton_pos, TLorentzVector lepton_neg, TLorentzVector b1, TLorentzVector b2, double met_ex, double met_ey, double met_phi, double mtop, double mtbar, double mWp, double mWn, double b_jvc, double bbar_jvc, bool print);
  std::vector<TLorentzVector> solveForNeutrinoEta(TLorentzVector* lepton, TLorentzVector* bJet, int index, int index_type, double mtop, double mW);
  //double neutrino_weight(TLorentzVector neutrino1, TLorentzVector neutrino2, double met_ex, double met_ey, double met_phi);
  double neutrino_weight(TLorentzVector neutrino1, TLorentzVector neutrino2, double met_ex, double met_ey, double met_phi, TLorentzVector lep_pos, TLorentzVector lep_neg, TLorentzVector b1, TLorentzVector b2, double mtop, double mtbar, double mWp, double mWn, double b_jvc, double bbar_jvc, bool print);


  TLorentzVector GetTop(){   return m_top;   };
  TLorentzVector GetTbar(){  return m_tbar;  };
  TLorentzVector GetTtbar(){ return m_ttbar; };
  TLorentzVector GetB(){     return m_b;     };
  TLorentzVector GetBbar(){  return m_bbar;  };
  TLorentzVector GetNu(){    return m_nu;    };
  TLorentzVector GetNubar(){ return m_nubar; };
  double GetWeight(){ return m_weight_max; };
  void Reset();
  void RecalculateEtas(double pos_lep_eta, double neg_lep_eta);
  void DoBothPairings(bool do_pairing){ m_do_both_pairings = do_pairing; };
  //void SetTrueParameters(double true_nu_eta, double true_nubar_eta, double true_top_m, double true_tbar_m, double true_wp_m, double true_wp_n);
  float GetCrystalBallWeight(float jet_pt);
};

#endif


