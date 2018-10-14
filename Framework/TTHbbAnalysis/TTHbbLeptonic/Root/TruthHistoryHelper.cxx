/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TruthHistoryHelper.h"

#include "TTHbbTruthHelper/ParticleIDHelper.h"
#include "TTHbbLeptonic/TruthxAODPartHelper.h"

#include <cstdlib>

TruthHistoryHelper::TruthHistoryHelper(){
  m_avoidSherpaLoop=true;
  m_isPythia6=false;
}

TruthHistoryHelper::~TruthHistoryHelper(){
}


bool TruthHistoryHelper::isLooping(const xAOD::TruthParticle* part, 
				   const xAOD::TruthParticle* child) const {
  return part->barcode()>=child->barcode() && m_avoidSherpaLoop;
}

bool TruthHistoryHelper::doesLoop(const xAOD::TruthParticle* part, std::set<const xAOD::TruthParticle*>& init) const {
  
  if(!part) return false;

  for(unsigned int i = 0; i<part->nParents(); ++i){
    const xAOD::TruthParticle* parent = part->parent(i);
    if(!parent) continue;
    /// lets speed up (to check truth history)
    if(parent->pdgId() != part->pdgId()) continue;
    if(!init.insert(parent).second) return true;
    if(doesLoop(parent, init)) return true;
  }

  return false;
}


const xAOD::TruthParticle* TruthHistoryHelper::getFinalState(const xAOD::TruthParticle* part) const {

  std::set<const xAOD::TruthParticle*> init;
  init.insert(part);
  bool doesloop = false;
  if(m_avoidSherpaLoop){
    doesloop = doesLoop(part, init);
  }

  for(unsigned int i = 0; i<part->nChildren(); ++i){
    const xAOD::TruthParticle* child = part->child(i);
    if(!child) continue;
    if(doesloop && isLooping(part, child)) continue;
    if(child->pdgId() == part->pdgId()){
      return getFinalState(child);
    }
  }
  return part;
}

const xAOD::TruthParticle* TruthHistoryHelper::getInitialState(const xAOD::TruthParticle* part) const {

  std::set<const xAOD::TruthParticle*> init;
  init.insert(part);
  bool doesloop = false;
  if(m_avoidSherpaLoop){
   doesloop = doesLoop(part, init);
  }

  for(unsigned int i = 0; i<part->nParents(); ++i){
    const xAOD::TruthParticle* parent = part->parent(i);
    if(!parent) continue;
    if(doesloop && isLooping(parent, part)) continue;
    if(parent->pdgId() == part->pdgId()){
      return getInitialState(parent);
    }
  }
  return part;

}



bool TruthHistoryHelper::isHFHadronFinalState(const xAOD::TruthParticle* part, int type) const{
   
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    const xAOD::TruthParticle* child = part->child(i);
    if(!child) continue;
    //if(isLooping(part,child))continue;
    if(type==5){ 
      if ( ParticleIDHelper::isBHadron(child->pdgId())  ){
	return false;
      }
      if(!ParticleIDHelper::isGeant(child->barcode())){ 
	if( !isHFHadronFinalState(child,type)) return false;
      }
    }
    if(type==4){
      if ( ParticleIDHelper::isCHadron(child->pdgId()) ){
	return false;
      }
      if(!ParticleIDHelper::isGeant(child->barcode())){ 
	if( !isHFHadronFinalState(child,type)) return false;
      }
    }
  }
  return true;

}

bool TruthHistoryHelper::isHFHadronInitialState(const xAOD::TruthParticle* part, int type) const{
     
  for(unsigned int i = 0; i<part->nParents(); ++i){
    const xAOD::TruthParticle* parent = part->parent(i);
    if(!parent) continue;
    //if(isLooping(parent,part))continue;
    if(type==5){
      if ( ParticleIDHelper::isBHadron(parent->pdgId()) ){
	return false;
      }
      /// do we really needed that, check gen history
      //if(!isHFHadronInitialState(child,type)) return false;
    }
    if(type==4){
      if ( ParticleIDHelper::isCHadron(parent->pdgId()) ){
	return false;
      }
      //if(!isHFHadronInitialState(child,type)) return false;
    }
  }
  return true;
}


bool TruthHistoryHelper::islepW(const xAOD::TruthParticle* part) const {
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if( ParticleIDHelper::isLep( part->child(i)->pdgId()) ) return true;
    if( ParticleIDHelper::isNu( part->child(i)->pdgId()) ) return true;
  }
  return false;
}
bool TruthHistoryHelper::ishadW(const xAOD::TruthParticle* part) const {
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if( ParticleIDHelper::isQuark(part->child(i)->pdgId()) ) return true;
  }
  return false;
}
  

bool TruthHistoryHelper::isInitialHiggs(const xAOD::TruthParticle* part) const{

  if(!ParticleIDHelper::isH(part->pdgId())) return false;

  for(unsigned int i = 0; i<part->nParents(); ++i){
    if(!part->parent(i)) continue;
    if(ParticleIDHelper::isH(part->parent(i)->pdgId())) return false;
  }
 
  return true;

}

bool TruthHistoryHelper::isFinalHiggs(const xAOD::TruthParticle* part) const {
  
  if(!ParticleIDHelper::isH(part->pdgId())) return false;

  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if(!part->child(i)) continue;
    if(ParticleIDHelper::isH(part->child(i)->pdgId())) return false;
  }
 
  return true;
}
bool TruthHistoryHelper::isFinalVectorBoson(const xAOD::TruthParticle* part) const {
  
  if(!ParticleIDHelper::isV(part->pdgId())) return false;
  if(!part->nChildren())return false;
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if(!part->child(i)) continue;
    if(part->child(i)->pdgId()==part->pdgId()) return false;
  }
  return true;
}
bool TruthHistoryHelper::isFinalTop(const xAOD::TruthParticle* part) const {

  if(!ParticleIDHelper::isTop(part->pdgId())) return false;
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if(!part->child(i)) continue;
    if(abs(part->child(i)->pdgId())<6)return true;
  }
  return false;
}

bool TruthHistoryHelper::islepTop(const xAOD::TruthParticle* part) const {
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if(ParticleIDHelper::isW(part->child(i)->pdgId())){
      const xAOD::TruthParticle* WfromTop = getFinalState(part->child(i));
      return islepW(WfromTop);
    }
  }
  return false;
}

bool TruthHistoryHelper::isFinalBSMHiggs(const xAOD::TruthParticle* part) const {
  if(!ParticleIDHelper::isBSMHiggs(part->pdgId())) return false;
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if(abs(part->child(i)->pdgId())==36) return true;
  }
  return false;
}

bool TruthHistoryHelper::isFinalChargedHiggs(const xAOD::TruthParticle* part) const {
  if(!ParticleIDHelper::isChargedHiggs(part->pdgId())) return false;

  for(unsigned int i = 0; i<part->nChildren(); ++i){
    if(!part->child(i)) continue;
    if(ParticleIDHelper::isbq(part->child(i)->pdgId())) return true;
    if(ParticleIDHelper::isTop(part->child(i)->pdgId())) return true;
  }
  
  return false;
}



bool TruthHistoryHelper::isfromHiggs(const xAOD::TruthParticle* part) const {
  for(unsigned int i = 0; i<part->nParents(); ++i){
    if(!part->parent(i)) continue;
    if(ParticleIDHelper::isH(part->parent(i)->pdgId())) return true;
  }	  
  return false;
}
bool TruthHistoryHelper::isfromTop(const xAOD::TruthParticle* part) const {
  for(unsigned int i = 0; i<part->nParents(); ++i){
    if(!part->parent(i)) continue;
    if(ParticleIDHelper::isTop(part->parent(i)->pdgId())) return true;
  }	  
  return false;
}
/// some generators have W from tau
bool TruthHistoryHelper::isfromTau(const xAOD::TruthParticle* part) const {
  for(unsigned int i = 0; i<part->nParents(); ++i){
    if(!part->parent(i)) continue;
    if(ParticleIDHelper::isTau(part->parent(i)->pdgId())) return true;
  }	  
  return false;
}

bool TruthHistoryHelper::isAssociatedWithChargedHiggs(const xAOD::TruthParticle* part) const {

  if(ParticleIDHelper::isbq(part->pdgId())) return false;
  for(unsigned int i = 0; i<part->nParents(); ++i){
    const  xAOD::TruthParticle* pparent = part->parent(i);
    if(!pparent) continue;
    for(unsigned int j = 0; j<pparent->nChildren(); ++j){
      if(!pparent->child(j)) continue;
      if(ParticleIDHelper::isChargedHiggs(pparent->child(j)->pdgId())) return true;
    }
  }
  return false;
}

