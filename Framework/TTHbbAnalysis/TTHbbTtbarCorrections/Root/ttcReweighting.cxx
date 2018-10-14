/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTtbarCorrections/ttcReweighting.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

ttcReweighting::ttcReweighting(int sampleID, std::string fileName ):   
  m_weightsFileName(fileName)
{
  //Load the histograms             
  m_tt1cHists = new vector<TH1F*>();
  m_tt2cHists = new vector<TH1F*>();
  m_ttccHists = new vector<TH1F*>();

  // Get the proper file name and open the file
  fileName = PathResolverFindCalibFile(m_weightsFileName);
 
  weightFile = TFile::Open(fileName.c_str(),"READ");
  
  if((sampleID >= 410501 && sampleID <= 410510) || (sampleID >= 410531 && sampleID < 410534)){
    std::cout<<"ttcReweighting::ttcReweighting: Loading ttc reweighting"<<std::endl;
    m_alwaysReturnOne = false;
  }
  else{
    std::cout<<"ttcReweighting::ttcReweighting: Attempting to set up ttc reweighting for the wrong sample. Will always return 1."<<std::endl;
    m_alwaysReturnOne = true;
  }

  // Check if the file exists
  if(!weightFile){
    std::cout << "ttcReweighting::ttcReweighting: File (" << m_weightsFileName << ") does not exist. Will always return 1." << std::endl;
  }

  m_tt1cHists->push_back( (TH1F*)weightFile->Get("powWeights1c_0") );
  m_tt1cHists->push_back( (TH1F*)weightFile->Get("powWeights1c_1") );
  m_tt1cHists->push_back( (TH1F*)weightFile->Get("powWeights1c_2") );

  m_tt2cHists->push_back( (TH1F*)weightFile->Get("powWeights2c_0") );
  m_tt2cHists->push_back( (TH1F*)weightFile->Get("powWeights2c_1") );
  m_tt2cHists->push_back( (TH1F*)weightFile->Get("powWeights2c_2") );

  m_ttccHists->push_back( (TH1F*)weightFile->Get("powWeightscc_0") );
  m_ttccHists->push_back( (TH1F*)weightFile->Get("powWeightscc_1") );
  m_ttccHists->push_back( (TH1F*)weightFile->Get("powWeightscc_2") );
  m_ttccHists->push_back( (TH1F*)weightFile->Get("powWeightscc_3") );

  //Check that the hists are there
  for(unsigned int i = 0; i < m_tt1cHists->size(); i++){
    if( !(m_tt1cHists->at(i)) ){
      std::cout<<"Could not load tt1c reweight hists, always returning 1"<<std::endl;
      m_alwaysReturnOne = true;
    }
  }
  for(unsigned int i = 0; i < m_tt2cHists->size(); i++){
    if( !(m_tt2cHists->at(i)) ){
      std::cout<<"Could not load tt2c reweight hists, always returning 1"<<std::endl;
      m_alwaysReturnOne = true;
    }
  }
  for(unsigned int i = 0; i < m_ttccHists->size(); i++){
    if( !(m_ttccHists->at(i)) ){
      std::cout<<"Could not load ttcc reweight hists, always returning 1"<<std::endl;
      m_alwaysReturnOne = true;
    }
  }
}


ttcReweighting::~ttcReweighting()
{
  weightFile->Close(); 
}



void ttcReweighting::changeDSID(int sampleID){

  if( (sampleID >= 410501 && sampleID <= 410510) || (sampleID >= 410531 && sampleID < 410534)) m_alwaysReturnOne = false;
  else m_alwaysReturnOne = true;

  // If file is not here, return 1
  if(!weightFile) m_alwaysReturnOne = true;

}


float ttcReweighting::applyWeight(int HF_Classification, float truth_top_pt, float truth_ttbar_pt, float HFClassification_q1_pt, float HFClassification_qq_dr, float HFClassification_qq_pt){
  
  if(m_alwaysReturnOne) return 1.;

  if(TMath::Abs(HF_Classification) >= 100) return 1.; //ttb event
  if(HF_Classification == 0) return 1.; //ttlight event
 
  float weight = 1.;

  //Avoid overflows
  if(truth_ttbar_pt > 500000.) truth_ttbar_pt = 499000.;
  if(truth_top_pt > 500000.) truth_top_pt = 499000.;
  if(HFClassification_q1_pt > 400000.) HFClassification_q1_pt = 399000.;
  if(HFClassification_qq_pt > 500000.) HFClassification_qq_pt = 499000.;

  if(TMath::Abs(HF_Classification) == 1){
    //ttC
    weight *= m_tt2cHists->at(0)->GetBinContent( m_tt2cHists->at(0)->FindBin( truth_ttbar_pt/1000. ) );
    weight *= m_tt2cHists->at(1)->GetBinContent( m_tt2cHists->at(1)->FindBin( truth_top_pt/1000. ) );
    weight *= m_tt2cHists->at(2)->GetBinContent( m_tt2cHists->at(2)->FindBin( HFClassification_q1_pt/1000. ) );
    
  }
  else if(TMath::Abs(HF_Classification) == 10){
    //ttc
    weight *= m_tt1cHists->at(0)->GetBinContent( m_tt1cHists->at(0)->FindBin( truth_ttbar_pt/1000. ) );
    weight *= m_tt1cHists->at(1)->GetBinContent( m_tt1cHists->at(1)->FindBin( truth_top_pt/1000. ) );
    weight *= m_tt1cHists->at(2)->GetBinContent( m_tt1cHists->at(2)->FindBin( HFClassification_q1_pt/1000. ) );
  }
  else{
    //ttcc
    weight *= m_ttccHists->at(0)->GetBinContent( m_ttccHists->at(0)->FindBin( truth_ttbar_pt/1000. ) );
    weight *= m_ttccHists->at(1)->GetBinContent( m_ttccHists->at(1)->FindBin( truth_top_pt/1000. ) );
    weight *= m_ttccHists->at(2)->GetBinContent( m_ttccHists->at(2)->FindBin( HFClassification_qq_dr ) );
    weight *= m_ttccHists->at(3)->GetBinContent( m_ttccHists->at(3)->FindBin( HFClassification_qq_pt/1000. ) );
  }

  return weight;

}


