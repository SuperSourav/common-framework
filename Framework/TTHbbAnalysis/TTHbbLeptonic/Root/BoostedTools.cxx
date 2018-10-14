/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/BoostedTools.h"

BoostedTools::BoostedTools() : m_boostedjet(0) {

}

BoostedTools::~BoostedTools(){
  
}

void BoostedTools::initialise(const xAOD::Jet &jet){

  m_boostedjet = &jet;

}

float BoostedTools::Tau21_wta(){

  float tau1_wta = m_boostedjet->getAttribute<float>("Tau1_wta");
  float tau2_wta = m_boostedjet->getAttribute<float>("Tau2_wta");
  float tau21_wta = (tau1_wta > 0.) ? tau2_wta/tau1_wta : -999.;

  return tau21_wta;

}

float BoostedTools::Tau32_wta(){

  float tau2_wta = m_boostedjet->getAttribute<float>("Tau2_wta");
  float tau3_wta = m_boostedjet->getAttribute<float>("Tau3_wta");
  float tau32_wta = (tau2_wta > 0.) ? tau3_wta/tau2_wta : -999.;

  return tau32_wta;

}

float BoostedTools::D2(){

  float d2 = 0;

  float ECF1 = m_boostedjet->getAttribute<float>("ECF1");
  float ECF2 = m_boostedjet->getAttribute<float>("ECF2");
  float ECF3 = m_boostedjet->getAttribute<float>("ECF3");
  
  d2 = (ECF2 > 0.) ? ECF3 * pow(ECF1, 3.0) / pow(ECF2, 3.0) : -999.;

  return d2;

}

float BoostedTools::C2(){

  float c2 = 0;

  float ECF1 = m_boostedjet->getAttribute<float>("ECF1");
  float ECF2 = m_boostedjet->getAttribute<float>("ECF2");
  float ECF3 = m_boostedjet->getAttribute<float>("ECF3");
  
  c2 = (ECF2 > 0.) ? ECF3 * ECF1 / pow(ECF2, 2.0) : -999.;

  return c2;

}
/*
// following top tagging pre-recommendations
bool BoostedTools::TopTag(){

  bool topTag = false;
  bool topTag50 = m_boostedjet->getAttribute<char>("isTopTagged_50" );

  if(topTag50) topTag = true;

  return topTag;
}

bool BoostedTools::TopTag_loose(){

  bool topTag_loose = false;
  bool topTag80 = m_boostedjet->getAttribute<char>("isTopTagged_80" );

  if(topTag80) topTag_loose = true;

  return topTag_loose;
}
*/

// following RUN-1 boson tagging pre-recommendations
bool BoostedTools::BosonTag(){

  bool bosonTag = false;

  // large-R jets mass window and pT control
  if( m_boostedjet->m() > 61000 && m_boostedjet->m() < 93000 && 
    m_boostedjet->pt() > 200000 && m_boostedjet->pt() < 350000){
    if( BoostedTools::D2() < 1.14 ) bosonTag = true;
  }

  if( m_boostedjet->m() > 71000 && m_boostedjet->m() < 91000 && 
    m_boostedjet->pt() > 350000 && m_boostedjet->pt() < 500000){
    if( BoostedTools::D2() < 1.23 ) bosonTag = true;
  }

  if( m_boostedjet->m() > 73000 && m_boostedjet->m() < 91000 && 
    m_boostedjet->pt() > 500000 && m_boostedjet->pt() < 1000000){
    if( BoostedTools::D2() < 1.35 ) bosonTag = true;
  }

  return bosonTag;
}

// for now just the same as BosonTag
bool BoostedTools::BosonTag_loose(){

  bool bosonTag_loose = false;

  // large-R jets mass window and pT control
  if( m_boostedjet->m() > 61000 && m_boostedjet->m() < 93000 && 
    m_boostedjet->pt() > 200000 && m_boostedjet->pt() < 350000){
    if( BoostedTools::D2() < 1.14 ) bosonTag_loose = true;
  }

  if( m_boostedjet->m() > 71000 && m_boostedjet->m() < 91000 && 
    m_boostedjet->pt() > 350000 && m_boostedjet->pt() < 500000){
    if( BoostedTools::D2() < 1.23 ) bosonTag_loose = true;
  }

  if( m_boostedjet->m() > 73000 && m_boostedjet->m() < 91000 && 
    m_boostedjet->pt() > 500000 && m_boostedjet->pt() < 1000000){
    if( BoostedTools::D2() < 1.35 ) bosonTag_loose = true;
  }

  return bosonTag_loose;
}


