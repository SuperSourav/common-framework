/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <iostream>
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMath.h"

#include "MVAVariables/MVAVariables.h"
#include "MVAVariables/JetOrderingTool.h"
//#include "MVAVariables/MVAUtils.h"

MVAVariables::MVAVariables() :  m_btagCheckString("isbtagged_70"), b_pairsCalculated(false), b_tensorsCalculated(false)
{
  DefinePairs();
  DefineTensorVariables();
}

MVAVariables::~MVAVariables(){
}

void MVAVariables::setBtagging(std::string wp){
  m_btagCheckString = "isbtagged_" + wp;
}

void MVAVariables::setBtagString(std::string wp){
  m_btagCheckString = wp;
}

void MVAVariables::initialiseEvent(TTHbb::Event* event){

  //m_event = event;
  b_pairsCalculated = false;
  b_tensorsCalculated = false;

  cleanVectors();
    
  for(auto jPtr : event->m_jets){
    jetVecs.push_back(jPtr);
    
    if(jPtr->charVariable(m_btagCheckString)){
      bjetVecs.push_back(jPtr);
    }
    else{
      ujetVecs.push_back(jPtr);
    }
  }//jet and bjet four momenta

  for(auto lep : event->m_leptons){
    lepVecs.push_back(lep);
  }// lepton four momenta


  allVecs.insert(allVecs.end(), jetVecs.begin(), jetVecs.end());
  allVecs.insert(allVecs.end(), lepVecs.begin(), lepVecs.end());

  met_met = event->met_met;
  met_phi = event->met_phi;

  SetPairs();

  //SetTensors();  
  //EvaluatePairs();
  //EvaluateTensors();
}

void MVAVariables::initialiseEvent(std::vector< std::shared_ptr<TTHbb::Particle> >& jets,
				   std::vector< std::shared_ptr<TTHbb::Particle> >& bjets,
				   std::vector< std::shared_ptr<TTHbb::Particle> >& ujets,
				   std::vector< std::shared_ptr<TTHbb::Particle> >& leps,
				   std::vector< std::shared_ptr<TTHbb::Particle> >& all,
				   float metmet, float metphi){

  b_pairsCalculated = false;
  b_tensorsCalculated = false;
  cleanVectors();
  
  jetVecs  = jets;
  bjetVecs = bjets;
  ujetVecs = ujets;
  lepVecs  = leps;  
  allVecs  = all;

  met_met = metmet;
  met_phi = metphi;

  SetPairs();

}

void MVAVariables::cleanVectors(){

  jetVecs.clear();
  bjetVecs.clear();
  ujetVecs.clear();
  lepVecs.clear();  
  allVecs.clear();

  mapping.clear();
  eventshape.clear();
  vectors.clear();
  collections.clear();  

  tripVecs.clear();
  quadVecs.clear();

}


//------//

PairedSystem MVAVariables::getEntry(pairing p, variable v){
  if(!b_pairsCalculated){
    EvaluatePairs();
  }  
  return mapping.at(entry(p, v));
}

float MVAVariables::getShape(const collection& c, const shapefunc& s){
  if(!b_tensorsCalculated){
    SetTensors();
    EvaluateTensors();
  }  
  return eventshape.at(shape(c,s));
}

PairedSystem MVAVariables::getDilepton(){
  if(lepVecs.size() < 2){
    TLorentzVector tempVec1 = TLorentzVector();
    TLorentzVector tempVec2 = TLorentzVector();
    tempVec1.SetPtEtaPhiM(-1., -9., -5., -1.);
    tempVec2.SetPtEtaPhiM(0., 0., 0., 0.);
    return PairedSystem(tempVec1, tempVec2);
  }
  return PairedSystem(lepVecs[0], lepVecs[1]);
}

PairedSystem MVAVariables::getTriplet(collection c, variable v){
  
  if(tripVecs.count(c) == 0){
    makeTriplets(c);
  }
  auto func = functions.at(v);
  return func(tripVecs.at(c));
}

PairedSystem MVAVariables::getQuadruplet(collection c, variable v){
  
  if(quadVecs.count(c) == 0){
    makeQuadruplets(c);
  }
  auto func = functions.at(v);
  return func(quadVecs.at(c));
}

//------//

int MVAVariables::nbTag(){
  //auto func = [&](const xAOD::Jet* j){ return MVAUtils::isBjet(j); }; 
  //return std::count_if(m_event->m_jets.begin(), m_event->m_jets.end(), MVAUtils::isBjet); 
  //return std::count_if(m_event->m_jets.begin(), m_event->m_jets.end(), func); 
  //// is this correct?
  return bjetVecs.size();
}

int MVAVariables::nJetsAbovePt(float Pt){
  return std::count_if(jetVecs.begin(), jetVecs.end(),
		       [&](std::shared_ptr<TTHbb::Particle> j){ return j->Pt() > Pt*1000.0;} );
}

//int MVAVariables::nTrackJetsAbovePt(float Pt){
//  return std::count_if(m_event->m_trackJets.begin(), m_event->m_trackJets.end(),
//		       [&](const xAOD::Jet* j){ return j->pt() > Pt*1000.0;} );
//}

float MVAVariables::HT(collection c){
  float HT = 0;
  auto func = [&](std::shared_ptr<TTHbb::Particle> part){HT += part->Pt();};

  if(c == collection::all)
    std::for_each(std::begin(allVecs), std::end(allVecs), func);

  if( c == collection::jets)
    std::for_each(std::begin(jetVecs), std::end(jetVecs), func);

  if(c == collection::bjets)
    std::for_each(std::begin(bjetVecs), std::end(bjetVecs), func);

  if(c == collection::ujets)
    std::for_each(std::begin(ujetVecs), std::end(ujetVecs), func);

  
  return HT;
}

float MVAVariables::Meff(collection c){
  return (this->HT(c) + met_met);
}

float MVAVariables::Centrality(collection c){
  float E = 0;
  auto func = [&](std::shared_ptr<TTHbb::Particle> part){E += part->E();};

  if(c == collection::all)
    std::for_each(std::begin(allVecs), std::end(allVecs), func);

  if( c == collection::jets)
    std::for_each(std::begin(jetVecs), std::end(jetVecs), func);

  if(c == collection::bjets)
    std::for_each(std::begin(bjetVecs), std::end(bjetVecs), func);

  if(c == collection::ujets)
    std::for_each(std::begin(ujetVecs), std::end(ujetVecs), func);

  if(E == 0) return 0;
  
  return HT(c) / E;
}


float MVAVariables::MCT(){
  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYD3PDSnippets#MCT
  // http://arxiv.org/pdf/0910.0174v3.pdf

  if (lepVecs.size() == 0)    return -999.;
  if (jetVecs.size() < 3)          return -999.;

  // Assume that the 2 first b-jets in MV2c20 are the real b's from ttbar.
  //TLorentzVector v1 = this->getPtOrderedLepton(0)->p4();
  //TLorentzVector v2 = this->getMV2cXXOrderedJet(2)->p4();
  JetOrderingTool::sortPt(lepVecs);
  std::shared_ptr<TTHbb::Particle> lep = lepVecs.at(0);
  std::shared_ptr<TTHbb::Particle> jet = getMV2OrderedJetContainer().at(2);

  float mct = pow( (lep->Mt() + jet->Mt()), 2) - (lep->p4() - jet->p4()).Perp2();
  return sqrt(fabs(mct));

  //float mct = (v1.Mt() + v2.Mt())*(v1.Mt() + v2.Mt()) - (v1-v2).Perp2();
  //return (mct >= 0.) ? sqrt(mct) : sqrt(-mct);
}

int MVAVariables::nHiggsCandidatesMassWindow(pairing p, int M){
  return std::count_if(std::begin(vectors.at(p)), std::end(vectors.at(p)),
		       [&](PairedSystem j){ return fabs(j.M() - higgsMass) < M*1000.0;});
}

int MVAVariables::nWCandidatesMassWindow(pairing p, int M){
  return std::count_if(std::begin(vectors.at(p)), std::end(vectors.at(p)),
		       [&](PairedSystem j){ return fabs(j.M() - Wmass) < M*1000.0;});
}

float MVAVariables::HhadT_frac_met(){
  float HfracT = 0;

  TVector2 m;//(m_event->m_met->mpx(), m_event->m_met->mpy());
  m.SetMagPhi(met_met, met_phi);

  auto func = [&](std::shared_ptr<TTHbb::Particle> part){
    TVector2 j;
    j.SetMagPhi(part->Pt(), part->Phi());
    if (fabs(j.DeltaPhi(m)) < TMath::Pi()/2.){
      HfracT += part->Pt();
    }
  };

  std::for_each(std::begin(jetVecs), std::end(jetVecs), func);

  HfracT /= this->HT(collection::jets);
  return HfracT;
}

float MVAVariables::HhadT_frac_lep(){
  float HfracT = 0;
  //TLorentzVector lep(this->getPtOrderedLepton(0)->p4());
  /// is this correct?
  //std::shared_ptr<TTHbb::Lepton> lep = m_event->m_leptons.at(0);
  JetOrderingTool::sortPt(lepVecs);
  std::shared_ptr<TTHbb::Particle> lep = lepVecs.at(0);

  auto func = [&](std::shared_ptr<TTHbb::Particle> part){
    if (fabs(part->DeltaPhi(lep->p4()))<TMath::Pi()/2.){
      HfracT += part->Pt();
    }
  };

  std::for_each(std::begin(jetVecs), std::end(jetVecs), func);
  HfracT /= this->HT(collection::jets);
  return HfracT;
}

std::shared_ptr<TTHbb::Particle> MVAVariables::getPtOrderedJet(collection c, unsigned int i){ 
  if(i <= 0){
    std::cout << "ERROR: Trying to get a jet lower than 1st, i = " << i << ". i must be at least 1, corresponding to the leading jet." <<std::endl;
    exit(42);
  }
  if(c == collection::jets){
    if(jetVecs.size() < i) return std::make_shared<TTHbb::Particle>();
    JetOrderingTool::sortPt(jetVecs);
    return std::dynamic_pointer_cast<TTHbb::Particle>( jetVecs.at(i-1) );
  }
  else if(c == collection::bjets){
    if(bjetVecs.size() < i) return std::make_shared<TTHbb::Particle>();
    JetOrderingTool::sortPt(bjetVecs);
    return std::dynamic_pointer_cast<TTHbb::Particle>( bjetVecs.at(i-1) );
  }
  else if(c == collection::ujets){
    if(ujetVecs.size() < i) return std::make_shared<TTHbb::Particle>();
    JetOrderingTool::sortPt(ujetVecs);
    return std::dynamic_pointer_cast<TTHbb::Particle>( ujetVecs.at(i-1) );
  }
  else{
    std::cout << "WARNING in MVAVariables: Getting using a none jet collection with MVAVariables::getPtOrderedJet" << std::endl;
    return std::make_shared<TTHbb::Particle>();
  }
}

std::shared_ptr<TTHbb::Particle> MVAVariables::getPtOrderedLepton(unsigned int i){
  if(i <= 0){
    std::cout << "ERROR: Trying to get a lepton lower than 1st, i = " << i << ". i must be at least 1, corresponding to the leading lepton." <<std::endl;
    exit(42);
  }
  JetOrderingTool::sortPt(lepVecs);
  return std::dynamic_pointer_cast<TTHbb::Particle>( lepVecs.at(i-1) );
}

std::vector< std::shared_ptr<TTHbb::Particle> > MVAVariables::getMV2OrderedJetContainer(){
  std::vector< std::shared_ptr<TTHbb::Particle> > jetCopy = jetVecs;

  std::sort(std::begin(jetCopy), std::end(jetCopy), 
	    [](std::shared_ptr<TTHbb::Particle> j1, std::shared_ptr<TTHbb::Particle> j2){
	      return j1->floatVariable("mv2") > j2->floatVariable("mv2");
	    });

  return jetCopy;
}

//----------------------------------------------//
//----------Protected member functions----------//

void MVAVariables::SetPairs(){
  //std::vector<const xAOD::Jet*> jetList(m_event->m_jets.begin(), m_event->m_jets.end());
  //creating vector of paired systems for each pairing
  std::vector<PairedSystem> jjVecs;
  std::vector<PairedSystem> bjVecs;
  std::vector<PairedSystem> buVecs;
  std::vector<PairedSystem> bbVecs;
  std::vector<PairedSystem> uuVecs;
  std::vector<PairedSystem> jlVecs;
  std::vector<PairedSystem> blVecs;
  std::vector<PairedSystem> ulVecs;
  
  // std::cout << "jetvec size = " << jetVecs.size() << std::endl;
  // std::cout << "bjetvec size = " << bjetVecs.size() << std::endl;
  // std::cout << "ujetvec size = " << ujetVecs.size() << std::endl;
  // std::cout << "lepvec size = " << lepVecs.size() << std::endl;

  /// Check if there are at least two jets to form pairs
  if(jetVecs.size() >= 2){
    /// Build all b*Vecs
    for(unsigned int i=0; i<bjetVecs.size(); ++i){
      std::shared_ptr<TTHbb::Particle> bjet1 = bjetVecs.at(i);
      /// bbVecs
      if(bjetVecs.size() >= 2){
	if(i != bjetVecs.size() -1){
	  for(unsigned int j=i+1; j<bjetVecs.size(); ++j){
	    std::shared_ptr<TTHbb::Particle> bjet2 = bjetVecs.at(j);
	    bbVecs.push_back(PairedSystem(bjet1,bjet2));
	    bjVecs.push_back(PairedSystem(bjet1,bjet2));
	    jjVecs.push_back(PairedSystem(bjet1,bjet2));
	  }
	}
      }
      /// bjVecs
      if(ujetVecs.size() != 0){
	for(unsigned int j=0; j<ujetVecs.size(); ++j){
	  std::shared_ptr<TTHbb::Particle> ujet = ujetVecs.at(j);
	  buVecs.push_back(PairedSystem(bjet1,ujet));
	  bjVecs.push_back(PairedSystem(bjet1,ujet));
	  jjVecs.push_back(PairedSystem(bjet1,ujet));
	}
      }
      /// blVecs
      if(lepVecs.size() != 0){
	for(unsigned int j=0; j<lepVecs.size(); ++j){
	  std::shared_ptr<TTHbb::Particle> lep = lepVecs.at(j);
	  blVecs.push_back(PairedSystem(bjet1,lep));
	  jlVecs.push_back(PairedSystem(bjet1,lep));
	}
      }
    }
    /// Build uuVecs and add remaining jjVecs and jlVecs
    for(unsigned int i=0; i<ujetVecs.size(); ++i){
      std::shared_ptr<TTHbb::Particle> ujet1 = ujetVecs.at(i);
      /// uuVecs
      if(ujetVecs.size() >= 2){
	if(i != (ujetVecs.size()-1)){
	  for(unsigned int j=i+1; j<ujetVecs.size(); ++j){
	    std::shared_ptr<TTHbb::Particle> ujet2 = ujetVecs.at(j);
	    uuVecs.push_back(PairedSystem(ujet1,ujet2));
	    jjVecs.push_back(PairedSystem(ujet1,ujet2));
	  }
	}
      }
      /// remaining jlVecs
      if(lepVecs.size() != 0){
	for(unsigned int j=0; j<lepVecs.size(); ++j){
	  std::shared_ptr<TTHbb::Particle> lep = lepVecs.at(j);
	  jlVecs.push_back(PairedSystem(ujet1,lep));
	  ulVecs.push_back(PairedSystem(ujet1,lep));
	}
      }      
    }
  }

  if(jjVecs.empty())    jjVecs.push_back(PairedSystem());
  if(bjVecs.empty())    bjVecs.push_back(PairedSystem());
  if(buVecs.empty())    buVecs.push_back(PairedSystem());
  if(bbVecs.empty())    bbVecs.push_back(PairedSystem());
  if(uuVecs.empty())    uuVecs.push_back(PairedSystem());
  if(jlVecs.empty())    jlVecs.push_back(PairedSystem());
  if(blVecs.empty())    blVecs.push_back(PairedSystem());
  if(ulVecs.empty())    ulVecs.push_back(PairedSystem());

  vectors.insert(pvector(pairing::jj, jjVecs));
  vectors.insert(pvector(pairing::bj, bjVecs));
  vectors.insert(pvector(pairing::bb, bbVecs));
  vectors.insert(pvector(pairing::bu, buVecs));
  vectors.insert(pvector(pairing::uu, uuVecs));
  vectors.insert(pvector(pairing::jl, jlVecs));
  vectors.insert(pvector(pairing::bl, blVecs));
  vectors.insert(pvector(pairing::bu, buVecs));
}

void MVAVariables::DefinePairs(){

  auto f_maxPt = [&](std::vector<PairedSystem> vecs) {
    sort( std::begin(vecs), std::end(vecs),
	  [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.Pt() > vec2.Pt(); } );
    return vecs.at(0);
  };

  auto f_minPt = [&](std::vector<PairedSystem> vecs) {
    sort( std::begin(vecs), std::end(vecs),
	  [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.Pt() < vec2.Pt(); } );
    return vecs.at(0);
  };
  
  auto f_maxdEta = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.DeltaEta() > vec2.DeltaEta(); } );
    return vecs.at(0);
  };
  
  auto f_mindEta = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.DeltaEta() < vec2.DeltaEta(); } );
    return vecs.at(0);
  };

  auto f_maxdPhi = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.DeltaPhi() > vec2.DeltaPhi(); } );
    return vecs.at(0);
  };
  
  auto f_mindPhi = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.DeltaPhi() < vec2.DeltaPhi(); } );
    return vecs.at(0);
  };
  
  auto f_maxdR = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.DeltaR() > vec2.DeltaR(); } );
    return vecs.at(0);
  };
    
  auto f_mindR = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return vec1.DeltaR() < vec2.DeltaR(); } );
    return vecs.at(0);
  };
    
  auto f_maxM = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) {return vec1.M() > vec2.M(); } );
    return vecs.at(0);
  };

  auto f_minM = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) {return vec1.M() < vec2.M(); } );
    return vecs.at(0);
  };

  auto f_HiggsMass = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return fabs(vec1.M() - higgsMass) < fabs(vec2.M() - higgsMass); });
    return vecs.at(0);
  };

  auto f_Wmass = [&](std::vector<PairedSystem> vecs) {
    sort(std::begin(vecs), std::end(vecs),
	 [](const PairedSystem& vec1, const PairedSystem& vec2) { return fabs(vec1.M() - Wmass) < fabs(vec2.M() - Wmass); });
    return vecs.at(0);
  };

  auto f_avg = [&](std::vector<PairedSystem> vecs) {
    return PairedSystem(vecs);
  };
  
  functions.insert(vpair(variable::MaxPt,       f_maxPt));
  functions.insert(vpair(variable::MinPt,       f_minPt));
  functions.insert(vpair(variable::MaxdEta,     f_maxdEta));
  functions.insert(vpair(variable::MindEta,     f_mindEta));
  functions.insert(vpair(variable::MaxdPhi,     f_maxdPhi));
  functions.insert(vpair(variable::MindPhi,     f_mindPhi));
  functions.insert(vpair(variable::MaxdR,       f_maxdR));
  functions.insert(vpair(variable::MindR,       f_mindR));
  functions.insert(vpair(variable::MaxM,        f_maxM));
  functions.insert(vpair(variable::MinM,        f_minM));
  functions.insert(vpair(variable::HiggsMass,   f_HiggsMass));
  functions.insert(vpair(variable::Wmass,       f_Wmass));
  functions.insert(vpair(variable::Avg,         f_avg));
 
}

void MVAVariables::EvaluatePairs(){
  for (auto& p : vectors) {
    for (auto& f : functions) {
      mapping.insert(std::pair<entry, PairedSystem>(entry(p.first, f.first), f.second(p.second)));
    }
  }
  b_pairsCalculated = true;
}

void MVAVariables::SetTensors(){

  std::vector< std::vector<float> > eigenvals;

  eigenvalues evAll;
  eigenvalues evJet;
  eigenvalues evBjet;
  eigenvalues evUjet;
  
  eigenvals = CalculateEigenvals(allVecs);
  evAll.insert(eigenpair(tensor::ATensor, eigenvals[0]));
  evAll.insert(eigenpair(tensor::TTensor, eigenvals[1]));
  evAll.insert(eigenpair(tensor::OTensor, eigenvals[2]));

  eigenvals = CalculateEigenvals(jetVecs);
  evJet.insert(eigenpair(tensor::ATensor, eigenvals[0]));
  evJet.insert(eigenpair(tensor::TTensor, eigenvals[1]));
  evJet.insert(eigenpair(tensor::OTensor, eigenvals[2]));
  
  eigenvals = CalculateEigenvals(bjetVecs);
  evBjet.insert(eigenpair(tensor::ATensor, eigenvals[0]));
  evBjet.insert(eigenpair(tensor::TTensor, eigenvals[1]));
  evBjet.insert(eigenpair(tensor::OTensor, eigenvals[2]));
  
  eigenvals = CalculateEigenvals(ujetVecs);
  evUjet.insert(eigenpair(tensor::ATensor, eigenvals[0]));
  evUjet.insert(eigenpair(tensor::TTensor, eigenvals[1]));
  evUjet.insert(eigenpair(tensor::OTensor, eigenvals[2]));

  collections.insert(cmap(collection::all,   evAll));
  collections.insert(cmap(collection::jets,  evJet));
  collections.insert(cmap(collection::bjets, evBjet));
  collections.insert(cmap(collection::ujets, evUjet));
}

void MVAVariables::DefineTensorVariables(){

  auto f_Aplanarity = [&](eigenvalues ev){
    return 1.5 * ev.at(tensor::ATensor)[2];
  };

  auto f_Aplanority = [&](eigenvalues ev){
    return 1.5 * ev.at(tensor::OTensor)[2];
  };

  auto f_Sphericity = [&](eigenvalues ev){
    return 1.5 * (ev.at(tensor::ATensor)[1] + ev.at(tensor::ATensor)[2]);
  };

  auto f_Spherocity = [&](eigenvalues ev){
    return 1.5 * (ev.at(tensor::OTensor)[1] + ev.at(tensor::OTensor)[2]);
  };

  auto f_SphericityT = [&](eigenvalues ev){
    return 2.0 * ev.at(tensor::TTensor)[1] / (ev.at(tensor::TTensor)[0] + ev.at(tensor::TTensor)[1]);
  };

  auto f_Planarity = [&](eigenvalues ev){
    return ev.at(tensor::ATensor)[1] - ev.at(tensor::ATensor)[2];
  };

  auto f_Variable_C = [&](eigenvalues ev){
    return 3.0 * (ev.at(tensor::ATensor)[0]*ev.at(tensor::ATensor)[1]
		  + ev.at(tensor::ATensor)[0]*ev.at(tensor::ATensor)[2]
		  + ev.at(tensor::ATensor)[1]*ev.at(tensor::ATensor)[2]);
  };

  auto f_Variable_D = [&](eigenvalues ev){
    return 27.0 * ev.at(tensor::ATensor)[0] * ev.at(tensor::ATensor)[1] * ev.at(tensor::ATensor)[2];
  };

  auto f_Circularity = [&](eigenvalues ev){
    if( (ev.at(tensor::ATensor)[0] + ev.at(tensor::ATensor)[1]) == 0)
      return 0.0;
    else
      return 2.0 * ev.at(tensor::ATensor)[1] /
	(ev.at(tensor::ATensor)[0] + ev.at(tensor::ATensor)[1] );
  };

  auto f_PlanarFlow = [&](eigenvalues ev){
    if( (ev.at(tensor::ATensor)[0] + ev.at(tensor::ATensor)[1])
	* (ev.at(tensor::ATensor)[0] + ev.at(tensor::ATensor)[1]) == 0)
      return 0.0;
    else
      return 4.0 * (ev.at(tensor::ATensor)[0] * ev.at(tensor::ATensor)[1])
	/ ((ev.at(tensor::ATensor)[0] + ev.at(tensor::ATensor)[1])
	   * (ev.at(tensor::ATensor)[0] + ev.at(tensor::ATensor)[1]));
  };
  
  shapefunctions.insert(spair(shapefunc::Aplanarity,   f_Aplanarity));
  shapefunctions.insert(spair(shapefunc::Aplanority,   f_Aplanority));
  shapefunctions.insert(spair(shapefunc::Sphericity,   f_Sphericity));
  shapefunctions.insert(spair(shapefunc::Spherocity,   f_Spherocity));
  shapefunctions.insert(spair(shapefunc::SphericityT,  f_SphericityT));
  shapefunctions.insert(spair(shapefunc::Planarity,    f_Planarity));
  shapefunctions.insert(spair(shapefunc::Variable_C,   f_Variable_C));
  shapefunctions.insert(spair(shapefunc::Variable_D,   f_Variable_D));
  shapefunctions.insert(spair(shapefunc::Circularity,  f_Circularity));
  shapefunctions.insert(spair(shapefunc::PlanarFlow,   f_PlanarFlow));

}

void MVAVariables::EvaluateTensors(){  
  for(auto &c : collections){
    for(auto &f : shapefunctions){
      eventshape.insert(std::pair<shape,float>(shape(c.first, f.first), f.second(c.second)));
    }
  } 
  b_tensorsCalculated = true;
}

std::vector< std::vector<float> > MVAVariables::CalculateEigenvals(std::vector< std::shared_ptr<TTHbb::Particle> >& Collection){
  
  TMatrixDSym MomentumTensor(3);
  TMatrixDSym MomentumTensorT(2);
  TMatrixDSym MomentumTensorO(3);

  double Sxx=0, Sxy=0, Sxz=0, Syy=0, Syz=0, Szz=0, normal=0;
  double Oxx=0, Oxy=0, Oxz=0, Oyy=0, Oyz=0, Ozz=0, Onormal=0;

  for(auto part : Collection){
    Sxx += part->Px() * part->Px();
    Sxy += part->Px() * part->Py();
    Sxz += part->Px() * part->Pz();
    Syy += part->Py() * part->Py();
    Syz += part->Py() * part->Pz();
    Szz += part->Pz() * part->Pz();
    normal += part->P() * part->P();
    
    Oxx += (part->Px() * part->Px())/part->P();
    Oxy += (part->Px() * part->Py())/part->P();
    Oxz += (part->Px() * part->Pz())/part->P();
    Oyy += (part->Py() * part->Py())/part->P();
    Oyz += (part->Py() * part->Pz())/part->P();
    Ozz += (part->Pz() * part->Pz())/part->P();
    Onormal += part->P();    
  }

  MomentumTensor(0, 0) = Sxx/normal;
  MomentumTensor(0, 1) = Sxy/normal;
  MomentumTensor(0, 2) = Sxz/normal;
  MomentumTensor(1, 0) = MomentumTensor(0, 1);
  MomentumTensor(1, 1) = Syy/normal;
  MomentumTensor(1, 2) = Syz/normal;
  MomentumTensor(2, 0) = MomentumTensor(0, 2);
  MomentumTensor(2, 1) = MomentumTensor(1, 2);
  MomentumTensor(2, 2) = Szz/normal;

  MomentumTensorT(0, 0) = MomentumTensor(0, 0);
  MomentumTensorT(0, 1) = MomentumTensor(0, 1);
  MomentumTensorT(1, 1) = MomentumTensor(1, 1);
  MomentumTensorT(1, 0) = MomentumTensor(1, 0);

  MomentumTensorO(0, 0) = Oxx/Onormal;
  MomentumTensorO(0, 1) = Oxy/Onormal;
  MomentumTensorO(0, 2) = Oxz/Onormal;
  MomentumTensorO(1, 0) = MomentumTensorO(0, 1);
  MomentumTensorO(1, 1) = Oyy/Onormal;
  MomentumTensorO(1, 2) = Oyz/Onormal;
  MomentumTensorO(2, 0) = MomentumTensorO(0, 2);
  MomentumTensorO(2, 1) = MomentumTensorO(1, 2);
  MomentumTensorO(2, 2) = Ozz/Onormal;

  std::vector< std::vector<float> > m_eigenvalVec;

  std::vector<float> m_eigenvalues;
  std::vector<float> m_eigenvaluesT;
  std::vector<float> m_eigenvaluesO;
  
  TMatrixDSymEigen Eigenvalues(MomentumTensor);
  TVectorD eigenVec = Eigenvalues.GetEigenValues();
  for(int i=0; i<3; i++)
    m_eigenvalues.push_back(eigenVec(i));

  TMatrixDSymEigen EigenvaluesT(MomentumTensorT);
  TVectorD eigenVecT = EigenvaluesT.GetEigenValues();
  for(int i=0; i<2; i++)
    m_eigenvaluesT.push_back(eigenVecT(i));

  TMatrixDSymEigen EigenvaluesO(MomentumTensorO);
  TVectorD eigenVecO = EigenvaluesO.GetEigenValues();
  for(int i=0; i<3; i++)
    m_eigenvaluesO.push_back(eigenVecO(i));

  m_eigenvalVec.push_back(m_eigenvalues);
  m_eigenvalVec.push_back(m_eigenvaluesT);
  m_eigenvalVec.push_back(m_eigenvaluesO);

  return m_eigenvalVec;
}

float MVAVariables::FoxWolfram(int order, bool transv, std::vector< std::shared_ptr<TTHbb::Particle> >& particles){
  const int maxFWOrder = 5;
  
  float H[maxFWOrder] = {};
  float P[maxFWOrder] = {};
  
  float sumE = 0, sumPt = 0, norm = 0;
    
  auto getx = [&](std::shared_ptr<TTHbb::Particle> lv1, std::shared_ptr<TTHbb::Particle> lv2){
    if(transv)  
      return cos( lv1->DeltaPhi(lv2->p4()) );
    else
      return cos( lv1->Angle(lv2->Vect()) );
  };
   
  for(unsigned int i = 0; i < particles.size(); i++){
    sumE += particles[i]->E();
    sumPt += particles[i]->Pt();
    
    for(unsigned int j = i/*+1*/; j < particles.size(); j++){
      float x = getx(particles[i],particles[j]);
        
      P[0] = 1;
      P[1] = x;
      P[2] = 0.5*(3*x*x-1);
      P[3] = 0.5*(5*x*x*x-3*x);
      P[4] = 0.125*(35*x*x*x*x-30*x*x+3);

      for(int k = 0; k<5; k++){
	//diagonal terms
	if(j==i)
	  H[k] += pow(fabs(particles[i]->P()), 2) * P[k];
	//off diagonal terms
	else
	  H[k] += 2*fabs(particles[i]->P() * particles[j]->P()) * P[k];
      }
    }
  }
  
  if(transv){
    if(sumPt*sumPt == 0) return 0;
    norm = sumPt*sumPt;
  }
  if(!transv){
    if(sumE*sumE == 0) return 0;
    norm = sumE*sumE;
  }
  
  return H[order] / norm;
}

float MVAVariables::FoxWolfram(int order, bool tranvs, collection c){
  if(c == collection::all) return FoxWolfram(order, tranvs, allVecs);
  else if(c == collection::jets) return FoxWolfram(order, tranvs, jetVecs);
  else if(c == collection::bjets) return FoxWolfram(order, tranvs, bjetVecs);
  else if(c == collection::ujets) return FoxWolfram(order, tranvs, ujetVecs);
  else return -999;
}

std::pair<float,TVector3> MVAVariables::setThrust(std::vector< std::shared_ptr<TTHbb::Particle> >& particles){
  
  TVector3 axis;
  axis.SetXYZ(0.0,0.0,0.0);
  float SumP = 0;
  float SumNP = 0;

  float epsilon = 1e-9;
  int maxsteps = 20;
  
  axis = particles[0]->Vect();
  axis.SetMag( 1.0 );

  int step = 0;
  float diff = 1;
  do{
    TVector3 tmp_axis = axis;
    //for(auto &p : particles){
    for(unsigned int i = 0; i< particles.size(); i++){
      axis += particles[i]->Vect() * TMath::Sign ( 1.0, particles[i]->Vect().Dot ( tmp_axis ) );
    }
    axis.SetMag( 1.0 );
    diff = tmp_axis.Angle(axis);
    step++;
  }while(step < maxsteps && diff > epsilon);

  //for(auto &p : particles){
  for(unsigned int i = 0; i< particles.size(); i++){
    SumP += particles[i]->Vect().Mag();
    SumNP += TMath::Abs ( particles[i]->Vect().Dot( axis ) );
  }
  
  float thrust = (SumP != 0) ? SumNP/SumP : 0;
  std::pair<float,TVector3> thrustvec;
  thrustvec.first = thrust;
  thrustvec.second = axis;

  return thrustvec;
}

std::pair<float,TVector3> MVAVariables::setThrust(collection c){
  if(c == collection::all) return setThrust(allVecs);
  else if(c == collection::jets) return setThrust(jetVecs);
  else if(c == collection::bjets) return setThrust(bjetVecs);
  else if(c == collection::ujets) return setThrust(ujetVecs);
  else{
    TVector3 t;
    std::pair<float,TVector3> temp;
    temp.first = 0;
    temp.second = t;
    return temp;
  }
}

void MVAVariables::makeTriplets(collection c){

  std::vector<std::shared_ptr<TTHbb::Particle> > * col = 0;
  if(c == collection::all){
    col = &allVecs;
  }
  else if(c == collection::jets){
    col = &jetVecs;
  }
  else if(c == collection::bjets){
    col = &bjetVecs;
  }
  else if(c == collection::ujets){
    col = &ujetVecs;
  }

  std::vector<PairedSystem> vecs;
  
  if(col->size() < 3){
    TLorentzVector tempVec1 = TLorentzVector();
    TLorentzVector tempVec2 = TLorentzVector();
    TLorentzVector tempVec3 = TLorentzVector();
    tempVec1.SetPtEtaPhiM(-1., -9., -5., -1.);
    tempVec2.SetPtEtaPhiM(0., 0., 0., 0.);
    tempVec3.SetPtEtaPhiM(0., 0., 0., 0.);
    vecs.push_back(PairedSystem(tempVec1, PairedSystem(tempVec2, tempVec3)));
  }
  else{
    for(unsigned int i=0; i < col->size() - 2; i++){
      for(unsigned int j=i+1; j < col->size() - 1; j++){
	for(unsigned int k=j+1; k < col->size(); k++){
	  vecs.push_back(PairedSystem(col->at(k)->p4(), PairedSystem(col->at(i), col->at(j))));
	}
      }
    }
  }
  tripVecs.insert(std::make_pair(c, vecs));
}

void MVAVariables::makeQuadruplets(collection c){

  std::vector<std::shared_ptr<TTHbb::Particle> > * col = 0;
  if(c == collection::all){
    col = &allVecs;
  }
  else if(c == collection::jets){
    col = &jetVecs;
  }
  else if(c == collection::bjets){
    col = &bjetVecs;
  }
  else if(c == collection::ujets){
    col = &ujetVecs;
  }

  std::vector<PairedSystem> vecs;

  if(col->size() < 4){
    TLorentzVector tempVec1 = TLorentzVector();
    TLorentzVector tempVec2 = TLorentzVector();
    TLorentzVector tempVec3 = TLorentzVector();
    TLorentzVector tempVec4 = TLorentzVector();
    tempVec1.SetPtEtaPhiM(-1., -9., -5., -1.);
    tempVec2.SetPtEtaPhiM(0., 0., 0., 0.);
    tempVec3.SetPtEtaPhiM(0., 0., 0., 0.);
    tempVec4.SetPtEtaPhiM(0., 0., 0., 0.);
    vecs.push_back(PairedSystem(tempVec1, PairedSystem(tempVec2, PairedSystem(tempVec3, tempVec4))));
  }
  else{    
    for(unsigned int i=0; i < col->size() - 3; i++){
      for(unsigned int j=i+1; j < col->size() - 2; j++){
	for(unsigned int k=j+1; k < col->size() - 1; k++){
	  for(unsigned int l=k+1; l < col->size(); l++){
	    vecs.push_back(PairedSystem(col->at(l)->p4(), PairedSystem(col->at(k)->p4(), PairedSystem(col->at(i), col->at(j)))));
	  }
	}
      }
    }
  }
  quadVecs.insert(std::make_pair(c, vecs));

}
