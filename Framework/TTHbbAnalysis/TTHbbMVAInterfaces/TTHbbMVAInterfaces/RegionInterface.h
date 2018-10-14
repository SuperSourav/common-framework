/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RegionInterface_HH
#define RegionInterface_HH

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "TTHbbMVAInterfaces/TMVAConfiguration.h"
#include "TTHbbObjects/BaseObject.h"

using namespace TTHbb;

class RegionInterface{

public:

  RegionInterface();
  virtual ~RegionInterface();

  class RegionDefinition{

    friend class RegionInterface;

  public:

    RegionDefinition();
    ~RegionDefinition();

    std::string name;
    int nJetsMin; /// >=
    int nJetsMax; // <
    int nBJetsMin; /// >=
    int nBJetsMax; // <
    int lepton; //// 0 for all
    int index;
    TMVAConfiguration tmvaConfiguration;

    void floatUpperCut(std::string, float);
    void floatLowerCut(std::string, float);

    void intUpperCut(std::string, int);
    void intLowerCut(std::string, int);


  private:

    std::map<std::string, float> m_fupCuts;
    std::map<std::string, float> m_floCuts;
    std::map<std::string, int> m_iupCuts;
    std::map<std::string, int> m_iloCuts;
    
  };


  void addRegion(std::string n, int jmin, int jmax, int bmin, int bmax, int lepton);
  void addRegion(TMVAConfiguration , int jmin, int jmax, int bmin, int bmax, int lepton);
  void addRegion(RegionDefinition region);

protected:

  void getRegions(std::vector<int>& regions, int njets, int nbjets, int lepton, BaseObject* vars); 
  void getRegions(std::vector<RegionDefinition>& regions, int njets, int nbjets, int lepton, BaseObject* vars);

  bool pass(RegionDefinition& r, int njets, int nbjets, int lepton);
  bool pass(RegionDefinition& r, BaseObject* vars);


  std::vector<RegionDefinition> m_regions;


};


#endif
