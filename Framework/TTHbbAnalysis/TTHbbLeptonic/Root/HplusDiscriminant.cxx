/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/HplusDiscriminant.h"
#include "HplusDiscriminant/HplusDiscriminantData.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TTHbbLeptonic/EventSaverHelper.h"

#include <iostream>
#include <algorithm>

HplusDiscriminant::HplusDiscriminant(std::string params, std::shared_ptr<top::TopConfig> config) :
  m_config(config), m_store_apmmj(true), m_store_likelihood(true){
  auto masspoints = TTHbb::util::vectoriseString(params," ");
  for(auto it=masspoints.begin(); it!=masspoints.end(); it++){
    m_masspoints.push_back(atoi(it->c_str()));
  }

  m_calculator=new TTHbb::HplusDiscriminantCalculator(m_masspoints);
  
  auto* esh = EventSaverHelper::get();

  for(std::vector<int>::const_iterator it=m_masspoints.begin(); it!=m_masspoints.end(); it++){
    TString title="HpDiscriminant_"; title+=(*it);
    esh->addVariableToBeSaved(title.Data(), TTHbb::DecorationType::Float);
    if(m_store_apmmj){
      title="HpDiscriminantAPM_"; title+=(*it);
      esh->addVariableToBeSaved(title.Data(), TTHbb::DecorationType::Float);
      title="HpDiscriminantMJ_"; title+=(*it);
      esh->addVariableToBeSaved(title.Data(), TTHbb::DecorationType::Float);
    }
    if(m_store_likelihood){
      title="HpDiscriminantLikelihoodHp_"; title+=(*it);
      esh->addVariableToBeSaved(title.Data(), TTHbb::DecorationType::Float);
    }
    if(m_store_apmmj && m_store_likelihood){
      title="HpDiscriminantLikelihoodHpAPM_"; title+=(*it);
      esh->addVariableToBeSaved(title.Data(), TTHbb::DecorationType::Float);
      title="HpDiscriminantLikelihoodHpMJ_"; title+=(*it);
      esh->addVariableToBeSaved(title.Data(), TTHbb::DecorationType::Float);
    }
  }
  if(m_store_likelihood) esh->addVariableToBeSaved("HpDiscriminantLikelihoodTtbar", TTHbb::DecorationType::Float);
  if(m_store_apmmj && m_store_likelihood){
    esh->addVariableToBeSaved("HpDiscriminantLikelihoodTtbarAPM", TTHbb::DecorationType::Float);
    esh->addVariableToBeSaved("HpDiscriminantLikelihoodTtbarMJ", TTHbb::DecorationType::Float);
  } 

}

HplusDiscriminant::~HplusDiscriminant(){
  if(m_calculator!=0) delete m_calculator;
}

bool HplusDiscriminant::apply(const top::Event & event) const{
  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  const double GeV=1000.;// MeV - GeV conversion factor
  bool ejets=tthevt->m_electrons.size()==1 && tthevt->m_muons.size()==0; 
  bool mujets=tthevt->m_electrons.size()==0 && tthevt->m_muons.size()==1;
  
  unsigned int njets=tthevt->m_jets.size();
  int nbjets=0;
  for(unsigned int ijet=0; ijet<tthevt->m_jets.size(); ijet++){
    if(tthevt->m_jets.at(ijet)->charVariable("isbtagged_77"))
      nbjets++;
  }
  
  if( (ejets || mujets) && njets>=5){ // removed the nbjets requirement
    //getting the data needed for calculating the discriminant
    TTHbb::HplusDiscriminantData *data=new TTHbb::HplusDiscriminantData();
    data->m_eventNumber=tthevt->m_info->eventNumber;
    data->m_ejets=ejets;
    data->m_mujets=mujets;
    data->m_njets=njets;
    data->m_nbjets=nbjets;
    if(tthevt->m_leptons.size()<1) std::cout << "HplusDiscriminant.cxx: ERROR: No lepton found tthevt though ejets or mujets selection was passed!" << std::endl;
    if(ejets)
      data->m_lepton.SetPtEtaPhiM( tthevt->m_leptons.at(0)->Pt()/GeV, tthevt->m_leptons.at(0)->Eta(), tthevt->m_leptons.at(0)->Phi(), 0.000511);
    if(mujets)
      data->m_lepton.SetPtEtaPhiM( tthevt->m_leptons.at(0)->Pt()/GeV, tthevt->m_leptons.at(0)->Eta(), tthevt->m_leptons.at(0)->Phi(), 0.10565);
    data->m_neutrino.SetPxPyPzE( tthevt->met_met/GeV * cos(tthevt->met_phi),
				 tthevt->met_met/GeV * sin(tthevt->met_phi),
				 0.,
				 tthevt->met_met/GeV);
    for(unsigned int ijet=0; ijet<tthevt->m_jets.size(); ijet++){
      TLorentzVector jet; jet.SetPtEtaPhiE( tthevt->m_jets.at(ijet)->Pt()/GeV, tthevt->m_jets.at(ijet)->Eta(), tthevt->m_jets.at(ijet)->Phi(), tthevt->m_jets.at(ijet)->E()/GeV );
      data->m_jets.push_back(jet);
      data->m_jets_mv2c10.push_back(tthevt->m_jets.at(ijet)->floatVariable("mv2"));
    }
    if(njets!=data->m_jets.size()) std::cout << "HplusDiscriminant.cxx: ERROR: inconsistency between number of jets from tthevt and jet vector!" << std::endl;
    
    // calculate discriminant
    m_calculator->CalculateDiscriminant(data);

    for(std::vector<int>::const_iterator it=m_masspoints.begin(); it!=m_masspoints.end(); it++){
      TString title="HpDiscriminant_"; title+=(*it);
      tthevt->floatVariable(title.Data()) = m_calculator->GetDiscriminant((*it));
      if(m_store_apmmj){
	title="HpDiscriminantAPM_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = m_calculator->GetDiscriminantAPM((*it));
	title="HpDiscriminantMJ_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = m_calculator->GetDiscriminantMJ((*it));
      }
      if(m_store_likelihood){
	title="HpDiscriminantLikelihoodHp_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = m_calculator->GetLikelihood((*it));
      }
      if(m_store_apmmj && m_store_likelihood){
	title="HpDiscriminantLikelihoodHpAPM_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = m_calculator->GetLikelihoodAPM((*it));
	title="HpDiscriminantLikelihoodHpMJ_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = m_calculator->GetLikelihoodMJ((*it));
      }
    }
    if(m_store_likelihood) tthevt->floatVariable("HpDiscriminantLikelihoodTtbar") = m_calculator->GetLikelihood(-1);
    if(m_store_apmmj && m_store_likelihood){
      tthevt->floatVariable("HpDiscriminantLikelihoodTtbarAPM") = m_calculator->GetLikelihoodAPM(-1);
      tthevt->floatVariable("HpDiscriminantLikelihoodTtbarMJ") = m_calculator->GetLikelihoodMJ(-1);
    }
    
    if(data!=0) delete data;
  }
  else{ // set default value for discriminant
    for(std::vector<int>::const_iterator it=m_masspoints.begin(); it!=m_masspoints.end(); it++){
      TString title="HpDiscriminant_"; title+=(*it);
      tthevt->floatVariable(title.Data()) = -9999999.;
      if(m_store_apmmj){
	title="HpDiscriminantAPM_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = -9999999.;
	title="HpDiscriminantMJ_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = -9999999.;
      }
      if(m_store_likelihood){
	title="HpDiscriminantLikelihoodHp_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = -9999999.;
      }
      if(m_store_apmmj && m_store_likelihood){
	title="HpDiscriminantLikelihoodHpAPM_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = -9999999.;
	title="HpDiscriminantLikelihoodHpMJ_"; title+=(*it);
	tthevt->floatVariable(title.Data()) = -9999999.;
      }
    }
    if(m_store_likelihood) tthevt->floatVariable("HpDiscriminantLikelihoodTtbarLikelihoodTtbar") = -9999999.;
    if(m_store_apmmj && m_store_likelihood){
      tthevt->floatVariable("HpDiscriminantLikelihoodTtbarAPM") = -9999999.;
      tthevt->floatVariable("HpDiscriminantLikelihoodTtbarMJ") = -9999999.;
    } 
  }
  
  return true;
}

std::string HplusDiscriminant::name() const{
  return "HplusDiscriminant";
}
