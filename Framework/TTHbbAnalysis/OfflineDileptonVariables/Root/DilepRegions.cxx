#include "OfflineDileptonVariables/DilepRegions.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

#include "MVAVariables/JetOrderingTool.h"

#include <memory>
#include "TFile.h"
#include "TCollection.h"
#include "TDirectory.h"
#include "TObject.h"

namespace TTHbb{

  void DilepRegions::initialise(){

    auto* config = TTHbb::GlobalConfiguration::get();
    std::string fileName = (*config)("DilepRegionsFile");

    TFile* f = TFile::Open(fileName.c_str());
    TIter next(f->GetListOfKeys());
    TObject* obj = 0;
    while ((obj = next())){
      std::string name = obj->GetName();
      regionhists[name] = *(TH2F*)(f->Get(name.c_str())->Clone(name.c_str()));
      std::cout << "INFO in TTHbb::DilepRegions:\tDecorating region scheme " << name << " to events." << std::endl;
    }
    f->Close();
  }

  void DilepRegions::apply(TTHbb::Event* event){
    
    int x = -9;
    int y = -9;
    
    if(event->m_jets.size() >= 4){
      auto bins = getBin(event->m_jets);
      x = bins.first;
      y = bins.second;
    }
    
    for(auto def : regionhists){
      std::string name = "DilepRegion_" + def.first;
      if(x > 0 && y > 0){
	event->floatVariable(name) = def.second.GetBinContent(x,y);
      }
      else{
      	event->floatVariable(name) = -9;
      }
    }  
  }

  void DilepRegions::finalise(){
    /*for(auto def : regionhists){
      def.second->Delete();
    }
    regionhists.clear();*/
  }

  std::pair<int, int> DilepRegions::getBin(std::vector<std::shared_ptr<TTHbb::Jet> > & jets){
    JetOrderingTool::sortFloatDecorUp(jets, "mv2");

    if(jets.size() >= 4){
      int j1 = 5 - jets.at(0)->intVariable("tagWeightBin");
      int j2 = 5 - jets.at(1)->intVariable("tagWeightBin");
      int j3 = 5 - jets.at(2)->intVariable("tagWeightBin");
      int j4 = 5 - jets.at(3)->intVariable("tagWeightBin");
      
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
      return std::make_pair(-1,-1);
    }
  }
}
