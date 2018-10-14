/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopDataPreparation/SampleXsection.h"
#include "PathResolver/PathResolver.h"

#include "TROOT.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"

#include <iostream>
#include <map>
#include <string>

int main(int argc, char *argv[]){

  if(argc == 1){
    std::cout << "Usage: addXsec (--treeName=<treename>) <file1.root> ( <file2.root> ... )" << std::endl;
    exit(42);
  }

  bool b_addToNominal(false);
  int nFiles = argc-1;
  int start = 1;
  std::string opt = argv[1];

  std::string m_treeName = "";

  if(opt.find("--treeName")!=std::string::npos){
    //if(*argv[1] == '-')
    b_addToNominal = true;
    m_treeName = opt.substr(11);
    std::cout << "Will add a normalisation weight per event to tree: " << m_treeName << std::endl;
    nFiles--;
    start++;
  }

  int FileNo = 1;
  for(int i=start; i<argc; ++i){
    std::cout << "Opening file " << FileNo << "/" << nFiles << ": " << argv[i] << std::endl;
    /***file weight***/
    TChain* mc = new TChain("sumWeights");
    mc->AddFile( argv[i] );
    float m_weightedEvents = 0;
    int dsid = 0;
    mc->SetBranchAddress( "totalEventsWeighted", &m_weightedEvents );
    mc->SetBranchAddress( "dsid", &dsid );
    std::map<int, float> sumOfWeights;

    int eEntries = mc->GetEntries();
    float sumofsumofweights = 0;
    for( Long64_t iEntry = 0; iEntry < eEntries; ++iEntry){
      mc->GetEntry( iEntry );
      if(sumOfWeights.find(dsid) == sumOfWeights.end())
	sumOfWeights[dsid] = 0;
      sumOfWeights[dsid] += m_weightedEvents;
      sumofsumofweights += m_weightedEvents;
    }

    delete mc;

    TFile* f = TFile::Open(argv[i], "UPDATE");

    std::string oldtrees="xsecs;*";
    gDirectory->Delete(oldtrees.c_str());
    TTree* t = new TTree("xsecs","xsecs");

    Int_t mcID(0);
    Float_t weightSum(0);
    Float_t xsec(0);

    std::map<int,float> normWeights;

    TBranch* ID = t->Branch("mcID",&mcID,"mcID/I");
    TBranch* wSum = t->Branch("SumOfWeights",&weightSum, "SumOfWeights/F");
    TBranch* csec = t->Branch("xsec",&xsec, "xsec/F");

    SampleXsection sampleXsection;
    std::string tdpfile = PathResolverFindCalibFile("TopDataPreparation/XSection-MC15-13TeV.data"); 
    sampleXsection.readFromFile(tdpfile.c_str());

    for(auto& dsid : sumOfWeights){
      mcID = dsid.first;
      weightSum = dsid.second;
      xsec = sampleXsection.getXsection(dsid.first);
      float norm = xsec/weightSum;
      std::cout << "mcID = " << mcID << "\nweightSum = " << weightSum << "\nxsec*kfactor = " << xsec 
		<< "\nweight_normalise = " << norm <<std::endl;
      ID->Fill();
      wSum->Fill();
      csec->Fill();
      if(b_addToNominal)
	normWeights[mcID] = norm;
    }
    std::cout << "\nSum of all weights: " << sumofsumofweights << std::endl;

    t->SetEntries(ID->GetEntries());

    if(b_addToNominal){
      //TTree* t2 = (TTree*)f->Get("nominal");
      TTree* t2 = (TTree*)f->Get(m_treeName.c_str());
      t2->SetBranchStatus("*",0);
      Float_t fweight(0);
      TBranch* finalWeight = t2->Branch("weight_normalise", &fweight, "weight_normalise/F");
      UInt_t mcChannelNum = 0;
      t2->SetBranchStatus("mcChannelNumber",1);
      t2->SetBranchAddress("mcChannelNumber",&mcChannelNum);
    

      unsigned int gEntries = t2->GetEntries();
      for(unsigned int j = 0; j < gEntries; ++j){
	if((j % 100000) == 0)
	  std::cout << "Adding info for event " << j  << "/" << gEntries << std::endl;
	t2->GetEntry(j);
	fweight = normWeights[mcChannelNum];
	finalWeight->Fill();
      }
      t2->SetBranchStatus("*",1);
    }

    f->Write("",TObject::kOverwrite);
    f->Close();
    FileNo++;
  }
  return 0;
}

