/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParticleIDHelper_HH
#define ParticleIDHelper_HH

#include <cstdlib>

class ParticleIDHelper{

public:

  static bool isQuark(int pdgId){ return abs(pdgId)<7 && abs(pdgId)>0; }
  static bool isbq(int pdgId){ return  abs(pdgId)==5; }
  static bool iscq(int pdgId){ return  abs(pdgId)==4; }
  static bool isTop(int pdgId){ return  abs(pdgId)==6; }
  static bool isPhoton(int pdgId){ return  pdgId==22; }
  static bool isGluon(int pdgId){ return  pdgId==21; }
  static bool isW(int pdgId){ return  abs(pdgId)==24; }
  static bool isZ(int pdgId){ return  abs(pdgId)==23; }
  static bool isV(int pdgId){ return isW(pdgId) || isZ(pdgId); }
  static bool isH(int pdgId){ return  abs(pdgId)==25; }
  static bool isLep(int pdgId){ return  abs(pdgId)==11 || abs(pdgId)==13 || abs(pdgId)==15; }
  static bool isTau(int pdgId){ return  abs(pdgId)==15; }
  static bool isMu(int pdgId){ return  abs(pdgId)==13; }
  static bool isNu(int pdgId){ return  abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16; }

  static bool isChargedHiggs(int pdgId){ return  abs(pdgId)==37; }
  static bool isBSMHiggs(int pdgId){ return  abs(pdgId)==35; }
  
  static bool isCHadron(int pdgId);
  static bool isBHadron(int pdgId);
  static bool isHFHadron(int pdgId){ return  isCHadron(pdgId) || isBHadron(pdgId); }

  static bool isGeant(int barcode){ return barcode >= 200000; }

};

#endif



