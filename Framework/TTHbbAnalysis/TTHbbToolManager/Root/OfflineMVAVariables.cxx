/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/OfflineMVAVariables.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

#include "MVAVariables/JetOrderingTool.h"

#include <iostream>

namespace TTHbb{

  OfflineMVAVariables::OfflineMVAVariables(std::string name) 
    : ToolBase(true)
  {
    m_name = name;
  }

  OfflineMVAVariables::~OfflineMVAVariables(){
  }

  void OfflineMVAVariables::initialise(){
    auto* config = TTHbb::GlobalConfiguration::get();
    std::vector<std::string> bTagWPs = 
      TTHbb::util::vectoriseString((*config)(m_name + ".bTagWPs"));
    
    for(auto wp : bTagWPs){
      mapOfMVAVars.insert(std::make_pair(wp,std::make_shared<MVAVariables>()));
      (mapOfMVAVars[wp])->setBtagging(wp);
    }
  }

  void OfflineMVAVariables::apply(TTHbb::Event* event){
    for(auto& mva : mapOfMVAVars){
      if(mva.first.find("Sort") == std::string::npos){
	mva.second->initialiseEvent(event);
      }
      else{
	std::vector< std::shared_ptr<TTHbb::Particle> > bjets, ujets;
	int maxB = std::stoi(mva.first.substr(4));
	JetOrderingTool::sortFloatDecorDown(event->m_jets,"mv2");
	int i = 0;
	for(auto jet : event->m_jets){
	  if(i <= maxB){
	    bjets.push_back(jet);
	  }
	  else{
	    ujets.push_back(jet);
	  }
	  ++i;
	}

	std::vector< std::shared_ptr<TTHbb::Particle> > jets;
	jets.reserve(event->m_jets.size());
	jets.insert(jets.end(), event->m_jets.begin(), event->m_jets.end());
	std::vector< std::shared_ptr<TTHbb::Particle> > leps;
	leps.reserve(event->m_leptons.size());
	leps.insert(leps.end(), event->m_leptons.begin(), event->m_leptons.end());
	std::vector< std::shared_ptr<TTHbb::Particle> > all;
	all.reserve(jets.size() + leps.size());
	all.insert(all.end(), jets.begin(), jets.end());
	all.insert(all.end(), leps.begin(), leps.end());
	mva.second->initialiseEvent(jets,
				    bjets,
				    ujets,
				    leps,
				    all,
				    event->met_met,
				    event->met_phi);
      }    
    }
  }
  
  void OfflineMVAVariables::finalise(){
    for(auto& mva : mapOfMVAVars){
      mva.second.reset();
    }
    mapOfMVAVars.clear();
  }

  std::shared_ptr<MVAVariables> OfflineMVAVariables::getMVAVariables(std::string name){
    if(!mapOfMVAVars.count(name)){
      std::cout << "ERROR in OfflineMVAVariables::getMVAVariables:"
		<<"\tTrying to access instance of MVAVariables that doesn't exist:" 
		<< name << std::endl;
      exit(55);
    }
    else{
      //std::weak_ptr<MVAVariables> inst = mapOfMVAVars.at(name);
      return mapOfMVAVars[name];
    }
  }
}
