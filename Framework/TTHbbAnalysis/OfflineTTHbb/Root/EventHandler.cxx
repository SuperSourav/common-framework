/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "OfflineTTHbb/EventHandler.h"

#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include "TDirectory.h"
#include "TSystem.h"
#include "TFile.h"

namespace TTHbb{

  EventHandler::EventHandler() : currentEvent(0), isData(false), doBfilter(false), doDilFilter(false), 
				 m_currentSystematic("nominal"), isNominalTree(true), useLargeJets(false)
  {
    initialisePointers();
    checkCollections();

    /// For variables which are optional depending on tools, get the config
    auto * config = TTHbb::GlobalConfiguration::get();

    /// Add the heavy flavour filter if doing ttbar skimming
    if(TTHbb::util::iequals((*config)("DoTTbarBfilter"),"true")){
      addVariableToEventDecor("int", "TopHeavyFlavorFilterFlag");
    }
    /// Add the dilepton filter if combining dilepton and inclusive samples
    if(TTHbb::util::iequals((*config)("DoTTbarDileptonFilter"),"true")){
      addVariableToEventDecor("int", "truth_top_dilep_filter");
    }
    /// Variables needed when running OfflineFakes
    if(TTHbb::util::iequals((*config)("OfflineFakes.TurnOn"),"true")){
      addVariableToObjectDecor("electron", "char", "el_isTight");
      addVariableToObjectDecor("muon", "char", "mu_isTight");
      addVariableToEventDecor("char", "HLT_e24_lhmedium_nod0_L1EM18VH");
      addVariableToEventDecor("char", "HLT_e26_lhtight_nod0_ivarloose");
      addVariableToEventDecor("char", "HLT_e60_lhmedium_nod0");
      addVariableToEventDecor("char", "HLT_e140_lhloose_nod0");
      addVariableToEventDecor("char", "HLT_e24_lhmedium_L1EM20VH");
      addVariableToEventDecor("char", "HLT_e60_lhmedium");
      addVariableToEventDecor("char", "HLT_e120_lhloose");
      addVariableToEventDecor("char", "HLT_mu26_ivarmedium");
      addVariableToEventDecor("char", "HLT_mu50");
      addVariableToEventDecor("char", "HLT_mu24");
      addVariableToEventDecor("char", "HLT_mu20_iloose_L1MU15");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e24_lhmedium_nod0_L1EM18VH");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e26_lhtight_nod0_ivarloose");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e60_lhmedium_nod0");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e140_lhloose_nod0");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e24_lhmedium_L1EM20VH");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e60_lhmedium");
      addVariableToObjectDecor("electron", "char", "el_trigMatch_HLT_e120_lhloose");
      addVariableToObjectDecor("muon", "char", "mu_trigMatch_HLT_mu26_ivarmedium");
      addVariableToObjectDecor("muon", "char", "mu_trigMatch_HLT_mu50");
      addVariableToObjectDecor("muon", "char", "mu_trigMatch_HLT_mu24");
      addVariableToObjectDecor("muon", "char", "mu_trigMatch_HLT_mu20_iloose_L1MU15");
      addVariableToEventDecor("float", "PS_HLT_e24_lhmedium_nod0_L1EM18VH");
      addVariableToEventDecor("float", "PS_HLT_mu24");
      addVariableToEventDecor("char", "ejets_2015_Loose");
      addVariableToEventDecor("char", "ejets_2016_Loose");
      addVariableToEventDecor("char", "mujets_2015_Loose");
      addVariableToEventDecor("char", "mujets_2016_Loose");
    }

    //Variables needed when running DilepFakes
    if(TTHbb::util::iequals((*config)("MCFakes.TurnOn"),"true")){
      addVariableToObjectDecor("electron", "int", "el_true_type");
      addVariableToObjectDecor("electron", "int", "el_true_origin");
      addVariableToObjectDecor("electron", "int", "el_true_originbkg");
      addVariableToObjectDecor("muon", "int", "mu_true_type");
      addVariableToObjectDecor("muon", "int", "mu_true_origin");
    }
    
    if(TTHbb::util::iequals((*config)("OfflineTtbarCorrections.TurnOn"),"true")){
      addVariableToEventDecor("int", "HF_Classification");
      addVariableToEventDecor("int", "HF_SimpleClassification");
      addVariableToEventDecor("float", "truth_top_pt");
      addVariableToEventDecor("float", "truth_ttbar_pt");
      addVariableToEventDecor("float", "HFClassification_q1_pt");
      addVariableToEventDecor("float", "HFClassification_qq_dr");
      addVariableToEventDecor("float", "HFClassification_qq_pt");
    }

    /// Add the provided list of variables to the event decorations, if file is provided
    std::string varFile = (*config)("VariableListToDecorate");
    if(varFile.size() != 0){
      setEventDecorationsFromFile(varFile);
    }

    /// Add btagging information
    std::string btagWpConfig = (*config)("BTaggingWP");
    if(btagWpConfig.find("MV2c10_60")!=std::string::npos) addVariableToObjectDecor("jet", "char", "jet_isbtagged_MV2c10_60", "isbtagged_MV2c10_60");
    if(btagWpConfig.find("MV2c10_70")!=std::string::npos) addVariableToObjectDecor("jet", "char", "jet_isbtagged_MV2c10_70", "isbtagged_MV2c10_70");
    if(btagWpConfig.find("MV2c10_77")!=std::string::npos) addVariableToObjectDecor("jet", "char", "jet_isbtagged_MV2c10_77", "isbtagged_MV2c10_77");
    if(btagWpConfig.find("MV2c10_85")!=std::string::npos) addVariableToObjectDecor("jet", "char", "jet_isbtagged_MV2c10_85", "isbtagged_MV2c10_85");
    if(btagWpConfig.find("PC")!=std::string::npos) addVariableToObjectDecor("jet", "int",  "jet_tagWeightBin", "tagWeightBin");
    if(btagWpConfig.find("DL1")!=std::string::npos) addVariableToObjectDecor("jet", "float", "jet_DL1", "DL1");
    
    /// Add jet truthflavour and jet truthmatch if running on MC
    bool useTTHbbVariables = TTHbb::util::iequals((*config)("UseTTHbbVariables"),"true");
    if(!isData){
      addVariableToObjectDecor("jet", "int", "jet_truthflav");
      if(useTTHbbVariables){
        addVariableToObjectDecor("jet", "int", "jet_truthmatch");
      }
    }

    /// Add top reconstruction information for requested reco bdt's
    if(TTHbb::util::iequals((*config)("CPVariables.TurnOn"),"true")){
      recobdts = TTHbb::util::vectoriseString((*config)("CPVariables.RecoBDTs"));
      reco_bdt_index.resize(recobdts.size());
      bool isdilepton = TTHbb::util::iequals((*config)("CPVariables.channel"),"dilepton");
      bool isljets = TTHbb::util::iequals((*config)("CPVariables.channel"),"ljets");
      for(auto recobdt : recobdts){
	if(isdilepton){
	  addVariableToEventDecor("float", recobdt + "_best_m_top_pX");
	  addVariableToEventDecor("float", recobdt + "_best_m_top_pY");
	  addVariableToEventDecor("float", recobdt + "_best_m_top_pZ");
	  addVariableToEventDecor("float", recobdt + "_best_m_top_E");
	  addVariableToEventDecor("float", recobdt + "_best_m_tbar_pX");
	  addVariableToEventDecor("float", recobdt + "_best_m_tbar_pY");
	  addVariableToEventDecor("float", recobdt + "_best_m_tbar_pZ");
	  addVariableToEventDecor("float", recobdt + "_best_m_tbar_E");
	}
	if(isljets){
	  addVariableToEventDecor("float", recobdt + "_best_nu_pt");
	  addVariableToEventDecor("float", recobdt + "_best_nu_eta");
	  addVariableToEventDecor("float", recobdt + "_best_nu_phi");
	}
      }
    }


  }

  EventHandler::~EventHandler(){
    /// Delete the event and clear all the maps
    if(currentEvent != 0){
      currentEvent.reset();
    }
    
    /// Clear all the object stores
    jetIntStore.clear();
    jetFloatStore.clear();
    jetCharStore.clear();

    elIntStore.clear();
    elFloatStore.clear();
    elCharStore.clear();

    muIntStore.clear();
    muFloatStore.clear();
    muCharStore.clear();
    
    ljetIntStore.clear();
    ljetFloatStore.clear();
    ljetCharStore.clear();

    for(auto& store : customIntStore) store.second.clear();
    for(auto& store : customFloatStore) store.second.clear();
    for(auto& store : customCharStore) store.second.clear();
    
    /// Clear weights maps
    nomWeights.clear();
    nomVecWeights.clear();

    systWeights.clear();
    systVecWeights.clear();

    intStore.clear();
    floatStore.clear();
    charStore.clear();
  }

  std::shared_ptr<TTHbb::Event> EventHandler::buildEvent(){
    /// If the event hasn't been created yet, create and link metadata
    if(currentEvent == 0){
      currentEvent = std::make_shared<TTHbb::Event>();
      currentEvent->m_info = this->eventData;
      this->eventData->isData = isData;
    }
    /// Otherwise clear the event without deleting anything
    else{ 
      currentEvent->m_jets.clear();
      currentEvent->m_leptons.clear();
      currentEvent->m_largeJets.clear();
      currentEvent->m_neutrinos.clear();
      for(auto& collection : currentEvent->m_customObj) collection.second.clear();
      resetEventDecorations();
    }

    /// Decorate the event with all the defined decorations
    for(auto& iDecor : intStore){
      currentEvent->intVariable(iDecor.first) = iDecor.second.first;
    }
    for(auto& fDecor : floatStore){
      currentEvent->floatVariable(fDecor.first) = fDecor.second.first;
    }
    for(auto& cDecor : charStore){
      currentEvent->charVariable(cDecor.first) = cDecor.second.first;
    }

    /// Build jets with all decorations
    for(unsigned int i = 0; i < vIn_jet_pt->size(); ++i){
      std::shared_ptr<TTHbb::Jet> Jet = currentEvent->addJet(vIn_jet_pt->at(i),
							     vIn_jet_eta->at(i),
							     vIn_jet_phi->at(i),
							     vIn_jet_e->at(i),
							     vIn_jet_mv2->at(i),
							     vIn_jet_jvt->at(i));
	 
      /// Get decorations and check there is a value for every jet. If not, set all to -999
      for(auto & jetIntDecor : jetIntStore){
	if(jetIntDecor.second.first == 0){
	  Jet->intVariable(jetIntDecor.first) = -999;
	}
	else if(jetIntDecor.second.first->size() != vIn_jet_pt->size()){
	  Jet->intVariable(jetIntDecor.first) = -999;
	}
	else{
	  Jet->intVariable(jetIntDecor.first) = jetIntDecor.second.first->at(i);
	}
      }
      for(auto & jetFloatDecor : jetFloatStore){
	if(jetFloatDecor.second.first == 0){
	  Jet->floatVariable(jetFloatDecor.first) = -999;
	}
	else if(jetFloatDecor.second.first->size() != vIn_jet_pt->size()){
	  Jet->floatVariable(jetFloatDecor.first) = -999;
	}
	else{
	  Jet->floatVariable(jetFloatDecor.first) = jetFloatDecor.second.first->at(i);
	}
      }
      for(auto & jetCharDecor : jetCharStore){
	if(jetCharDecor.second.first == 0){
	  Jet->charVariable(jetCharDecor.first) = ' ';
	}
	else if(jetCharDecor.second.first->size() != vIn_jet_pt->size()){
	  Jet->charVariable(jetCharDecor.first) = ' ';
	}
	else{
	  Jet->charVariable(jetCharDecor.first) = jetCharDecor.second.first->at(i);
	}
      }

      /// decoreate the best candidate variable for the reco BDT's
      if(recobdts.size()){
	unsigned int j = 0;
	for(auto jetRecBDT : recobdts){
	  Jet->intVariable(jetRecBDT + "_best_Cand") = reco_bdt_index[j]->at(i);
	  j++;
	}
      }
    }

    /// Build leptons, will do electrons first and then muons, this does mean, however, that they are not sorted by pt
    for(unsigned int i = 0; i < vIn_el_pt->size(); ++i){
      std::shared_ptr<TTHbb::Lepton> El = currentEvent->addLepton(vIn_el_pt->at(i),
								  vIn_el_eta->at(i),
								  vIn_el_phi->at(i),
								  vIn_el_e->at(i));
      El->charge(vIn_el_charge->at(i));
      El->type(leptonType::electron);

      /// Get decorations and check there is a value for every electron. If not, set all to -999
      for(auto & elIntDecor : elIntStore){
	if(elIntDecor.second.first == 0){
	  El->intVariable(elIntDecor.first) = -999;
	}
	else if(elIntDecor.second.first->size() != vIn_el_pt->size()){
	  El->intVariable(elIntDecor.first) = -999;
	}
	else{
	  El->intVariable(elIntDecor.first) = elIntDecor.second.first->at(i);
	}
      }
      for(auto & elFloatDecor : elFloatStore){
	if(elFloatDecor.second.first == 0){
	  El->floatVariable(elFloatDecor.first) = -999;
	}
	else if(elFloatDecor.second.first->size() != vIn_el_pt->size()){
	  El->floatVariable(elFloatDecor.first) = -999;
	}
	else{
	  El->floatVariable(elFloatDecor.first) = elFloatDecor.second.first->at(i);
	}
      }
      for(auto & elCharDecor : elCharStore){
	if(elCharDecor.second.first == 0){
	  El->charVariable(elCharDecor.first) = ' ';
	}
	else if(elCharDecor.second.first->size() != vIn_el_pt->size()){
	  El->charVariable(elCharDecor.first) = ' ';
	}
	else{
	  El->charVariable(elCharDecor.first) = elCharDecor.second.first->at(i);
	}
      }
    }

    /// Muons
    for(unsigned int i = 0; i < vIn_mu_pt->size(); ++i){
      std::shared_ptr<TTHbb::Lepton> Mu = currentEvent->addLepton(vIn_mu_pt->at(i),
								  vIn_mu_eta->at(i),
								  vIn_mu_phi->at(i),
								  vIn_mu_e->at(i));
      Mu->charge(vIn_mu_charge->at(i));
      Mu->type(leptonType::muon);

      /// Get decorations and check there is a value for every muon. If not, set all to -999
      for(auto & muIntDecor : muIntStore){
	if(muIntDecor.second.first == 0){
	  Mu->intVariable(muIntDecor.first) = -999;
	}
	else if(muIntDecor.second.first->size() != vIn_mu_pt->size()){
	  Mu->intVariable(muIntDecor.first) = -999;
	}
	else{
	  Mu->intVariable(muIntDecor.first) = muIntDecor.second.first->at(i);
	}
      }
      for(auto & muFloatDecor : muFloatStore){
	if(muFloatDecor.second.first == 0){
	  Mu->floatVariable(muFloatDecor.first) = -999;
	}
	else if(muFloatDecor.second.first->size() != vIn_mu_pt->size()){
	  Mu->floatVariable(muFloatDecor.first) = -999;
	}
	else{
	  Mu->floatVariable(muFloatDecor.first) = muFloatDecor.second.first->at(i);
	}
      }
      for(auto & muCharDecor : muCharStore){
	if(muCharDecor.second.first == 0){
	  Mu->charVariable(muCharDecor.first) = ' ';
	}
	else if(muCharDecor.second.first->size() != vIn_mu_pt->size()){
	  Mu->charVariable(muCharDecor.first) = ' ';
	}
	else{
	  Mu->charVariable(muCharDecor.first) = muCharDecor.second.first->at(i);
	}
      }
    }
    /// Let's sort the leptons by pT
    std::sort(std::begin(currentEvent->m_leptons), std::end(currentEvent->m_leptons),
	      [&](std::shared_ptr<TTHbb::Lepton> l1, std::shared_ptr<TTHbb::Lepton> l2){ return l1->Pt() > l2->Pt(); });

    /// Build large jets with all decorations
    if(useLargeJets){
      for(unsigned int i = 0; i < vIn_ljet_pt->size(); ++i){
	std::shared_ptr<TTHbb::LargeJet> LargeJet = currentEvent->addLargeJet(vIn_ljet_pt->at(i),
									      vIn_ljet_eta->at(i),
									      vIn_ljet_phi->at(i),
									      vIn_ljet_e->at(i),
									      (int)vIn_ljet_topTag->at(i),
									      vIn_ljet_D2->at(i),
									      vIn_ljet_tau32wta->at(i));

	/// Get decorations and check there is a value for every ljet. If not, set all to -999
	for(auto & ljetIntDecor : ljetIntStore){
	  if(ljetIntDecor.second.first == 0){
	    LargeJet->intVariable(ljetIntDecor.first) = -999;
	  }
	  else if(ljetIntDecor.second.first->size() != vIn_ljet_pt->size()){
	    LargeJet->intVariable(ljetIntDecor.first) = -999;
	  }
	  else{
	    LargeJet->intVariable(ljetIntDecor.first) = ljetIntDecor.second.first->at(i);
	  }
	}
	for(auto & ljetFloatDecor : ljetFloatStore){
	  if(ljetFloatDecor.second.first == 0){
	    LargeJet->floatVariable(ljetFloatDecor.first) = -999;
	  }
	  else if(ljetFloatDecor.second.first->size() != vIn_ljet_pt->size()){
	    LargeJet->floatVariable(ljetFloatDecor.first) = -999;
	  }
	  else{
	    LargeJet->floatVariable(ljetFloatDecor.first) = ljetFloatDecor.second.first->at(i);
	  }
	}
	for(auto & ljetCharDecor : ljetCharStore){
	  if(ljetCharDecor.second.first == 0){
	    LargeJet->charVariable(ljetCharDecor.first) = ' ';
	  }
	  else if(ljetCharDecor.second.first->size() != vIn_ljet_pt->size()){
	    LargeJet->charVariable(ljetCharDecor.first) = ' ';
	  }
	  else{
	    LargeJet->charVariable(ljetCharDecor.first) = ljetCharDecor.second.first->at(i);
	  }
	}
      }
    }
    
    /// Build Custom Collections defined in the config file
    auto* config = TTHbb::GlobalConfiguration::get();
    std::vector<std::string> customObjNames = TTHbb::util::vectoriseString((*config)("CustomObjects"));
    for(auto name : customObjNames){
      /// Get the size of the collection (can't just check pt)
      unsigned int size = 0;
      if(customIntStore[name].size() > 0){
        size = customIntStore[name].begin()->second.first->size();
      }
      else if(customFloatStore[name].size() > 0){
        size = customFloatStore[name].begin()->second.first->size();
      }
      else if(customCharStore[name].size() > 0){
        size = customCharStore[name].begin()->second.first->size();
      }
      /// if no decorations for collection declared, skip this object collection
      if(size == 0){
        continue;
      }
      for(unsigned int i = 0; i < size; ++i){
	/// Create custom objects in the event and set type to custom
	std::shared_ptr<TTHbb::Particle> Obj = currentEvent->addCustomObject(name);
	//Obj->objectType(PhysObjectType::CustomObjectType);

        /// Get decorations and check there is a value for every object, If not, set all to -999
	for(auto & customIntDecor : customIntStore[name]){
	  if(customIntDecor.second.first == 0){
	    Obj->intVariable(customIntDecor.first) = -999;
	  }
	  else if(customIntDecor.second.first->size() != size){
	    Obj->intVariable(customIntDecor.first) = -999;
	  }
	  else{
	    Obj->intVariable(customIntDecor.first) = customIntDecor.second.first->at(i);
	  }
	}
	for(auto & customFloatDecor : customFloatStore[name]){
	  if(customFloatDecor.second.first == 0){
	    Obj->floatVariable(customFloatDecor.first) = -999;
	  }
	  else if(customFloatDecor.second.first->size() != size){
	    Obj->floatVariable(customFloatDecor.first) = -999;
	  }
	  else{
	    Obj->floatVariable(customFloatDecor.first) = customFloatDecor.second.first->at(i);
	  }
	}
	for(auto & customCharDecor : customCharStore[name]){
	  if(customCharDecor.second.first == 0){
	    Obj->charVariable(customCharDecor.first) = ' ';
	  }
	  else if(customCharDecor.second.first->size() != size){
	    Obj->charVariable(customCharDecor.first) = ' ';
	  }
	  else{
	    Obj->charVariable(customCharDecor.first) = customCharDecor.second.first->at(i);
	  }
	}
      }
    }
    
    /// Add the MET information to the event
    currentEvent->met_met = met_met;
    currentEvent->met_phi = met_phi;

    return currentEvent;
  }

  void EventHandler::addVariableToEventDecor(std::string type, std::string varName, std::string varToStore){

    /// Check type of variable
    bool isInt(false), isFloat(false), isChar(false);
    isInt = TTHbb::util::iequals(type, "int");
    if(!isInt){
      isFloat = TTHbb::util::iequals(type, "float");
      if(!isFloat){
	isChar = TTHbb::util::iequals(type, "char");
      }
    }

    /// Skip variable if neither int nor float
    if(!isInt && !isFloat && !isChar){
      std::cout << "WARNING in TTHbb::EventHandler::addVariableToEventHandler:\t"
		<< "Variable type must be either int, float or char (or vector of them). Variable  \""
		<< varName << "\" has given type " << type << std::endl;
      return;
    }

    /// If no name given, set to same as branch name
    if(varToStore.size() == 0){
      varToStore = varName;
    }
    
    /// Add variable to appropriate store
    if(isInt){
      /// Check whether variable already in the store
      if(intStore.count(varToStore) == 0){
	intStore.insert(std::make_pair(varToStore, std::make_pair(-999,varName)));
      }
    }
    else if(isFloat){
      /// Check whether variable already in the store
      if(floatStore.count(varToStore) == 0){
	floatStore.insert(std::make_pair(varToStore, std::make_pair(-999.0,varName)));
      }
    }
    else if(isChar){
      /// Check whether variable already in the store
      if(charStore.count(varToStore) == 0){
	charStore.insert(std::make_pair(varToStore, std::make_pair(' ',varName)));
      }
    }
    else{
      std::cout << "ERROR in TTHbb::EventHandler::addVariableToEventHandler:\t"
		<< "Somehow the int, float or char (or vector of them) requirement has failed." << std::endl;
      exit(21);
    }
  }

  void EventHandler::addVariableToObjectDecor(std::string object, std::string type, std::string varName, std::string varToStore){
    /// Check which object the variable is to be decorated onto
    bool isJet(false), isElectron(false), isMuon(false), isLJet(false);
    bool isCustom(false);
    auto* config = TTHbb::GlobalConfiguration::get();
    std::vector<std::string> customObjNames = TTHbb::util::vectoriseString((*config)("CustomObjects"));
    isJet = TTHbb::util::iequals(object, "jet");
    if(!isJet){
      isElectron = TTHbb::util::iequals(object, "electron");
      if(!isElectron){
	isMuon =  TTHbb::util::iequals(object, "muon");
	if(!isMuon){
	  isLJet = TTHbb::util::iequals(object, "ljet");
	  if(!isLJet){
	    for(auto name : customObjNames){
	      if(TTHbb::util::iequals(object, name)){
	        isCustom = true;
		break;
	      }
	    }
	  }
	}
      }
    }
    /// Check that one of the four objects is specified, otherwise the variable will be skipped
    if(!(isJet || isElectron || isMuon || isLJet || isCustom)){
      std::cout << "WARNING in TTHbb::EventHandler::addVariableToObjectDecor:\t";
      if(customObjNames.size()==0){
	std::cout << "Object can be \"jet\", \"electron\", \"muon\" or \"ljet\"";
      }
      else{
	std::cout << "Object can be \"jet\", \"electron\", \"muon\", \"ljet\"";
	for(auto name : customObjNames){
	  std::cout << ", \"" << name << "\"";
	}
      }
      std::cout << ".\n\t\t\t\tVariable  \"" << varName << "\" has given object "
		<< object << std::endl;
      return;
    }

    /// Check the variable type to be decorated
    bool isInt(false), isFloat(false), isChar(false);
    isInt = TTHbb::util::iequals(type, "int");
    if(!isInt){
      isFloat = TTHbb::util::iequals(type, "float");
      if(!isFloat){
	isChar = TTHbb::util::iequals(type, "char");
      }
    }

    /// Check that either int or float has been specified else skip
    if(!isInt && !isFloat && !isChar){
      std::cout << "WARNING in TTHbb::EventHandler::addVariableToObjectDecor:\t"
		<< "Variable type must be either int, float or char. Variable  \""
		<< varName << "\" has given type " << type << std::endl;
      return;
    }

    /// If no name given, use the name of the branch
    if(varToStore.size() == 0){
      varToStore = varName;
    }

    std::unordered_map<std::string,std::pair<std::vector<int>*,std::string>>*   intVec;
    std::unordered_map<std::string,std::pair<std::vector<float>*,std::string>>* floatVec;
    std::unordered_map<std::string,std::pair<std::vector<char>*,std::string>>* charVec;

    /// Set the int and float maps to point to the correct objects store
    if(isJet){
      intVec   = &jetIntStore;
      floatVec = &jetFloatStore;
      charVec  = &jetCharStore;
    }
    else if(isElectron){
      intVec   = &elIntStore;
      floatVec = &elFloatStore;
      charVec  = &elCharStore;
    }
    else if(isMuon){
      intVec   = &muIntStore;
      floatVec = &muFloatStore;
      charVec  = &muCharStore;
    }
    else if(isLJet){
      intVec   = &ljetIntStore;
      floatVec = &ljetFloatStore;
      charVec  = &ljetCharStore;
    }
    else if(isCustom){
      intVec   = &customIntStore[object];
      floatVec = &customFloatStore[object];
      charVec  = &customCharStore[object];
    }
    else{
      std::cout << "ERROR in TTHbb::EventHandler::addVariableToObjectDecor:\t"
		<< "Somehow the object requirement has failed." << std::endl;
      exit(22);
    }

    if(isInt){
      /// Check variable not already in store
      if(intVec->count(varToStore) == 0){
	intVec->insert(std::make_pair(varToStore, std::pair<std::vector<int>*,std::string>(0x0, varName)));
      }
    }
    else if(isFloat){
      /// Check variable not already in store
      if(floatVec->count(varToStore) == 0){
	floatVec->insert(std::make_pair(varToStore, std::pair<std::vector<float>*,std::string>(0x0, varName)));
      }
    }
    else if(isChar){
      /// Check variable not already in store
      if(charVec->count(varToStore) == 0){
	charVec->insert(std::make_pair(varToStore, std::pair<std::vector<char>*,std::string>(0x0, varName)));
      }
    }
    else{
      std::cout << "ERROR in TTHbb::EventHandler::addVariableToObjectDecor:\t"
		<< "Somehow the int or float requirement has failed." << std::endl;
      exit(21);
    }
  }

  void EventHandler::setEventDecorBranchAddresses(){

    /// Lets set status to 0 for everything and then enable what we want
    m_chain->SetBranchStatus("*",0);

    /// Set the branch addresses for all variables that will be needed to build the event
    defaultVariableGetter();

    /// Set the branch addresses for all the int variables defined for the event decorations
    for(auto & intDecor : intStore){
      m_chain->SetBranchStatus( intDecor.second.second.c_str(), 1);
      m_chain->SetBranchAddress(intDecor.second.second.c_str(), &(intDecor.second.first));
    }

    /// Set the branch addresses for all the float variables defined for the event decorations
    for(auto & floatDecor : floatStore){
      m_chain->SetBranchStatus( floatDecor.second.second.c_str(), 1);
      m_chain->SetBranchAddress(floatDecor.second.second.c_str(), &(floatDecor.second.first));
    }
    for(auto & charDecor : charStore){
      m_chain->SetBranchStatus( charDecor.second.second.c_str(), 1);
      m_chain->SetBranchAddress(charDecor.second.second.c_str(), &(charDecor.second.first));
    }

    /// Set the branch addresses for all the ints and floats to be decorated onto each object type
    /// These are separately handled due to being vectors of ints and floats 
    objectDecorGetter(jetIntStore,    jetFloatStore,   jetCharStore);
    objectDecorGetter(elIntStore,     elFloatStore,    elCharStore);
    objectDecorGetter(muIntStore,     muFloatStore,    muCharStore);
    if(useLargeJets){
      objectDecorGetter(ljetIntStore,   ljetFloatStore,   ljetCharStore);
    }
    /// Set the branch addresses for the custom objects
    auto* config = TTHbb::GlobalConfiguration::get();
    std::vector<std::string> customObjNames = TTHbb::util::vectoriseString((*config)("CustomObjects"));
    for(auto name : customObjNames){
      objectDecorGetter(customIntStore[name],   customFloatStore[name],   customCharStore[name]);
    }

    /// Get the branches for all the weights if running on MC
    if(!isData){
      weightBranchGetter();
    }
  }

  void EventHandler::setWeightBranches(std::string fileName){
    std::string line;
    std::ifstream infile;

    std::unordered_map<std::string, float>* floatWeight = &nomWeights;
    std::unordered_map<std::string, std::vector<float>*>* vecWeight = &nomVecWeights;

    std::vector<std::string> vecWeightNames;

    auto* config = TTHbb::GlobalConfiguration::get();

    /// Open file with list of weights
    infile.open(fileName);

    /// Loop through all lines in the file
    while(std::getline(infile,line)){
      /// # will mark the beginning of a comment
      size_t hash = line.find("#");
      if (hash != std::string::npos){
  	line = line.substr(0, hash);
      }
      
      if(line.empty()) continue;

      /// remove whitespace from the beginning and end of line
      const auto strBgn = line.find_first_not_of(" \t");
      const auto strEnd = line.find_last_not_of(" \t");
      line = line.substr(strBgn, strEnd+1);
      
      /// check line isn't empty!
      if(line.empty()) continue;
      
      /// File is split into nominal weights and systematic weights by a line containing only SYSTEMATICS
      /// Nominal weights are to be saved for all trees
      /// Systematic weights are only stored in the nominal tree _if_ specified to be saved
      /// When the line SYSTEMATICS is found, either change the vectors for the syst weights or end the loop
      if(line == "SYSTEMATICS"){
	if(isNominalTree && TTHbb::util::iequals((*config)("SaveSystematicWeights"),"true")){
	  floatWeight = &systWeights;
	  vecWeight = &systVecWeights;
	  continue;
	}
      	else{
	  break;
	}
      }
      
      std::stringstream iss(line);
      std::string p1,p2;
      iss >> p1 >> p2;
      
      /// If it is a vector initialise the pointer to zero
      if(util::iequals(p1,"vec")){
	(*vecWeight)[p2] = 0;
      }
      else{
  	floatWeight->insert(std::make_pair(p1, 1.0));
      }
    }
    infile.close();
  }

  void EventHandler::setEventDecorationsFromFile(std::string fileName){
    std::string line;
    std::ifstream infile;

    /// Open file with list of weights
    infile.open(fileName);

    /// Loop through all lines in the file
    while(std::getline(infile,line)){
      /// # will mark the beginning of a comment
      size_t hash = line.find("#");
      if (hash != std::string::npos){
  	line = line.substr(0, hash);
      }
      
      if(line.empty()) continue;

      /// remove whitespace from the beginning and end of line
      const auto strBgn = line.find_first_not_of(" \t");
      const auto strEnd = line.find_last_not_of(" \t");
      line = line.substr(strBgn, strEnd+1);
      
      /// check line isn't empty!
      if(line.empty()) continue;
      
      std::stringstream iss(line);
      std::string p1,p2,p3;
      iss >> p1 >> p2 >> p3;
      if(!p3.size()){
	addVariableToEventDecor(p1, p2);
      }
      else{
	addVariableToObjectDecor(p1, p2, p3);
      }
    }
    infile.close();
  }

  void EventHandler::setSystematic(std::string syst){
    m_currentSystematic = syst;
    this->eventData->currentSyst = m_currentSystematic;
    /// Check whether the tree name is nominal to se the bool
    isNominalTree = (TTHbb::util::iequals(syst, "nominal")) || (TTHbb::util::iequals(syst, "nominal_Loose"));
    /// Make the chain for the systematic
    m_chain = std::make_shared<TChain>(syst.c_str());
  }

  void EventHandler::addFile(std::string file){
    if(m_chain){
      m_chain->AddFile(file.c_str());
    }
    else{
      std::cout << "ERROR in TTHbb::EventHandler:\tHave not set the systematic before adding input files" 
		<< std::string(29,' ') << "\tNeeds to be set first to know the tree to read!" << std::endl;
      exit(23);
    }
  }

  void EventHandler::addFiles(std::vector<std::string> files){
    std::cout << "INFO: Adding " << files.size() << " files" << std::endl;
    for(auto file : files){
      this->addFile(file);
    }
  }
  
  void EventHandler::addNormalisationWeights(std::map<int,float> weights){
    normalisationWeights = weights;
  }

  void EventHandler::addMCNormalisationWeights(std::map<int, std::map<std::string, float> > weights){
    mcNormalisationWeights = weights;
  }

  void EventHandler::GetEntry(unsigned int entry){
    m_currentEntry = entry;
    
    /// Check whether the chain has been created then get entry
    if(m_chain){
      m_chain->GetEntry(entry);
    }
    else{
      std::cout << "ERROR in TTHbb::EventHandler:\tTrying to get an event before systematic or files added!" << std::endl;
      exit(23); 
    }

    /// If normalisation weights have been provided, get the current events
    if(normalisationWeights.size()){
      this->eventData->eventNormWeight = normalisationWeights.at(this->eventData->mcChannelNumber);
    }
    if(mcNormalisationWeights.size()){
      this->eventData->eventMCNormName->clear();
      this->eventData->eventMCNormWeight->clear();
      for( auto& name_weight : mcNormalisationWeights.at(this->eventData->mcChannelNumber) ){
	this->eventData->eventMCNormName->push_back(name_weight.first);
	this->eventData->eventMCNormWeight->push_back(name_weight.second);
      }
    }

  }

  void EventHandler::defineRegions(){
    
    auto* config = TTHbb::GlobalConfiguration::get();
    
    std::string regionString((*config)("Regions"));
    
    /// If no regions are given, simply return
    if(regionString.size() == 0){
      return;
    }

    /// The colon splits the region name and definition
    /// Finding the next colon will give us the end of the name and the start of definition
    while(regionString.find(":") != std::string::npos){
      
      std::string name("");
      std::string cut("");
    
      /// Get start of definition by looking for quotation marks after the colon
      /// By containing within quotation marks, we can use all the powers of TCut
      size_t pos = regionString.find(":");
      size_t cutBegin = regionString.find('"',pos);
      size_t cutEnd = regionString.find('"',cutBegin+1);
      
      /// Name is everything before the colon
      name = regionString.substr(0,pos);
      /// Cut is everything between the quotation marks
      cut = regionString.substr(cutBegin+1, (cutEnd-cutBegin) - 1);

      /// See whether a flag has been included in the regions
      /// FLAG should be contained inside {...}
      std::string flag = "";
      if(cut.find("{")!=std::string::npos){
	if(cut.find("}")==std::string::npos){
	  std::cout << "ERROR in TTHbb::EventHandler: { specified in cut expression but no corresponding }." << std::endl;
	}
	std::cout << "INFO in TTHbb::EventHandler: Flag found in region " << name << ": ";
	flag = cut.substr(cut.find("{")+1, cut.find("}")-cut.find("{") - 1);
	std::cout << flag << " ; ";
	cut.erase(cut.find("{"+flag+"}"),flag.length()+2);
	std::cout << "Remaining cut expression looks like this: " << cut << std::endl;
      }
      
      /// Add the region to the map
      region reg;
      reg.cut = "(" + cut + ")";
      reg.flag = flag;
      regionMap.insert(std::make_pair(name,reg));
      
      /// If there is another comma, remove the string up to the comma+1
      /// This is the end of the current definition
      if(regionString.find(',') != std::string::npos){
	regionString = regionString.substr(regionString.find(',',cutEnd)+1);
      }
      /// Otherwise, go to the end and this will end the while loop
      else{
	regionString = regionString.substr(cutEnd);
      }
    }
  }
  
  void EventHandler::calculateRegions(){
    /// Print out the process for alculating the regions
    /// For a large number of input files, this can take quite a large amount of time
    int i = 0;
    int width = 10;
    int m = regionMap.size();
    for(auto& reg : regionMap){
      std::cout << "INFO: Calculating regions [";
      for(int j = 0; j < width; ++j){
	if((float)j/(float)width < (float)i/(float)m){
	  std::cout << "#";
	}
	else if(j/m >= i/m){
	  std::cout << " ";
	}
      }
      std::cout << "] " << i << "/" << m << "\r";
      std::cout << std::flush;

      /// Make use of the power of TTree::Draw to get EventLists we can use to check by entry number
      m_chain->SetEstimate(m_chain->GetEntries(reg.second.cut.c_str()));
      m_chain->Draw((">>temp_calcof_"+reg.first+m_currentSystematic).c_str(),reg.second.cut.c_str(),"goff");
      /// Get the Event list from roots objects and save it to the region
      reg.second.eventList.reset((TEventList*)gDirectory->Get(("temp_calcof_"+reg.first+m_currentSystematic).c_str()));
      ++i;
    }
    std::cout << "INFO: Calculating regions [" << std::string(width, '#') << "] " << m << "/" << m << std::endl;
  }

  bool EventHandler::eventInAnyRegion(){
    /// Check whether any regions have been defined, if not return true
    if(!regionMap.size()){
      return true;
    }
    else{
      /// Loop through all regions, and check if event is in region's event list
      for(auto& reg : regionMap){
	if(reg.second.eventList->Contains(m_currentEntry)){
	  return true;
	}
      }
      return false;
    }
  }

  bool EventHandler::skipEventCheck(){
    /// Check if it is in any of the regions
    if(!eventInAnyRegion()){
      return true;
    }
    /// Check if bfilter is being applied and whether it is to be skipped
    else if(doBfilter && isEventSkippedForTTbarBfilter()){
      return true;
    }
    /// Check if dil filter is being applied and whether it is to be skipped
    else if(doDilFilter && isEventSkippedForTTbarDilepton()){
      return true;
    }
    else{
      return false;
    }
  }

  bool EventHandler::eventPassSelPostProc(std::string flag){
    if(flag=="") return true;
    /// check event decoration corresponding to speficied FLAG
    if(currentEvent->checkCharVariable(flag)) return currentEvent->charVariable(flag);
    if(currentEvent->checkIntVariable(flag)) return currentEvent->intVariable(flag)>0;
    if(currentEvent->checkFloatVariable(flag)) return currentEvent->floatVariable(flag)>0.;
    return false;
  }
  
  bool EventHandler::eventInAnyRegionPostProc(){
    /// Check whether any regions have been defined, if not return true
    if(!regionMap.size()){
      return true;
    }
    else{
      /// Loop through all regions, and check if event fullfills region's requirements
      for(auto& reg : regionMap){
	if(eventPassSelPostProc(reg.second.flag)){
	  return true;
	}
      }
      return false;
    }
  }

  bool EventHandler::skipEventCheckPostProc(){
    /// Check if it is in any of the post-process regions
    if(!eventInAnyRegionPostProc()){
      return true;
    }
    else{
      return false;
    }
  }
  
  void EventHandler::createOutputs(){
    auto* config = TTHbb::GlobalConfiguration::get();
    bool createOutFileFirst = TTHbb::util::iequals((*config)("CreateOutputFileFirst"),"true");
    if(createOutFileFirst) createOutputFiles();
    if(regionMap.size() != 0){
      for(auto& reg : regionMap){
        if(createOutFileFirst) m_files[reg.first]->cd();
	createSingleOutput(reg.second.tree);
      }
    }
    else{
      if(createOutFileFirst) m_files["output"]->cd();
      createSingleOutput(outputTree);
    }
  }

  void EventHandler::createSingleOutput(std::shared_ptr<TTree>& tree){

    /// Open configuration to check what is to be saved
    auto* config = TTHbb::GlobalConfiguration::get();

    /// Create tree with systematic name
    tree = std::make_shared<TTree>(m_currentSystematic.c_str(), m_currentSystematic.c_str());

    /// See if all variables decorated to event are to be dumped
    if(TTHbb::util::iequals((*config)("SaveAllDecorations"), "true")){
      /// Loop over all floats in the event decorations and add them to the tree
      for(auto floatVar : currentEvent->variableFloatList()){
        tree->Branch(floatVar.first.c_str(), &(currentEvent->floatVariable(floatVar.first)), (floatVar.first + "/F").c_str());
      }
      /// Loop over all ints in the event decorations and add them to the tree
      for(auto intVar : currentEvent->variableIntList()){
        tree->Branch(intVar.first.c_str(), &(currentEvent->intVariable(intVar.first)), (intVar.first + "/I").c_str());
      }
      /// Loop over all chars in the event decorations and add them to the tree
      for(auto charVar : currentEvent->variableCharList()){
        tree->Branch(charVar.first.c_str(), &(currentEvent->charVariable(charVar.first)), (charVar.first + "/B").c_str());
      }
    }
    /// Otherwise, open file with list of variables to store
    else{
      /// Get file name from config file
      std::string varsFile = (*config)("VariablesToSaveFile");
      /// Read file and return vector of strings
      std::vector<std::string> varsToSave = TTHbb::util::fileToVector(varsFile);
      /// Loop over variable names and see if variable exists in int or float
      for(auto & var : varsToSave){
	bool isInt   = currentEvent->checkIntVariable(var);
	bool isFloat = currentEvent->checkFloatVariable(var);
	bool isChar  = currentEvent->checkCharVariable(var);
	/// Double check the name isn't in both int and floats
	/// Safeguard by adding twice with different name
	if(((int)isInt + (int)isFloat + (int)isChar) > 1){
	  if(isInt){
	    tree->Branch((var + "_int").c_str(),   &(currentEvent->intVariable(var)),   (var + "_int/I").c_str());
	  }
	  if(isFloat){
	    tree->Branch((var + "_float").c_str(), &(currentEvent->floatVariable(var)), (var + "_float/F").c_str());
	  }
	  if(isChar){
	    tree->Branch((var + "_char").c_str(), &(currentEvent->charVariable(var)), (var + "_char/B").c_str());
	  }
	}
	else if(isInt){
	  tree->Branch(var.c_str(),   &(currentEvent->intVariable(var)),   (var + "/I").c_str());
	}
	else if(isFloat){
	  tree->Branch(var.c_str(),   &(currentEvent->floatVariable(var)), (var + "/F").c_str());
	}
	else if(isChar){
	  tree->Branch(var.c_str(),   &(currentEvent->charVariable(var)), (var + "/B").c_str());
	}
	else{
	  std::cout << "WARNING in TTHbb::EventHandler:\tVariable called \"" << var << "\" requested to be saved not dressed to event."
		    << " Skipping variable." << std::endl;
	}
      }
    }

    /// See whether the object vectors, such as jet_pt, should be saved
    std::string saveVars = (*config)("SaveDefaultVariables");
    if(TTHbb::util::iequals(saveVars, "true")){
      saveDefaultVariables(tree);
    }
    else if(!(TTHbb::util::iequals(saveVars, "false")) && saveVars.size() > 0){
      saveSomeDefaultVariables(saveVars, tree);
    }
    
    /// Eventually save all the other object decorations
    if(TTHbb::util::iequals((*config)("SaveAllObjectDecorations"), "true")){
      saveAllObjectDecorations(tree);
    }

    /// Now to add the weights to the output
    /// Start with all nominal weights for all trees
    for(auto& weight : nomWeights){
      tree->Branch(weight.first.c_str(),  &(weight.second), (weight.first + "/F").c_str() );
    }
    for(auto& weight : nomVecWeights){
      tree->Branch(weight.first.c_str(),  &(weight.second));
    }
    /// Now check if this is a nominal tree to save systematic weights
    if(isNominalTree){
      for(auto& weight : systWeights){
	tree->Branch(weight.first.c_str(),  &(weight.second), (weight.first + "/F").c_str() );
      }
      for(auto& weight : systVecWeights){
	tree->Branch(weight.first.c_str(),  &(weight.second));
      }
    }

    /// Finally, add the normalisation weight
    if(normalisationWeights.size()){
      tree->Branch("weight_normalise", &(this->eventData->eventNormWeight), "weight_normalise/F");
    }  
    if(mcNormalisationWeights.size()){
      tree->Branch("weight_mcweight_name", &(this->eventData->eventMCNormName));
      tree->Branch("weight_mcweight_normalise", &(this->eventData->eventMCNormWeight));
    }
    
  }

  void EventHandler::fillOutputs(){
    /// Fill vectors for object decorations from tools
    auto* config = TTHbb::GlobalConfiguration::get();
    if(TTHbb::util::iequals((*config)("SaveAllObjectDecorations"), "true")){
      fillAllObjectVectorsFromDecorations();
    }
    /// If no regions are defined, fill the simple outputTree
    if(outputTree){
      outputTree->Fill();
    }
    /// Fill the region's tree if the event is in the region
    for(auto& reg : regionMap){
      // if(reg.second.eventList->Contains(m_currentEntry)){
      if(reg.second.eventList->Contains(m_currentEntry) && eventPassSelPostProc(reg.second.flag)){
	reg.second.tree->Fill();
      }
    }
  }

  void EventHandler::createOutputFiles(){
    /// Create TFiles in the directory format
    auto* config = TTHbb::GlobalConfiguration::get();
    std::string topDir = (*config)("OutputDirectory");
    std::string fileName = (*config)("SampleName");
    gSystem->mkdir( topDir.c_str() );
    for(auto& reg : regionMap){
      std::string currDir = topDir + "/" + reg.first;
      gSystem->mkdir( currDir.c_str() , true);
      m_files[reg.first] = std::make_shared<TFile>((currDir + "/" + fileName + ".root").c_str() , "UPDATE");
    }
    /// If using outputTree, ie no regions, just save into the sample name in top director
    if(regionMap.size()==0){
      gSystem->mkdir( topDir.c_str() , true);
      m_files["output"] = std::make_shared<TFile>((topDir + "/" + fileName + ".root").c_str(), "UPDATE");
    }
  }

  void EventHandler::saveOutputs(){
    /// Save the outputs in the directory format
    auto* config = TTHbb::GlobalConfiguration::get();
    bool createOutFileFirst = TTHbb::util::iequals((*config)("CreateOutputFileFirst"),"true");
    std::string topDir = (*config)("OutputDirectory");
    std::string fileName = (*config)("SampleName");
    if(!createOutFileFirst) gSystem->mkdir( topDir.c_str() );

    TFile* file = 0;

    /// Make a new directory for each region
    /// Then save the systematic tree into the file of the sample name
    /// Update the file, so that it keeps old data
    /// Overwrite current systematic tree
    for(auto& reg : regionMap){
      TDirectory *dir = gDirectory;
      if(!createOutFileFirst){
        std::string currDir = topDir + "/" + reg.first;
        gSystem->mkdir( currDir.c_str() , true);
        file = TFile::Open( (currDir + "/" + fileName + ".root").c_str() , "UPDATE");
        file->cd();
      }
      else{
        m_files[reg.first]->cd();
      }
      if(!reg.second.tree){
	std::cout << "WARNING: No events passed the requirements for region \"" << reg.first << "\" in the Systematic \"" << m_currentSystematic << "\". Making empty tree." << std::endl;
	reg.second.tree = std::make_shared<TTree>(m_currentSystematic.c_str(), m_currentSystematic.c_str());
      }
      /// Check whether a temporary file has been created, if so clone tree and save the clone
      if(TTHbb::util::iequals((*config)("CreateTempOutputFile"),"true")){
	TTree* newtree = reg.second.tree->CloneTree();
	newtree->Write( m_currentSystematic.c_str(), TObject::kOverwrite );
	delete newtree;
      }
      else{
	reg.second.tree->Write( m_currentSystematic.c_str(), TObject::kOverwrite );
      }
      /// Save and close the file, make sure memory is cleared
      if(!createOutFileFirst){
        file->Save();
        file->Close();
        file = 0;
      }
      else{
        m_files[reg.first]->Save();
        dir->cd();
        m_files[reg.first]->Close();
        m_files[reg.first] = 0;
      }
      reg.second.eventList.reset();
      reg.second.tree.reset();
    }
    
    /// If using outputTree, ie no regions, just save into the sample name in top director
    if(outputTree != 0){
      TDirectory *dir = gDirectory;
      if(!createOutFileFirst){
        gSystem->mkdir( topDir.c_str() , true);
        file = TFile::Open( (topDir + "/" + fileName + ".root").c_str(), "UPDATE");
        file->cd();
      }
      else{
        m_files["output"]->cd();
      }
      /// Check whether a temporary fie has been created, if so clone tree and save the clone
      if(TTHbb::util::iequals((*config)("CreateTempOutputFile"),"true")){
	TTree* newtree = outputTree->CloneTree();
	newtree->Write( m_currentSystematic.c_str(), TObject::kOverwrite );
	delete newtree;
      }
      else{
	outputTree->Write( m_currentSystematic.c_str(), TObject::kOverwrite );
      }
      /// Save and close file and clear memory
      if(!createOutFileFirst){
        file->Save();
        file->Close();
        file = 0;
      }
      else{
        m_files["output"]->Save();
        dir->cd();
        m_files["output"]->Close();
        m_files["output"] = 0;
      }
      outputTree.reset();
    }
  }


  /**********************************/
  /*****Private Member Functions*****/
  /**********************************/

  void EventHandler::objectDecorGetter(std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>>& intVec, std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>>& floatVec, std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>>& charVec){

    /// Set the addresses of the int variables
    for(auto & iDecor : intVec){
      m_chain->SetBranchStatus( iDecor.second.second.c_str(), 1);
      m_chain->SetBranchAddress(iDecor.second.second.c_str(), &(iDecor.second.first));
    }

    /// Set the addresses of the float variables
    for(auto & fDecor : floatVec){
      m_chain->SetBranchStatus( fDecor.second.second.c_str(), 1);
      m_chain->SetBranchAddress(fDecor.second.second.c_str(), &(fDecor.second.first));
    }

    /// Set the addresses of the char variables
    for(auto & cDecor : charVec){
      m_chain->SetBranchStatus( cDecor.second.second.c_str(), 1);
      m_chain->SetBranchAddress(cDecor.second.second.c_str(), &(cDecor.second.first));
    }
  }

  void EventHandler::weightBranchGetter(){
    /// Check that the weights have been loaded properly and print a warning if not
    if(nomWeights.size() == 0 && nomVecWeights.size() == 0){
      std::cout << "WARNING in TTHbb::EventHandler:\tNo nominal weights provided" << std::endl;
    }
    else{
      /// Loop through all the weights at set the branches
      for(auto & weight : nomWeights){
	m_chain->SetBranchStatus( weight.first.c_str(), 1);
	m_chain->SetBranchAddress(weight.first.c_str(), &(weight.second));
      }
      /// Loop through all the vector weights and set the branches
      for(auto & weight : nomVecWeights){
	m_chain->SetBranchStatus( weight.first.c_str(), 1);
	m_chain->SetBranchAddress(weight.first.c_str(), &(weight.second));
      }
    }

    /// Check if it is the nominal tree, systematics weights are only saved in the nominal
    if(isNominalTree){
      /// Check that the weights have been loaded properly and print a warning if not
      if(systWeights.size() == 0 && systVecWeights.size() == 0){
	std::cout << "WARNING in TTHbb::EventHandler:\tNo systematic weights provided" << std::endl;
      }
      else{
	/// Loop through all weights and set addresses
	for(auto & weight : systWeights){
	  m_chain->SetBranchStatus( weight.first.c_str(), 1);
	  m_chain->SetBranchAddress(weight.first.c_str(), &(weight.second));
	}
	/// Loop through all vector weights as set addresses
	for(auto & weight : systVecWeights){
	  m_chain->SetBranchStatus( weight.first.c_str(), 1);
	  m_chain->SetBranchAddress(weight.first.c_str(), &(weight.second));
	}
      }
    }
  }

  void EventHandler::defaultVariableGetter(){
    /// List of all the default variables we will always need and use in every event

    /// Event meta data
    m_chain->SetBranchStatus( "eventNumber",     1);
    m_chain->SetBranchAddress("eventNumber",     &(this->eventData->eventNumber));
    m_chain->SetBranchStatus( "mcChannelNumber", 1);
    m_chain->SetBranchAddress("mcChannelNumber", &(this->eventData->mcChannelNumber));
    m_chain->SetBranchStatus( "runNumber",       1);
    m_chain->SetBranchAddress("runNumber",       &(this->eventData->runNumber));

    /// Jet branches
    m_chain->SetBranchStatus( "jet_pt",          1);
    m_chain->SetBranchAddress("jet_pt",          &vIn_jet_pt);
    m_chain->SetBranchStatus( "jet_eta",         1);
    m_chain->SetBranchAddress("jet_eta",         &vIn_jet_eta);
    m_chain->SetBranchStatus( "jet_phi",         1);
    m_chain->SetBranchAddress("jet_phi",         &vIn_jet_phi);
    m_chain->SetBranchStatus( "jet_e",           1);
    m_chain->SetBranchAddress("jet_e",           &vIn_jet_e);
    m_chain->SetBranchStatus( "jet_mv2c10",      1);
    m_chain->SetBranchAddress("jet_mv2c10",      &vIn_jet_mv2);
    m_chain->SetBranchStatus( "jet_jvt",         1);
    m_chain->SetBranchAddress("jet_jvt",         &vIn_jet_jvt);
    
    /// Large jet branches
    if(useLargeJets){
      m_chain->SetBranchStatus( "ljet_pt",         1);
      m_chain->SetBranchAddress("ljet_pt",         &vIn_ljet_pt);
      m_chain->SetBranchStatus( "ljet_eta",        1);
      m_chain->SetBranchAddress("ljet_eta",        &vIn_ljet_eta);
      m_chain->SetBranchStatus( "ljet_phi",        1);
      m_chain->SetBranchAddress("ljet_phi",        &vIn_ljet_phi);
      m_chain->SetBranchStatus( "ljet_e",          1);
      m_chain->SetBranchAddress("ljet_e",          &vIn_ljet_e);
      m_chain->SetBranchStatus( "ljet_D2",         1);
      m_chain->SetBranchAddress("ljet_D2",         &vIn_ljet_D2);
      m_chain->SetBranchStatus( "ljet_tau32_wta",  1);
      m_chain->SetBranchAddress("ljet_tau32_wta",  &vIn_ljet_tau32wta);
      m_chain->SetBranchStatus( "ljet_topTag",     1);
      m_chain->SetBranchAddress("ljet_topTag",     &vIn_ljet_topTag);
    }

    /// Electron branches
    m_chain->SetBranchStatus( "el_pt",           1);
    m_chain->SetBranchAddress("el_pt",           &vIn_el_pt);
    m_chain->SetBranchStatus( "el_eta",          1);
    m_chain->SetBranchAddress("el_eta",          &vIn_el_eta);
    m_chain->SetBranchStatus( "el_phi",          1);
    m_chain->SetBranchAddress("el_phi",          &vIn_el_phi);
    m_chain->SetBranchStatus( "el_e",            1);
    m_chain->SetBranchAddress("el_e",            &vIn_el_e);
    m_chain->SetBranchStatus( "el_charge",       1);
    m_chain->SetBranchAddress("el_charge",       &vIn_el_charge);

    /// Muon branches
    m_chain->SetBranchStatus( "mu_pt",           1);
    m_chain->SetBranchAddress("mu_pt",           &vIn_mu_pt);
    m_chain->SetBranchStatus( "mu_eta",          1);
    m_chain->SetBranchAddress("mu_eta",          &vIn_mu_eta);
    m_chain->SetBranchStatus( "mu_phi",          1);
    m_chain->SetBranchAddress("mu_phi",          &vIn_mu_phi);
    m_chain->SetBranchStatus( "mu_e",            1);
    m_chain->SetBranchAddress("mu_e",            &vIn_mu_e);
    m_chain->SetBranchStatus( "mu_charge",       1);
    m_chain->SetBranchAddress("mu_charge",       &vIn_mu_charge);

    /// MET branches
    m_chain->SetBranchStatus( "met_met",         1);
    m_chain->SetBranchAddress("met_met",         &met_met);
    m_chain->SetBranchStatus( "met_phi",         1);
    m_chain->SetBranchAddress("met_phi",         &met_phi);

    /// Jet_best_Cand branches
    if(recobdts.size()){
      unsigned i = 0;
      for(auto recobdt :recobdts){
	m_chain->SetBranchStatus(("jet_" + recobdt + "_best_Cand").c_str(),  1);
	m_chain->SetBranchAddress(("jet_" + recobdt + "_best_Cand").c_str(), &(reco_bdt_index.at(i)));
	++i;
      }
    }
  }

 
  void EventHandler::saveAllObjectDecorations(std::shared_ptr<TTree>& tree){
    /// Add object decorations from tools to the Stores
    /// Jets
    if(currentEvent->m_jets.size()>0){
      for (auto v : currentEvent->m_jets.at(0)->variableIntList()){
        if (v.first.find("tagWeightBin")!=std::string::npos) continue;
        if (jetIntStore.count(v.first)==0){
          jetIntStore.insert(std::make_pair(v.first, std::pair<std::vector<int>*,std::string>(new std::vector<int>, v.first)));
        }
      }
      for (auto v : currentEvent->m_jets.at(0)->variableFloatList()){
        if (v.first.find("mv2")!=std::string::npos || v.first.find("jvt")!=std::string::npos) continue;
        if (jetFloatStore.count(v.first)==0){
          jetFloatStore.insert(std::make_pair(v.first, std::pair<std::vector<float>*,std::string>(new std::vector<float>, v.first)));
        }
      }
      for (auto v : currentEvent->m_jets.at(0)->variableCharList()){
        if (v.first.find("isbtagged")!=std::string::npos) continue;
        if (jetCharStore.count(v.first)==0){
          jetCharStore.insert(std::make_pair(v.first, std::pair<std::vector<char>*,std::string>(new std::vector<char>, v.first)));
        }
      }
    }
    /// LJets
    if(useLargeJets){
      if(currentEvent->m_largeJets.size()>0){
        for (auto v : currentEvent->m_largeJets.at(0)->variableIntList()){
          if (ljetIntStore.count(v.first)==0){
            ljetIntStore.insert(std::make_pair(v.first, std::pair<std::vector<int>*,std::string>(new std::vector<int>, v.first)));
          }
        }
        for (auto v : currentEvent->m_largeJets.at(0)->variableFloatList()){
          if (ljetFloatStore.count(v.first)==0){
            ljetFloatStore.insert(std::make_pair(v.first, std::pair<std::vector<float>*,std::string>(new std::vector<float>, v.first)));
          }
        }
        for (auto v : currentEvent->m_largeJets.at(0)->variableCharList()){
          if (ljetCharStore.count(v.first)==0){
            ljetCharStore.insert(std::make_pair(v.first, std::pair<std::vector<char>*,std::string>(new std::vector<char>, v.first)));
          }
        }
      }
    }
    /// Electrons and Muon
    /// ... problematic...
    ///
    /// Jet decorations
    for(auto & jetIntDecor : jetIntStore){
      tree->Branch(jetIntDecor.first.c_str(),  &jetIntDecor.second.first);
    }
    for(auto & jetFloatDecor : jetFloatStore){
      tree->Branch(jetFloatDecor.first.c_str(),  &jetFloatDecor.second.first);
    }
    for(auto & jetCharDecor : jetCharStore){
      tree->Branch(jetCharDecor.first.c_str(),  &jetCharDecor.second.first);
    }
    /// Large-R jet decorations
    if(useLargeJets){
      for(auto & ljetIntDecor : ljetIntStore){
        tree->Branch(ljetIntDecor.first.c_str(),  &ljetIntDecor.second.first);
      }
      for(auto & ljetFloatDecor : ljetFloatStore){
        tree->Branch(ljetFloatDecor.first.c_str(),  &ljetFloatDecor.second.first);
      }
      for(auto & ljetCharDecor : ljetCharStore){
        tree->Branch(ljetCharDecor.first.c_str(),  &ljetCharDecor.second.first);
      }
    }
    /// Electron decorations
    for(auto & elIntDecor : elIntStore){
      tree->Branch(elIntDecor.first.c_str(),  &elIntDecor.second.first);
    }
    for(auto & elFloatDecor : elFloatStore){
      tree->Branch(elFloatDecor.first.c_str(),  &elFloatDecor.second.first);
    }
    for(auto & elCharDecor : elCharStore){
      tree->Branch(elCharDecor.first.c_str(),  &elCharDecor.second.first);
    }
    /// Muon decorations
    for(auto & muIntDecor : muIntStore){
      tree->Branch(muIntDecor.first.c_str(),  &muIntDecor.second.first);
    }
    for(auto & muFloatDecor : muFloatStore){
      tree->Branch(muFloatDecor.first.c_str(),  &muFloatDecor.second.first);
    }
    for(auto & muCharDecor : muCharStore){
      tree->Branch(muCharDecor.first.c_str(),  &muCharDecor.second.first);
    }
    /// Custom obj decorations
    auto* config = TTHbb::GlobalConfiguration::get();
    std::vector<std::string> customObjNames = TTHbb::util::vectoriseString((*config)("CustomObjects"));
    for(auto name : customObjNames){
      for(auto & customIntDecor : customIntStore[name]){
        tree->Branch(customIntDecor.first.c_str(),  &customIntDecor.second.first);
      }
      for(auto & customFloatDecor : customFloatStore[name]){
        tree->Branch(customFloatDecor.first.c_str(),  &customFloatDecor.second.first);
      }
      for(auto & customCharDecor : customCharStore[name]){
        tree->Branch(customCharDecor.first.c_str(),  &customCharDecor.second.first);
      }
    }
    
  }

  void EventHandler::fillAllObjectVectorsFromDecorations(){
    if(currentEvent->m_jets.size()>0){
      for (auto v : currentEvent->m_jets.at(0)->variableIntList()){
        if (v.first.find("tagWeightBin")!=std::string::npos) continue;
        jetIntStore.at(v.first).first->clear();
        for (unsigned int ijet=0; ijet<currentEvent->m_jets.size(); ijet++){
          jetIntStore.at(v.first).first->push_back(currentEvent->m_jets.at(ijet)->intVariable(v.first));
        }
      }
      for (auto v : currentEvent->m_jets.at(0)->variableFloatList()){
        if (v.first.find("mv2")!=std::string::npos || v.first.find("jvt")!=std::string::npos) continue;
        jetFloatStore.at(v.first).first->clear();
        for (unsigned int ijet=0; ijet<currentEvent->m_jets.size(); ijet++){
          jetFloatStore.at(v.first).first->push_back(currentEvent->m_jets.at(ijet)->floatVariable(v.first));
        }
      }
      for (auto v : currentEvent->m_jets.at(0)->variableCharList()){
        if (v.first.find("isbtagged")!=std::string::npos) continue;
        jetCharStore.at(v.first).first->clear();
        for (unsigned int ijet=0; ijet<currentEvent->m_jets.size(); ijet++){
          jetCharStore.at(v.first).first->push_back(currentEvent->m_jets.at(ijet)->charVariable(v.first));
        }
      }
    }
    if(useLargeJets && currentEvent->m_largeJets.size()>0){
      for (auto v : currentEvent->m_largeJets.at(0)->variableIntList()){
        ljetIntStore.at(v.first).first->clear();
        for (unsigned int iljet=0; iljet<currentEvent->m_largeJets.size(); iljet++){
          ljetIntStore.at(v.first).first->push_back(currentEvent->m_largeJets.at(iljet)->intVariable(v.first));
        }
      }
      for (auto v : currentEvent->m_largeJets.at(0)->variableFloatList()){
        ljetFloatStore.at(v.first).first->clear();
        for (unsigned int iljet=0; iljet<currentEvent->m_largeJets.size(); iljet++){
          ljetFloatStore.at(v.first).first->push_back(currentEvent->m_largeJets.at(iljet)->floatVariable(v.first));
        }
      }
      for (auto v : currentEvent->m_largeJets.at(0)->variableCharList()){
        ljetCharStore.at(v.first).first->clear();
        for (unsigned int iljet=0; iljet<currentEvent->m_largeJets.size(); iljet++){
          ljetCharStore.at(v.first).first->push_back(currentEvent->m_largeJets.at(iljet)->charVariable(v.first));
        }
      }
    }
  }
  
  void EventHandler::saveDefaultVariables(std::shared_ptr<TTree>& tree){
    /// All the default variables we will always need and use in every event
    /// Add them to the output tree if this function is run

    /// Event meta data
    tree->Branch("eventNumber",     &(this->eventData->eventNumber));
    tree->Branch("mcChannelNumber", &(this->eventData->mcChannelNumber));
    tree->Branch("runNumber",       &(this->eventData->runNumber));

    /// Jet branches
    tree->Branch("jet_pt",          &vIn_jet_pt);
    tree->Branch("jet_eta",         &vIn_jet_eta);
    tree->Branch("jet_phi",         &vIn_jet_phi);
    tree->Branch("jet_e",           &vIn_jet_e);
    tree->Branch("jet_mv2c10",      &vIn_jet_mv2);
    tree->Branch("jet_jvt",         &vIn_jet_jvt);
    
    /// Large jet branches
    if(useLargeJets){
      tree->Branch("ljet_pt",         &vIn_ljet_pt);
      tree->Branch("ljet_eta",        &vIn_ljet_eta);
      tree->Branch("ljet_phi",        &vIn_ljet_phi);
      tree->Branch("ljet_e",          &vIn_ljet_e);
      tree->Branch("ljet_D2",         &vIn_ljet_D2);
      tree->Branch("ljet_tau32_wta",  &vIn_ljet_tau32wta);
      tree->Branch("ljet_topTag",     &vIn_ljet_topTag);
    }

    /// Electron branches
    tree->Branch("el_pt",           &vIn_el_pt);
    tree->Branch("el_eta",          &vIn_el_eta);
    tree->Branch("el_phi",          &vIn_el_phi);
    tree->Branch("el_e",            &vIn_el_e);
    tree->Branch("el_charge",       &vIn_el_charge);

    /// Muon branches
    tree->Branch("mu_pt",           &vIn_mu_pt);
    tree->Branch("mu_eta",          &vIn_mu_eta);
    tree->Branch("mu_phi",          &vIn_mu_phi);
    tree->Branch("mu_e",            &vIn_mu_e);
    tree->Branch("mu_charge",       &vIn_mu_charge);

    /// MET branches
    tree->Branch("met_met",         &met_met);
    tree->Branch("met_phi",         &met_phi);
  }

  void EventHandler::saveSomeDefaultVariables(std::string list, std::shared_ptr<TTree>& tree){
    std::vector<std::string> varsToSave = TTHbb::util::vectoriseString(list);
    auto toBeSaved = [&](std::string var){ return (std::find(varsToSave.begin(), varsToSave.end(), var)
						   != varsToSave.end()); };
    if(toBeSaved("eventNumber")){
      tree->Branch("eventNumber",     &(this->eventData->eventNumber));
    }
    if(toBeSaved("mcChannelNumber")){
      tree->Branch("mcChannelNumber", &(this->eventData->mcChannelNumber));
    }
    if(toBeSaved("runNumber")){
      tree->Branch("runNumber",       &(this->eventData->runNumber));
    }
    /// Jet branches
    if(toBeSaved("jet_pt")){
      tree->Branch("jet_pt",          &vIn_jet_pt);
    }
    if(toBeSaved("jet_eta")){
      tree->Branch("jet_eta",         &vIn_jet_eta);
    }
    if(toBeSaved("jet_phi")){
      tree->Branch("jet_phi",         &vIn_jet_phi);
    }
    if(toBeSaved("jet_e")){
      tree->Branch("jet_e",           &vIn_jet_e);
    }
    if(toBeSaved("jet_mv2c10")){
      tree->Branch("jet_mv2c10",      &vIn_jet_mv2);
    }
    if(toBeSaved("jet_jvt")){
      tree->Branch("jet_jvt",         &vIn_jet_jvt);
    }
    
    /// Large jet branches
    if(useLargeJets){
      if(toBeSaved("ljet_pt")){
	tree->Branch("ljet_pt",         &vIn_ljet_pt);
      }
      if(toBeSaved("ljet_eta")){
	tree->Branch("ljet_eta",        &vIn_ljet_eta);
      }
      if(toBeSaved("ljet_phi")){
	tree->Branch("ljet_phi",        &vIn_ljet_phi);
      }
      if(toBeSaved("ljet_e")){
	tree->Branch("ljet_e",          &vIn_ljet_e);
      }
      if(toBeSaved("ljet_D2")){
	tree->Branch("ljet_D2",         &vIn_ljet_D2);
      }
      if(toBeSaved("ljet_tau32_wta")){
	tree->Branch("ljet_tau32_wta",  &vIn_ljet_tau32wta);
      }
      if(toBeSaved("ljet_topTag")){
	tree->Branch("ljet_topTag",     &vIn_ljet_topTag);
      }
    }

    /// Electron branches
    if(toBeSaved("el_pt")){
      tree->Branch("el_pt",           &vIn_el_pt);
    }
    if(toBeSaved("el_eta")){
      tree->Branch("el_eta",          &vIn_el_eta);
    }
    if(toBeSaved("el_phi")){
      tree->Branch("el_phi",          &vIn_el_phi);
    }
    if(toBeSaved("el_e")){
      tree->Branch("el_e",            &vIn_el_e);
    }
    if(toBeSaved("el_charge")){
      tree->Branch("el_charge",       &vIn_el_charge);
    }

    /// Muon branches
    if(toBeSaved("mu_pt")){
      tree->Branch("mu_pt",           &vIn_mu_pt);
    }
    if(toBeSaved("mu_eta")){
      tree->Branch("mu_eta",          &vIn_mu_eta);
    }
    if(toBeSaved("mu_phi")){
      tree->Branch("mu_phi",          &vIn_mu_phi);
    }
    if(toBeSaved("mu_e")){
      tree->Branch("mu_e",            &vIn_mu_e);
    }
    if(toBeSaved("mu_charge")){
      tree->Branch("mu_charge",       &vIn_mu_charge);
    }

    /// MET branches
    if(toBeSaved("met_met")){
      tree->Branch("met_met",         &met_met);
    }
    if(toBeSaved("met_phi")){
      tree->Branch("met_phi",         &met_phi);
    }
  }

  void EventHandler::initialisePointers(){
    /// initialise all pointers to zero
    vIn_jet_pt        = 0;
    vIn_jet_eta       = 0;
    vIn_jet_phi       = 0;
    vIn_jet_e         = 0;
    vIn_jet_mv2       = 0;
    vIn_jet_jvt       = 0;

    vIn_ljet_pt       = 0;
    vIn_ljet_eta      = 0;
    vIn_ljet_phi      = 0;
    vIn_ljet_e        = 0;
    vIn_ljet_topTag   = 0;
    vIn_ljet_D2       = 0;
    vIn_ljet_tau32wta = 0;

    vIn_el_pt     = 0;
    vIn_el_eta    = 0;
    vIn_el_phi    = 0;
    vIn_el_e      = 0;
    vIn_el_charge = 0;

    vIn_mu_pt     = 0;
    vIn_mu_eta    = 0;
    vIn_mu_phi    = 0;
    vIn_mu_e      = 0;
    vIn_mu_charge = 0;

    eventData = std::make_shared<TTHbb::EventData>();
  }

  void EventHandler::resetEventDecorations(){
    for(auto intVar : currentEvent->variableIntList()){
      currentEvent->intVariable(intVar.first) = -999;
    }
    
    for(auto floatVar : currentEvent->variableFloatList()){
      currentEvent->floatVariable(floatVar.first) = -999;
    }
    
    for(auto charVar : currentEvent->variableCharList()){
      currentEvent->charVariable(charVar.first) = -9;
    }
  }

  void EventHandler::checkCollections(){
    /// Check whether to use large jet collection
    auto* config = TTHbb::GlobalConfiguration::get();
    if(TTHbb::util::iequals((*config)("SampleType"),"data")){
      isData = true;
    }
    if(TTHbb::util::iequals((*config)("UseLargeJets"),"true")){
      useLargeJets = true;
    }
  }

  bool EventHandler::isEventSkippedForTTbarBfilter(){
    /// Please please please please keep these in numerical order
    std::vector<int> inclusiveSamples = {410000, 410009, 410225, 410226, 410250, 410251, 410252, 410501, 410503, 410511, 410512, 410515, 410516, 410525, 410527};
    std::vector<int> bfilterSamples   = {410120, 410121, 410274, 410275, 410281, 410282, 410283, 410504, 410505, 410517, 410518, 410519, 410520, 410528, 410529};
    
    /// Check whether dsid in either list
    /// Check whether the value is the filter value and skip appropriately
    if(std::binary_search(inclusiveSamples.begin(), inclusiveSamples.end(), this->eventData->mcChannelNumber)){
      if((this->intStore.at("TopHeavyFlavorFilterFlag")).first == 5){
	return true;
      }
      else{
	return false;
      }
    }
    else if(std::binary_search(bfilterSamples.begin(), bfilterSamples.end(), this->eventData->mcChannelNumber)){
      if((this->intStore.at("TopHeavyFlavorFilterFlag")).first == 5){
	return false;
      }
      else{
	return true;
      }
    }
    return false;
  }

  bool EventHandler::isEventSkippedForTTbarDilepton(){
    /// Please please please please keep these in numerical order
    std::vector<int> inclusiveSamples = {410000, 410120, 410225, 410250, 410251, 410274, 410501, 410504, 410525, 410528};
    //std::vector<int> dileptonSamples  = {410009, 410121, 410503};
    
    /// Check whether dsid in list
    /// Check whether the value is the filter value and skip
    if(std::binary_search(inclusiveSamples.begin(), inclusiveSamples.end(), this->eventData->mcChannelNumber)){
      if((this->intStore.at("truth_top_dilep_filter")).first){
	return true;
      }
      else{
	return false;
      }
    }
    /*else if(std::binary_search(bfilterSamples.begin(), bfilterSamples.end(), mcChannelNumber)){
      if(currentEvent->intVariable("truth_top_dilep_filter")){
      return false;
      }
      else{
      return true;
      }
      }*/
    return false;
  }
}
