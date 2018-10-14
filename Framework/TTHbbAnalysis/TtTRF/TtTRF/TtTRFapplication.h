#ifndef TTTRFAPPLICATION_H
#define TTTRFAPPLICATION_H

#include "CustomTRF/CustomTRF.h"
#include "TtTRF/TtTRFreader.h"

#include "TLorentzVector.h"

class TtTRFapplication {
public:
  TtTRFapplication(int ntags=3,std::string path=".",std::string sample="data",std::string systematic="nominal",
                   std::string selection="5j6j",std::string splitting="split3b4b",std::string parametrisation="pt/eta/dRj",
                   bool debug=false);
  ~TtTRFapplication(){};
  
//   void AddJet(int idx,float pt,float eta,float phi,float e,float weight,int istagged=-1);
  void AddJet(int idx,float pt,float eta,float phi,float e,int istagged=-1);
  void Reset();
  void Evaluate();
  float GetTRFweight();
  bool GetJetIsTagged(int idx);
  int GetNBTags();
  
private:
  bool m_debug;
  
  TtTRFreader *m_TRFreader0;
  TtTRFreader *m_TRFreader1;
//   std::vector<TtTRFreader*> m_systTRFreader0;
//   std::vector<TtTRFreader*> m_systTRFreader1;

  CustomTRF *m_TRF;
  CustomTRF *m_TRF0;
  CustomTRF *m_TRF1;
  CustomTRF *m_TRF2;
  
  std::vector<int> m_jet_index;
  std::vector<TLorentzVector> m_jet;
  std::vector<float> m_jet_weight;
  std::vector<int> m_jet_istagged;
  
  float m_weight;
  int m_ntags;
  int m_incl;
  std::string sel;
  
  std::vector<int> m_jet_istagged_ttTRF;
  int m_nBtags_ttTRF;
  
  bool m_split3b;
  bool m_split4b;
}; 

#endif
