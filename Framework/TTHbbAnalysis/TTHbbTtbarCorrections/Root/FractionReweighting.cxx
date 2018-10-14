/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTtbarCorrections/FractionReweighting.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

FractionReweighting::FractionReweighting(int sampleID, int reweightID, std::string fileName ):   
  m_weightHist(nullptr),
  NumeratorHist(nullptr)
{


  // Get the proper file name and open the file
  m_weightsFileName = PathResolverFindCalibFile(fileName);

  TFile* weightFile = TFile::Open(m_weightsFileName.c_str(),"READ","myFractionRWfile");
  std::map<int, int> DSIDMap = loadDSIDMap();
  
  std::string sampleIDString =  std::to_string( DSIDMap[sampleID] );
  std::string reweightIDString = std::to_string(reweightID);

  // Load numerator and denominator histograms, and make sure they exit
  TH1D *DenominatorHist = (TH1D *) weightFile->Get(("h_"+sampleIDString).c_str());
  NumeratorHist = (TH1D *) weightFile->Get(("h_"+reweightIDString).c_str());
  if( !NumeratorHist ){
    std::cout << "FractionReweighting::loadWeight : Could not load reweighting histogram of reference dsid "+reweightIDString+". All weights will be one." << std::endl;
  } else if( !DenominatorHist ){
    std::cout << "FractionReweighting::loadWeight : Could not load reweighting histogram of dsid " <<sampleID  << ". All weights will be one." << std::endl;
  }else{

    std::string numHistName = NumeratorHist->GetName();
    numHistName = "weightHist_"+numHistName;
    m_weightHist = (TH1D *)NumeratorHist->Clone( numHistName.c_str() );
    m_weightHist->Divide(DenominatorHist);
    
    NumeratorHist->SetDirectory(0);
    m_weightHist->SetDirectory(0);
  }
  weightFile->Close();
}

FractionReweighting::~FractionReweighting()
{  
   if( m_weightHist ){
    m_weightHist->Clear();
    m_weightHist->Delete();
    m_weightHist = nullptr;
   }
   if( NumeratorHist ){
    NumeratorHist->Clear();
    NumeratorHist->Delete();
    NumeratorHist = nullptr;
   }
   
}

void FractionReweighting::changeDSID(int sampleID){


  std::map<int, int> DSIDMap = loadDSIDMap();
  std::string sampleIDString =  std::to_string( DSIDMap[sampleID] );

  TFile* weightFile = TFile::Open(m_weightsFileName.c_str(),"READ","myFractionRWfile");
  TH1D *DenominatorHist = (TH1D *) weightFile->Get(("h_"+sampleIDString).c_str());
  if( !NumeratorHist ){
    std::cout << "FractionReweighting::loadWeight : Still could not load reweighting histogram of reference. All weights will be one." << std::endl;
  } else if( !DenominatorHist ){
    std::cout << "FractionReweighting::loadWeight : Could not load reweighting histogram of dsid " << sampleIDString  << ". All weights will be one." << std::endl;
  }else{

    if( m_weightHist ){
      m_weightHist->Clear();
      m_weightHist->Delete();
      m_weightHist = nullptr;
    }

    std::string numHistName = NumeratorHist->GetName();
    numHistName = "weightHist_"+numHistName;
    m_weightHist = (TH1D *)NumeratorHist->Clone( numHistName.c_str() );
    m_weightHist->Divide(DenominatorHist);
    m_weightHist->SetDirectory(0);
  }
  
  weightFile->Close();

}

std::map<int, int> FractionReweighting::loadDSIDMap(){

  std::map<int, int> DSIDMap;

  // Powheg+Pythia8
  DSIDMap[410501] = 410501;
  DSIDMap[410502] = 410501;
  DSIDMap[410503] = 410501;
  DSIDMap[410504] = 410501;
  DSIDMap[410505] = 410501;
  DSIDMap[410506] = 410501;
  DSIDMap[410507] = 410501;
  DSIDMap[410508] = 410501;
  DSIDMap[410509] = 410501;
  DSIDMap[410510] = 410501;
  DSIDMap[410531] = 410501;
  DSIDMap[410532] = 410501;
  DSIDMap[410533] = 410501;

  //aMC@NLO+Pythia8
  DSIDMap[410225] = 410225;
  DSIDMap[410226] = 410225;
  DSIDMap[410227] = 410225;
  DSIDMap[410274] = 410225;
  DSIDMap[410275] = 410225;

  //Powheg+H7
  DSIDMap[410525] = 410525;
  DSIDMap[410526] = 410525;
  DSIDMap[410527] = 410525;
  DSIDMap[410528] = 410525;
  DSIDMap[410529] = 410525;
  
  //Powheg+Pythia8 Tune VarationUp
  DSIDMap[410511] = 410511;
  DSIDMap[410513] = 410511;
  DSIDMap[410515] = 410511;
  DSIDMap[410517] = 410511;
  DSIDMap[410519] = 410511;
  DSIDMap[410521] = 410511;
  DSIDMap[410523] = 410511;
  DSIDMap[410568] = 410511;
  
  //Powheg+Pythia8 Tune VarationDown
  DSIDMap[410512] = 410512;
  DSIDMap[410514] = 410512;
  DSIDMap[410516] = 410512;
  DSIDMap[410518] = 410512;
  DSIDMap[410520] = 410512;
  DSIDMap[410522] = 410512;
  DSIDMap[410524] = 410512;
  DSIDMap[410569] = 410512;

  //Sherpa 5F samples
  //Note that this histogram contains the three different decays of the nonallhad
  DSIDMap[410249] = 410250;
  DSIDMap[410250] = 410250;
  DSIDMap[410251] = 410250;
  DSIDMap[410252] = 410250;
  DSIDMap[410282] = 410250;
  DSIDMap[410283] = 410250;
  DSIDMap[410281] = 410250;
  
  return DSIDMap;
}

float FractionReweighting::applyWeight(int HF_SimpleClassification){
  
    if (!m_weightHist){
       return 1.;
    }
    else{
     if(HF_SimpleClassification== 1){
        return m_weightHist->GetBinContent(3);
        //tt+bb
      }
      else if(HF_SimpleClassification== 0){
        return m_weightHist->GetBinContent(2);
        //tt+light
      }
      else if (HF_SimpleClassification== -1){
        return m_weightHist->GetBinContent(1);
        //tt+cc
      }
      else {
        std::cout << "This event is not tt+bb, or tt+cc, or tt+ligh, Please check!" << std::endl;
        return 0; 
      }   
    }
}


