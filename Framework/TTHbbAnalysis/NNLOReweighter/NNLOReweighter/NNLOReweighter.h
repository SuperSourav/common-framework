/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NNLOREWEIGHTER_H_
#define NNLOREWEIGHTER_H_

#include <string>
#include "TH1.h"
#include "TFile.h"

class NNLOReweighter {

  public:
    NNLOReweighter(int sampleID=0,std::string dirName="");
    ~NNLOReweighter();
    void SetInputDirectory(std::string dirName);
    void SetInputFile(std::string fileName); // to specify a non-default file name
    void SetSampleID(int sampleID);
    void Init();
    float GetTtbarPtWeight(float ttbar_pt); // in MeV!!
    float GetTopPtWeight(float top_pt); // in MeV!!
    float GetExtendedTopPtWeight(float top_pt); // in MeV!!
    float GetTopPtAfterTtbarPtWeight(float top_pt); // in MeV!!
    float GetTtbarAndTopPtWeight(float ttbar_pt, float top_pt); // in MeV!!
    float GetTtbarAndTopPtWeight_topPtUp(float ttbar_pt, float top_pt); // in MeV!!
    float GetTtbarAndTopPtWeight_ttbarPtUp(float ttbar_pt, float top_pt); // in MeV!!
    float GetMttbarWeight(float m_ttbar); // in MeV!!


  private:
    int m_sampleID;
    std::string m_weightsDirName;
    std::string m_weightsFileName;
    TFile *m_Weights_file;
    TH1* m_Hist_topPt;
    TH1* m_Hist_topPt_extended;
    TH1* m_Hist_ttbarPt;
    TH1* m_Hist_topPtSeq;
    TH1* m_Hist_ttbarPt_ttbarPtUp;
    TH1* m_Hist_topPtSeq_topPtUp;
    TH1* m_Hist_Mttbar;

};

#endif
