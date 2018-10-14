/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NNLOReweighter/NNLOReweighter.h"
#include <iostream>
#include "PathResolver/PathResolver.h"

NNLOReweighter::NNLOReweighter(int sampleID, std::string dirName) :
  m_sampleID(sampleID),
  m_weightsFileName(""),
  m_Weights_file(0),
  m_Hist_topPt(0),
  m_Hist_topPt_extended(0),
  m_Hist_ttbarPt(0),
  m_Hist_topPtSeq(0),
  m_Hist_ttbarPt_ttbarPtUp(0),
  m_Hist_topPtSeq_topPtUp(0),
  m_Hist_Mttbar(0)

{
  if(m_weightsDirName==""){
    m_weightsDirName = PathResolverFindCalibDirectory("NNLOReweighter/");
  }
  else
    m_weightsDirName = dirName;
}

NNLOReweighter::~NNLOReweighter(){
  if(m_Weights_file){
    m_Weights_file->Close();
    delete m_Weights_file;
  }
}

void NNLOReweighter::SetInputDirectory(std::string dirName){
  m_weightsDirName = dirName;
}

void NNLOReweighter::SetInputFile(std::string fileName){
  m_weightsFileName = fileName;
}

void NNLOReweighter::SetSampleID(int sampleID){
  m_sampleID = sampleID;
}

void NNLOReweighter::Init(){
  // set the fileName according to sample ID
  if(m_weightsFileName==""){
    if(m_sampleID!=0){
      if(m_sampleID==410000 || // powheg+pythia6 ttbar non-all-had
         m_sampleID==410009 || m_sampleID==410120 || m_sampleID==410121 || // dilep and b-filtered samples
         m_sampleID==407009 || m_sampleID==407010 || m_sampleID==407011 || m_sampleID==407012 // HT/MET filtered samples
      ) m_weightsFileName = m_weightsDirName+"/PowPyt6";

      else if(m_sampleID==410004) m_weightsFileName = m_weightsDirName+"/PowHer";
      else if(m_sampleID==410003) m_weightsFileName = m_weightsDirName+"/aMCHer";
      else if(m_sampleID==410002) m_weightsFileName = m_weightsDirName+"/PowPytRadLo";
      else if(m_sampleID==410001) m_weightsFileName = m_weightsDirName+"/PowPytRadHi";
      else if(m_sampleID==410500) m_weightsFileName = m_weightsDirName+"/PowPyt8";
      else if(m_sampleID==410159) m_weightsFileName = m_weightsDirName+"/aMCPyt8";

//added January 2017
      else if (m_sampleID==410501 || // powheg+pythia8 ttbar non-all-had
	       m_sampleID==410502 || m_sampleID==410503 || m_sampleID==410504 || 
               m_sampleID==410505 || m_sampleID==410506 || m_sampleID==410509 
      ) m_weightsFileName = m_weightsDirName+"/PowPyt8_410501";

      else if (m_sampleID==410511) m_weightsFileName = m_weightsDirName+"/PowPyt8Up_410511";
      else if (m_sampleID==410512) m_weightsFileName = m_weightsDirName+"/PowPyt8Do_410512";
      else if (m_sampleID==410525) m_weightsFileName = m_weightsDirName+"/PowHer7_410525";
      else if (m_sampleID==410225) m_weightsFileName = m_weightsDirName+"/aMCPyt8_410225";
      else if (m_sampleID==410250 || m_sampleID==410251 || m_sampleID==410252 ) m_weightsFileName = m_weightsDirName+"Sherpa221_410250";
/////////////////////////


      else{
        std::cout << "NNLOReweighter::WARNING: No input file nor valid sampleID specified. Not able to get reweighting." << std::endl;
        return;
      }
      m_weightsFileName += "_TopPt_TTbarPt_TopPtSeq_rew.root";
    }
    else{
      std::cout << "NNLOReweighter::WARNING: No input file nor valid sampleID specified. Not able to get reweighting." << std::endl;
      return;
    }
  }
  //
  m_Weights_file = TFile::Open(m_weightsFileName.c_str());
  if(m_Weights_file==0x0){
    std::cout << "NNLOReweighter::ERROR: Not able to open weight file " << m_weightsFileName << std::endl;
    return;
  }

  if(m_Weights_file->Get("TopPt_rew"))    m_Hist_topPt    = (TH1*)m_Weights_file->Get("TopPt_rew");
  if(m_Weights_file->Get("TTbarPt_rew"))  m_Hist_ttbarPt  = (TH1*)m_Weights_file->Get("TTbarPt_rew");
  if(m_Weights_file->Get("TopPt_rew_extended"))  m_Hist_topPt_extended    = (TH1*)m_Weights_file->Get("TopPt_rew_extended"   );
  if(m_Weights_file->Get("TopPtSeq_rew"))  m_Hist_topPtSeq = (TH1*)m_Weights_file->Get("TopPtSeq_rew");
  if(m_Weights_file->Get("Mtt_rew"))       m_Hist_Mttbar   = (TH1*)m_Weights_file->Get("Mtt_rew");
  if(m_Weights_file->Get("TTbarPt_UP")) m_Hist_ttbarPt_ttbarPtUp  = (TH1*)m_Weights_file->Get("TTbarPt_UP");
  if(m_Weights_file->Get("TopPt_UP"))   m_Hist_topPtSeq_topPtUp   = (TH1*)m_Weights_file->Get("TopPt_UP");

  // systematics availabe only for 410000
  if(m_sampleID==410000 && (m_Hist_ttbarPt_ttbarPtUp==0x0 || m_Hist_topPtSeq_topPtUp==0x0))
    std::cout << "NNLOReweighter::WARNING: no systematic histograms found..." << std::endl;

  if (m_Hist_topPt) m_Hist_topPt   ->SetDirectory(0);
  if (m_Hist_ttbarPt)   m_Hist_ttbarPt ->SetDirectory(0);
  if (m_Hist_topPt_extended)   m_Hist_topPt_extended   ->SetDirectory(0);
  if (m_Hist_topPtSeq)  m_Hist_topPtSeq->SetDirectory(0);
  if (m_Hist_Mttbar)  m_Hist_Mttbar->SetDirectory(0);
  if (m_Hist_ttbarPt_ttbarPtUp) m_Hist_ttbarPt_ttbarPtUp->SetDirectory(0);
  if (m_Hist_topPtSeq_topPtUp) m_Hist_topPtSeq_topPtUp->SetDirectory(0);

  m_Weights_file->Close();
}

float NNLOReweighter::GetTtbarPtWeight(float ttbar_pt){
  if(m_Hist_ttbarPt==0x0) return 1.;
  return m_Hist_ttbarPt ->GetBinContent( m_Hist_ttbarPt ->FindBin( ttbar_pt/1000. ) );
}

float NNLOReweighter::GetTopPtWeight(float top_pt){
  if(m_Hist_topPt==0x0) return 1.;
  return m_Hist_topPt->GetBinContent( m_Hist_topPt->FindBin( top_pt  /1000. ) );
}

float NNLOReweighter::GetTopPtAfterTtbarPtWeight(float top_pt){
  if(m_Hist_topPtSeq==0x0) return 1.;
  return m_Hist_topPtSeq->GetBinContent( m_Hist_topPtSeq->FindBin( top_pt  /1000. ) );
}

float NNLOReweighter::GetTtbarAndTopPtWeight(float ttbar_pt, float top_pt){
  if(m_Hist_ttbarPt==0x0 || m_Hist_topPtSeq==0x0) return 1.;
  return GetTtbarPtWeight(ttbar_pt)*GetTopPtAfterTtbarPtWeight(top_pt);
}

float NNLOReweighter::GetTtbarAndTopPtWeight_topPtUp(float ttbar_pt, float top_pt){
  if(m_Hist_topPtSeq==0x0) return 1.;
  if(m_Hist_topPtSeq_topPtUp==0x0) return 1.;
  if(m_Hist_ttbarPt==0x0) return 1.;
//   if(m_Hist_ttbarPt_ttbarPtUp==0x0) return 1.;
  return m_Hist_ttbarPt ->GetBinContent( m_Hist_ttbarPt ->FindBin( ttbar_pt/1000. ) )*m_Hist_topPtSeq_topPtUp->GetBinContent( m_Hist_topPtSeq_topPtUp->FindBin( top_pt  /1000. ) );
}

float NNLOReweighter::GetTtbarAndTopPtWeight_ttbarPtUp(float ttbar_pt, float top_pt){
  if(m_Hist_topPtSeq==0x0) return 1.;
//   if(m_Hist_topPtSeq_topPtUp==0x0) return 1.;
  if(m_Hist_ttbarPt==0x0) return 1.;
  if(m_Hist_ttbarPt_ttbarPtUp==0x0) return 1.;
  return m_Hist_ttbarPt_ttbarPtUp->GetBinContent( m_Hist_ttbarPt_ttbarPtUp->FindBin( ttbar_pt/1000. ) )*m_Hist_topPtSeq->GetBinContent( m_Hist_topPtSeq->FindBin( top_pt  /1000. ) );
}


float NNLOReweighter::GetExtendedTopPtWeight(float top_pt){
  if(m_Hist_topPt_extended==0x0) return 1.;
  return m_Hist_topPt_extended->GetBinContent( m_Hist_topPt_extended->FindBin( top_pt  /1000. ) );
}

float NNLOReweighter::GetMttbarWeight(float m_ttbar){
  if(m_Hist_Mttbar==0x0) return 1.;
  return m_Hist_Mttbar->GetBinContent( m_Hist_Mttbar->FindBin( m_ttbar  /1000. ) );
}
