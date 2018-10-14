/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/RecoCombinationsDecorator.h"
#include "TTHbbReconstruction/NeutrinoWeighter.h"

#include "TTHbbTruthHelper/TruthMatchHelper.h"
#include <set>


/// this code needs some cleanup to remove some duplicates and especially for the continuous btag support.


RecoCombinationsDecorator::RecoCombinationsDecorator(){
  ///...
}
  

RecoCombinationsDecorator::~RecoCombinationsDecorator(){
 ///...
}


void RecoCombinationsDecorator::decorateHiggsCombination(Combination* comb){


  comb->floatVariable("Higgs_mass") = comb->higgs->p4().M();
  comb->floatVariable("Higgs_pt") = comb->higgs->p4().Pt();
  comb->floatVariable("bbHiggs_dR") = comb->higgs->child1->p4().DeltaR(comb->higgs->child2->p4());  
  comb->floatVariable("bbHiggs_dEta") = fabs(comb->higgs->child1->p4().Eta() - comb->higgs->child2->p4().Eta());
  comb->floatVariable("bbHiggs_angle") = comb->higgs->child1->p4().Angle(comb->higgs->child2->p4().Vect());
  float b1H_mv2 = comb->higgs->child1->floatVariable("mv2");
  float b2H_mv2 = comb->higgs->child2->floatVariable("mv2");
  comb->floatVariable("b1Higgs_mv2") = b1H_mv2;
  comb->floatVariable("b2Higgs_mv2") = b2H_mv2;
  comb->floatVariable("bbHiggs_mv2_sum") = comb->higgs->child1->floatVariable("mv2")+comb->higgs->child2->floatVariable("mv2");   
  comb->floatVariable("Higgs_eta") = comb->higgs->p4().Eta();
  comb->floatVariable("Higgs_pt") = comb->higgs->p4().Pt();
  comb->floatVariable("b2overb1Higgs_pt") = (comb->higgs->child2->p4().Pt())/(comb->higgs->child1->p4().Pt());

  
  float pt_lbHiggs = (b1H_mv2 > b2H_mv2)? comb->higgs->child1->p4().Pt() : comb->higgs->child2->p4().Pt();
  float pt_slbHiggs = (b1H_mv2 < b2H_mv2)? comb->higgs->child1->p4().Pt() : comb->higgs->child2->p4().Pt();
  comb->floatVariable("b1Higgsmv2_pT") = pt_lbHiggs;
  comb->floatVariable("b2Higgsmv2_pT") = pt_slbHiggs;

  for(auto ss : m_contBTagNames){
 
    comb->intVariable("b1Higgs_"+ss) = comb->higgs->child1->intVariable(ss);     
    comb->intVariable("b2Higgs_"+ss) = comb->higgs->child2->intVariable(ss);      
    comb->intVariable("bbHiggs_"+ss+"_sum") = comb->higgs->child1->intVariable(ss)+comb->higgs->child2->intVariable(ss);   

    comb->intVariable("bHiggs_"+ss+"_1") = TMath::Max(comb->higgs->child1->intVariable(ss),  comb->higgs->child2->intVariable(ss));
    comb->intVariable("bHiggs_"+ss+"_2") = TMath::Min(comb->higgs->child1->intVariable(ss),  comb->higgs->child2->intVariable(ss));


  }


}


void RecoCombinationsDecorator::decorateCommonCombination(Combination* comb){

  decorateHiggsCombination(comb);

  comb->floatVariable("leptop_mass") = comb->lepTop->p4().M();  
  comb->floatVariable("leptop_pt") = comb->lepTop->p4().Pt(); 
  comb->floatVariable("lepblepTop_dR")  = comb->lepTop->childw->child1->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("blepTop_mv2")  =  comb->lepTop->childb->floatVariable("mv2"); 

  comb->floatVariable("lepWb1Higgs_mass") = (comb->lepTop->childw->p4()+comb->higgs->child1->p4()).M(); 
  comb->floatVariable("lepWb2Higgs_mass") = (comb->lepTop->childw->p4()+comb->higgs->child2->p4()).M();

  //RT new vars
  comb->floatVariable("b1Higgsbleptop_dR") = comb->higgs->child1->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("b2Higgsbleptop_dR") = comb->higgs->child2->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("lepb1Higgs_dR")   = comb->lepTop->childw->child1->p4().DeltaR(comb->higgs->child1->p4());
  comb->floatVariable("lepb2Higgs_dR")   = comb->lepTop->childw->child1->p4().DeltaR(comb->higgs->child2->p4());
  comb->floatVariable("Higgslep_dR")     = comb->lepTop->childw->child1->p4().DeltaR(comb->higgs->p4());
  comb->floatVariable("Higgsbleptop_dR") = comb->lepTop->childb->p4().DeltaR(comb->higgs->p4());
  comb->floatVariable("Higgsleptop_dR")  = comb->lepTop->p4().DeltaR(comb->higgs->p4());
  comb->floatVariable("Higgsbleptop_mass") = (comb->higgs->p4() + comb->lepTop->childb->p4()).M();

  float lepWminbhiggslepW_mass;
  float minbhiggslepW_dR;
  float lwdr1 = comb->higgs->child1->p4().DeltaR(comb->lepTop->childw->p4());
  float lwdr2 = comb->higgs->child2->p4().DeltaR(comb->lepTop->childw->p4());
  if(lwdr1>lwdr2){
    lepWminbhiggslepW_mass = (comb->higgs->child2->p4() + comb->lepTop->childw->p4()).M();
    minbhiggslepW_dR       = lwdr2;  
  }else{
    lepWminbhiggslepW_mass = (comb->higgs->child1->p4() + comb->lepTop->childw->p4()).M();
    minbhiggslepW_dR       = lwdr1;
  }
  comb->floatVariable("lepWminbhiggslepW_mass") = lepWminbhiggslepW_mass;
  comb->floatVariable("minbhiggslepW_dR")       = minbhiggslepW_dR;

  float lepWminbhiggsbleptop_mass;
  float lepWminbhiggsbleptop_dR;
  float lwdrb1 = comb->higgs->child1->p4().DeltaR(comb->lepTop->childb->p4());
  float lwdrb2 = comb->higgs->child2->p4().DeltaR(comb->lepTop->childb->p4());
  if(lwdrb1>lwdrb2){
    lepWminbhiggsbleptop_mass = (comb->higgs->child2->p4() + comb->lepTop->childw->p4()).M();
    lepWminbhiggsbleptop_dR   = comb->higgs->child2->p4().DeltaR(comb->lepTop->childw->p4());
  }else{
    lepWminbhiggsbleptop_mass = (comb->higgs->child1->p4() + comb->lepTop->childw->p4()).M();
    lepWminbhiggsbleptop_dR   = comb->higgs->child1->p4().DeltaR(comb->lepTop->childw->p4());
  }
  comb->floatVariable("lepWminbhiggsbleptop_mass") = lepWminbhiggsbleptop_mass;
  comb->floatVariable("lepWminbhiggsbleptop_dR")   = lepWminbhiggsbleptop_dR;
  //

  for(auto ss : m_contBTagNames){
 
    comb->intVariable("blepTop_"+ss)  =  comb->lepTop->childb->intVariable(ss); 

  }

}

void RecoCombinationsDecorator::decorateCombination(Combination* comb){
    
  decorateCommonCombination(comb);

  comb->floatVariable("hadW_mass") = comb->hadTop->childw->p4().M(); 
  comb->floatVariable("qqhadW_angle") = comb->hadTop->childw->child1->p4().Angle(comb->hadTop->childw->child2->p4().Vect());   
  comb->floatVariable("q1hadW_mv2") = comb->hadTop->childw->child1->floatVariable("mv2");    
  comb->floatVariable("q2hadW_mv2") = comb->hadTop->childw->child2->floatVariable("mv2");   
  comb->floatVariable("qhadW_mv2_1") = TMath::Max(comb->hadTop->childw->child1->floatVariable("mv2"),  comb->hadTop->childw->child2->floatVariable("mv2"));
  comb->floatVariable("qhadW_mv2_2") = TMath::Min(comb->hadTop->childw->child1->floatVariable("mv2"),  comb->hadTop->childw->child2->floatVariable("mv2"));
  comb->floatVariable("qqhadW_mv2_sum") = comb->hadTop->childw->child1->floatVariable("mv2")+comb->hadTop->childw->child2->floatVariable("mv2");

  comb->floatVariable("hadtop_mass") = comb->hadTop->p4().M(); 
  comb->floatVariable("hadtop_pt") = comb->hadTop->p4().Pt(); 
  comb->floatVariable("hadWbhadTop_dR") = comb->hadTop->childw->p4().DeltaR(comb->hadTop->childb->p4());  
  comb->floatVariable("lepWbhadTop_mass") = (comb->lepTop->childw->p4()+comb->hadTop->childb->p4()).M();  
  float drbwj1 = comb->hadTop->childw->child1->p4().DeltaR(comb->hadTop->childb->p4());
  float drbwj2 = comb->hadTop->childw->child2->p4().DeltaR(comb->hadTop->childb->p4());
  float mindrbwj = (drbwj1>drbwj2)?drbwj2:drbwj1;
  comb->floatVariable("bhadTopq1hadW_dR") = drbwj1; 
  comb->floatVariable("bhadTopq2hadW_dR") = drbwj2; 
  comb->floatVariable("minbhadTopqhadW_dR") = mindrbwj; 

  comb->floatVariable("bhadTop_mv2")  =  comb->hadTop->childb->floatVariable("mv2"); 
    
  comb->floatVariable("hadWblepTop_mass") = (comb->hadTop->childw->p4()+comb->lepTop->childb->p4()).M(); 

  comb->floatVariable("hadWb1Higgs_mass") = (comb->hadTop->childw->p4()+comb->higgs->child1->p4()).M(); 
  comb->floatVariable("hadWb2Higgs_mass") = (comb->hadTop->childw->p4()+comb->higgs->child2->p4()).M(); 

  comb->floatVariable("hadWblepTop_dR") = comb->hadTop->childw->p4().DeltaR(comb->lepTop->childb->p4());   
  comb->floatVariable("lepbhadTop_dR") = comb->lepTop->childw->child1->p4().DeltaR(comb->hadTop->childb->p4());    
  comb->floatVariable("blepTopbhadTop_dR") = comb->hadTop->childb->p4().DeltaR(comb->lepTop->childb->p4()); /// change var name...done
  comb->floatVariable("lepTophadTop_dR") = comb->hadTop->p4().DeltaR(comb->lepTop->p4());  /// change to dr when above is fixed...done
  TLorentzVector ttbarp4 = comb->hadTop->p4()+comb->lepTop->p4();
  comb->floatVariable("lepTophadTop_m") = ttbarp4.M(); 
  comb->floatVariable("lepTophadTop_pt") = ttbarp4.Pt(); 


  float drlb = comb->lepTop->childw->child1->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("diff_mindRbhadTopqhadW_dRlepblepTop") = mindrbwj-drlb;  /// change var name...done

  //RT
  comb->floatVariable("qqhadW_dR") = comb->hadTop->childw->child1->p4().DeltaR(comb->hadTop->childw->child2->p4());    
  comb->floatVariable("b1Higgsbhadtop_dR") = comb->higgs->child1->p4().DeltaR(comb->hadTop->childb->p4());
  comb->floatVariable("b2Higgsbhadtop_dR") = comb->higgs->child2->p4().DeltaR(comb->hadTop->childb->p4());
  comb->floatVariable("Higgsbhadtop_dR") = (comb->higgs->p4()).DeltaR(comb->hadTop->childb->p4());
  comb->floatVariable("Higgshadtop_dR")  = (comb->higgs->p4()).DeltaR(comb->hadTop->p4());
  comb->floatVariable("Higgsttbar_dR")   = (comb->higgs->p4()).DeltaR(comb->lepTop->p4()+comb->hadTop->p4());
  comb->floatVariable("ttH_Ht")  = comb->higgs->child1->p4().Pt() + comb->higgs->child2->p4().Pt() + comb->lepTop->childb->p4().Pt() + comb->hadTop->childb->p4().Pt() + 
    comb->hadTop->childw->child1->p4().Pt() + comb->hadTop->childw->child2->p4().Pt();
  comb->floatVariable("Higgsq1hadW_mass") =  (comb->higgs->p4() + comb->hadTop->childw->child1->p4()).M();
  comb->floatVariable("Higgsbhadtop_mass") = (comb->higgs->p4() + comb->hadTop->childb->p4()).M();

  float hadWminbhiggshadW_mass;
  float minbhiggshadW_dR;
  float dr1 = comb->higgs->child1->p4().DeltaR(comb->hadTop->childw->p4());
  float dr2 = comb->higgs->child2->p4().DeltaR(comb->hadTop->childw->p4());
  if(dr1>dr2){
    hadWminbhiggshadW_mass = (comb->higgs->child2->p4() + comb->hadTop->childw->p4()).M();
    minbhiggshadW_dR       = dr2;  
  }else{
    hadWminbhiggshadW_mass = (comb->higgs->child1->p4() + comb->hadTop->childw->p4()).M();
    minbhiggshadW_dR       = dr1;
  }
  comb->floatVariable("hadWminbhiggshadW_mass") = hadWminbhiggshadW_mass;
  comb->floatVariable("minbhiggshadW_dR")   = minbhiggshadW_dR;

  float hadWminbhiggsbhadtop_mass;
  float hadWminbhiggsbhadtop_dR;
  float drb1 = comb->higgs->child1->p4().DeltaR(comb->hadTop->childb->p4());
  float drb2 = comb->higgs->child2->p4().DeltaR(comb->hadTop->childb->p4());
  if(drb1>drb2){
    hadWminbhiggsbhadtop_mass = (comb->higgs->child2->p4() + comb->hadTop->childw->p4()).M();
    hadWminbhiggsbhadtop_dR   = comb->higgs->child2->p4().DeltaR(comb->hadTop->childw->p4());
  }else{
    hadWminbhiggsbhadtop_mass = (comb->higgs->child1->p4() + comb->hadTop->childw->p4()).M();
    hadWminbhiggsbhadtop_dR   = comb->higgs->child1->p4().DeltaR(comb->hadTop->childw->p4());
  }
  comb->floatVariable("hadWminbhiggsbhadtop_mass") = hadWminbhiggsbhadtop_mass;
  comb->floatVariable("hadWminbhiggsbhadtop_dR")   = hadWminbhiggsbhadtop_dR;
  //

  for(auto ss : m_contBTagNames){

    comb->intVariable("q1hadW_"+ss) = comb->hadTop->childw->child1->intVariable(ss);    
    comb->intVariable("q2hadW_"+ss) = comb->hadTop->childw->child2->intVariable(ss);   
    comb->intVariable("qhadW_"+ss+"_1") = TMath::Max(comb->hadTop->childw->child1->intVariable(ss),  comb->hadTop->childw->child2->intVariable(ss));
    comb->intVariable("qhadW_"+ss+"_2") = TMath::Min(comb->hadTop->childw->child1->intVariable(ss),  comb->hadTop->childw->child2->intVariable(ss));
    comb->intVariable("bhadTop_"+ss)  =  comb->hadTop->childb->intVariable(ss); 

    comb->intVariable("bbTop_"+ss+"_sum")  =  comb->hadTop->childb->intVariable(ss)+comb->lepTop->childb->intVariable(ss); 

    comb->intVariable("bbTop_bbHiggs_"+ss+"_diff") = comb->intVariable("bbTop_"+ss+"_sum") - comb->intVariable("bbHiggs_"+ss+"_sum");


  }

}

void RecoCombinationsDecorator::decorateInCompleteCombination(Combination* comb){

  decorateCommonCombination(comb);

  ///all variable assuming a hadw formed of one jet hadTop->child2

  // comb->floatVariable("hadW_mass") = comb->incompleteHadTop->child2->p4().M(); 
  // comb->floatVariable("qqhadW_angle") = 0;  
  comb->floatVariable("q1hadW_mv2") = comb->incompleteHadTop->child2->floatVariable("mv2");    
  // comb->floatVariable("q2hadW_mv2") = 0;  
  // comb->floatVariable("qhadW_mv2_1") = 0;
  // comb->floatVariable("qhadW_mv2_2") = 0;
  // comb->floatVariable("qqhadW_mv2_sum") = 0;
  double mindrbwj = comb->incompleteHadTop->child2->p4().DeltaR(comb->incompleteHadTop->child1->p4());;
  comb->floatVariable("hadtop_mass") = comb->incompleteHadTop->p4().M(); 
  comb->floatVariable("hadtop_pt") = comb->incompleteHadTop->p4().Pt(); 
  comb->floatVariable("hadWbhadTop_dR") =   mindrbwj;
  // comb->floatVariable("bhadTopq1hadW_dR") = 0; 
  // comb->floatVariable("bhadTopq2hadW_dR") = 0; 
  // comb->floatVariable("minbhadTopqhadW_dR") = 0; 

  comb->floatVariable("bhadTop_mv2")  =  comb->incompleteHadTop->child1->floatVariable("mv2"); 
    
  comb->floatVariable("hadWblepTop_mass") = (comb->incompleteHadTop->child2->p4()+comb->lepTop->childb->p4()).M(); 
  comb->floatVariable("lepWbhadTop_mass") = (comb->lepTop->childw->p4()+comb->incompleteHadTop->child1->p4()).M(); 
  comb->floatVariable("hadWb1Higgs_mass") = (comb->incompleteHadTop->child2->p4()+comb->higgs->child1->p4()).M(); 
  comb->floatVariable("hadWb2Higgs_mass") = (comb->incompleteHadTop->child2->p4()+comb->higgs->child2->p4()).M(); 

  comb->floatVariable("hadWblepTop_dR") = comb->incompleteHadTop->child2->p4().DeltaR(comb->lepTop->childb->p4());   
  comb->floatVariable("lepbhadTop_dR") = comb->lepTop->childw->child1->p4().DeltaR(comb->incompleteHadTop->child1->p4());    
  comb->floatVariable("blepTopbhadTop_dR") = comb->incompleteHadTop->child1->p4().DeltaR(comb->lepTop->childb->p4()); /// change var name...done 
  comb->floatVariable("lepTophadTop_dR") = comb->incompleteHadTop->p4().DeltaR(comb->lepTop->p4());  /// change to dr when above is fixed...done
  TLorentzVector ttbarp4 = comb->incompleteHadTop->p4()+comb->lepTop->p4();
  comb->floatVariable("lepTophadTop_m") = ttbarp4.M(); 
  comb->floatVariable("lepTophadTop_pt") = ttbarp4.Pt(); 

  float drlb = comb->lepTop->childw->child1->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("diff_mindRbhadTopqhadW_dRlepblepTop") = mindrbwj-drlb;  /// change var name...done

  //RT 
  comb->floatVariable("b1Higgsbhadtop_dR") = comb->higgs->child1->p4().DeltaR(comb->incompleteHadTop->child1->p4());
  comb->floatVariable("b2Higgsbhadtop_dR") = comb->higgs->child2->p4().DeltaR(comb->incompleteHadTop->child1->p4());
  comb->floatVariable("Higgsbhadtop_dR") = (comb->higgs->p4()).DeltaR(comb->incompleteHadTop->child1->p4());
  comb->floatVariable("Higgshadtop_dR") = (comb->higgs->p4()).DeltaR(comb->incompleteHadTop->p4());
  comb->floatVariable("Higgsttbar_dR") = (comb->higgs->p4()).DeltaR(comb->lepTop->p4()+comb->incompleteHadTop->p4());
  comb->floatVariable("ttH_Ht")  = comb->higgs->child1->p4().Pt() + comb->higgs->child2->p4().Pt() + comb->lepTop->childb->p4().Pt() +  
    comb->incompleteHadTop->child1->p4().Pt() + comb->incompleteHadTop->child2->p4().Pt() ;

  comb->floatVariable("Higgsq1hadW_mass") = (comb->higgs->p4() + comb->incompleteHadTop->child2->p4()).M();
  comb->floatVariable("Higgsbhadtop_mass") = (comb->higgs->p4() + comb->incompleteHadTop->child1->p4()).M();

  float hadWminbhiggshadW_mass;
  float minbhiggshadW_dR;
  float dr1 = comb->higgs->child1->p4().DeltaR(comb->incompleteHadTop->child2->p4());
  float dr2 = comb->higgs->child2->p4().DeltaR(comb->incompleteHadTop->child2->p4());
  if(dr1>dr2){
    hadWminbhiggshadW_mass = (comb->higgs->child2->p4() + comb->incompleteHadTop->child2->p4()).M();
    minbhiggshadW_dR       = dr2;  
  }else{
    hadWminbhiggshadW_mass = (comb->higgs->child1->p4() + comb->incompleteHadTop->child2->p4()).M();
    minbhiggshadW_dR       = dr1;
  }
  comb->floatVariable("hadWminbhiggshadW_mass") = hadWminbhiggshadW_mass;
  comb->floatVariable("minbhiggshadW_dR")   = minbhiggshadW_dR;

  float hadWminbhiggsbhadtop_mass;
  float hadWminbhiggsbhadtop_dR;
  float drb1 = comb->higgs->child1->p4().DeltaR(comb->incompleteHadTop->child1->p4());
  float drb2 = comb->higgs->child2->p4().DeltaR(comb->incompleteHadTop->child1->p4());
  if(drb1>drb2){
    hadWminbhiggsbhadtop_mass = (comb->higgs->child2->p4() + comb->incompleteHadTop->child2->p4()).M();
    hadWminbhiggsbhadtop_dR   = comb->higgs->child2->p4().DeltaR(comb->incompleteHadTop->child2->p4());
  }else{
    hadWminbhiggsbhadtop_mass = (comb->higgs->child1->p4() + comb->incompleteHadTop->child2->p4()).M();
    hadWminbhiggsbhadtop_dR   = comb->higgs->child1->p4().DeltaR(comb->incompleteHadTop->child2->p4());
  }
  comb->floatVariable("hadWminbhiggsbhadtop_mass") = hadWminbhiggsbhadtop_mass;
  comb->floatVariable("hadWminbhiggsbhadtop_dR")   = hadWminbhiggsbhadtop_dR;


  for(auto ss : m_contBTagNames){

    comb->intVariable("q1hadW_"+ss) = comb->incompleteHadTop->child2->intVariable(ss);    
    // comb->intVariable("q2hadW_"+ss) = 0;   
    // comb->intVariable("qhadW_"+ss+"_1") = 0;
    // comb->intVariable("qhadW_"+ss+"_2") = 0;
    comb->intVariable("bhadTop_"+ss)  =  comb->incompleteHadTop->child1->intVariable(ss); 

  }
 
}

void RecoCombinationsDecorator::decorateDilepCombination(Combination* comb){


  decorateTTbarDilepCombination(comb);
  decorateHiggsCombination(comb);

  comb->floatVariable("btopbhiggs_drdiff") = fabs(comb->floatVariable("blepTop1lepTop2_dR") - comb->floatVariable("bbHiggs_dR"));

  TLorentzVector ttbar =  comb->lepTop1->p4()+comb->lepTop2->p4();
  TLorentzVector higgs = comb->higgs->p4();
  TLorentzVector higgsttbar = higgs + ttbar;

  comb->floatVariable("Higgsttbar_dR") = ttbar.DeltaR(higgs);
  comb->floatVariable("Higgsttbar_pt") = higgsttbar.Pt();


  if (comb->floatVariable("weight_nu") > -4){
    if (comb->floatVariable("weight_nu") > 0){
      TLorentzVector m_ttbar =  comb->recolepTop1->p4()+comb->recolepTop2->p4();
      TLorentzVector m_lbH =  comb->higgs->child1->p4();
      TLorentzVector m_slbH =  comb->higgs->child2->p4();
      TLorentzVector m_higgsttbar = higgs + m_ttbar;

      comb->floatVariable("m_ttbar_pT") = m_ttbar.Pt(); 
      comb->floatVariable("m_ttbar_y") = m_ttbar.Rapidity();
      comb->floatVariable("m_ttbar_m") = m_ttbar.M();
      comb->floatVariable("m_top_pT") = comb->recolepTop1->p4().Pt(); 
      comb->floatVariable("m_top_y") = comb->recolepTop1->p4().Rapidity(); 
      comb->floatVariable("m_tbar_pT") = comb->recolepTop2->p4().Pt(); 
      comb->floatVariable("m_tbar_y") = comb->recolepTop2->p4().Rapidity(); 
      comb->floatVariable("m_toptbar_dPhi") = fabs(comb->recolepTop1->p4().DeltaPhi(comb->recolepTop2->p4())); 
      comb->floatVariable("m_toptbar_dEta") = fabs(comb->recolepTop1->p4().Eta() - comb->recolepTop2->p4().Eta()); 
      comb->floatVariable("m_toptbar_dy") = fabs(comb->recolepTop1->p4().Rapidity() - comb->recolepTop2->p4().Rapidity()); 
      comb->floatVariable("m_ttbarHiggs_dPhi") = fabs(m_ttbar.DeltaPhi(higgs)); 
      comb->floatVariable("m_ttbarHiggs_dEta") = fabs(m_ttbar.Eta() - higgs.Eta()); 


      comb->floatVariable("m_Higgsttbar_Mass") = m_higgsttbar.M();
      comb->floatVariable("m_reco_cos_helicity_p") = cos_theta_helicity(   comb->recolepTop1->p4(), comb->recolepTop1->p4(),  m_ttbar, comb->recolepTop1->childw->child1->p4(), +1);
      comb->floatVariable("m_reco_cos_helicity_m") = cos_theta_helicity(   comb->recolepTop1->p4(), comb->recolepTop2->p4(), m_ttbar, comb->recolepTop2->childw->child1->p4(), -1);
      comb->floatVariable("m_reco_cos_helicity") = comb->floatVariable("m_reco_cos_helicity_m") * comb->floatVariable("m_reco_cos_helicity_p");
      comb->floatVariable("m_reco_cos_raxis_p") = cos_theta_raxis(      comb->recolepTop1->p4(), comb->recolepTop1->p4(),  m_ttbar, comb->recolepTop1->childw->child1->p4(), +1);
      comb->floatVariable("m_reco_cos_raxis_m") = cos_theta_raxis(      comb->recolepTop1->p4(), comb->recolepTop2->p4(), m_ttbar, comb->recolepTop2->childw->child1->p4(), -1);
      comb->floatVariable("m_reco_cos_raxis") = comb->floatVariable("m_reco_cos_raxis_m") * comb->floatVariable("m_reco_cos_raxis_p");
      comb->floatVariable("m_reco_cos_transverse_p") = cos_theta_transverse( comb->recolepTop1->p4(), comb->recolepTop1->p4(),  m_ttbar, comb->recolepTop1->childw->child1->p4(), +1);
      comb->floatVariable("m_reco_cos_transverse_m") = cos_theta_transverse( comb->recolepTop1->p4(), comb->recolepTop2->p4(), m_ttbar, comb->recolepTop2->childw->child1->p4(), -1);
      comb->floatVariable("m_reco_cos_transverse") = comb->floatVariable("m_reco_cos_transverse_m") * comb->floatVariable("m_reco_cos_transverse_p");

      comb->floatVariable("m_top_pX") = comb->recolepTop1->p4().Px(); 
      comb->floatVariable("m_top_pY") = comb->recolepTop1->p4().Py(); 
      comb->floatVariable("m_top_pZ") = comb->recolepTop1->p4().Pz(); 
      comb->floatVariable("m_top_E") = comb->recolepTop1->p4().E(); 
      comb->floatVariable("m_tbar_pX") = comb->recolepTop2->p4().Px(); 
      comb->floatVariable("m_tbar_pY") = comb->recolepTop2->p4().Py(); 
      comb->floatVariable("m_tbar_pZ") = comb->recolepTop2->p4().Pz(); 
      comb->floatVariable("m_tbar_E") = comb->recolepTop2->p4().E(); 
      comb->floatVariable("m_lbH_pX") = m_lbH.Px(); 
      comb->floatVariable("m_lbH_pY") = m_lbH.Py(); 
      comb->floatVariable("m_lbH_pZ") = m_lbH.Pz(); 
      comb->floatVariable("m_lbH_E") = m_lbH.E(); 
      comb->floatVariable("m_slbH_pX") = m_slbH.Px(); 
      comb->floatVariable("m_slbH_pY") = m_slbH.Py(); 
      comb->floatVariable("m_slbH_pZ") = m_slbH.Pz(); 
      comb->floatVariable("m_slbH_E") = m_slbH.E(); 
      comb->floatVariable("m_ttbarHiggs_dR") = higgs.DeltaR(m_ttbar);
      comb->floatVariable("m_topHiggs_dR") = higgs.DeltaR(comb->recolepTop1->p4());
      comb->floatVariable("m_tbarHiggs_dR") = higgs.DeltaR(comb->recolepTop2->p4());
    }
    else{
      comb->floatVariable("m_ttbar_pT") = -99; 
      comb->floatVariable("m_ttbar_y") = -99;
      comb->floatVariable("m_ttbar_m") = -99;
      comb->floatVariable("m_top_pT") = -99; 
      comb->floatVariable("m_top_y") = -99; 
      comb->floatVariable("m_tbar_pT") = -99; 
      comb->floatVariable("m_tbar_y") = -99; 
      comb->floatVariable("m_toptbar_dPhi") = -99; 
      comb->floatVariable("m_toptbar_dEta") = -99; 
      comb->floatVariable("m_toptbar_dy") = -99; 
      comb->floatVariable("m_ttbarHiggs_dPhi") = -99; 
      comb->floatVariable("m_ttbarHiggs_dEta") = -99; 
      comb->floatVariable("m_Higgsttbar_Mass") = -1;
      comb->floatVariable("m_reco_cos_helicity_p") = -5;
      comb->floatVariable("m_reco_cos_helicity_m") = -5;
      comb->floatVariable("m_reco_cos_helicity") = -5;
      comb->floatVariable("m_reco_cos_raxis_p") = -5;
      comb->floatVariable("m_reco_cos_raxis_m") = -5;
      comb->floatVariable("m_reco_cos_raxis") = -5;
      comb->floatVariable("m_reco_cos_transverse_p") = -5;
      comb->floatVariable("m_reco_cos_transverse_m") = -5;
      comb->floatVariable("m_reco_cos_transverse") = -5;

      comb->floatVariable("m_top_pX") = -99;
      comb->floatVariable("m_top_pY") = -99;
      comb->floatVariable("m_top_pZ") = -99;
      comb->floatVariable("m_top_E") = -99;
      comb->floatVariable("m_tbar_pX") = -99;
      comb->floatVariable("m_tbar_pY") = -99;
      comb->floatVariable("m_tbar_pZ") = -99;
      comb->floatVariable("m_tbar_E") = -99;
      comb->floatVariable("m_ttbarHiggs_dR") = -99;
      comb->floatVariable("m_topHiggs_dR") = -99;
      comb->floatVariable("m_tbarHiggs_dR") = -99;
      comb->floatVariable("m_lbH_pX") = -99;
      comb->floatVariable("m_lbH_pY") = -99;
      comb->floatVariable("m_lbH_pZ") = -99;
      comb->floatVariable("m_lbH_E") = -99;
      comb->floatVariable("m_slbH_pX") = -99;
      comb->floatVariable("m_slbH_pY") = -99;
      comb->floatVariable("m_slbH_pZ") = -99;
      comb->floatVariable("m_slbH_E") = -99;
    }
  }
  float dR_Higgstop1lep = higgs.DeltaR(comb->lepTop1->child1->p4());
  float dR_Higgstop2lep = higgs.DeltaR(comb->lepTop2->child1->p4());

  comb->floatVariable("Higgsleptop_dRmin") = (dR_Higgstop1lep > dR_Higgstop2lep)? dR_Higgstop2lep : dR_Higgstop1lep;
  comb->floatVariable("Higgsleptop_dRmax") = (dR_Higgstop1lep > dR_Higgstop2lep)? dR_Higgstop1lep : dR_Higgstop2lep;

  // float dEta_Higgstop1lep = fabs(higgs.Eta() - comb->lepTop1->child1->p4().Eta());
  // float dEta_Higgstop2lep = fabs(higgs.Eta() - comb->lepTop2->child1->p4().Eta());

  // comb->floatVariable("Higgsleptop_dEtamin") = (dEta_Higgstop1lep > dEta_Higgstop2lep)? dEta_Higgstop2lep : dEta_Higgstop1lep;
  // comb->floatVariable("Higgsleptop_dEtamax") = (dEta_Higgstop1lep > dEta_Higgstop2lep)? dEta_Higgstop1lep : dEta_Higgstop2lep;


  float dR_Higgstop1b   = higgs.DeltaR(comb->lepTop1->child2->p4());
  float dR_Higgstop2b   = higgs.DeltaR(comb->lepTop2->child2->p4());
  comb->floatVariable("Higgsbtop_dRmin") = (dR_Higgstop1b > dR_Higgstop2b)? dR_Higgstop2b : dR_Higgstop1b;
  comb->floatVariable("Higgsbtop_dRmax") = (dR_Higgstop1b > dR_Higgstop2b)? dR_Higgstop1b : dR_Higgstop2b;

	
  float dEta_Higgstop1lep = fabs(higgs.Eta() - comb->lepTop1->child1->p4().Eta());
  float dEta_Higgstop2lep = fabs(higgs.Eta() - comb->lepTop2->child1->p4().Eta());
  comb->floatVariable("Higgsleptop_dEtamin") = (dEta_Higgstop1lep > dEta_Higgstop2lep)? dEta_Higgstop2lep : dEta_Higgstop1lep;
  comb->floatVariable("Higgsleptop_dEtamax") = (dEta_Higgstop1lep > dEta_Higgstop2lep)? dEta_Higgstop1lep : dEta_Higgstop2lep;
  float dEta_Higgstop1b   = fabs(higgs.Eta() - comb->lepTop1->child2->p4().Eta());
  float dEta_Higgstop2b   = fabs(higgs.Eta() - comb->lepTop2->child2->p4().Eta());
  comb->floatVariable("Higgsbtop_dEtamin") = (dEta_Higgstop1b > dEta_Higgstop2b)? dEta_Higgstop2b : dEta_Higgstop1b;
  comb->floatVariable("Higgsbtop_dEtamax") = (dEta_Higgstop1b > dEta_Higgstop2b)? dEta_Higgstop1b : dEta_Higgstop2b;


}



void RecoCombinationsDecorator::decorateTTbarCombination(Combination* comb){

  comb->floatVariable("leptop_mass") = comb->lepTop->p4().M(); 
  comb->floatVariable("leptop_pt") = comb->lepTop->p4().Pt();  
  comb->floatVariable("lepblepTop_dR")  = comb->lepTop->childw->child1->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("blepTop_mv2")  =  comb->lepTop->childb->floatVariable("mv2"); 

  comb->floatVariable("hadW_mass") = comb->hadTop->childw->p4().M(); 
  comb->floatVariable("qqhadW_angle") = comb->hadTop->childw->child1->p4().Angle(comb->hadTop->childw->child2->p4().Vect());   
  comb->floatVariable("q1hadW_mv2") = comb->hadTop->childw->child1->floatVariable("mv2");    
  comb->floatVariable("q2hadW_mv2") = comb->hadTop->childw->child2->floatVariable("mv2");   
  comb->floatVariable("qhadW_mv2_1") = TMath::Max(comb->hadTop->childw->child1->floatVariable("mv2"),  comb->hadTop->childw->child2->floatVariable("mv2"));
  comb->floatVariable("qhadW_mv2_2") = TMath::Min(comb->hadTop->childw->child1->floatVariable("mv2"),  comb->hadTop->childw->child2->floatVariable("mv2"));
  comb->floatVariable("qqhadW_mv2_sum") = comb->hadTop->childw->child1->floatVariable("mv2")+comb->hadTop->childw->child2->floatVariable("mv2");
  comb->floatVariable("qqhadW_dR") = comb->hadTop->childw->child1->p4().DeltaR(comb->hadTop->childw->child2->p4());    

  comb->floatVariable("hadtop_mass") = comb->hadTop->p4().M(); 
  comb->floatVariable("hadtop_pt") = comb->hadTop->p4().Pt(); 
  comb->floatVariable("hadWbhadTop_dR") = comb->hadTop->childw->p4().DeltaR(comb->hadTop->childb->p4());  
  comb->floatVariable("lepWbhadTop_mass") = (comb->lepTop->childw->p4()+comb->hadTop->childb->p4()).M();  
  float drbwj1 = comb->hadTop->childw->child1->p4().DeltaR(comb->hadTop->childb->p4());
  float drbwj2 = comb->hadTop->childw->child2->p4().DeltaR(comb->hadTop->childb->p4());
  float mindrbwj = (drbwj1>drbwj2)?drbwj2:drbwj1;
  comb->floatVariable("bhadTopq1hadW_dR") = drbwj1; 
  comb->floatVariable("bhadTopq2hadW_dR") = drbwj2; 
  comb->floatVariable("minbhadTopqhadW_dR") = mindrbwj; 

  comb->floatVariable("bhadTop_mv2")  =  comb->hadTop->childb->floatVariable("mv2"); 
  comb->floatVariable("hadWblepTop_mass") = (comb->hadTop->childw->p4()+comb->lepTop->childb->p4()).M(); 

  comb->floatVariable("hadWblepTop_dR") = comb->hadTop->childw->p4().DeltaR(comb->lepTop->childb->p4());   
  comb->floatVariable("lepbhadTop_dR") = comb->lepTop->childw->child1->p4().DeltaR(comb->hadTop->childb->p4());    
  comb->floatVariable("blepTopbhadTop_dR") = comb->hadTop->childb->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("lepTophadTop_dR") = comb->hadTop->p4().DeltaR(comb->lepTop->p4()); 
  comb->floatVariable("lepTophadTop_dPhi") = fabs(comb->hadTop->p4().DeltaPhi(comb->lepTop->p4())); 
  TLorentzVector ttbarp4 = comb->hadTop->p4()+comb->lepTop->p4();
  comb->floatVariable("lepTophadTop_m") = ttbarp4.M(); 
  comb->floatVariable("lepTophadTop_pt") = ttbarp4.Pt(); 
  comb->floatVariable("lepTophadTop_rapidity") = ttbarp4.Rapidity(); 

  float drlb = comb->lepTop->childw->child1->p4().DeltaR(comb->lepTop->childb->p4());
  comb->floatVariable("diff_mindRbhadTopqhadW_dRlepblepTop") = mindrbwj-drlb;  


}

void RecoCombinationsDecorator::decorateTTbarDilepCombination(Combination* comb){

  float drbl1=comb->lepTop1->child1->p4().DeltaR(comb->lepTop1->child2->p4());
  float drbl2=comb->lepTop2->child1->p4().DeltaR(comb->lepTop2->child2->p4());

  comb->floatVariable("leptop1_mass") = comb->lepTop1->p4().M(); 
  comb->floatVariable("leptop1_pt") = comb->lepTop1->p4().Pt();  
  comb->floatVariable("lepblepTop1_dR")  = drbl1;
  comb->floatVariable("blepTop1_mv2")  =  comb->lepTop1->child2->floatVariable("mv2"); 
  comb->floatVariable("blepTop1_pt")  =  comb->lepTop1->child2->p4().Pt(); 

  comb->floatVariable("leptop2_mass") = comb->lepTop2->p4().M(); 
  comb->floatVariable("leptop2_pt") = comb->lepTop2->p4().Pt();  
  comb->floatVariable("lepblepTop2_dR")  = drbl2;
  comb->floatVariable("blepTop2_mv2")  =  comb->lepTop2->child2->floatVariable("mv2"); 
  comb->floatVariable("blepTop2_pt")  =  comb->lepTop2->child2->p4().Pt(); 

  comb->floatVariable("lepblepTop_diff") = fabs(drbl1-drbl2);
  comb->floatVariable("diff_lepblepTop_mass") = fabs(comb->lepTop1->p4().M() - comb->lepTop2->p4().M());

  comb->floatVariable("blepTop1lepTop2_dR") = comb->lepTop1->child2->p4().DeltaR(comb->lepTop2->child2->p4());
  comb->floatVariable("blepTop1lepTop2_mass") = (comb->lepTop1->child2->p4()+comb->lepTop2->child2->p4()).M();
  comb->floatVariable("lep1lep2_dR") = comb->lepTop1->child1->p4().DeltaR(comb->lepTop2->child1->p4());

  comb->floatVariable("b1_pT") = comb->lepTop1->child2->p4().Pt();
  comb->floatVariable("b2_pT") = comb->lepTop2->child2->p4().Pt();

  comb->floatVariable("blepTop1lepTop2_dPhi") = fabs(comb->lepTop1->child2->p4().DeltaPhi(comb->lepTop2->child2->p4()));

 float lep1b2_dR=comb->lepTop1->child1->p4().DeltaR(comb->lepTop2->child2->p4());
 float lep2b1_dR=comb->lepTop1->child2->p4().DeltaR(comb->lepTop2->child1->p4());

 comb->floatVariable("lep1b2_dR") = lep1b2_dR;
 comb->floatVariable("lep2b1_dR") = lep2b1_dR;

 float   mindR_btop1_l = (drbl1 < lep2b1_dR)?drbl1:lep2b1_dR;
 float   mindR_btop2_l = (drbl2 < lep1b2_dR)?drbl2:lep1b2_dR;
 comb->floatVariable("topblep_dRmin") = (mindR_btop1_l < mindR_btop2_l)? mindR_btop1_l : mindR_btop2_l;

 float   maxdR_btop1_l = (drbl1 > lep2b1_dR)?drbl1:lep2b1_dR;
 float   maxdR_btop2_l = (drbl2 > lep1b2_dR)?drbl2:lep1b2_dR;
 comb->floatVariable("topblep_dRmax") = (maxdR_btop1_l > maxdR_btop2_l)? maxdR_btop1_l : maxdR_btop2_l;


 float dEtabl1=fabs(comb->lepTop1->child1->p4().Eta() - comb->lepTop1->child2->p4().Eta());
 float dEtabl2=fabs(comb->lepTop2->child1->p4().Eta() - comb->lepTop2->child2->p4().Eta());

 comb->floatVariable("lepblepTop1_dEta") = dEtabl1;
 comb->floatVariable("lepblepTop2_dEta") = dEtabl2;

 float lep1b2_dEta=fabs(comb->lepTop1->child1->p4().Eta() - comb->lepTop2->child2->p4().Eta());
 float lep2b1_dEta=fabs(comb->lepTop2->child2->p4().Eta() - comb->lepTop1->child1->p4().Eta());

 comb->floatVariable("lep1b2_dEta") = lep1b2_dEta;
 comb->floatVariable("lep2b1_dEta") = lep2b1_dEta;

 float   mindEta_btop1_l = (dEtabl1 < lep2b1_dEta)?dEtabl1:lep2b1_dEta;
 float   mindEta_btop2_l = (dEtabl2 < lep1b2_dEta)?dEtabl2:lep1b2_dEta;
 comb->floatVariable("topblep_dEtamin") = (mindEta_btop1_l < mindEta_btop2_l)? mindEta_btop1_l : mindEta_btop2_l;


 float   maxdEta_btop1_l = (dEtabl1 > lep2b1_dEta)?drbl1:lep2b1_dEta;
 float   maxdEta_btop2_l = (dEtabl2 > lep1b2_dEta)?drbl2:lep1b2_dEta;
 comb->floatVariable("topblep_dEtamax") = (maxdEta_btop1_l > maxdEta_btop2_l)? maxdEta_btop1_l : maxdEta_btop2_l;


  comb->floatVariable("lep1b2_m") = (comb->lepTop1->child1->p4()+comb->lepTop2->child2->p4()).M();
  comb->floatVariable("lep2b1_m") = (comb->lepTop1->child2->p4()+comb->lepTop2->child1->p4()).M();

  comb->floatVariable("lepTop12_dR") = comb->lepTop1->p4().DeltaR(comb->lepTop2->p4()); 
  comb->floatVariable("lepTop12_dPhi") = fabs(comb->lepTop1->p4().DeltaPhi(comb->lepTop2->p4())); 
  TLorentzVector ttbarp4 = comb->lepTop1->p4()+comb->lepTop2->p4();
  comb->floatVariable("ttbar_m") = ttbarp4.M(); 
  comb->floatVariable("ttbar_pt") = ttbarp4.Pt(); 
  comb->floatVariable("ttbar_rapidity") = ttbarp4.Rapidity(); 

  for(auto ss : m_contBTagNames){
 
    comb->intVariable("blepTop1_"+ss)  =  comb->lepTop1->child2->intVariable(ss); 
    comb->intVariable("blepTop2_"+ss)  =  comb->lepTop2->child2->intVariable(ss); 

  }

}


Composite* RecoCombinationsDecorator::createLepW(std::shared_ptr<Lepton> lep,std::shared_ptr<Neutrino> nu, bool isMC){

  Composite* comp = new Composite();
  comp->name = "lepw";
  comp->child1=lep;
  comp->child2=nu;
  comp->p4(lep->p4()+nu->p4());
  if(isMC){
    /// assume all OK for lepton
    comp->isMatch=true;
  }

  return comp;

}

Composite* RecoCombinationsDecorator::createBoostedHadW(std::shared_ptr<Jet> jet1, bool isMC){

  Composite* comp = new Composite();
  comp->name = "hadw";

  comp->child1=jet1;

  comp->p4(jet1->p4());
  
  if(isMC){
    comp->isOneMatch = JetTruthMatchHelper::matchJetFromW(jet1.get());
  }
  return comp;

}


Composite* RecoCombinationsDecorator::createHadW(std::shared_ptr<Jet> jet1,std::shared_ptr<Jet> jet2, bool isMC){

  Composite* comp = new Composite();
  comp->name = "hadw";

  if(jet1->p4().Pt() > jet2->p4().Pt()){
    comp->child1=jet1;
    comp->child2=jet2;
  }
  else{
    comp->child1=jet2;
    comp->child2=jet1;
  }
  comp->p4(jet1->p4()+jet2->p4());
  if(isMC){
    comp->isMatch = JetTruthMatchHelper::matchJetFromW(jet1.get()) && JetTruthMatchHelper::matchJetFromW(jet2.get());
    if(!comp->isMatch){
      comp->isOneMatch = JetTruthMatchHelper::matchJetFromW(jet1.get()) || JetTruthMatchHelper::matchJetFromW(jet2.get());
    }
  }
  return comp;

}

Composite* RecoCombinationsDecorator::createHiggs(std::shared_ptr<Jet> jet1,std::shared_ptr<Jet> jet2, bool isMC){

  Composite* comp = new Composite();
  comp->name = "higgs";

  if(jet1->p4().Pt() > jet2->p4().Pt()){
    comp->child1=jet1;
    comp->child2=jet2;
  }
  else{
    comp->child1=jet2;
    comp->child2=jet1;
  }
  comp->p4(jet1->p4()+jet2->p4());
  if(isMC){
    comp->isMatch = JetTruthMatchHelper::matchBFromHiggs(jet1.get()) && JetTruthMatchHelper::matchBFromHiggs(jet2.get());
    if(!comp->isMatch){
      comp->isOneMatch = JetTruthMatchHelper::matchBFromHiggs(jet1.get()) || JetTruthMatchHelper::matchBFromHiggs(jet2.get());
    }
  }   

  return comp;
}

TopComposite* RecoCombinationsDecorator::createLepTop(std::shared_ptr<Jet> j, std::shared_ptr<Composite> w, bool isMC){


  TopComposite* comp = new TopComposite();
  comp->name = "leptop";
  comp->childw=w;
  comp->childb=j;
  comp->p4(w->p4()+j->p4());
  if(isMC){
    comp->isBMatch = JetTruthMatchHelper::matchBFromLepTop(j.get());
    comp->isWMatch = w->isMatch;
    comp->isMatch = comp->isBMatch && comp->isWMatch;
  }

  return comp;

}


TopComposite* RecoCombinationsDecorator::createHadTop(std::shared_ptr<Jet> j, std::shared_ptr<Composite> w, bool isMC){

  TopComposite* comp = new TopComposite();
  comp->name = "hadtop";
  comp->childw=w;
  comp->childb=j;
  comp->p4(w->p4()+j->p4());
  if(isMC){
    comp->isBMatch = JetTruthMatchHelper::matchBFromHadTop(j.get());
    comp->isWMatch = w->isMatch;
    comp->isMatch = comp->isBMatch && comp->isWMatch;
  }

  return comp;

}


Composite* RecoCombinationsDecorator::createIncompleteHadTop(std::shared_ptr<Jet> jet1,std::shared_ptr<Jet> jet2, bool isMC){

  Composite* comp = new Composite();
  comp->name = "ihadtop";
  comp->child1=jet1;
  comp->child2=jet2;
  comp->p4(jet1->p4()+jet2->p4());
  if(isMC){
    comp->isMatch = JetTruthMatchHelper::matchBFromHadTop(jet1.get()) && JetTruthMatchHelper::matchJetFromW(jet2.get());
    if(!comp->isMatch){
      comp->isOneMatch = JetTruthMatchHelper::matchBFromHadTop(jet1.get()) || JetTruthMatchHelper::matchJetFromW(jet2.get());
    }
  }

  return comp;
}



Composite* RecoCombinationsDecorator::createIncompLepTop(std::shared_ptr<Lepton> lep,std::shared_ptr<Jet> jet, bool isMC){

  Composite* comp = new Composite();
  comp->name = "lepTop";
  comp->child1=lep;
  comp->child2=jet;
  comp->p4(lep->p4()+jet->p4());
  if(isMC){
    /// assume all OK for lepton
    if(lep->charVariable("isPrompt")){
      if(lep->intVariable("true_pdg") == -11 || lep->intVariable("true_pdg") == -13){
	if(JetTruthMatchHelper::bitMatch(jet->truthMatch(), TTHbbTruth::JetMatchBTop))comp->isMatch=true;
      }
      if(lep->intVariable("true_pdg") == 11 || lep->intVariable("true_pdg") == 13){
	if(JetTruthMatchHelper::bitMatch(jet->truthMatch(), TTHbbTruth::JetMatchBAntiTop))comp->isMatch=true;
      }
    }
  }

  return comp;

}


Combination* RecoCombinationsDecorator::createCombination(std::shared_ptr<TopComposite> ltop, 
							  std::shared_ptr<TopComposite> htop, 
							  std::shared_ptr<Composite> higgs, bool isMC){


  Combination* comb = new Combination();

  comb->higgs=higgs;
  comb->lepTop=ltop;
  comb->hadTop=htop;  
  if(isMC){
    comb->isMatch = higgs->isMatch && ltop->isMatch && htop->isMatch;
    comb->truthMatchPattern=0;

    //// not requesting leading at reco level to match leading at truth level!
    if( JetTruthMatchHelper::matchBFromHiggs(higgs->child1.get()) )
      comb->truthMatchPattern |= (1<<JetMatchLeadingBHiggs);
    if( JetTruthMatchHelper::matchBFromHiggs(higgs->child2.get()) )
      comb->truthMatchPattern |= (1<<JetMatchSubLeadingBHiggs);

    if( JetTruthMatchHelper::matchBFromLepTop(ltop->childb.get()) )
      comb->truthMatchPattern |= (1<<JetMatchBLepTop);
    if( JetTruthMatchHelper::matchBFromHadTop(htop->childb.get()) )
      comb->truthMatchPattern |= (1<<JetMatchBHadTop);
	  
    if( JetTruthMatchHelper::matchJetFromW(htop->childw->child1.get()) )
      comb->truthMatchPattern |= (1<<JetMatchLeadingJetW);
    if( JetTruthMatchHelper::matchJetFromW(htop->childw->child2.get()) )
      comb->truthMatchPattern |= (1<<JetMatchSubLeadingJetW);

  }

  return comb;

}



Combination* RecoCombinationsDecorator::createIncompleteCombination(std::shared_ptr<TopComposite> ltop, 
								    std::shared_ptr<Composite> htop, 
								    std::shared_ptr<Composite> higgs, bool isMC){

  Combination* comb = new Combination();

  comb->higgs=higgs;
  comb->lepTop=ltop;
  comb->incompleteHadTop=htop;  
  if(isMC){
    comb->isMatch = higgs->isMatch && ltop->isMatch && htop->isMatch;
    comb->truthMatchPattern=0;

    //// not requesting leading at reco level to match leading at truth level!
    if( JetTruthMatchHelper::matchBFromHiggs(higgs->child1.get()) )
      comb->truthMatchPattern |= (1<<JetMatchLeadingBHiggs);
    if( JetTruthMatchHelper::matchBFromHiggs(higgs->child2.get()) )
      comb->truthMatchPattern |= (1<<JetMatchSubLeadingBHiggs);

    if( JetTruthMatchHelper::matchBFromLepTop(ltop->childb.get()) )
      comb->truthMatchPattern |= (1<<JetMatchBLepTop);
    if( JetTruthMatchHelper::matchBFromHadTop(htop->child1.get()) )
      comb->truthMatchPattern |= (1<<JetMatchBHadTop);
	  
    if( JetTruthMatchHelper::matchJetFromW(htop->child2.get()) )
      comb->truthMatchPattern |= (1<<JetMatchLeadingJetW);

  }

  return comb;
}


Combination* RecoCombinationsDecorator::createCombination(std::shared_ptr<TopComposite> ltop, 
							  std::shared_ptr<TopComposite> htop, 
							  bool isMC){


  Combination* comb = new Combination();

  comb->lepTop=ltop;
  comb->hadTop=htop;  
  if(isMC){
    comb->isMatch = ltop->isMatch && htop->isMatch;
    comb->truthMatchPattern=0;

    //// not requesting leading at reco level to match leading at truth level!
    if( JetTruthMatchHelper::matchBFromLepTop(ltop->childb.get()) )
      comb->truthMatchPattern |= (1<<JetMatchBLepTop);
    if( JetTruthMatchHelper::matchBFromHadTop(htop->childb.get()) )
      comb->truthMatchPattern |= (1<<JetMatchBHadTop);
	  
    if( JetTruthMatchHelper::matchJetFromW(htop->childw->child1.get()) )
      comb->truthMatchPattern |= (1<<JetMatchLeadingJetW);
    if( JetTruthMatchHelper::matchJetFromW(htop->childw->child2.get()) )
      comb->truthMatchPattern |= (1<<JetMatchSubLeadingJetW);

  }

  return comb;

}


Combination* RecoCombinationsDecorator::createCombination(std::shared_ptr<Composite> ltop1, 
							  std::shared_ptr<Composite> ltop2, 
							  std::shared_ptr<Composite> higgs,
                double met_met, double met_phi,
							  int eventNumber, bool isMC){

  Combination* comb = createCombination(ltop1, ltop2, met_met, met_phi, eventNumber, isMC);

  comb->higgs=higgs;

  if(isMC){
    comb->isMatch = higgs->isMatch && comb->isMatch;

    //// not requesting leading at reco level to match leading at truth level!
    if( JetTruthMatchHelper::matchBFromHiggs(higgs->child1.get()) )
      comb->truthMatchPattern |= (1<<JetMatchLeadingBHiggs);
    if( JetTruthMatchHelper::matchBFromHiggs(higgs->child2.get()) )
      comb->truthMatchPattern |= (1<<JetMatchSubLeadingBHiggs);

  }

  return comb;

}


Combination* RecoCombinationsDecorator::createCombination(std::shared_ptr<Composite> ltop1, 
							  std::shared_ptr<Composite> ltop2, 
                double met_met, double met_phi,
							  int eventNumber, bool isMC){


  Combination* comb = new Combination();

  comb->lepTop1=ltop1;
  comb->lepTop2=ltop2;
  if(isMC){
    comb->isMatch = ltop1->isMatch && ltop2->isMatch;
    comb->truthMatchPattern=0;

    int lepPDG1=0;
    int lepPDG2=0;
    if(ltop1->child1->charVariable("isPrompt")){
      lepPDG1=ltop1->child1->intVariable("true_pdg");
    }
    if(ltop2->child1->charVariable("isPrompt")){
      lepPDG2=ltop2->child1->intVariable("true_pdg");
    }
 

    //// not requesting leading at reco level to match leading at truth level!
    //// the matching is done is based on charge
    if( lepPDG1 == -11 || lepPDG1 == -13 ){
      comb->truthMatchPattern |= (1<<DiLepMatchLepTop1);  
      if(ltop1->isMatch) comb->truthMatchPattern |= (1<<DilepJetMatchBTop1);  
    } 
    if( lepPDG1 == 11 || lepPDG1 == 13 ){
      comb->truthMatchPattern |= (1<<DiLepMatchLepTop1);  
      if(ltop1->isMatch) comb->truthMatchPattern |= (1<<DilepJetMatchBTop1);  
    } 
	  
    if( lepPDG2 == -11 || lepPDG2 == -13 ){
      comb->truthMatchPattern |= (1<<DiLepMatchLepTop2);  
      if(ltop2->isMatch) comb->truthMatchPattern |= (1<<DilepJetMatchBTop2);  
    } 
    if( lepPDG2 == 11 || lepPDG2 == 13 ){
      comb->truthMatchPattern |= (1<<DiLepMatchLepTop2);  
      if(ltop2->isMatch) comb->truthMatchPattern |= (1<<DilepJetMatchBTop2);  
    } 


  }
 
  if (met_met !=-10e10 && met_phi !=-10e10){
    NeutrinoWeighter nuW = NeutrinoWeighter((1<<2 | 1<<3), eventNumber);// Ingnore the second argument,it's just a random string
    std::shared_ptr<Particle> lep_pos  = ltop1->child1;
    std::shared_ptr<Particle> lep_neg  = ltop2->child1;
    std::shared_ptr<Particle> bjet     = ltop1->child2;
    std::shared_ptr<Particle> bjet_bar = ltop2->child2;
    if (ltop1->child1->intVariable("true_pdg") > 0){
      lep_pos  = ltop2->child1;
      lep_neg  = ltop1->child1;
      bjet     = ltop2->child2;
      bjet_bar = ltop1->child2;
    }
    nuW.RecalculateEtas(lep_pos->p4().Eta(), lep_neg->p4().Eta());
    double met_ex =met_met * cos(met_phi);
    double met_ey =met_met * sin(met_phi);
    float weight_nu = nuW.Reconstruct(lep_pos->p4(), lep_neg->p4(), bjet->p4(), bjet_bar->p4(), met_ex, met_ey, met_phi, 0,0);
    if(weight_nu > 0.){
      comb->floatVariable("weight_nu") = weight_nu;
      Composite* lepw_pos = new Composite();
      lepw_pos->name = "lepw";
      lepw_pos->child1=lep_pos;
      Particle* nu = new Particle();
      nu->p4(nuW.GetNu());
      lepw_pos->child2=std::shared_ptr<Particle>(nu);
      lepw_pos->child2=lep_neg;
      lepw_pos->p4(lep_pos->p4()+nuW.GetNu());
      // if(isMC){
        /// assume all OK for lepton
        lepw_pos->isMatch=true;
      // }
      TopComposite* recolepTop1 = new TopComposite();
      recolepTop1->name = "leptop";
      recolepTop1->childb=bjet;
      recolepTop1->childw= std::shared_ptr<Composite>(lepw_pos->deepClone());
      recolepTop1->p4(lepw_pos->p4()+bjet->p4());
      // if(isMC){
      //   recolepTop1->isBMatch = JetTruthMatchHelper::matchBFromLepTop(j.get());
      //   recolepTop1->isWMatch = w->isMatch;
      //   recolepTop1->isMatch = comp->isBMatch && comp->isWMatch;
      // }
      //
      Composite* lepw_neg = new Composite();
      lepw_neg->name = "lepn";
      lepw_neg->child1=lep_neg;
      // std::shared_ptr<Particle> nu_bar;
      Particle* nu_bar = new Particle();
      nu_bar->p4(nuW.GetNubar());
      lepw_neg->child2=std::shared_ptr<Particle>(nu_bar);
      lepw_neg->p4(lep_neg->p4()+nuW.GetNubar());
      if(isMC){
        /// assume all OK for lepton
        lepw_pos->isMatch=true;
      }
      TopComposite* recolepTop2 = new TopComposite();
      recolepTop2->name = "leptop2";
      recolepTop2->childb=bjet_bar;
      recolepTop2->childw= std::shared_ptr<Composite>(lepw_neg->deepClone());
      recolepTop2->p4(lepw_neg->p4()+bjet_bar->p4());
      // if(isMC){
      //   recolepTop1->isBMatch = JetTruthMatchHelper::matchBFromLepTop(j.get());
      //   recolepTop1->isWMatch = w->isMatch;
      //   recolepTop1->isMatch = comp->isBMatch && comp->isWMatch;
      // }

          // // std::shared_ptr<Particle> nu;
          // // nu->p4(nuW.GetNu());
          // // lepw_pos->child2=nu;

          // // std::shared_ptr<Particle> nubar;
          // // nubar->p4(nuW.GetNubar());
          // // lepw_neg->child2=nubar;
          comb->recolepTop1 = std::shared_ptr<TopComposite>(recolepTop1->deepClone());
          comb->recolepTop2 = std::shared_ptr<TopComposite>(recolepTop2->deepClone());
          
          // delete recolepTop1;
          // delete recolepTop2;
          // delete lepw_pos;
          // delete lepw_neg;
          // delete nu;
          // delete nubar;
    }
    else{
      // comb->recolepTop1 = std::shared_ptr<TopComposite>(recolepTop1->deepClone());
      // comb->recolepTop2 = std::shared_ptr<TopComposite>(recolepTop2->deepClone());
      comb->floatVariable("weight_nu") = -2;
    }
  }
  else{
    comb->floatVariable("weight_nu") = -5;
  }

  return comb;

}


void RecoCombinationsDecorator::fillAdditionalJets(JetContainer* jets, Combination* comb){

  comb->additionalJets.clear();
  std::set<Jet*> usedJets;
  bool checkerror=true;

  if(comb->lepTop){
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->lepTop->childb.get())).second;
  }
  if(comb->lepTop1){
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->lepTop1->child2.get())).second;
  }
  if(comb->lepTop2){
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->lepTop2->child2.get())).second;
  }
  if(comb->hadTop){
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->hadTop->childb.get())).second;
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->hadTop->childw->child1.get())).second;
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->hadTop->childw->child2.get())).second;
  }
  if(comb->incompleteHadTop){
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->incompleteHadTop->child1.get())).second;
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->incompleteHadTop->child2.get())).second;
  }
  if(comb->higgs){
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->higgs->child1.get())).second;
    checkerror=checkerror&&usedJets.insert(ObjectHelper::toJet(comb->higgs->child2.get())).second;
  }
  
  if(!checkerror){
    std::cout << "ERROR: in RecoCombinationsDecorator::fillAdditionalJets duplicated jets in combination " << std::endl;
    return;
  }

  for(auto jet : *jets){
    if(usedJets.find(jet.get()) != usedJets.end()) continue;
    comb->additionalJets.push_back(jet);
  }



}


void RecoCombinationsDecorator::decorateAdditionalJets(Combination* comb){

  std::vector<std::shared_ptr<Jet> > jet_ptsorted = comb->additionalJets;
  std::vector<std::shared_ptr<Jet> > jet_mv2sorted = comb->additionalJets;
  

  for(unsigned int i=0; i<jet_mv2sorted.size(); ++i){
    bool ordered=true;
    for(unsigned int j=0; j<jet_mv2sorted.size()-1; ++j){
      if(jet_mv2sorted[j]->floatVariable("mv2") < jet_mv2sorted[j+1]->floatVariable("mv2")){
	ordered=false;
	auto holder = jet_mv2sorted[j];
	jet_mv2sorted[j] = jet_mv2sorted[j+1];
	jet_mv2sorted[j+1] = holder;
      }
    }
    if(ordered) break;
  }

  for(unsigned int i=0; i<jet_ptsorted.size(); ++i){
    bool ordered=true;
    for(unsigned int j=0; j<jet_ptsorted.size()-1; ++j){
      if(jet_ptsorted[j]->p4().Pt() < jet_ptsorted[j+1]->p4().Pt()){
	ordered=false;
	auto holder = jet_ptsorted[j];
	jet_ptsorted[j] = jet_ptsorted[j+1];
	jet_ptsorted[j+1] = holder;
      }
    }
    if(ordered) break;
  }


  comb->floatVariable("addJets_pt1_pt") =-9;
  comb->floatVariable("addJets_pt1_mv2") =-9;

  comb->floatVariable("addJets_pt12_rapidity") =-9;
  comb->floatVariable("addJets_pt12_ttbar_dphi") =-9;
  comb->floatVariable("addJets_pt12_ttbar_dpt") =-9;

  comb->floatVariable("addJets_mv21_pt") =-9;
  comb->floatVariable("addJets_mv21_mv2") =-9;

  comb->floatVariable("addJets_mv212_rapidity") =-9;
  comb->floatVariable("addJets_mv212_ttbar_dphi") =-9;
  comb->floatVariable("addJets_mv212_ttbar_dpt") =-9;

  comb->floatVariable("addJets_pt2_pt") =-9;
  comb->floatVariable("addJets_pt2_mv2") =-9;
  comb->floatVariable("addJets_pt12_dr") =-9;
  comb->floatVariable("addJets_pt12_m") =-9;
  comb->floatVariable("addJets_pt12_pt") =-9;
  comb->floatVariable("addJets_pt12_dphi") =-9;

  comb->floatVariable("addJets_mv22_pt") =-9;
  comb->floatVariable("addJets_mv22_mv2") =-9;
  comb->floatVariable("addJets_mv212_dr") =-9;
  comb->floatVariable("addJets_mv212_m") =-9;
  comb->floatVariable("addJets_mv212_pt") =-9;
  comb->floatVariable("addJets_mv212_dphi") =-9;

  comb->floatVariable("addJets_mv212_pt1_pt") =-9;
  comb->floatVariable("addJets_mv212_pt2_pt") =-9;



  //// careful if you want variables everywhere with default values
  if(jet_ptsorted.size()==0) return;

  float pt1_pt=0;
  float pt2_pt=0;
  float pt1_mv2=0;
  float pt2_mv2=0;

  float pt12_dr=0;
  float pt12_m=0;
  float pt12_pt=0;
  float pt12_rapidity=0;
  float pt12_dphi=0;
  float pt12_ttbar_dphi=0;
  float pt12_ttbar_dpt=0;

  float mv21_pt=0;
  float mv22_pt=0;
  float mv21_mv2=0;
  float mv22_mv2=0;

  float mv212_dr=0;
  float mv212_m=0;
  float mv212_pt=0;
  float mv212_rapidity=0;
  float mv212_dphi=0;
  float mv212_ttbar_dphi=0;
  float mv212_ttbar_dpt=0;

  if(jet_ptsorted.size()>0){

    TLorentzVector ttbar_p4(0,0,0,0);

    if(comb->lepTop) ttbar_p4+=comb->lepTop->p4();
    if(comb->lepTop1)ttbar_p4+=comb->lepTop1->p4();
    if(comb->lepTop2)ttbar_p4+=comb->lepTop2->p4();
    if(comb->hadTop)ttbar_p4+=comb->hadTop->p4();

    TLorentzVector pt1_p4 = jet_ptsorted[0]->p4();
    TLorentzVector mv21_p4 = jet_mv2sorted[0]->p4();

    TLorentzVector pt12_p4 = pt1_p4;
    TLorentzVector mv212_p4 = mv21_p4;
    
    pt1_pt=pt1_p4.Pt();
    pt1_mv2=jet_ptsorted[0]->floatVariable("mv2");

    mv21_pt=mv21_p4.Pt();
    mv21_mv2=jet_mv2sorted[0]->floatVariable("mv2");

    if(jet_ptsorted.size()>1){

      TLorentzVector pt2_p4 = jet_ptsorted[1]->p4();
      TLorentzVector mv22_p4 = jet_mv2sorted[1]->p4();
    
      pt12_p4+=pt2_p4;
      mv212_p4+=mv22_p4;

      pt2_pt=pt2_p4.Pt();
      pt2_mv2=jet_ptsorted[1]->floatVariable("mv2");

      mv22_pt=mv22_p4.Pt();
      mv22_mv2=jet_mv2sorted[1]->floatVariable("mv2");

      pt12_dr=pt2_p4.DeltaR(pt1_p4);
      pt12_m=pt12_p4.M();
      pt12_pt=pt12_p4.Pt();
      pt12_dphi=fabs(pt2_p4.DeltaPhi(pt1_p4));

      mv212_dr=mv22_p4.DeltaR(mv21_p4);
      mv212_m=mv212_p4.M();
      mv212_pt=mv212_p4.Pt();
      mv212_dphi=fabs(mv22_p4.DeltaPhi(mv21_p4));

    }

    pt12_rapidity=pt12_p4.Rapidity();
    pt12_ttbar_dphi=fabs(pt12_p4.DeltaPhi(ttbar_p4));
    pt12_ttbar_dpt=pt12_p4.Pt()-ttbar_p4.Pt();
    
    mv212_rapidity=mv212_p4.Rapidity();
    mv212_ttbar_dphi=fabs(mv212_p4.DeltaPhi(ttbar_p4));
    mv212_ttbar_dpt=mv212_p4.Pt()-ttbar_p4.Pt();

  }

  comb->floatVariable("addJets_pt1_pt") =pt1_pt;
  comb->floatVariable("addJets_pt1_mv2") =pt1_mv2;

  comb->floatVariable("addJets_pt12_rapidity") =pt12_rapidity;
  comb->floatVariable("addJets_pt12_ttbar_dphi") =pt12_ttbar_dphi;
  comb->floatVariable("addJets_pt12_ttbar_dpt") =pt12_ttbar_dpt;

  comb->floatVariable("addJets_mv21_pt") =mv21_pt;
  comb->floatVariable("addJets_mv21_mv2") =mv21_mv2;

  comb->floatVariable("addJets_mv212_rapidity") =mv212_rapidity;
  comb->floatVariable("addJets_mv212_ttbar_dphi") =mv212_ttbar_dphi;
  comb->floatVariable("addJets_mv212_ttbar_dpt") =mv212_ttbar_dpt;


  //// careful if you want variables everywhere with default values
  if(jet_ptsorted.size()<2) return;


  comb->floatVariable("addJets_pt2_pt") =pt2_pt;
  comb->floatVariable("addJets_pt2_mv2") =pt2_mv2;
  comb->floatVariable("addJets_pt12_dr") =pt12_dr;
  comb->floatVariable("addJets_pt12_m") =pt12_m;
  comb->floatVariable("addJets_pt12_pt") =pt12_pt;
  comb->floatVariable("addJets_pt12_dphi") =pt12_dphi;

  comb->floatVariable("addJets_mv22_pt") =mv22_pt;
  comb->floatVariable("addJets_mv22_mv2") =mv22_mv2;
  comb->floatVariable("addJets_mv212_dr") =mv212_dr;
  comb->floatVariable("addJets_mv212_m") =mv212_m;
  comb->floatVariable("addJets_mv212_pt") =mv212_pt;
  comb->floatVariable("addJets_mv212_dphi") =mv212_dphi;



  comb->floatVariable("addJets_mv212_pt1_pt") = TMath::Max(mv22_pt,mv21_pt);
  comb->floatVariable("addJets_mv212_pt2_pt") = TMath::Min(mv22_pt,mv21_pt);



}

float RecoCombinationsDecorator::cos_theta_helicity(TLorentzVector top, TLorentzVector parent_t, TLorentzVector ttbar, TLorentzVector lep, float sign){

  TVector3 boost_to_ttbar = ttbar.BoostVector();
  boost_to_ttbar *= -1.;
    
  parent_t.Boost(boost_to_ttbar);
  top.Boost(boost_to_ttbar);
  lep.Boost(boost_to_ttbar);
    
  TVector3 boost_to_parent_t = parent_t.BoostVector();
  boost_to_parent_t *= -1.;

  lep.Boost(boost_to_parent_t);
  
  TVector3 k_vector = top.Vect().Unit();
  k_vector *= sign;
  float theta = lep.Vect().Unit().Dot(k_vector);  

  //-- If we have a nan move to this to the overflow bins --//
  if (isnan(theta)){
    return -55.;
  } else {
    return theta;
  }
}

float RecoCombinationsDecorator::cos_theta_raxis(TLorentzVector top, TLorentzVector parent_t, TLorentzVector ttbar, TLorentzVector lep, float sign){

  TVector3 boost_to_ttbar = ttbar.BoostVector();
  boost_to_ttbar *= -1.;
    
  parent_t.Boost(boost_to_ttbar);
  top.Boost(boost_to_ttbar);
  lep.Boost(boost_to_ttbar);
    
  TVector3 boost_to_parent_t = parent_t.BoostVector();
  boost_to_parent_t *= -1.;

  lep.Boost(boost_to_parent_t);
  
  TVector3 k_vector = top.Vect().Unit();
  k_vector *= sign;
  TVector3 p_vector(0,0,1);

  float y = p_vector.Dot(k_vector);
  float r = pow((1. - y*y),0.5);

  TVector3 r_vector = (1./r)*(p_vector - y*k_vector);
  if (sign == 1){
    ///-- We're in the a axis --///
    if(y > 0) r_vector *= 1.;
    if(y < 0) r_vector *= -1.;
  } else if (sign == -1){
    ///-- We're in the b axis --///
    if(y > 0) r_vector *= -1.;
    if(y < 0) r_vector *= 1.;
  }

  float theta = lep.Vect().Unit().Dot(r_vector);

  //-- If we have a nan move to this to the overflow bins --//
  if (isnan(theta)){
    return -55.;
  } else {
    return theta;
  }
}

float RecoCombinationsDecorator::cos_theta_transverse(TLorentzVector top, TLorentzVector parent_t, TLorentzVector ttbar, TLorentzVector lep, float sign){

  TVector3 boost_to_ttbar = ttbar.BoostVector();
  boost_to_ttbar *= -1.;
    
  parent_t.Boost(boost_to_ttbar);
  top.Boost(boost_to_ttbar);
  lep.Boost(boost_to_ttbar);
    
  TVector3 boost_to_parent_t = parent_t.BoostVector();
  boost_to_parent_t *= -1.;

  lep.Boost(boost_to_parent_t);
  
  TVector3 k_vector = top.Vect().Unit();
  k_vector *= sign;
  TVector3 p_vector(0,0,1);

  float y = p_vector.Dot(k_vector);
  float r = pow((1. - y*y),0.5);

  TVector3 n_vector = (1./r)*(p_vector.Cross(k_vector)); ///-- Should this be Unit Vector? --///

  if (sign == 1){
    ///-- We're in the a axis --///
    if(y > 0) n_vector *= 1.;
    if(y < 0) n_vector *= -1.;
  } else if (sign == -1){
    ///-- We're in the b axis --///
    if(y > 0) n_vector *= -1.;
    if(y < 0) n_vector *= 1.;
  }

  float theta = lep.Vect().Unit().Dot(n_vector);

  //-- If we have a nan move to this to the overflow bins --//
  if (isnan(theta)){
    return -55.;
  } else {
    return theta;
  }
}
