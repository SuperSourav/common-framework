/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/OnlineMVAVariables.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "MVAVariables/JetOrderingTool.h"

#include <iostream>
#include <algorithm>

OnlineMVAVariables::OnlineMVAVariables(std::string params, std::shared_ptr<top::TopConfig> config) :
  m_config(config){

  /// Dilepton BDT variables

  this->addFloatVariable("dEtabb_Avg", 
			 [](MVAVariables* mva){ return mva->deltaEtaofPair(pairing::bb, variable::Avg);},
			 true);
  this->addFloatVariable("dRbb_MaxPt", 
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::MaxPt);},
			 true);
  this->addFloatVariable("Mbb_HiggsMass",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::HiggsMass);},
			 true);
  this->addFloatVariable("dRbb_HiggsMass",
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::HiggsMass);},
			 true);
  this->addIntVariable  ("nHiggsbb30",
			 [](MVAVariables* mva){ return mva->nHiggsCandidatesMassWindow(pairing::bb, 30);},
			 true);
  this->addFloatVariable("dEtajl_MaxdEta",
			 [](MVAVariables* mva){ return mva->deltaEtaofPair(pairing::jl, variable::MaxdEta);});
  this->addFloatVariable("H0_all",
			 [](MVAVariables* mva){ return mva->FirstFoxWolframMoment(collection::all);});
  this->addFloatVariable("Mbb_MinM",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::MinM);},
			 true);
  this->addFloatVariable("Aplanarity_jets",
			 [](MVAVariables* mva){ return mva->Aplanarity(collection::jets);});
  this->addFloatVariable("Centrality_all",
			 [](MVAVariables* mva){ return mva->Centrality(collection::all);});
  this->addFloatVariable("Mbb_MindR",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::MindR);},
			 true);
  this->addFloatVariable("dEtabb_MaxdEta",
			 [](MVAVariables* mva){ return mva->deltaEtaofPair(pairing::bb, variable::MaxdEta);},
			 true);
  this->addFloatVariable("dEtajj_Avg",
			 [](MVAVariables* mva){ return mva->deltaEtaofPair(pairing::jj, variable::Avg);});
  this->addFloatVariable("dRbb_MaxM",
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::MaxM);},
			 true);
  this->addIntVariable  ("nHiggsjj30",
			 [](MVAVariables* mva){ return mva->nHiggsCandidatesMassWindow(pairing::jj, 30);});
  this->addFloatVariable("HT_all",
			 [](MVAVariables* mva){ return mva->HT(collection::all);});
  this->addFloatVariable("H2_jets",
			 [](MVAVariables* mva){ return mva->ThirdFoxWolframMoment(collection::jets);});
  this->addFloatVariable("Mbb_MaxPt",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::MaxPt);},
			 true);
  this->addFloatVariable("pTbb_MindR",
			 [](MVAVariables* mva){ return mva->PtofPair(pairing::bb, variable::MindR);},
			 true);
  this->addIntVariable  ("nJets_Pt40",
			 [](MVAVariables* mva){ return mva->nJetsAbovePt(40);});
  /// Dilepton Run One variables not in BDT variables
  //this->addFloatVariable("dRbb_MaxPt",
  //			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::MaxPt); },
  //			 true);
  this->addFloatVariable("Mjj_MaxPt",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::jj, variable::MaxPt); });
  this->addFloatVariable("dEtajj_MaxdEta",
			 [](MVAVariables* mva){ return mva->deltaEtaofPair(pairing::jj, variable::MaxdEta); });
  this->addFloatVariable("Mjj_MinM",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::jj, variable::MinM); });
  this->addFloatVariable("Mjj_HiggsMass",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::jj, variable::HiggsMass); });
  this->addFloatVariable("dRlj_MindR",
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::jl, variable::MindR); });
  this->addFloatVariable("H4_all",
			 [](MVAVariables* mva){ return mva->FifthFoxWolframMoment(collection::all); });
  this->addFloatVariable("pT_jet3",
			 [](MVAVariables* mva){ return mva->getPtOrderedJet(collection::jets, 3)->Pt(); });


  /// Semilep Run One variables
  this->addFloatVariable("Centrality_all",
			 [](MVAVariables* mva){ return mva->Centrality(collection::all); });
  this->addFloatVariable("Mbb_MindR",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::MindR); },
			 true);
  //this->addFloatVariable("dRbb_MaxPt",
  //			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::MaxPt); },
  //			 true);
  this->addFloatVariable("Mjj_MaxPt",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::jj, variable::MaxPt); });
  this->addFloatVariable("pT_jet5",
			 [](MVAVariables* mva){ return mva->getPtOrderedJet(collection::jets, 5)->Pt(); });
  this->addFloatVariable("H1_all",
			 [](MVAVariables* mva){ return mva->SecondFoxWolframMoment(collection::all); });
  this->addFloatVariable("dRbb_avg",
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::Avg); },
			 true);
  this->addFloatVariable("Mbj_MaxPt",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bj, variable::MaxPt); },
			 true);
  this->addFloatVariable("Muu_MindR",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::uu, variable::MindR); },
			 true);
  this->addFloatVariable("Aplanarity_bjets",
			 [](MVAVariables* mva){ return mva->Aplanarity(collection::bjets); },
			 true);
  this->addFloatVariable("Mbj_MindR",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bj, variable::MindR); },
			 true);
  this->addFloatVariable("HT_jets",
			 [](MVAVariables* mva){ return mva->HT(collection::jets); });
  this->addFloatVariable("Mjj_MindR",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::jj, variable::MindR); });
  this->addFloatVariable("Mbb_MaxPt",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::MaxPt); },
			 true);
  this->addFloatVariable("pTuu_MindR",
			 [](MVAVariables* mva){ return mva->PtofPair(pairing::uu, variable::MindR); },
			 true);
  this->addFloatVariable("Mbb_MaxM",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bb, variable::MaxM); },
			 true);
  this->addFloatVariable("dRuu_MindR",
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::uu, variable::MindR); },
			 true);
  this->addFloatVariable("Mjjj_MaxPt",
			 [](MVAVariables* mva){ return mva->MassofJetTriplet(variable::MaxPt); });
  this->addFloatVariable("Mbj_Wmass",
			 [](MVAVariables* mva){ return mva->MassofPair(pairing::bj, variable::Wmass); },
			 true);
  this->addFloatVariable("dRbj_Wmass",
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bj, variable::Wmass); },
			 true);
  this->addFloatVariable("dRlepbb_MindR",
			 [](MVAVariables* mva){ std::shared_ptr<TTHbb::Particle> lep = mva->getPtOrderedLepton(1);
			   std::shared_ptr<PairedSystem> lepbb_MindR = std::make_shared<PairedSystem>(mva->getEntry(pairing::bb, variable::MindR).p4(), lep->p4());
			   return lepbb_MindR->DeltaR();},
			 true);
  this->addIntVariable  ("nJets_Pt40",
			 [](MVAVariables* mva){ return mva->nJetsAbovePt(40);});

  this->addFloatVariable("dRbb_MindR", 
			 [](MVAVariables* mva){ return mva->deltaRofPair(pairing::bb, variable::MindR);},
			 true);

  /********************************************/

  m_params = params;
  bTagWPs = TTHbb::util::vectoriseString(params," ");
  for(auto wp : bTagWPs){
    if(wp.find("Sort") != std::string::npos){
      sortMethods.push_back(wp);
    }
  }
  for(auto wp : sortMethods){
    bTagWPs.erase(std::remove(bTagWPs.begin(), bTagWPs.end(), wp), bTagWPs.end());
  }

  initialise();

  /********************************************/

  auto* esh = EventSaverHelper::get();

  for(auto var : intDict){
    esh->addVariableToBeSaved(var.first, TTHbb::DecorationType::Int/*,false,"OnlineMVAVariables"*/);
  }

  for(auto var : floatDict){
    esh->addVariableToBeSaved(var.first, TTHbb::DecorationType::Float/*,false,"OnlineMVAVariables"*/);
  }

  for(auto wp : bTagWPs){
    for(auto var : bTagIntDict){
      esh->addVariableToBeSaved(var.first + "_" + TTHbb::util::shortBtagWP(wp), TTHbb::DecorationType::Int/*,false,"OnlineMVAVariables"*/);
    }
    for(auto var : bTagFloatDict){
      esh->addVariableToBeSaved(var.first + "_" + TTHbb::util::shortBtagWP(wp), TTHbb::DecorationType::Float/*,false,"OnlineMVAVariables"*/);
    }
  }

  for(auto wp : sortMethods){
    for(auto var : bTagIntDict){
      esh->addVariableToBeSaved(var.first + "_" + TTHbb::util::shortBtagWP(wp), TTHbb::DecorationType::Int/*,false,"OnlineMVAVariables"*/);
    }
    for(auto var : bTagFloatDict){
      esh->addVariableToBeSaved(var.first + "_" + TTHbb::util::shortBtagWP(wp), TTHbb::DecorationType::Float/*,false,"OnlineMVAVariables"*/);
    }
  }
}

OnlineMVAVariables::~OnlineMVAVariables(){
  for(auto& mva : mapOfMVAVars){
    mva.second.reset();
  }
  mapOfMVAVars.clear();

  for(auto& mva : mapOfMVAVars_sort){
    mva.second.reset();
  }
  mapOfMVAVars_sort.clear();
}

bool OnlineMVAVariables::apply(const top::Event & event) const{

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  calculateVariables(tthevt.get());

  return true;
}

void OnlineMVAVariables::initialise(){
  for(auto wp : bTagWPs){
    std::string swp = TTHbb::util::shortBtagWP(wp); 
    std::cout << "INFO: Making MVAVariables for " << swp << std::endl;
    mapOfMVAVars.insert(std::make_pair(swp,std::make_shared<MVAVariables>()));
    (mapOfMVAVars[swp])->setBtagString("isbtagged_"+swp);
  }
  
  for(auto method : sortMethods){
    std::cout << "INFO: Making MVAVariables for " << method << std::endl;
    mapOfMVAVars_sort.insert(std::make_pair(method,std::make_shared<MVAVariables>()));
  }

}

void OnlineMVAVariables::calculateVariables(TTHbb::Event* evt) const{

  for(auto mva : mapOfMVAVars){
    mva.second->initialiseEvent(evt);
  }

  for(auto mva : mapOfMVAVars_sort){
    std::vector< std::shared_ptr<TTHbb::Particle> > bjets, ujets;
    int maxB = std::stoi(mva.first.substr(4));
    for(auto jet : evt->m_jets){
      if(jet->intVariable("tagWeightBin_order") <= maxB){
	bjets.push_back(jet);
      }
      else{
	ujets.push_back(jet);
      }
    }
    std::vector< std::shared_ptr<TTHbb::Particle> > jets;
    jets.reserve(evt->m_jets.size());
    jets.insert(jets.end(), evt->m_jets.begin(), evt->m_jets.end());
    std::vector< std::shared_ptr<TTHbb::Particle> > leps;
    leps.reserve(evt->m_leptons.size());
    leps.insert(leps.end(), evt->m_leptons.begin(), evt->m_leptons.end());
    std::vector< std::shared_ptr<TTHbb::Particle> > all;
    all.reserve(jets.size() + leps.size());
    all.insert(all.end(), jets.begin(), jets.end());
    all.insert(all.end(), leps.begin(), leps.end());
    mva.second->initialiseEvent(jets,
				bjets,
				ujets,
				leps,
				all,
				evt->met_met,
				evt->met_phi);
  }

  for(auto var : intDict){
    evt->intVariable(var.first) = var.second(  (mapOfMVAVars.begin())->second.get() );
  }
  for(auto var : floatDict){
    evt->floatVariable(var.first) = var.second( (mapOfMVAVars.begin())->second.get() );
  }

  for(auto mva : mapOfMVAVars){
    for(auto var : bTagIntDict){
      evt->intVariable((var.first + "_" + mva.first)) = var.second(mva.second.get());
    }
    for(auto var : bTagFloatDict){
      evt->floatVariable((var.first + "_" + mva.first)) = var.second(mva.second.get());
    } 
  }

  for(auto mva : mapOfMVAVars_sort){
    for(auto var : bTagIntDict){
      evt->intVariable((var.first + "_" + mva.first)) = var.second(mva.second.get());
    }
    for(auto var : bTagFloatDict){
      evt->floatVariable((var.first + "_" + mva.first)) = var.second(mva.second.get());
    } 
  }
}

void OnlineMVAVariables::addIntVariable(std::string name, std::function<int(MVAVariables*)> func, bool isBtag){//=false){
  if(isBtag){
    if(bTagIntDict.find(name) != bTagIntDict.end()){
      return;
    }
    bTagIntDict.insert(std::make_pair(name, func));
  }
  else{
    if(intDict.find(name) != intDict.end()){
      return;
    }
    intDict.insert(std::make_pair(name, func));
  }  
}

void OnlineMVAVariables::addFloatVariable(std::string name, std::function<float(MVAVariables*)> func, bool isBtag){//=false){
  if(isBtag){
    if(bTagFloatDict.find(name) != bTagFloatDict.end()){
      return;
    }
    bTagFloatDict.insert(std::make_pair(name,func));
  }
  else{
    if(floatDict.find(name) != floatDict.end()){
      return;
    }
    floatDict.insert(std::make_pair(name, func));
  }  
}

std::string OnlineMVAVariables::name() const{
  return "MVAVARS";
}
