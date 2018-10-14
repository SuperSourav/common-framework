/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTtbarCorrections/ttbbShapeReweighting.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "TKey.h"

using namespace std;

ttbbShapeReweighting::ttbbShapeReweighting(int sampleID, std::vector<std::string> rwTargets, std::vector<std::string> observableNames, std::string fileName ) 
{
  m_debug = false;
  if(m_debug) std::cout << "Creating ttbbShapeReweighting tool" << std::endl;

  m_channels = {"ttb", "ttB", "ttbb", "tt3b"};
  m_RWTargets = rwTargets;
  m_observableNames = observableNames;
  
  // Get the proper file name
  m_weightsFileName = PathResolverFindCalibFile(fileName);

  changeDSID(sampleID);

}

ttbbShapeReweighting::~ttbbShapeReweighting()
{
  for( unsigned int i=0; i < m_rwHists.size(); ++i){
    for( unsigned int j=0; j < m_rwHists.at(i).size(); ++j){
      for( unsigned int k=0; k < m_rwHists.at(i).at(j).size(); ++k){
        delete m_rwHists.at(i).at(j).at(k);
      }
    }
  }
  m_rwHists.clear();
  m_isGeV.clear();
  m_channelMaps.clear();

}

void ttbbShapeReweighting::changeDSID(int sampleID){
  
  std::map<int, int> DSIDMap = loadDSIDMap();
  std::string sampleIDString =  std::to_string( DSIDMap[sampleID] );
    
    
  // Delete previous histograms and vectors  
  m_isGeV.clear();
  m_channelMaps.clear();
  for( unsigned int i=0; i < m_rwHists.size(); ++i){
    for( unsigned int j=0; j < m_rwHists.at(i).size(); ++j){
      for( unsigned int k=0; k < m_rwHists.at(i).at(j).size(); ++k){
        delete m_rwHists.at(i).at(j).at(k);
      }
    }
  }
  m_rwHists.clear();

  // If this DSID is not found, keep m_rwHists empty and return 1 for each event
  if( DSIDMap[sampleID] == 0){
    return;
  }
  
  TFile* weightFile = TFile::Open(m_weightsFileName.c_str(),"READ","myShapeRWfile");
  
  //std::cout << "ttbbShapeReweighting::loadWeight : Could not load reweighting histogram of reference dsid "+reweightIDString+". All weights will be one." << std::endl;
  
  // For each rwTarget, get the relevant set of histograms
  //  m_rwHists; //The 1st dimension is the rwTarget, the 2nd dimension is channel, the 3rd dimension is observables
  for(unsigned int iT = 0; iT < m_RWTargets.size(); ++iT){
    std::vector< std::vector< TH1F* > > rwTargetHists;

    for(unsigned int iC = 0; iC < m_channels.size(); ++iC){
      if(m_debug) std::cout << "adding histograms for target file " << m_RWTargets.at(iT) << " and flavor channel " << m_channels.at(iC) << std::endl;
      std::vector< TH1F* > channelHists;

      std::string histName = "rwSeq_"+sampleIDString+"_"+m_RWTargets.at(iT)+"_"+m_channels.at(iC)+"_";
      if(m_debug) std::cout << "Histograms must include " << histName << std::endl;
  
      TKey *key;
      TIter next(weightFile->GetListOfKeys());
      while ((key = (TKey*)next())) {
        std::string thisHistName = key->GetName();
        //if(m_debug) std::cout << "Found histogram " << thisHistName << std::endl;
        if ( thisHistName.find( histName ) != std::string::npos ){

          if(m_debug) std::cout << "It's a match! Adding histogram " << thisHistName << std::endl;
  
          TH1F* thisHist = (TH1F*) weightFile->Get( key->GetName() );
          thisHist->SetDirectory(0);
          channelHists.push_back( thisHist );
  
        } // if a relevant histogram for this rwTarget
      } //for each histogram in weightFile

      //Sort histograms into ascending order
      std::sort (channelHists.begin(), channelHists.end(), isLarger);

      rwTargetHists.push_back( channelHists );
    }//for each channel type
    m_rwHists.push_back( rwTargetHists );
  } // for each rwTarget

  for( unsigned int iO = 0; iO < m_observableNames.size(); ++iO ){
   if (m_observableNames.at(iO).find("pt") != std::string::npos){
        m_isGeV.push_back( true );
      }else {
        m_isGeV.push_back( false );
      }
  } 
  //Check order of observables.  It should be the same for each rwTarget, but can differ between flavor channels
  //It maps the histograms to the observable index
  for(unsigned int iC = 0; iC < m_channels.size(); ++iC){
    std::vector<int> channelMap;

    //Get a vector of the components of the last histogram's name
    //The last histogram will include all variables
    std::string histName = m_rwHists.at(0).at(iC).at( m_rwHists.at(0).at(iC).size()-1 )->GetName();
    std::vector< std::string > obs_ordering = splitLine( histName );
    //Erase the components that are not observables, e.g. rwSeq,410501,SherpaNominal, and ttB
    obs_ordering.erase(obs_ordering.begin(), obs_ordering.begin() + 4);
    for( unsigned int iV = 0; iV < obs_ordering.size(); ++iV ){
      for( unsigned int iO = 0; iO < m_observableNames.size(); ++iO ){

        //Get the observable name without '_'
        std::string shortObsName = m_observableNames.at(iO);
        shortObsName.erase(std::remove(shortObsName.begin(), shortObsName.end(), '_'), shortObsName.end());

        if( obs_ordering.at(iV) == shortObsName ){
          //std::cout << shortObsName << " and " << obs_ordering.at(iV) << " are equal!" << std::endl;
          channelMap.push_back( iO );
        }
      }//For histogram ordering
    }//For each given observable


    //for( unsigned int iV = 0; iV < obs_ordering.size(); ++iV ){
    //  std::cout << obs_ordering.at(iV) << " chosen with " << m_observableNames.at(channelMap.at(iV)) << std::endl;
    // }
    
    if( channelMap.size() != obs_ordering.size() ){
      if(m_debug) std::cout << "Error, could not find all the necessary observables for histogram " << histName << std::endl;
    }
    m_channelMaps.push_back( channelMap );

  }//for each channel type
  weightFile->Close(); 

}
bool  ttbbShapeReweighting::isLarger (TH1F* i,TH1F* j) {
  std::string iName = i->GetName();
  std::string jName = j->GetName();
  return (iName.size()<jName.size()); 
}

std::vector< std::string  > ttbbShapeReweighting::splitLine( std::string inLine ){
  // Get vector of selections, splitting by commas
  std::vector< std::string > lineVec;
  std::istringstream ss(inLine);
  std::string subStr = "";
  while( std::getline(ss, subStr, '_') ){
    lineVec.push_back( subStr );
  }
  return lineVec;
}

std::map<int, int> ttbbShapeReweighting::loadDSIDMap(){

  std::map<int, int> DSIDMap;

  //SherpaOL
  DSIDMap[410051] = 410051;

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
  //Using B-Filter sample
  DSIDMap[410249] = 410281; 
  DSIDMap[410250] = 410281; 
  DSIDMap[410251] = 410281; 
  DSIDMap[410252] = 410281; 
  DSIDMap[410282] = 410281; 
  DSIDMap[410283] = 410281; 
  DSIDMap[410281] = 410281; 

  
  return DSIDMap;
}

std::vector<float> ttbbShapeReweighting::applyWeight(int HF_Classification, std::vector<float> observables){

  if(m_debug)  std::cout << "Apply ttbbShapeReweighting" << std::endl;

  std::vector<float> weights;
  //If no RW hists were loaded for this sample, return a single 1.
  if(m_rwHists.size() == 0){
    if(m_debug)  std::cout << "No RW hist, returning 1.0" << std::endl;
    //This is dumb, fix the TTree decorator code
    for(unsigned int iTarget=0; iTarget < m_RWTargets.size(); ++iTarget){
      weights.push_back(1.);
    }
    return weights;
  }

  // Find the channel index for this event
  if(m_debug)  std::cout << "Choose HF_Classification channel" << std::endl;
  unsigned int iChannel = -1;
  // m_channels = {"ttb", "ttB", "ttbb", "tt3b"};
  if( (HF_Classification >= 1000) && (HF_Classification < 1100) ){ //tt+b
    iChannel = 0;
  } else if ( (HF_Classification >= 100) && (HF_Classification < 200) ){ //tt+B
    iChannel = 1;   
  } else if( (HF_Classification >= 2000) && (HF_Classification < 2100) ){ //tt+bb
    iChannel = 2;
  } else if ( ((HF_Classification >= 1100) && (HF_Classification < 2000)) || 
              ((HF_Classification >= 200) && (HF_Classification < 1000)) || 
              (HF_Classification >= 2100) ){ //tt+>=3b
    iChannel = 3;
  } else {
    //std::cout << "Category of tt+>=1b not found" << std::endl;
    for( unsigned int iT=0; iT < m_rwHists.size(); ++iT){
      weights.push_back(1.);
    }
    return weights;
  } 

  //Loop over target RW hists
  for( unsigned int iT=0; iT < m_rwHists.size(); ++iT){
    if(m_debug)  std::cout << "RW to target " << m_RWTargets.at(iT)  << std::endl;
    float thisWeight = 1.0;
    std::vector<TH1F*> theseHists = m_rwHists.at(iT).at(iChannel);
    for( unsigned int iH=0; iH < theseHists.size(); ++iH ){
      // Find the index of the observable for the iHth RW histogram for channel iChannel
      float thisObservableValue = observables.at( m_channelMaps.at(iChannel).at(iH) );
      if (m_isGeV.at(m_channelMaps.at(iChannel).at(iH) ) == true) {
        thisObservableValue = thisObservableValue/1000.;
      }
      thisWeight *= theseHists.at(iH)->GetBinContent( theseHists.at(iH)->FindBin( thisObservableValue ) ); 
    }
    weights.push_back( thisWeight );

  } //for target RW hists

  return weights;
}


