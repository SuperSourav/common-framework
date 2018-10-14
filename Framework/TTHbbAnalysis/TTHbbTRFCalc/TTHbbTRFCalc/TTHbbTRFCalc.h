/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHBBTRFCALC_H_
#define TTHBBTRFCALC_H_

#include "BTaggingTruthTagging/BTaggingTruthTaggingTool.h"
#include "TTHbbObjects/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEventSelectionTools/EventSelectorBase.h"

struct CombinedTRF {
  std::vector<std::vector<bool> > m_TRFPerm_in;
  std::vector<std::vector<bool> > m_TRFPerm_ex;
  std::map<std::string, std::vector<double> > m_map_TTweight_ex;
  std::map<std::string, std::vector<double> > m_map_TTweight_in;
};

class TTHbbTRFCalc {
 public:
  TTHbbTRFCalc(std::map<std::string, std::string> m_trf_config);
  ~TTHbbTRFCalc();

  bool DecorateJets(std::vector<std::shared_ptr<TTHbb::Jet> >& m_jets);
  bool getTRF(std::vector<std::shared_ptr<TTHbb::Jet> >& m_jets,
              int eventNumber);

  CombinedTRF GetCombinedTRF();

 private:
  bool Clean();
  unsigned int MapIndex;
  std::string m_OP;
  std::string m_jetAlg;
  std::string m_taggerName;
  std::string m_cdi;
  std::string wp_lab;
  bool m_ignoreSF;
  bool m_usePermutations;
  bool m_useQuantile;
  std::string m_calibName;
  unsigned int m_nbjetMax;
  bool m_ignoreMCMC;
  BTaggingTruthTaggingTool* m_trf;

  // Variables that hold the results
  // For the nominal TRF variables
  std::vector<std::vector<bool> > m_TRFPerm_in;                   //!
  std::vector<std::vector<bool> > m_TRFPerm_ex;                   //!
  std::map<std::string, std::vector<double> > m_map_TTweight_ex;  //!
  std::map<std::string, std::vector<double> > m_map_TTweight_in;  //!

  std::vector<std::vector<int> > m_trf_bin_ex;
  std::vector<std::vector<int> > m_trf_bin_in;

  CombinedTRF members;  //!
};

#endif
