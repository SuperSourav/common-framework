/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/TTHbbUtils.h"
#include "PathResolver/PathResolver.h"

#include "TopDataPreparation/SampleXsection.h"
#include <iostream>
#include <fstream>

#include "TChain.h"
#include "TFile.h"
#include "TH1.h"

bool TTHbb::util::iequals(const std::string& a, const std::string& b)
{
  unsigned int sz = a.size();
  if (b.size() != sz)
    return false;
  for (unsigned int i = 0; i < sz; ++i)
    if (tolower(a[i]) != tolower(b[i]))
      return false;
  return true;
}

std::vector<std::string> TTHbb::util::vectoriseString(std::string str, std::string delim){

  std::vector<std::string> vec;


  std::string::size_type start = 0, end = 0;
  while ((end = str.find(delim, start)) != std::string::npos){ 
    std::string token = str.substr(start, end - start);
    if(token.size()) vec.push_back(token);
    start = end + 1;
  }
  std::string lasttoken = str.substr(start);
  if(lasttoken.size()) vec.push_back(lasttoken);

  return vec;

}

std::vector<std::string> TTHbb::util::fileToVector(std::string file){

  std::string line;
  std::ifstream infile;
  infile.open(file);

  std::vector<std::string> vec;

  while(std::getline(infile,line))
    {
      size_t hash = line.find("#");
      if (hash != std::string::npos){
	line = line.substr(0, hash);
      }
      //remove whitespace from the beginning and end of line
      const auto strBgn = line.find_first_not_of(" \t");
      const auto strEnd = line.find_last_not_of(" \t");
      line = line.substr(strBgn, strEnd+1);

      //check line isn't empty!
      if(line.empty()) continue;

      //Add line to vector
      vec.push_back(line);
    }
  infile.close();
  
  return vec;
}

std::map<int,float> TTHbb::util::makeNormalisationMaps(std::vector<std::string> inputs){

  std::map<int,float> sumOfWeights;
  std::map<int,float> normWeights;
  
  TChain* mc = new TChain("sumWeights");
  for(auto s : inputs)
    mc->AddFile( s.c_str() );
  float m_weightedEvents = 0;
  int mcdsid = 0;
  mc->SetBranchAddress( "totalEventsWeighted", &m_weightedEvents );
  mc->SetBranchAddress( "dsid", &mcdsid );

  int eEntries = mc->GetEntries();
  for( Long64_t iEntry = 0; iEntry < eEntries; ++iEntry){
    mc->GetEntry( iEntry );
    if(sumOfWeights.find(mcdsid) == sumOfWeights.end())
      sumOfWeights[mcdsid] = 0;
    sumOfWeights[mcdsid] += m_weightedEvents;
  }

  SampleXsection sampleXsection;
  std::string tdpfile = PathResolverFindCalibFile("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/TopDataPreparation/XSection-MC15-13TeV.data");
  sampleXsection.readFromFile(tdpfile.c_str());

  for(auto& dsid : sumOfWeights){
    float mcID = dsid.first;
    float weightSum = dsid.second;
    float xsec = sampleXsection.getXsection(dsid.first);
    float norm = xsec/weightSum;
    normWeights[mcID] = norm;
    std::cout << "INFO: Normalisation weight for DSID: " << mcID << " = " << norm << std::endl;
  }
  delete mc;

  return normWeights;
}

std::map<int,float> TTHbb::util::makeNormalisationMapsFromCutflow(std::vector<std::string> inputs){

  std::map<int,float> sumOfWeights;
  std::map<int,float> normWeights;


  TH1* h_cutflow = 0;
  TFile* f = 0;
  TString m_cutflow("");
  TChain* t_sumWeights = new TChain("sumWeights");
  int dsid = 0;

  t_sumWeights->SetBranchAddress("dsid", &dsid);

  for(auto s : inputs){

    f = TFile::Open(s.c_str());
    if(m_cutflow == ""){
      /// Get list of objects in the input
      TList* l = (TList*)f->GetListOfKeys();
      /// A selection, containing cutflow hists will be at the top
      m_cutflow = l->At(0)->GetTitle();
      m_cutflow.Append("/cutflow_mc");
    }
    
    /// Get normal cutflow
    h_cutflow = (TH1*)f->Get(m_cutflow);
   
    /// If null pointer returned, check loose cutflow
    if(h_cutflow == 0x0){
      m_cutflow.Append("_Loose");
    }
    h_cutflow = (TH1*)f->Get(m_cutflow);
   
    if(h_cutflow == 0x0){
      std::cout << "ERROR: Cannot find cutflow named " << m_cutflow << " in file. Exiting." << std::endl;
      exit(63);
    }
    t_sumWeights->Add(s.c_str());
    t_sumWeights->GetEntry(t_sumWeights->GetEntries() - 1);
    
    /// If dsid not already in the sum of weights map, create entry and set to zero
    if(sumOfWeights.find(dsid) == sumOfWeights.end()){
      sumOfWeights[dsid] = 0.;
    }
    /// Increment sum of weights by first bin of cutflow
    sumOfWeights[dsid] += h_cutflow->GetBinContent(1);

    f->Close();
  }
  delete t_sumWeights;
  t_sumWeights = 0;
  f = 0;

  SampleXsection sampleXsection;
  std::string tdpfile = PathResolverFindCalibFile("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/TopDataPreparation/XSection-MC15-13TeV.data");
  sampleXsection.readFromFile(tdpfile.c_str());

  /// Add norm weights to a lookup map
  for(auto& mcdsid : sumOfWeights){
    float mcID = mcdsid.first;
    float weightSum = mcdsid.second;
    float xsec = sampleXsection.getXsection(mcdsid.first);
    float norm = xsec/weightSum;
    normWeights[mcID] = norm;
    std::cout << "INFO: Normalisation weight for DSID: " << mcID << " = " << norm << std::endl;
  }
  
  return normWeights;
}


std::map<int, std::map<std::string,float> > TTHbb::util::makeMCWeightNormalisationMaps(std::vector<std::string> inputs){

  std::map<int, std::map<std::string,float> > mcSumOfWeights;
  std::map<int, std::map<std::string,float> > mcNormWeights;

  TChain* mc = new TChain("sumWeights");
  for(auto s : inputs)
    mc->AddFile( s.c_str() );
  float m_weightedEvents = 0;
  int mcdsid = 0;
  std::vector<std::string>* m_mcWeightNames = new std::vector<std::string>();
  std::vector<float>* m_mcWeightedEvents = new std::vector<float>();
  mc->SetBranchAddress( "totalEventsWeighted", &m_weightedEvents );
  mc->SetBranchAddress( "dsid", &mcdsid );
  mc->SetBranchAddress( "names_mc_generator_weights", &m_mcWeightNames);
  mc->SetBranchAddress( "totalEventsWeighted_mc_generator_weights", &m_mcWeightedEvents);

  int eEntries = mc->GetEntries();
  for( Long64_t iEntry = 0; iEntry < eEntries; ++iEntry){
    mc->GetEntry( iEntry );
    if(mcSumOfWeights.find(mcdsid) == mcSumOfWeights.end()){
      mcSumOfWeights[mcdsid] = {};//std::map<std::string, float>();
    }
    for(unsigned int i = 0; i < m_mcWeightNames->size(); i++){
      if(mcSumOfWeights[mcdsid].find( m_mcWeightNames->at(i) ) == mcSumOfWeights[mcdsid].end()){
        mcSumOfWeights[mcdsid][m_mcWeightNames->at(i)] = 0;
      }
      mcSumOfWeights[mcdsid][m_mcWeightNames->at(i)] += m_mcWeightedEvents->at(i);
    }
    
  }

  SampleXsection sampleXsection;
  std::string tdpfile = PathResolverFindCalibFile("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/TopDataPreparation/XSection-MC15-13TeV.data");
  sampleXsection.readFromFile(tdpfile.c_str());

  for(auto& dsid : mcSumOfWeights){
    float mcID = dsid.first;
    float xsec = sampleXsection.getXsection(dsid.first);
    for(auto& name_sum : dsid.second){
      std::string name = name_sum.first;
      float weightSum  = name_sum.second;
      float norm = xsec/weightSum;
      mcNormWeights[mcID][name] = norm;
      std::cout << "INFO: Normalisation weight for DSID: " << mcID << ", MC weight: " << name << " = " << norm << std::endl;
    }
  }
  delete mc;

  return mcNormWeights;
}


std::string TTHbb::util::shortBtagWP (const std::string WP) {
  std::string str = "FixedCutBEff_";
  std::string out = WP;
  if (out.find(str) != std::string::npos) {
    out.replace(out.find(str),str.length(),"");
  }
  return out;
}

std::string TTHbb::util::onlyBtagWP (const std::string WP) {
  std::size_t pos = WP.find_last_of("_");  
  return WP.substr(pos+1);
}


void TTHbb::util::decodeRegionName(std::string region, int& njets, int& nmaxjets, int& nbjets, int& nmaxbjets){

  char njetsi;
  char nbjetsi;

  char testj;
  char testb;

  if(region.size() !=6){
    std::cout << "ERROR: could not decode region name " << region << " : size!=6"<< std::endl;
    abort();
  }

  int n = sscanf(region.c_str(),"%d%c%c%d%c%c", &njets, &testj, &njetsi, &nbjets, &testb, &nbjetsi);

  if(n!=6){
    std::cout << "ERROR: could not sscanf region name " << region << " : read char = " << n << std::endl;
    abort();
  }

  if(testj != 'j' || testb != 'b'
     || (njetsi != 'i' && njetsi != 'e') || (nbjetsi != 'i' && nbjetsi != 'e') ){
    std::cout << "ERROR: decodeRegionName unknown format " << region << std::endl;
    abort();
  }

  nmaxjets=njets;
  nmaxbjets=nbjets;

  if(njetsi == 'i') nmaxjets=10000;
  if(nbjetsi == 'i') nmaxbjets=10000;

}



void TTHbb::util::decodeRegionName(std::string region, std::string &name, int &val){

  
  std::vector<std::string> vects = vectoriseString(region, "=");

  if(vects.size()!=2){
    std::cout << "ERROR: could not decode region name " << region << std::endl;
    abort();
  }

  name=vects[0];
  val=std::atoi(vects[1].c_str());


}
