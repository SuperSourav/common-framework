/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTtbarCorrections/ttbbNormReweighting.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

ttbbNormReweighting::ttbbNormReweighting(int sampleID, std::vector<std::string> reweightID, std::string fileName ) 
{
  
  m_debug = false;
  if(m_debug) std::cout << "Created ttbbNormReweighting tool" << std::endl;
  
  m_weightsFileName = PathResolverFindCalibFile(fileName);
  m_reweightID = reweightID; 

  changeDSID( sampleID );

  
}

ttbbNormReweighting::~ttbbNormReweighting()
{
  for(unsigned int iH = 0; iH < m_rwHists.size(); ++iH){
    if( m_rwHists.at(iH) ){
      m_rwHists.at(iH)->Clear();
      m_rwHists.at(iH)->Delete();
    }
  }
  m_rwHists.clear();
}

void ttbbNormReweighting::changeDSID(int sampleID){

  // Clear the currently loaded histograms.  Maybe loop through and delete them?
  for(unsigned int iH = 0; iH < m_rwHists.size(); ++iH){
    if( m_rwHists.at(iH) )
      m_rwHists.at(iH)->Delete();
  }
  m_rwHists.clear();

  //Find the dsid of the input file 
  std::map<int, int> DSIDMap = loadDSIDMap();

  auto iter_DSID = DSIDMap.find(sampleID);
  
  // If this DSID is not found, we will set m_rwHists to empty and return 1. for every event
  if(iter_DSID == DSIDMap.end()) {
    m_rwHists.clear();
    return;
  }

  //Replace sampleID with the mapped version
  sampleID = iter_DSID->second;
  
  //temporary hard-code fix!!  The code that calls ttbbNormReweighting should chose the vector m_reweightID it wants to use for sampleID, but setting it in the config file
  //If it is not sample 410501, use only the first entry in the m_reweightID vector by setting the rest to null
  if (sampleID != 410501){
    //m_reweightID.erase( m_reweightID.begin()+1, m_reweightID.end() );
    for(unsigned int iID = 1; iID < m_reweightID.size(); ++iID){
      m_reweightID.at(iID) = "NONE";
    }
    //std::cout << "No ttbbNorm Sys, weights will be 1 " << std::endl;
  }

  // Get the proper file name and open the file
  TFile* weightFile = TFile::Open(m_weightsFileName.c_str(),"READ","myNormRWfile");
  
  std::string sampleIDString =  std::to_string( sampleID );
  TH1D *DenominatorHist = (TH1D *) weightFile->Get(("h_"+sampleIDString).c_str());

  // If this histogram is not found, set m_rwHists to empty and return 1 for every event
  if( !DenominatorHist ){
    if(m_debug) std::cout << "ttbbNormReweighting::loadWeight : Could not load reweighting histogram or Sherpa sys variations are not available for dsid  " <<sampleID  << ". All weights will be one." << std::endl;
    m_rwHists.clear();
    return;
  }
  DenominatorHist->SetDirectory(0);

  // Load numerator histograms, and make sure they exit
  //std::vector<std::string> m_reweightIDString;
  for (unsigned int iT = 0; iT < m_reweightID.size(); ++iT){
    //std::string thisReweightIDString = std::to_string(m_reweightID.at(iT));
 
    TH1D* NumeratorHist = (TH1D *) weightFile->Get(("h_"+m_reweightID.at(iT)).c_str());

    //If this variation does not exist, push back and empty TH1.  It will be treated as a weight of 1 in the apply function.
    if( !NumeratorHist ){
      if(m_debug) std::cout << "ttbbNormReweighting::loadWeight : Could not load reweighting histogram or Sherpa sys variations are available for dsid " <<sampleID  << ". All weights will be one." << std::endl;
      TH1D* nullHist = nullptr;
      m_rwHists.push_back( nullHist ); 
    }else{
      TH1D* thisRWeightHist = (TH1D*) NumeratorHist->Clone();
      thisRWeightHist->Divide(DenominatorHist);
      thisRWeightHist->SetDirectory(0);
      m_rwHists.push_back( thisRWeightHist );
	
    }
  }

  weightFile->Close(); 
}

std::map<int, int> ttbbNormReweighting::loadDSIDMap(){

  std::map<int, int> DSIDMap;

  //SherpaOL
  DSIDMap[410051] = 410051;
  // New SherpaOL
  DSIDMap[410323] = 410323;
  DSIDMap[410324] = 410323;
  DSIDMap[410325] = 410323;

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
  // Reweighted to B-Filter
  DSIDMap[410249] = 410281; 
  DSIDMap[410250] = 410281; 
  DSIDMap[410251] = 410281; 
  DSIDMap[410252] = 410281; 
  DSIDMap[410282] = 410281; 
  DSIDMap[410283] = 410281; 
  DSIDMap[410281] = 410281; 
  
  return DSIDMap;
}

std::vector<float> ttbbNormReweighting::applyWeight(int HF_Classification){
 
    std::vector<float> weights; 

    //If no rwHists, just return 1 value
    if (m_rwHists.size() == 0){
      for(unsigned int iH = 0; iH < m_reweightID.size(); ++iH){
        weights.push_back(1.);
      }
      return weights;
    }


    int bin = -1;
    if((HF_Classification >= 1000) && (HF_Classification < 1100)){
      bin = 1;
      //tt+b
    }
    else if((HF_Classification >= 2000) && (HF_Classification < 2100)){
      bin = 2;
      //tt+bb
    }
    else if ((HF_Classification >= 100) && (HF_Classification < 200)){
      bin = 3;
      //tt+B
    }
    else if (((HF_Classification >= 1100) && (HF_Classification < 2000)) || ((HF_Classification >= 200) && (HF_Classification < 1000)) || (HF_Classification >= 2100)){
      bin = 4;
      //tt+>=3b
    }
    else {
      //std::cout << "Category of tt+>=1b not found" << std::endl;
      for(unsigned int iH = 0; iH < m_rwHists.size(); ++iH){
        weights.push_back(1.);
      }
      return weights; 
    }   
    for(unsigned int iH = 0; iH < m_rwHists.size(); ++iH){
      if( m_rwHists.at(iH) )
        weights.push_back( m_rwHists.at(iH)->GetBinContent(bin) );
      else
        //Histogram doesn't exist because this sys variation is not valid for the input dsid
        weights.push_back( 1. );
    }
    return weights;
}


