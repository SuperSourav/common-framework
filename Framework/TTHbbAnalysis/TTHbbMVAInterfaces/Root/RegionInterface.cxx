/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbMVAInterfaces/RegionInterface.h"


RegionInterface::RegionInterface(){

}

RegionInterface::~RegionInterface(){
  ////
}



void RegionInterface::addRegion(std::string n, int jmin, int jmax, int bmin, int bmax, int lepton){

  RegionDefinition r;
  r.name=n;
  r.nJetsMin=jmin;
  r.nJetsMax=jmax;
  r.nBJetsMin=bmin;
  r.nBJetsMax=bmax;
  r.lepton=lepton;
  r.index=m_regions.size();

  m_regions.push_back(r);
}


void RegionInterface::addRegion(TMVAConfiguration config, int jmin, int jmax, int bmin, int bmax, int lepton){

  RegionDefinition r;
  //r.name=n;
  r.tmvaConfiguration=config;
  r.nJetsMin=jmin;
  r.nJetsMax=jmax;
  r.nBJetsMin=bmin;
  r.nBJetsMax=bmax;
  r.lepton=lepton;
  r.index=m_regions.size();

  m_regions.push_back(r);
}


void RegionInterface::addRegion(RegionDefinition region){

  region.index=m_regions.size();
  m_regions.push_back(region);

}


void RegionInterface::getRegions(std::vector<RegionDefinition>& regions, int njets, int nbjets, int lepton, BaseObject* vars){

  for(auto r : m_regions){

    if(!pass(r, njets, nbjets, lepton))continue;
    if(!pass(r, vars))continue;
    
    regions.push_back(r);
  }

}

void RegionInterface::getRegions(std::vector<int>& regions, int njets, int nbjets, int lepton, BaseObject* vars){

  for(auto r : m_regions){

    if(!pass(r, njets, nbjets, lepton))continue;
    if(!pass(r, vars))continue;
   
    
    regions.push_back(r.index);
  }

}


bool RegionInterface::pass(RegionDefinition& r, int njets, int nbjets, int lepton){


  if(r.lepton != 0){
    if(lepton != r.lepton) return false;
  }

  if(r.nJetsMin<=r.nJetsMax){
    if(njets<r.nJetsMin && r.nJetsMin>=0) return false;
    if(njets>r.nJetsMax && r.nJetsMax>=0) return false;
  }

  if(r.nBJetsMin<=r.nBJetsMax){
    if(nbjets<r.nBJetsMin && r.nBJetsMin>=0) return false;
    if(nbjets>r.nBJetsMax && r.nBJetsMax>=0) return false;
  }
    
  return true;

}

bool RegionInterface::pass(RegionDefinition& r, BaseObject* vars){

  if(!vars) return true;

  for(auto a : r.m_fupCuts){
    if(!vars->checkFloatVariable(a.first)){
      std::cout << "Warning RegionInterface::pass float variable " << a.first << " not available " << std::endl;
      return false;
    }
    if(vars->floatVariable(a.first) > a.second) return false;
  }

  for(auto a : r.m_floCuts){
    if(!vars->checkFloatVariable(a.first)){
      std::cout << "Warning RegionInterface::pass float variable " << a.first << " not available " << std::endl;
      return false;
    }
    if(vars->floatVariable(a.first) < a.second) return false;
  }


  for(auto a : r.m_iupCuts){
    if(!vars->checkIntVariable(a.first)){
      std::cout << "Warning RegionInterface::pass int variable " << a.first << " not available " << std::endl;
      return false;
    }
    if(vars->intVariable(a.first) > a.second) return false;
  }

  for(auto a : r.m_iloCuts){
    if(!vars->checkIntVariable(a.first)){
      std::cout << "Warning RegionInterface::pass int variable " << a.first << " not available " << std::endl;
      return false;
    }
    if(vars->intVariable(a.first) < a.second) return false;
  }



  return true;

}


RegionInterface::RegionDefinition::RegionDefinition(){

  name = "unknown_region";
  nJetsMin=-1; /// >=
  nJetsMax=-1; // <=
  nBJetsMin=-1; /// >=
  nBJetsMax=-1; // <=
  lepton=0; //// 0 for all
  index=-1;

}

RegionInterface::RegionDefinition::~RegionDefinition(){

}

/// no protection for now
void RegionInterface::RegionDefinition::floatUpperCut(std::string s, float a){
  m_fupCuts[s]=a;
}

void RegionInterface::RegionDefinition::floatLowerCut(std::string s, float a){
  m_floCuts[s]=a;
}

void RegionInterface::RegionDefinition::intUpperCut(std::string s, int a){
  m_iupCuts[s]=a;
}

void RegionInterface::RegionDefinition::intLowerCut(std::string s, int a){
  m_iloCuts[s]=a;
}
