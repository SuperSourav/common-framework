/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHFClassificationTools/ClassifyAndCalculateHF.h"

#include <cstdlib>

ClassifyAndCalculateHF::ClassifyAndCalculateHF(){

  m_jetPtCut=15000;
  m_jetEtaCut=2.5;
  m_leadingHadronPtCut=5000.;
  m_leadingHadronPtRatioCut=-1;

}

ClassifyAndCalculateHF::~ClassifyAndCalculateHF(){
}


void ClassifyAndCalculateHF::flagJets(TTHbb::JetContainer& jets, std::string hfmatchname, std::string hfDecorationName){

  for(auto jet: jets){

    int flav=0;
    int id=0;
    int count=0;

    int bcount=0;
    int ccount=0;
    int bcountcut=0;
    int ccountcut=0;

    int bid=0;
    int cid=0;

    const std::vector<std::weak_ptr<TTHbb::Particle> >& hadrons=jet->particleMatch(hfmatchname);

    for(auto a : hadrons){
      auto hf = a.lock();
      if(!hf){
	std::cout << "WARNING ClassifyAndCalculateHF::flagJets: hadron-jet association expired" << std::endl;
	continue;
      }
      int hforigin = hf->intVariable("TopHadronOriginFlag");
      if(6==hforigin) continue;

      float ptratio = hf->p4().Pt()/jet->p4().Pt();

      int hftype=hf->charVariable("hadronType");

      if(5==hftype){
	if(bid<hforigin)bid=hforigin;
	++bcount;
	if(hf->p4().Pt()>m_leadingHadronPtCut && ptratio>m_leadingHadronPtRatioCut){
	  ++bcountcut;
	}
	continue;
      }
      if(4==hftype){
	if(cid>hforigin)cid=hforigin;
	++ccount;
	if(hf->p4().Pt()>m_leadingHadronPtCut && ptratio>m_leadingHadronPtRatioCut){
	  ++ccountcut;
	}
	continue;
      }
      std::cout << "ClassifyAndCalculateHF ERROR: hadron type " << hftype << std::endl;
    }

    if(bcountcut){
      flav=5;
      id=bid;
      count=bcount;
    }
    else if(ccountcut){
      flav=4;
      id=cid;
      count=ccount;
    }

    jet->intVariable(hfDecorationName+"_flav")=flav;
    jet->intVariable(hfDecorationName+"_id")=id;
    jet->intVariable(hfDecorationName+"_count")=count;


  }

}


    // typedef enum {extrajet=0,
    // 		  c_MPI     =-1, b_MPI      =1,
    // 		  c_FSR     =-2, b_FSR      =2,
    // 		  c_from_W  =-3, b_from_W   =3,
    // 		  c_from_top=-4, b_from_top =4,
    // 		  c_from_H  =-5, b_from_H   =5} HF_id;


int ClassifyAndCalculateHF::computeHFClassification(TTHbb::JetContainer& jets, std::string hfDecorationName){


  int b=0, B=0, c=0, C=0;
  int b_prompt=0, B_prompt=0, c_prompt=0, C_prompt=0;

  int mpifsr_code=0;

  for(auto jet: jets){
    if(jet->p4().Pt() < m_jetPtCut) continue;
    if(fabs(jet->p4().Eta()) > m_jetEtaCut) continue;
    /// count just additional HF for btype <3 while for c-type <1 and >-3
    int flav = jet->intVariable(hfDecorationName+"_flav");
    int id =  jet->intVariable(hfDecorationName+"_id");
    int count =  jet->intVariable(hfDecorationName+"_count");

    /// all at once
    if(flav==5 && id < 3){
      if(count > 1){
	B++;
      }
      else{
	b++;
      }
    }
    if(flav==4 && (id==0 || id==-1 || id==-2)){
      if(count > 1){
	C++;
      }
      else{
	c++;
      }
    }

    /// only prompt
    if(flav==5 && id==0){
      if(count > 1){
	B_prompt++;
      }
      else{
	b_prompt++;
      }
    }
    if(flav==4 && id==0){
      if(count > 1){
	C_prompt++;
      }
      else{
	c_prompt++;
      }
    }

    //// MPI and FSR
    if(id==1 && flav==5){ // b MPI
	mpifsr_code -= 1000;
      } 
      else if(id==2 && flav==5){ // b FSR
	mpifsr_code -= 100;
      } 
      else if(id==-1 && flav==4){ // c MPI
	mpifsr_code -= 10;
      } 
      else if(id==-2 && flav==4) { // c FSR
	mpifsr_code -= 1;
      }


  }

  int ext_code = 1000*b+100*B+10*c+1*C;
  int prompt_code=1000*b_prompt+100*B_prompt+10*c_prompt+1*C_prompt;

  // MPI and FSR categories if no prompt
  if(prompt_code==0 && ext_code!=0){
    return mpifsr_code;
  }

  ///// otherwise all mixed
  return ext_code;

}

int ClassifyAndCalculateHF::getSimpleClassification(int hfclassif){

  if(abs(hfclassif)>=100){
    return 1;
  }
  else if(hfclassif==0){
    return 0;
  }

  return -1;

}


void ClassifyAndCalculateHF::getReweightingVariables(TTHbb::JetContainer& jets, int hfclassif, TTHbb::BaseObject* event, std::string hfDecorationName){

  event->floatVariable(hfDecorationName+"_q1_pt")  = -99;
  event->floatVariable(hfDecorationName+"_q1_eta") = -99;
  event->floatVariable(hfDecorationName+"_q1_phi") = -99;
  event->floatVariable(hfDecorationName+"_q1_m")   = -99;
  event->floatVariable(hfDecorationName+"_q2_pt")  = -99;
  event->floatVariable(hfDecorationName+"_q2_eta") = -99;
  event->floatVariable(hfDecorationName+"_q2_phi") = -99;
  event->floatVariable(hfDecorationName+"_q2_m")   = -99;
  
  event->floatVariable(hfDecorationName+"_qq_pt")  = -99;
  event->floatVariable(hfDecorationName+"_qq_ht")  = -99;
  event->floatVariable(hfDecorationName+"_qq_dr")  = -99;
  event->floatVariable(hfDecorationName+"_qq_m")   = -99;

  int i1 =-1, i2=-1;

  /// are these assumed to be pT sorted??
  for(unsigned int j=0; j< jets.size() && i2<0; j++){

    auto jet = jets[j];

    if(jet->p4().Pt() < m_jetPtCut) continue;
    if(fabs(jet->p4().Eta()) > m_jetEtaCut) continue;
    int flav = jet->intVariable(hfDecorationName+"_flav");
    int id =  jet->intVariable(hfDecorationName+"_id");

    if( (hfclassif >= 100 && flav==5 && id<3) ||
	(hfclassif < 100 && flav==4 && (id==0 || id==-1 || id==-2) ) 
	){
      if(i1==-1){
	i1 = j;
      }
      else{
	i2 = j;
	break;
      }
    }
  }


  TLorentzVector v1, v2;
  if(i1!=-1){
    v1=jets[i1]->p4();

    event->floatVariable(hfDecorationName+"_q1_pt")=v1.Pt();
    event->floatVariable(hfDecorationName+"_q1_eta")=v1.Eta();
    event->floatVariable(hfDecorationName+"_q1_phi")=v1.Phi();
    event->floatVariable(hfDecorationName+"_q1_m")=v1.M();

    if(i2!=-1){
      v2=jets[i2]->p4();

      event->floatVariable(hfDecorationName+"_q2_pt")=v2.Pt();
      event->floatVariable(hfDecorationName+"_q2_eta")=v2.Eta();
      event->floatVariable(hfDecorationName+"_q2_phi")=v2.Phi();
      event->floatVariable(hfDecorationName+"_q2_m")=v2.M();

      event->floatVariable(hfDecorationName+"_qq_pt")=(v1+v2).Pt();
      event->floatVariable(hfDecorationName+"_qq_ht")= v1.Pt()+v2.Pt();
      event->floatVariable(hfDecorationName+"_qq_dr")=v1.DeltaR(v2);
      event->floatVariable(hfDecorationName+"_qq_m")= (v1+v2).M();

    }
  }


}




