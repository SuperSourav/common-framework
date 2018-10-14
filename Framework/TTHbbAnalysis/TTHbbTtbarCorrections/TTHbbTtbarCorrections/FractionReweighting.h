/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FractionReweighting_HH
#define FractionReweighting_HH

#include <string>
#include "TH1.h"
#include "TFile.h"
#include <map>

class FractionReweighting {

  public:

    FractionReweighting(int sampleID, int reweightID = 410501, std::string fileName = "TTHbbTtbarCorrections/FractionReweighting.root");
    ~FractionReweighting();
    float applyWeight (int HF_SimpleClassification);
    void changeDSID(int sampleID);

  private:
    
    bool hasLoadedWeight;
    std::string m_weightsFileName;
    TH1D *m_weightHist; 
    std::map<int, int> loadDSIDMap();
    TH1D *NumeratorHist;
    //TFile* weightFile;


};
      
#endif
