/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ttbbShapeReweighting_HH
#define ttbbShapeReweighting_HH

#include <string>
#include "TH1.h"
#include "TFile.h"
#include <map>

class ttbbShapeReweighting {

  public:

    ttbbShapeReweighting(int sampleID, std::vector<std::string> rwTargets, std::vector<std::string> observableNames, std::string fileName= "TTHbbTtbarCorrections/ttbbShapeReweighting.root" );
    ~ttbbShapeReweighting();
    std::vector<float> applyWeight(int HF_Classification, std::vector<float> observables);
    void changeDSID(int sampleID);
    static bool isLarger (TH1F* i,TH1F* j);
    std::vector< std::string  > splitLine( std::string inLine );
    bool m_debug;
    std::vector<std::string> m_RWTargets;
    std::vector<std::string> m_observableNames;
    std::vector<bool> m_isGeV;

  private:


    
    std::vector< std::vector<int> > m_channelMaps;
    std::vector< std::string > m_channels;
    bool hasLoadedWeight;
    std::string m_weightsFileName;
    std::map<int, int> loadDSIDMap();
    std::vector< std::vector< std::vector< TH1F* > > > m_rwHists; //The 1st dimension is the rwTarget, the 2nd dimension is channel, the 3rd dimension is observables


};
      
#endif
