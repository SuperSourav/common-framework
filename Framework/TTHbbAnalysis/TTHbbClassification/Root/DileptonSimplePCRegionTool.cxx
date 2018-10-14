/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbClassification/DileptonSimplePCRegionTool.h"

#include "MVAVariables/JetOrderingTool.h"

#include "TFile.h"
#include "TCollection.h"
#include "TDirectory.h"
#include "TObject.h"

namespace TTHbb{

  DileptonSimplePCRegionTool::DileptonSimplePCRegionTool(std::string fileName) : m_pcvarname("tagWeightBin") {

    TFile* f = TFile::Open(fileName.c_str());
    TIter next(f->GetListOfKeys());
    TObject* obj = 0;
    while ((obj = next())){
      std::string name = obj->GetName();
      regionhists[name] = *(TH2F*)(f->Get(name.c_str())->Clone(name.c_str()));
      std::cout << "INFO in TTHbb::DileptonSimplePCRegionTool:\tDecorating region scheme " << name << " to events." << std::endl;
    }
    f->Close();

  }

  DileptonSimplePCRegionTool::~DileptonSimplePCRegionTool(){    
  }
  
  std::vector< std::pair<std::string, int> > DileptonSimplePCRegionTool::getEventRegions(std::vector<std::shared_ptr<TTHbb::Jet> > & jets){
    
    int x = -9;
    int y = -9;
    
    if(jets.size() >= 4){
      auto bins = getBin(jets);
      x = bins.first;
      y = bins.second;
    }
    
    std::vector<std::pair<std::string, int>> m_regionsVec;
    
    for(auto& def : regionhists){
      std::string name = "DilepRegion_" + def.first;
      if(x > 0 && y > 0){
	m_regionsVec.push_back(std::make_pair(name, def.second.GetBinContent(x,y)));
      }
      else{
 	m_regionsVec.push_back(std::make_pair(name, -9));
      }
    }  
    
    return m_regionsVec;
    
  }

  
  int DileptonSimplePCRegionTool::getEventRegionBin(std::vector<std::shared_ptr<TTHbb::Jet> > & jets, std::string region){
    
    int x = -9;
    int y = -9;
    
    if(jets.size() >= 4){
      auto bins = getBin(jets);
      x = bins.first;
      y = bins.second;
    }
    std::string name = "DilepRegion_" + region;
    if(x > 0 && y > 0){
      return regionhists[region].GetBinContent(x,y);
    }
    else{
      return -9;
    }
  }

  std::vector<std::string> DileptonSimplePCRegionTool::getRegionList(){
    std::vector<std::string> regions;
    for(auto& m : regionhists){
      regions.push_back(m.first);
    }
    return regions;
  }

  std::pair<int, int> DileptonSimplePCRegionTool::getBin(std::vector<std::shared_ptr<TTHbb::Jet> > & jets){
    JetOrderingTool::sortFloatDecorUp(jets, "mv2");
    
    if(jets.size() >= 4){
      int j1 = 5 - jets.at(0)->intVariable("tagWeightBin");
      int j2 = 5 - jets.at(1)->intVariable("tagWeightBin");
      int j3 = 5 - jets.at(2)->intVariable("tagWeightBin");
      int j4 = 5 - jets.at(3)->intVariable("tagWeightBin");
      
      JetOrderingTool::sortPt(jets);

      int y = j1 + 0.5 * j2 + 0.5 * j2 * j2 + 1;
      int x = (j4 != 4)? j4 + 3.5 * j3 - 0.5 * j3 * j3 + 1 : 11 + j3;

      if(y > 6 || y < 1 || x > 15 || x < 1){
	return std::make_pair(-1,-1);
      } 
      else{
	return std::make_pair(x,y);
      }
    }
    else{
      JetOrderingTool::sortPt(jets);
      return std::make_pair(-1,-1);
    }
  }

}
