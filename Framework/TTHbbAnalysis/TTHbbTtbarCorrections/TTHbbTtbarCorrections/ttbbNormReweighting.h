/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ttbbNormReweighting_HH
#define ttbbNormReweighting_HH

#include <string>
#include "TH1.h"
#include "TFile.h"
#include <map>

class ttbbNormReweighting {

  public:

    ttbbNormReweighting(int sampleID, std::vector<std::string>  reweightID, std::string fileName = "TTHbbTtbarCorrections/ttbbNormReweighting.root");
    ~ttbbNormReweighting();
    std::vector<float> applyWeight (int HF_Classification);
    void changeDSID(int sampleID);
    std::vector<std::string> m_reweightID;
    bool m_debug;

  private:
    
    std::string m_weightsFileName;
    std::map<int, int> loadDSIDMap();
    std::vector<TH1D*> m_rwHists; 


};
      
#endif
