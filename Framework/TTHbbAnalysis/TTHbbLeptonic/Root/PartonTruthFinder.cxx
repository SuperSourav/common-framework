/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/PartonTruthFinder.h"

#include "TTHbbTruthHelper/ParticleIDHelper.h"

#include "TTHbbLeptonic/TruthxAODPartHelper.h"

//-----------------------------------------------//

PartonTruthFinder::PartonTruthFinder()
{
  m_truthHistoryHelper=0;
}

PartonTruthFinder::~PartonTruthFinder(){

}

//--------//

/// migrate the following 2 functions?
void PartonTruthFinder::setTruthPartDecoration(const xAOD::TruthParticle* part, 
					       TTHbbTruth::TTHbbTruthParticleLabel bit) const{
  if(!part->isAvailable<long long>("TTHbbGeneralTruthParticleLabel")){
    part->auxdecor<long long>("TTHbbGeneralTruthParticleLabel")=0;
  }
  long long one=1;
  part->auxdecor<long long>("TTHbbGeneralTruthParticleLabel")|= (one<<bit);
}


void PartonTruthFinder::sortPt(std::vector<const xAOD::TruthParticle*>& parts) const{

  for(unsigned int i=0; i<parts.size(); ++i){
    bool ok=true;
    for(unsigned int j=0; j<parts.size()-1; ++j){
      if(parts[j]->pt() < parts[j+1]->pt()){
	ok=false;
	auto hold = parts[j];
	parts[j]=parts[j+1];
	parts[j+1]=hold;
      }
    }
    if(ok) break;
  }

}

void PartonTruthFinder::fillPartons(const xAOD::TruthParticleContainer* truthcontainer, 
					 std::set<const xAOD::TruthParticle*>& partons) const{

 
  ///disclaimer
  //// this logic will work only on generators with "proper" history where the mother<->child relation is conserved
  //// this is for example not the case for Pythia6 where status 3 (initial) states are not always connected to status 2 (final) states
  //// for ttbar PP6 you will get additional W bosons not from top (even if they come from top)

  std::vector<const xAOD::TruthParticle*> sortedTops;

  for(const xAOD::TruthParticle*  part : *truthcontainer) {//loop truth

    if(!part) continue;
    if(ParticleIDHelper::isGeant(part->barcode())) break; /// we are at geant and we don't want those

    ///TruthxAODPartHelper::printParticle(part);

    if(m_truthHistoryHelper->isInitialHiggs(part)){
      setTruthPartDecoration(part, TTHbbTruth::higgs);
      setTruthPartDecoration(part, TTHbbTruth::initialState);
      partons.insert(part);
    }

    if(m_truthHistoryHelper->isFinalHiggs(part)){
      setTruthPartDecoration(part, TTHbbTruth::higgs);
      setTruthPartDecoration(part, TTHbbTruth::finalState);
      partons.insert(part);
      int hdec = fillHiggsDaughters(part, partons);
      part->auxdecor<int>("TTHbbHiggsDecayFlag")=hdec;
    } 

    //tops, keep them for sorting
    if(m_truthHistoryHelper->isFinalTop(part)){
      sortedTops.push_back(part);
    }

    //// vector bosons
    if(m_truthHistoryHelper->isFinalVectorBoson(part)){
      fillVectorBosonsTruth(part, partons);
    }

    //   //BSM higgs
    if(m_truthHistoryHelper->isFinalBSMHiggs(part)){
      fillBSMHiggsMatching(part, partons);
    }

    //   //Charged higgs
    if(m_truthHistoryHelper->isFinalChargedHiggs(part)){
      fillChargedHiggsMatching(part, partons);
    }
    if(m_truthHistoryHelper->isAssociatedWithChargedHiggs(part)){
      fillAssociatedBwithChargedHiggs(part, partons);
    }

  }

  fillTopTruth(sortedTops, partons);

}



int PartonTruthFinder::fillHiggsDaughters(const xAOD::TruthParticle* part, 
					  std::set<const xAOD::TruthParticle*>& parts) const{

  TTHbbTruth::TTHbbTruthHiggsDecay hDecay = TTHbbTruth::Hother;

  std::vector<const xAOD::TruthParticle*> hdaughters; /// need a new one to sort
  std::vector<const xAOD::TruthParticle*> hdaughtersFinal; /// need a new one to sort

  /// lets have the children in order
  for(unsigned int i = 0; i<part->nChildren(); ++i){
    auto hchild = part->child(i);
    if(!hchild) continue;
    /// remove radiations (some generators, to check)
    if(part->nChildren()!=2){
      if(ParticleIDHelper::isGluon(hchild->pdgId())) continue;
      if(ParticleIDHelper::isPhoton(hchild->pdgId())) continue;
    }
    auto hchildfinal = m_truthHistoryHelper->getFinalState(hchild);

    setTruthPartDecoration(hchild, TTHbbTruth::childFromHiggs);
    setTruthPartDecoration(hchildfinal, TTHbbTruth::childFromHiggs);

    setTruthPartDecoration(hchild, TTHbbTruth::initialState);
    setTruthPartDecoration(hchildfinal, TTHbbTruth::finalState);

    hdaughters.push_back(hchild);
    hdaughtersFinal.push_back(hchildfinal);

    parts.insert(hchild);
    parts.insert(hchildfinal);

  }

  sortPt(hdaughters);
  sortPt(hdaughtersFinal);

  if(hdaughters.size()>1){
    setTruthPartDecoration(hdaughters[1], TTHbbTruth::subleadingChildFromHiggs);
    setTruthPartDecoration(hdaughtersFinal[1], TTHbbTruth::subleadingChildFromHiggs);
  }
  if(hdaughters.size()>0){
    setTruthPartDecoration(hdaughters[0], TTHbbTruth::leadingChildFromHiggs);
    setTruthPartDecoration(hdaughtersFinal[0], TTHbbTruth::leadingChildFromHiggs);
  }


  int nz=0;
  int nga=0;

  for(auto child : hdaughtersFinal){
    /// will not do complicated checks unless we find something weird in some truth history
    if(ParticleIDHelper::isbq(child->pdgId())) hDecay=TTHbbTruth::Hbb;
    if(ParticleIDHelper::iscq(child->pdgId())) hDecay=TTHbbTruth::Hcc;
    if(ParticleIDHelper::isW(child->pdgId())) hDecay=TTHbbTruth::HWW;
    if(ParticleIDHelper::isZ(child->pdgId())) ++nz;
    if(ParticleIDHelper::isPhoton(child->pdgId())) ++nga;
    if(ParticleIDHelper::isTau(child->pdgId())) hDecay=TTHbbTruth::Htt;
    if(ParticleIDHelper::isMu(child->pdgId())) hDecay=TTHbbTruth::Hmumu;
    if(ParticleIDHelper::isGluon(child->pdgId()) && hDecay==TTHbbTruth::Hother) hDecay=TTHbbTruth::Hgg;

    /// keep only final state here
    if(ParticleIDHelper::isW(child->pdgId())){
      const xAOD::TruthParticle* Wfinal = child;//// already m_truthHistoryHelper->getFinalState(child);
      for(unsigned int i = 0; i<Wfinal->nChildren(); ++i){
    	auto wchild = Wfinal->child(i);
	if(!wchild) continue;
	//// remove photons
	if(ParticleIDHelper::isPhoton(wchild->pdgId())) continue;
	if(ParticleIDHelper::isGluon(wchild->pdgId())) continue;
    	setTruthPartDecoration(wchild, TTHbbTruth::childFromWHiggs);
    	setTruthPartDecoration(wchild, TTHbbTruth::finalState);
    	parts.insert(wchild);
      }
    }
    if(ParticleIDHelper::isZ(child->pdgId())){
      const xAOD::TruthParticle* Zfinal = child; //// already m_truthHistoryHelper->getFinalState(child);
      for(unsigned int i = 0; i<Zfinal->nChildren(); ++i){
    	auto zchild = Zfinal->child(i);
	if(!zchild) continue;
	//// remove photons
	if(ParticleIDHelper::isPhoton(zchild->pdgId())) continue;
	if(ParticleIDHelper::isGluon(zchild->pdgId())) continue;
    	setTruthPartDecoration(zchild, TTHbbTruth::childFromZHiggs);
    	setTruthPartDecoration(zchild, TTHbbTruth::finalState);
    	parts.insert(zchild);
      }
    }
  }
   
  if(nga==2 && hDecay==TTHbbTruth::Hother) hDecay=TTHbbTruth::Hgaga;
  if(nz==2) hDecay=TTHbbTruth::HZZ;

  return (int)hDecay;

}




void PartonTruthFinder::fillTopTruth(const std::vector<const xAOD::TruthParticle*>& sortedTops,
				     std::set<const xAOD::TruthParticle*>& parts) const{

  std::vector<const xAOD::TruthParticle*> sortedTopsInit;

  for(unsigned int t=0; t<sortedTops.size(); ++t){

    auto part = sortedTops[t];
    sortedTopsInit.push_back(m_truthHistoryHelper->getInitialState(part));

    bool isLeading= (t==0);
    bool isSubleading= (t==1);

    bool isLep=false;
    bool isHad=false;

    bool isT = (part->pdgId()==6);
    bool isTbar = (part->pdgId()==-6);

    std::vector<const xAOD::TruthParticle*> topchildren;topchildren.reserve(2);
    std::vector<const xAOD::TruthParticle*> wchildren;wchildren.reserve(2);
    std::vector<const xAOD::TruthParticle*> topchildrenFinal;topchildrenFinal.reserve(2);
    std::vector<const xAOD::TruthParticle*> wchildrenFinal;wchildrenFinal.reserve(2);
  
    for(unsigned int i = 0; i<part->nChildren(); ++i){
      if(!part->child(i)) continue;
      if(ParticleIDHelper::isQuark(part->child(i)->pdgId())){
	topchildrenFinal.push_back(m_truthHistoryHelper->getFinalState(part->child(i)));
	topchildren.push_back(part->child(i));
      }
      if (ParticleIDHelper::isW(part->child(i)->pdgId())){
	const xAOD::TruthParticle* Wfinal = m_truthHistoryHelper->getFinalState(part->child(i));
	topchildrenFinal.push_back(Wfinal);
	topchildren.push_back(part->child(i));

	for(unsigned int j = 0; j<Wfinal->nChildren(); ++j){
	  if(!Wfinal->child(j)) continue;
	  const xAOD::TruthParticle* wchild = m_truthHistoryHelper->getFinalState(Wfinal->child(j));
	  const xAOD::TruthParticle* wchildini = Wfinal->child(j);
	  //// remove photons
	  if(ParticleIDHelper::isPhoton(wchild->pdgId())) continue;
	  if(ParticleIDHelper::isGluon(wchild->pdgId())) continue;
	  if(ParticleIDHelper::isQuark(wchild->pdgId()))isHad=true;
	  if(ParticleIDHelper::isLep(wchild->pdgId()))isLep=true;
	  wchildren.push_back(wchildini);
	  wchildrenFinal.push_back(wchild);
	}
      }
    }

    sortPt(wchildren);
    sortPt(wchildrenFinal);

    if(wchildren.size()>1){
      setTruthPartDecoration(wchildren[1], TTHbbTruth::subleadingChildFromWTop);
      setTruthPartDecoration(wchildrenFinal[1], TTHbbTruth::subleadingChildFromWTop);
    }
    if(wchildren.size()>0){
      setTruthPartDecoration(wchildren[0], TTHbbTruth::leadingChildFromWTop);
      setTruthPartDecoration(wchildrenFinal[0], TTHbbTruth::leadingChildFromWTop);
    }

    if( (isLep && isHad) || (!isLep && !isHad) ){
      std::cout << "WARNING: PartonTruthFinder::fillTopTruth unkown top decay: isLep="<<isLep<<" and isHad="<<isHad<< " barcode " <<part->barcode()<< std::endl;
    }

    ///save top
    const xAOD::TruthParticle* top = part;
    const xAOD::TruthParticle* topinit = m_truthHistoryHelper->getInitialState(part);

    setTruthPartDecoration(top, TTHbbTruth::finalState);
    setTruthPartDecoration(topinit, TTHbbTruth::initialState);

    if(isLeading) setTruthPartDecoration(top, TTHbbTruth::leadingTop);
    if(isSubleading) setTruthPartDecoration(top, TTHbbTruth::subleadingTop);
    if(isLep){
      setTruthPartDecoration(top, TTHbbTruth::lepTop);
      setTruthPartDecoration(topinit, TTHbbTruth::lepTop);
    }
    if(isHad){
      setTruthPartDecoration(top, TTHbbTruth::hadTop);
      setTruthPartDecoration(topinit, TTHbbTruth::hadTop);
    }
    parts.insert(top);
    parts.insert(topinit);


    ///save top children
    for(auto topchild : topchildren){
      if(isLeading) setTruthPartDecoration(topchild, TTHbbTruth::childFromLeadingTop);
      if(isSubleading) setTruthPartDecoration(topchild, TTHbbTruth::childFromSubleadingTop);
      if(isLep) setTruthPartDecoration(topchild, TTHbbTruth::childFromLepTop);
      if(isHad) setTruthPartDecoration(topchild, TTHbbTruth::childFromHadTop);
      setTruthPartDecoration(topchild, TTHbbTruth::directChildFromTop);
      if(isT) setTruthPartDecoration(topchild, TTHbbTruth::childFromTop);
      if(isTbar) setTruthPartDecoration(topchild, TTHbbTruth::childFromAntiTop);
      setTruthPartDecoration(topchild, TTHbbTruth::initialState);
      parts.insert(topchild);
    }
   ///save top children
    for(auto topchild : topchildrenFinal){
      if(isLeading) setTruthPartDecoration(topchild, TTHbbTruth::childFromLeadingTop);
      if(isSubleading) setTruthPartDecoration(topchild, TTHbbTruth::childFromSubleadingTop);
      if(isLep) setTruthPartDecoration(topchild, TTHbbTruth::childFromLepTop);
      if(isHad) setTruthPartDecoration(topchild, TTHbbTruth::childFromHadTop);
      setTruthPartDecoration(topchild, TTHbbTruth::directChildFromTop);
      if(isT) setTruthPartDecoration(topchild, TTHbbTruth::childFromTop);
      if(isTbar) setTruthPartDecoration(topchild, TTHbbTruth::childFromAntiTop);
      setTruthPartDecoration(topchild, TTHbbTruth::finalState);
      parts.insert(topchild);
    }

    ///save W children
    for(auto topchild : wchildren){
      if(isLeading) setTruthPartDecoration(topchild, TTHbbTruth::childFromLeadingTop);
      if(isSubleading) setTruthPartDecoration(topchild, TTHbbTruth::childFromSubleadingTop);
      if(isLep) setTruthPartDecoration(topchild, TTHbbTruth::childFromLepTop);
      if(isHad) setTruthPartDecoration(topchild, TTHbbTruth::childFromHadTop);
      if(isT) setTruthPartDecoration(topchild, TTHbbTruth::childFromTop);
      if(isTbar) setTruthPartDecoration(topchild, TTHbbTruth::childFromAntiTop);
      setTruthPartDecoration(topchild, TTHbbTruth::initialState);
      parts.insert(topchild);
    }
    for(auto topchild : wchildrenFinal){
      if(isLeading) setTruthPartDecoration(topchild, TTHbbTruth::childFromLeadingTop);
      if(isSubleading) setTruthPartDecoration(topchild, TTHbbTruth::childFromSubleadingTop);
      if(isLep) setTruthPartDecoration(topchild, TTHbbTruth::childFromLepTop);
      if(isHad) setTruthPartDecoration(topchild, TTHbbTruth::childFromHadTop);
      if(isT) setTruthPartDecoration(topchild, TTHbbTruth::childFromTop);
      if(isTbar) setTruthPartDecoration(topchild, TTHbbTruth::childFromAntiTop);
      setTruthPartDecoration(topchild, TTHbbTruth::finalState);
      parts.insert(topchild);
    }


  } /// final tops

  sortPt(sortedTopsInit);

  if(sortedTopsInit.size()>1){
    setTruthPartDecoration(sortedTopsInit[1], TTHbbTruth::subleadingTop);
  }
  if(sortedTopsInit.size()>0){
    setTruthPartDecoration(sortedTopsInit[0], TTHbbTruth::subleadingTop);
  }

}



void PartonTruthFinder::fillVectorBosonsTruth(const xAOD::TruthParticle* part, 
					      std::set<const xAOD::TruthParticle*>& parts) const{
  

  /// fixme (remove w status 3 for ttbar pp6)
  /// save only final, add initial if needed

  auto initial = m_truthHistoryHelper->getInitialState(part);
  if(m_truthHistoryHelper->isfromHiggs(initial) 
     || m_truthHistoryHelper->isfromTop(initial) 
     || m_truthHistoryHelper->isfromTau(initial)) return;

  setTruthPartDecoration(part, TTHbbTruth::vectorBoson);
  setTruthPartDecoration(part, TTHbbTruth::finalState);
  parts.insert(part);

  for(unsigned int j = 0; j<part->nChildren(); ++j){
    if(!part->child(j)) continue;
    const xAOD::TruthParticle* wchild = m_truthHistoryHelper->getFinalState(part->child(j));
    //// remove photons
    if(ParticleIDHelper::isPhoton(wchild->pdgId())) continue;
    if(ParticleIDHelper::isGluon(wchild->pdgId())) continue;
    setTruthPartDecoration(wchild, TTHbbTruth::childFromVectorBoson);
    setTruthPartDecoration(wchild, TTHbbTruth::finalState);
    parts.insert(wchild);
  }

}


void PartonTruthFinder::fillBSMHiggsMatching(const xAOD::TruthParticle* part, 
					     std::set<const xAOD::TruthParticle*>& parts) const{

  /// addded protection, you might want to add a warning 
  if(part->nChildren()<2) return;

  const xAOD::TruthParticle* fa1 = m_truthHistoryHelper->getFinalState(part->child(0));//pseudoscalar 1
  const xAOD::TruthParticle* fa2 = m_truthHistoryHelper->getFinalState(part->child(1));//pseudoscalar 2

  const xAOD::TruthParticle* a1(0);
  const xAOD::TruthParticle* a2(0);

  if(fa1->pt() > fa2->pt()){
    a1 = fa1;
    a2 = fa2;
  } 
  else{
    a1 = fa2;
    a2 = fa1;
  }

  const xAOD::TruthParticle* fb11 = m_truthHistoryHelper->getFinalState(a1->child(0));//b
  const xAOD::TruthParticle* fb12 = m_truthHistoryHelper->getFinalState(a1->child(1));//bbar
  const xAOD::TruthParticle* fb21 = m_truthHistoryHelper->getFinalState(a2->child(0));//b
  const xAOD::TruthParticle* fb22 = m_truthHistoryHelper->getFinalState(a2->child(1));//bbar

  const xAOD::TruthParticle* b11(0);
  const xAOD::TruthParticle* b12(0);
  const xAOD::TruthParticle* b21(0);
  const xAOD::TruthParticle* b22(0);

  if(fb11->pt() > fb12->pt()){
    b11 = fb11;
    b12 = fb12;
  }else{
    b11 = fb12;
    b12 = fb11;
  }

  if(fb21->pt() > fb22->pt()){
    b21 = fb21;
    b22 = fb22;
  }else{
    b21 = fb22;
    b22 = fb21;
  }

  setTruthPartDecoration(part, TTHbbTruth::BSMHiggs);
  setTruthPartDecoration(a1, TTHbbTruth::leadingAFromHiggs);
  setTruthPartDecoration(a2, TTHbbTruth::subleadingAFromHiggs);
  setTruthPartDecoration(b11, TTHbbTruth::leadingChildFromLeadingA);
  setTruthPartDecoration(b12, TTHbbTruth::subleadingChildFromLeadingA);
  setTruthPartDecoration(b21, TTHbbTruth::leadingChildFromSubleadingA);
  setTruthPartDecoration(b22, TTHbbTruth::subleadingChildFromSubleadingA);
  

  setTruthPartDecoration(part, TTHbbTruth::finalState);
  setTruthPartDecoration(a1, TTHbbTruth::finalState);
  setTruthPartDecoration(a2, TTHbbTruth::finalState);
  setTruthPartDecoration(b11, TTHbbTruth::finalState);
  setTruthPartDecoration(b12, TTHbbTruth::finalState);
  setTruthPartDecoration(b21, TTHbbTruth::finalState);
  setTruthPartDecoration(b22, TTHbbTruth::finalState);

  parts.insert(part);
  parts.insert(a1);
  parts.insert(a2);
  parts.insert(b11);
  parts.insert(b12);
  parts.insert(b21);
  parts.insert(b22);

}

void PartonTruthFinder::fillChargedHiggsMatching(const xAOD::TruthParticle* part,
						 std::set<const xAOD::TruthParticle*>& parts) const{

  const xAOD::TruthParticle* HplusInit = m_truthHistoryHelper->getInitialState(part);

  setTruthPartDecoration(part, TTHbbTruth::ChargedHiggs);
  setTruthPartDecoration(HplusInit, TTHbbTruth::ChargedHiggs);
  setTruthPartDecoration(part, TTHbbTruth::finalState);
  setTruthPartDecoration(HplusInit, TTHbbTruth::initialState);

  parts.insert(part);
  parts.insert(HplusInit);

  for(unsigned int i = 0; i<part->nChildren(); ++i){
    auto hchild = part->child(i);
    if(!hchild) continue;

    if(part->nChildren()!=2){
      if(ParticleIDHelper::isGluon(hchild->pdgId())) continue;
      if(ParticleIDHelper::isPhoton(hchild->pdgId())) continue;
    }

    auto hchildfinal = m_truthHistoryHelper->getFinalState(hchild);

    if(ParticleIDHelper::isbq(hchild->pdgId())){
      setTruthPartDecoration(hchild, TTHbbTruth::childFromChargedHiggs);
      setTruthPartDecoration(hchildfinal, TTHbbTruth::childFromChargedHiggs);
      setTruthPartDecoration(hchild, TTHbbTruth::initialState);
      setTruthPartDecoration(hchildfinal, TTHbbTruth::finalState);
      parts.insert(hchild);
      parts.insert(hchildfinal);
    }

    if(ParticleIDHelper::isTop(hchild->pdgId())){
      setTruthPartDecoration(hchild, TTHbbTruth::childFromChargedHiggs);
      setTruthPartDecoration(hchildfinal, TTHbbTruth::childFromChargedHiggs);
    }

  }

}

void PartonTruthFinder::fillAssociatedBwithChargedHiggs(const xAOD::TruthParticle* part, 
							std::set<const xAOD::TruthParticle*>& parts) const{

  const xAOD::TruthParticle* b0 = m_truthHistoryHelper->getFinalState(part);
  setTruthPartDecoration(b0, TTHbbTruth::bFromCollide);
  setTruthPartDecoration(part, TTHbbTruth::bFromCollide);
  setTruthPartDecoration(b0, TTHbbTruth::finalState);
  setTruthPartDecoration(part, TTHbbTruth::initialState);

  parts.insert(b0);
  parts.insert(part);
}
