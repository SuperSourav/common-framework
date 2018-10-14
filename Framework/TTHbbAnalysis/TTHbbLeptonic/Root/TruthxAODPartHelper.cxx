/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TruthxAODPartHelper.h"

#include <iostream>

std::shared_ptr<TTHbb::TruthParticle> TruthxAODPartHelper::createTruthPart(const xAOD::TruthParticle* part){

  auto tthpart = std::make_shared<TTHbb::TruthParticle>();
  tthpart->SetPtEtaPhiM(part->pt(), part->eta(), part->phi(), part->m());
  tthpart->intVariable("pdgid")=part->pdgId();
  tthpart->intVariable("barcode")=part->barcode();
  tthpart->intVariable("status")=part->status();
  tthpart->floatVariable("charge")=part->charge();

  return tthpart;

}


void TruthxAODPartHelper::printdeepParticle(const xAOD::TruthParticle* part, int deep){
  printParticle(part);
  if(part->hasDecayVtx()){
    const xAOD::TruthVertex* vertex = part->decayVtx();
    //printVertex(vertex);
    std::cout << "depp---- " <<deep<<std::endl;
    if(deep != 0){
      for(unsigned int i=0; i<vertex->nOutgoingParticles(); ++i){
	//			  std::cout << "nOutgoingParticles " <<vertex->nOutgoingParticles()<<std::endl;
	//           std::cout << "part bar,i " <<part->barcode()<<","<<i<<std::endl;
	printdeepParticle(vertex->outgoingParticle(i),deep-1);
      }	
    }
  }	  
}

void TruthxAODPartHelper::printParticle(const xAOD::TruthParticle* part){
	std::cout << "TruthParticle" <<
	  " barcode " <<  part->barcode() <<
	  " pdg " <<  part->pdgId() <<
	  " status " <<  part->status() <<
	  " pt " <<  part->pt() <<
	  " eta " <<  part->eta() <<
	  " phi " <<  part->phi() <<
	  " e " <<  part->e() <<
	  " m " <<  part->p4().M()<<
	  " childrens " <<part->nChildren() <<"[";
	for(unsigned int i = 0; i<part->nChildren(); ++i){
	  if(!part->child(i)) continue;
	  std::cout << part->child(i)->barcode() <<",";
	}
	
	std::cout<<std::endl;

}

void TruthxAODPartHelper::printVertex(const xAOD::TruthVertex* vertex){
  if (vertex->barcode() != 0) {
    if (vertex->x() != 0.0 && vertex->y() != 0.0 && vertex->z() != 0.0) {
      std::cout << "TruthVertex ";		
      std::cout << " barcode:";
      std::cout.width(9);
      std::cout << vertex->barcode();
      std::cout << " ID:";
      std::cout.width(5);
      std::cout << vertex->id();
      std::cout << " (X,cT)=";
      std::cout.width(9);
      std::cout.precision(2);
      std::cout << vertex->x() << ",";
      std::cout.width(9);
      std::cout.precision(2);
      std::cout << vertex->y() << ",";
      std::cout.width(9);
      std::cout.precision(2);
      std::cout << vertex->z() << ",";
      std::cout.width(9);
      std::cout.precision(2);
      std::cout << vertex->t();
      std::cout << " outgoing Particles  :";	
      std::cout << vertex->nOutgoingParticles();
      std::cout << std::endl;
    }
  }
}
  


