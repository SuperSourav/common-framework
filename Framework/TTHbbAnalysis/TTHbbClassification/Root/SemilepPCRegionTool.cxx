/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbClassification/SemilepPCRegionTool.h"

#include "MVAVariables/JetOrderingTool.h"

#include "TFile.h"
#include "TCollection.h"
#include "TDirectory.h"
#include "TObject.h"

namespace TTHbb{

  SemilepPCRegionTool::SemilepPCRegionTool() : m_pcvarname("tagWeightBin") {

  }

  SemilepPCRegionTool::~SemilepPCRegionTool(){    
  }
  
  int SemilepPCRegionTool::SemilepPCRegionsNumber(TTHbb::JetContainer* jets, std::string option){
    int nJ = jets->size();
    int PCBdiscr = SemilepPCRegionDiscri(jets);
    return SemilepPCRegionsNumber(nJ, PCBdiscr, option);
  }

  int SemilepPCRegionTool::SemilepPCRegionsNumber(int nJ, int nB60, int nB70, int nB77, int nB85, std::string option){
    int PCBdiscr = SemilepPCRegionDiscri(nB60,nB70,nB77,nB85);
    return SemilepPCRegionsNumber(nJ, PCBdiscr, option);
  }

  int SemilepPCRegionTool::SemilepPCRegionsNumber(int nJ, int PCBdiscr, std::string option){

    if(nJ<4){
      //std::cout<<"WARNING:: SemilepPCRegionTool::SemilepPCRegionsNumber(): not supported # of jets < 4: nJ = "<<nJ<<std::endl;
      return nJ*100;
    }

    int regionNum = 0;
    if(option=="baseline"){
      if     (getSemilepPCRegionBit("baseline_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("baseline_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("baseline_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("baseline_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("baseline_5",PCBdiscr))  regionNum=5;
    }

    else if(option=="optionA"){
      if     (getSemilepPCRegionBit("optionA_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionA_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionA_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionA_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionA_5",PCBdiscr))  regionNum=5;
      else if(getSemilepPCRegionBit("optionA_6",PCBdiscr))  regionNum=6;
    }

    else if(option=="optionB"){
      if     (getSemilepPCRegionBit("optionB_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionB_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionB_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionB_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionB_5",PCBdiscr))  regionNum=5;
    }

    else if(option=="optionC"){
      if     (getSemilepPCRegionBit("optionC_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionC_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionC_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionC_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionC_5",PCBdiscr))  regionNum=5;
    }

    else if(option=="optionD"){
      if     (getSemilepPCRegionBit("optionD_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionD_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionD_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionD_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionD_5",PCBdiscr))  regionNum=5;
    }

    else if(option=="optionE"){
      if     (getSemilepPCRegionBit("optionE_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionE_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionE_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionE_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionE_5",PCBdiscr))  regionNum=5;
      else if(getSemilepPCRegionBit("optionE_6",PCBdiscr))  regionNum=6;
    }

    else if(option=="optionF"){
      if     (getSemilepPCRegionBit("optionF_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionF_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionF_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionF_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionF_5",PCBdiscr))  regionNum=5;
    }

    else if(option=="optionG"){
      if     (getSemilepPCRegionBit("optionG_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionG_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionG_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionG_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionG_5",PCBdiscr))  regionNum=5;
      else if(getSemilepPCRegionBit("optionG_6",PCBdiscr))  regionNum=6;
    }

    else if(option=="optionH"){
      if     (getSemilepPCRegionBit("optionH_1",PCBdiscr))  regionNum=1;
      else if(getSemilepPCRegionBit("optionH_2",PCBdiscr))  regionNum=2;
      else if(getSemilepPCRegionBit("optionH_3",PCBdiscr))  regionNum=3;
      else if(getSemilepPCRegionBit("optionH_4",PCBdiscr))  regionNum=4;
      else if(getSemilepPCRegionBit("optionH_5",PCBdiscr))  regionNum=5;
    }

    else if(option=="signalRegions"){
      if(nJ>=6 && getSemilepPCRegionBit("signalRegions_6J",PCBdiscr))      regionNum=1;
      else if(nJ==5 && getSemilepPCRegionBit("signalRegions_5J",PCBdiscr)) regionNum=1;
    }

    if(regionNum<0){
      std::cout<<"ERROR:: SemilepPCRegionTool::SemilepPCRegionsNumber(): not defined regions: "<<option<<std::endl;
      return -999;
    }

    return 100*(TMath::Min(nJ,6)) + regionNum;
  }

  bool SemilepPCRegionTool::getSemilepPCRegionBit(std::string name, TTHbb::JetContainer* jets){
    int PCBdiscr = SemilepPCRegionDiscri(jets);
    return getSemilepPCRegionBit(name, PCBdiscr);
  }

  bool SemilepPCRegionTool::getSemilepPCRegionBit(std::string name, int nB60, int nB70, int nB77, int nB85){
    int PCBdiscr = SemilepPCRegionDiscri(nB60,nB70,nB77,nB85);
    return getSemilepPCRegionBit(name, PCBdiscr);
  }

  bool SemilepPCRegionTool::getSemilepPCRegionBit(std::string name, int PCBdiscr){

    // int nB60 = getNBTags(PCBdiscr, 60);
    // int nB70 = getNBTags(PCBdiscr, 70);
    // int nB77 = getNBTags(PCBdiscr, 77);
    // int nB85 = getNBTags(PCBdiscr, 85);

    //=== baseline option ===//

    if(name=="baseline_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="baseline_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      if(PCBdiscr%100==44) return true; //*,*,70,70
      if(PCBdiscr%100==43) return true; //*,*,70,77
      if(PCBdiscr%100==33) return true; //*,*,77,77
      return false;
    }
    if(name=="baseline_3"){
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="baseline_4"){
      if(PCBdiscr%100==42) return true; //*,*,70,85
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==32) return true; //*,*,77,85
      if(PCBdiscr%100==31) return true; //*,*,77,100
      return false;
    }
    if(name=="baseline_5"){
      if(PCBdiscr==5522) return true; //60,60,85,85
      if(PCBdiscr==5521) return true; //60,60,85,100
      if(PCBdiscr==5511) return true; //60,60,100,100
      return false;
    }

    //=== optionA ===//

    if(name=="optionA_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionA_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      return false;
    }
    if(name=="optionA_3"){
      if(PCBdiscr%100==44) return true; //*,*,70,70
      if(PCBdiscr%100==43) return true; //*,*,70,77
      if(PCBdiscr%100==33) return true; //*,*,77,77
      return false;
    }
    if(name=="optionA_4"){
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionA_5"){
      if(PCBdiscr%100==42) return true; //*,*,70,85
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==32) return true; //*,*,77,85
      if(PCBdiscr%100==31) return true; //*,*,77,100
      return false;
    }
    if(name=="optionA_6"){
      if(PCBdiscr==5522) return true; //60,60,85,85
      if(PCBdiscr==5521) return true; //60,60,85,100
      if(PCBdiscr==5511) return true; //60,60,100,100
      return false;
    }

    //=== optionB ===//

    if(name=="optionB_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionB_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      return false;
    }
    if(name=="optionB_3"){
      if(PCBdiscr%100==44) return true; //*,*,70,70
      if(PCBdiscr%100==43) return true; //*,*,70,77
      if(PCBdiscr%100==33) return true; //*,*,77,77
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionB_4"){
      if(PCBdiscr%100==42) return true; //*,*,70,85
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==32) return true; //*,*,77,85
      if(PCBdiscr%100==31) return true; //*,*,77,100
      return false;
    }
    if(name=="optionB_5"){
      if(PCBdiscr==5522) return true; //60,60,85,85
      if(PCBdiscr==5521) return true; //60,60,85,100
      if(PCBdiscr==5511) return true; //60,60,100,100
      return false;
    }

    //=== optionC ===//

    if(name=="optionC_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      if(PCBdiscr==5554) return true; //60,60,60,70
      return false;
    }
    if(name=="optionC_2"){
      if(PCBdiscr==5553) return true; //60,60,60,77
      if(PCBdiscr%100==44) return true; //*,*,70,70
      if(PCBdiscr%100==43) return true; //*,*,70,77
      if(PCBdiscr%100==33) return true; //*,*,77,77
      return false;
    }
    if(name=="optionC_3"){
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionC_4"){
      if(PCBdiscr%100==42) return true; //*,*,70,85
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==32) return true; //*,*,77,85
      if(PCBdiscr%100==31) return true; //*,*,77,100
      return false;
    }
    if(name=="optionC_5"){
      if(PCBdiscr==5522) return true; //60,60,85,85
      if(PCBdiscr==5521) return true; //60,60,85,100
      if(PCBdiscr==5511) return true; //60,60,100,100
      return false;
    }

    //=== optionD ===//

    if(name=="optionD_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionD_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5544) return true; //60,60,70,70
      if(PCBdiscr==5543) return true; //60,60,70,77
      return false;
    }
    if(name=="optionD_3"){
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionD_4"){
      if(PCBdiscr%1000==444) return true; //*,70,70,70
      if(PCBdiscr%1000==443) return true; //*,70,70,77
      if(PCBdiscr%100==42)   return true; //*,*,70,85
      if(PCBdiscr%100==33)   return true; //*,*,77,77
      if(PCBdiscr%100==32)   return true; //*,*,77,85
      return false;
    }
    if(name=="optionD_5"){
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==31) return true; //*,*,77,100
      if(PCBdiscr==5522)   return true; //60,60,85,85
      if(PCBdiscr==5521)   return true; //60,60,85,100
      if(PCBdiscr==5511)   return true; //60,60,100,100
      return false;
    }

    //=== optionE ===//

    if(name=="optionE_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionE_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      return false;
    }
    if(name=="optionE_3"){
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5544) return true; //60,60,70,70
      if(PCBdiscr==5543) return true; //60,60,70,77
      return false;
    }
    if(name=="optionE_4"){
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionE_5"){
      if(PCBdiscr%1000==444) return true; //*,70,70,70
      if(PCBdiscr%1000==443) return true; //*,70,70,77
      if(PCBdiscr%100==42)   return true; //*,*,70,85
      if(PCBdiscr%100==33)   return true; //*,*,77,77
      if(PCBdiscr%100==32)   return true; //*,*,77,85
      return false;
    }
    if(name=="optionE_6"){
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==31) return true; //*,*,77,100
      if(PCBdiscr==5522)   return true; //60,60,85,85
      if(PCBdiscr==5521)   return true; //60,60,85,100
      if(PCBdiscr==5511)   return true; //60,60,100,100
      return false;
    }

    //=== optionF ===//

    if(name=="optionF_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionF_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5544) return true; //60,60,70,70
      if(PCBdiscr==5543) return true; //60,60,70,77
      if(PCBdiscr==5542) return true; //60,60,70,85
      if(PCBdiscr==5533) return true; //60,60,77,77
      if(PCBdiscr==5444) return true; //60,70,70,70
      return false;
    }
    if(name=="optionF_3"){
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionF_4"){
      if(PCBdiscr==4444)     return true; //70,70,70,70
      if(PCBdiscr%1000==443) return true; //*,70,70,77
      if(PCBdiscr%1000==442) return true; //*,70,70,85
      if(PCBdiscr%1000==433) return true; //*,70,77,77
      if(PCBdiscr%1000==333) return true; //*,77,77,77
      if(PCBdiscr%100==32)   return true; //*,*,77,85
      if(PCBdiscr%1000==441) return true; //*,70,70,100
      if(PCBdiscr%1000==431) return true; //*,70,77,100
      if(PCBdiscr%1000==331) return true; //*,77,77,100
      return false;
    }
    if(name=="optionF_5"){
      if(PCBdiscr==5541) return true; //60,60,70,100
      if(PCBdiscr==5531) return true; //60,60,77,100
      if(PCBdiscr==5522) return true; //60,60,85,85
      if(PCBdiscr==5521) return true; //60,60,85,100
      if(PCBdiscr==5511) return true; //60,60,100,100
      return false;
    }

    //=== optionG ===//

    if(name=="optionG_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionG_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      return false;
    }
    if(name=="optionG_3"){
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5544) return true; //60,60,70,70
      if(PCBdiscr==5543) return true; //60,60,70,77
      if(PCBdiscr==5542) return true; //60,60,70,85
      if(PCBdiscr==5533) return true; //60,60,77,77
      if(PCBdiscr==5444) return true; //60,70,70,70
      return false;
    }
    if(name=="optionG_4"){
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionG_5"){
      if(PCBdiscr==4444)     return true; //70,70,70,70
      if(PCBdiscr%1000==443) return true; //*,70,70,77
      if(PCBdiscr%1000==442) return true; //*,70,70,85
      if(PCBdiscr%1000==433) return true; //*,70,77,77
      if(PCBdiscr%1000==333) return true; //*,77,77,77
      if(PCBdiscr%100==32)   return true; //*,*,77,85
      if(PCBdiscr%1000==441) return true; //*,70,70,100
      if(PCBdiscr%1000==431) return true; //*,70,77,100
      if(PCBdiscr%1000==331) return true; //*,77,77,100
      return false;
    }
    if(name=="optionG_6"){
      if(PCBdiscr==5541) return true; //60,60,70,100
      if(PCBdiscr==5531) return true; //60,60,77,100
      if(PCBdiscr==5522) return true; //60,60,85,85
      if(PCBdiscr==5521) return true; //60,60,85,100
      if(PCBdiscr==5511) return true; //60,60,100,100
      return false;
    }

    //=== optionH ===//

    if(name=="optionH_1"){
      if(PCBdiscr==5555) return true; //60,60,60,60
      return false;
    }
    if(name=="optionH_2"){
      if(PCBdiscr==5554) return true; //60,60,60,70
      if(PCBdiscr==5553) return true; //60,60,60,77
      if(PCBdiscr==5552) return true; //60,60,60,85
      if(PCBdiscr==5544) return true; //60,60,70,70
      return false;
    }
    if(name=="optionH_3"){
      if(PCBdiscr==5543) return true; //60,60,70,77
      if(PCBdiscr==5542) return true; //60,60,70,85
      if(PCBdiscr==5551) return true; //60,60,60,100
      return false;
    }
    if(name=="optionH_4"){
      if(PCBdiscr%1000==444) return true; //*,70,70,70
      if(PCBdiscr%1000==443) return true; //*,70,70,77
      if(PCBdiscr%1000==442) return true; //*,70,70,85
      if(PCBdiscr%100==33)   return true; //*,*,77,77
      if(PCBdiscr%100==32)   return true; //*,*,77,85
      return false;
    }
    if(name=="optionH_5"){
      if(PCBdiscr%100==41) return true; //*,*,70,100
      if(PCBdiscr%100==31) return true; //*,*,77,100
      if(PCBdiscr==5522)   return true; //60,60,85,85
      if(PCBdiscr==5521)   return true; //60,60,85,100
      if(PCBdiscr==5511)   return true; //60,60,100,100
      return false;
    }

    //=== SR ===//

    if(name=="signalRegions_6J"){
       int nB77 = getNBTags(PCBdiscr, 77);
       int nB85 = getNBTags(PCBdiscr, 85);
       if(nB77>=3 || nB85>=4) return true;
       return false;
    }
    if(name=="signalRegions_5J"){
       int nB60 = getNBTags(PCBdiscr, 60);
       int nB85 = getNBTags(PCBdiscr, 85);
       if(nB60>=3 || nB85>=4) return true;
       return false;
    }

    return false;

  }

  int SemilepPCRegionTool::getBTagWPMapAddressX(int PCBdiscr){

    int wp3 = (PCBdiscr/10)%10;
    int wp4 = (PCBdiscr/1)%10;
    
    if(wp3==5)
      if     (wp4==5) return 0;
      else if(wp4==4) return 1;
      else if(wp4==3) return 2;
      else if(wp4==2) return 3;
      else            return 10;
    else if(wp3==4)
      if     (wp4==4) return 4;
      else if(wp4==3) return 5;
      else if(wp4==2) return 6;
      else            return 11;
    else if(wp3==3)
      if     (wp4==3) return 7;
      else if(wp4==2) return 8;
      else            return 12;
    else if(wp3==2)
      if     (wp4==2) return 9;
      else            return 13;
    else return 14;
    
    return -1;
    
  }

  int SemilepPCRegionTool::getBTagWPMapAddressY(int PCBdiscr){
    
    int wp1 = (PCBdiscr/1000)%10;
    int wp2 = (PCBdiscr/100)%10;
    
    if(wp2==5)
      if     (wp1==5) return 0;
      else {;}
    else if(wp2==4)
      if     (wp1==5) return 1;
      else if(wp1==4) return 2;
      else {;}
    else if(wp2==3)
      if     (wp1==5) return 3;
      else if(wp1==4) return 4;
      else if(wp1==3) return 5;
      else {;}
    else if(wp2==2)
      if     (wp1==5) return 6;
      else if(wp1==4) return 7;
      else if(wp1==3) return 8;
      else if(wp1==2) return 9;
      else {;}
    else
      if     (wp1==5) return 10;
      else if(wp1==4) return 11;
      else if(wp1==3) return 12;
      else if(wp1==2) return 13;
      else return 14;
    
    return -1;
    
  }

  int SemilepPCRegionTool::getNBTags(int PCBdiscr, int WP){
    
    if(WP!=60 && WP!=70 && WP!=77 && WP!=85){
      std::cout<<"ERROR:: SemilepPCRegionTool::getNBTags(): invalid WP = "<<WP<<std::endl;
      return -1;
    }

    int pcbwp=-1;
    if(WP==60) pcbwp=5;
    else if(WP==70) pcbwp=4;
    else if(WP==77) pcbwp=3;
    else if(WP==85) pcbwp=2;

    int nB=0;

    if(((PCBdiscr/1000)%10)>=pcbwp)nB++;
    if(((PCBdiscr/100)%10)>=pcbwp)nB++;
    if(((PCBdiscr/10)%10)>=pcbwp)nB++;
    if(((PCBdiscr/1)%10)>=pcbwp)nB++;

    return nB;
    
  }

  int SemilepPCRegionTool::SemilepPCRegionDiscri(int nB60, int nB70, int nB77, int nB85){

    int PCBdiscr = 1111;
    
    for(int ib=0; ib<TMath::Min(nB85,4); ib++) PCBdiscr+=TMath::Power(10,3-ib);
    for(int ib=0; ib<TMath::Min(nB77,4); ib++) PCBdiscr+=TMath::Power(10,3-ib);
    for(int ib=0; ib<TMath::Min(nB70,4); ib++) PCBdiscr+=TMath::Power(10,3-ib);
    for(int ib=0; ib<TMath::Min(nB60,4); ib++) PCBdiscr+=TMath::Power(10,3-ib);

    return PCBdiscr;

  }

  int SemilepPCRegionTool::SemilepPCRegionDiscri(TTHbb::JetContainer* jets){

    std::vector<int> jetmv2;
    jetmv2.clear();
    jetmv2.resize(jets->size());
    int ij=0;
    for(auto part: *jets){
      jetmv2[ij] = part->intVariable(m_pcvarname);
      ij++;
    }
    std::sort(std::begin(jetmv2),std::end(jetmv2),[](int v1, int v2){return (v1>v2);});

    int PCBdiscr = -1;
  
    if(jets->size() >=4 && jetmv2[0]>=0 && jetmv2[1]>=0 && jetmv2[2]>=0 && jetmv2[3]>=0){
	PCBdiscr = 1000*jetmv2[0] + 100*jetmv2[1] + 10*jetmv2[2] + jetmv2[3];
    }

    return PCBdiscr;

  }


}
