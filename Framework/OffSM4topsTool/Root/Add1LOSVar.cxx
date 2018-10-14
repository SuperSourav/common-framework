#include "OffSM4topsTool/Add1LOSVar.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

namespace TTHbb{

  // ----------------------------- public -----------------------------------------

  void Add1LOSVar::initialise(){
    // Check if input ntuple is data
    isData = false;
    auto* config = TTHbb::GlobalConfiguration::get();
    if(TTHbb::util::iequals((*config)("SampleType"),"data"))
      isData = true;   
  }

  void Add1LOSVar::apply(TTHbb::Event* event){
    event->intVariable("SR") = signalRegion(event);
  }

  void Add1LOSVar::finalise(){
  }

  // ----------------------------- private -----------------------------------------
  
  int Add1LOSVar::signalRegion(TTHbb::Event* event){    

    // Todo: Implement Trigger Matching cuts

    // Extract the lepton channel flavor
    int lepChan = event->intVariable("lepFlavChannel");    

    // Check 1L channel
    if( lepChan == 1 || lepChan == 2 ){ // 1L channel 

      // Pre-selection
      float met = event->met_met, mtw = event->floatVariable("mtw");
      if( met < 20000 || (met+mtw) < 60000 ) return 0;

      // Signal region caterogization
      int Nj = event->intVariable("nJets");
      int NJ = event->intVariable("nRCJets");
      int Nb = event->intVariable("nBTags_MV2c10_77");
      if( Nb==3 and Nj==9 and NJ==0 ) return 1;
      else if( Nb==3 and Nj==9 and NJ==1 ) return 2;
      else if( Nb==3 and Nj==9 and NJ>1 ) return 3;
      else if( Nb>3 and Nj==9 and NJ==0 ) return 4;
      else if( Nb>3 and Nj==9 and NJ==1 ) return 5;
      else if( Nb>3 and Nj==9 and NJ>1 ) return 6;
      else if( Nb==3 and Nj>9 and NJ==0 ) return 7;
      else if( Nb==3 and Nj>9 and NJ==1 ) return 8;
      else if( Nb==3 and Nj>9 and NJ>1 ) return 9;
      else if( Nb>3 and Nj>9 and NJ==0 ) return 10;
      else if( Nb>3 and Nj>9 and NJ==1 ) return 11;
      else if( Nb>3 and Nj>9 and NJ>1 ) return 12;
      else return 0;
    }    

    // Check 2LOS channel
    else if( lepChan == 3 || lepChan == 4 || lepChan == 5 ){ // 2LOS channel 

      // Pre-selection
      if( lepChan == 3 || lepChan == 4 ){ 
	float mll = event->floatVariable("mll");
	if( mll<15000 || (mll>83000 && mll<99000) ) return 0;
      }

      // Signal region caterogization
      int Nj = event->intVariable("nJets");
      int NJ = event->intVariable("nRCJets");
      int Nb = event->intVariable("nBTags_MV2c10_77");
      if( Nb==3 and Nj==7 and NJ==0 ) return 13;
      else if( Nb==3 and Nj==7 and NJ>0 ) return 14;
      else if( Nb>3 and Nj==7 and NJ==0 ) return 15;
      else if( Nb>3 and Nj==7 and NJ>0 ) return 16;
      else if( Nb==3 and Nj>7 and NJ==0 ) return 17;
      else if( Nb==3 and Nj>7 and NJ>0 ) return 18;
      else if( Nb>3 and Nj>7 and NJ==0 ) return 19;
      else if( Nb>3 and Nj>7 and NJ>0 ) return 20;
      else return 0;
    }

    return 0;
  }

}

