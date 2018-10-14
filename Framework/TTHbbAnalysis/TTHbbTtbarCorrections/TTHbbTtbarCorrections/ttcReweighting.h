/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ttcReweighting_HH
#define ttcReweighting_HH

#include <string>
#include "TH1.h"
#include "TFile.h"
#include <map>

class ttcReweighting {

  public:

    ttcReweighting(int sampleID, std::string fileName = "TTHbbTtbarCorrections/ttcReweighting.root");
    ~ttcReweighting();
    float applyWeight (int HF_Classification, float top_pt, float ttbar_pt, float q1_pt, float qq_dr, float qq_pt);
    void changeDSID(int sampleID);	

  private:
    
    bool hasLoadedWeight;
    std::string m_weightsFileName;
    std::vector<TH1F*> *m_tt1cHists; 
    std::vector<TH1F*> *m_tt2cHists; 
    std::vector<TH1F*> *m_ttccHists; 
    TFile* weightFile;

    bool m_alwaysReturnOne;

};
      
#endif
