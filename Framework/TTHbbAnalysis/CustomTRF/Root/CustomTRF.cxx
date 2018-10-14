/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CustomTRF/CustomTRF.h"

CustomTRF::CustomTRF(){
  fDebug = false;
  myRand=new TRandom3();
  comb=0;
  tempTest.clear();
}

CustomTRF::~CustomTRF(){
  delete myRand;
//  for (unsigned int i=0; i<tempTest.size(); i++) {
//    delete comb[i];
//  }
//  delete comb;
}

void CustomTRF::SetWorkingPoints(vector<string> wp_names){
  fWorkingPoint = wp_names;
}

void CustomTRF::AddWorkingPoint(string wp_name){
  fWorkingPoint.push_back(wp_name);
}

void CustomTRF::SetJetBtagEff(int jet_index,int wp_index,float eff){
  // extend the range if needed
  if(jet_index+1 > (int)fJetBtagEff.size()){
    vector<float> v;
    fJetBtagEff.push_back(v);
  }
  if(wp_index+1 > (int)fJetBtagEff[jet_index].size()){
    fJetBtagEff[jet_index].push_back(0.);
  }
  // set the value at the correct position
  fJetBtagEff[jet_index][wp_index] = eff;
}

void CustomTRF::SetJetBtagEff(int jet_index,string wp_name,float eff){
  int wp_index = find(fWorkingPoint.begin(),fWorkingPoint.end(),wp_name)-fWorkingPoint.begin();
  SetJetBtagEff(jet_index,wp_index,eff);
}

void CustomTRF::SetJetBtagEffs(int jet_index,vector<float> eff_vec){
  // extend the range if needed
  if(jet_index+1 > (int)fJetBtagEff.size()){
    vector<float> v;
    fJetBtagEff.push_back(v);
  }
  // set the value at the correct position
  fJetBtagEff[jet_index] = eff_vec;
}

void CustomTRF::SetJetsBtagEff(int wp_index,vector<float> eff_vec){
  for(int jet_index=0;jet_index<(int)eff_vec.size();jet_index++){
    // extend the range if needed
    if(jet_index+1 > (int)fJetBtagEff.size()){
      vector<float> v;
      fJetBtagEff.push_back(v);
    }
    if(wp_index+1 > (int)fJetBtagEff[jet_index].size()){
      fJetBtagEff[jet_index].push_back(0.);
    }
    // set the value at the correct position
    fJetBtagEff[jet_index][wp_index] = eff_vec[jet_index]; 
  }
}

void CustomTRF::SetJetsBtagEff(string wp_name,vector<float> eff_vec){
  int wp_index = find(fWorkingPoint.begin(),fWorkingPoint.end(),wp_name)-fWorkingPoint.begin();
  SetJetsBtagEff(wp_index,eff_vec);
}

void CustomTRF::SetJetsBtagEffs(vector<vector<float> > eff_vec){
  fJetBtagEff = eff_vec;
}


// Methods to set the initial b-tagging requirement (this will determine the TRF weight)

void CustomTRF::SetBtagRequirement(int number_of_tags, bool inclusive, int wp_index){
  fNumReqBtag = number_of_tags;
  fBtagReqIncl = inclusive;
  fBtagReqWPidx = wp_index;
}

void CustomTRF::SetBtagRequirement(int number_of_tags, bool inclusive, string wp_name){
  int wp_index = find(fWorkingPoint.begin(),fWorkingPoint.end(),wp_name)-fWorkingPoint.begin();
  SetBtagRequirement(number_of_tags, inclusive, wp_index);
}


// set the b-tag configuration, according to the per jet b-tag effs and the initial b-tag requirement
// (this also sets the TRF weight)

float CustomTRF::GetChosenPermutationProb() {
  if (prob.size()==0 )  return 0.;
  if (idx<0 )           return 0.;
  if (idx>=(int)prob.size()) return 0.;
  return prob[idx];
}

bool* CustomTRF::GetChosenPermutation() {
  if (idx<0 )           return NULL;
  return comb[idx];
}

float CustomTRF::GetPermutationProb(bool *perm) {
  float TOTprob = 1;
  for(int j=0;j<fNumJets;j++){
    if( perm[j]) TOTprob *=fJetBtagEff[j][fBtagReqWPidx];
    else         TOTprob *=(1 - fJetBtagEff[j][fBtagReqWPidx]);
  }
  return TOTprob;
}


void CustomTRF::Reset(){
  fJetIsTag.clear();
  fJetBtagEff.clear();
}

void CustomTRF::Evaluate(int EventNumber){
  myRand->SetSeed(EventNumber);
  
  fNumJets = fJetBtagEff.size();
  // initialize isTag to false
  fJetIsTag.clear();
  for(int j=0;j<fNumJets;j++){
    vector<float> v; v.clear();
    for(int wp_index=0;wp_index<(int)fJetBtagEff[j].size();wp_index++){
      v.push_back(0);
    }
    fJetIsTag.push_back(v);
  }
  float rand;
  //
  // -- step 1 --
  // get all the allowed combinations of chosen w.p. b-tags, with proper prob.
  tempTest.clear();
  comb = new bool*[(int)pow((double)2,(double)fNumJets)];
  for(int i=0;i<pow((double)2,(double)fNumJets);i++){
    comb[i] = new bool[fNumJets];
    tempTest.push_back(true);
  }
  int ncomb = Combinations(fNumJets,comb,fNumReqBtag,fBtagReqIncl);
  if(fDebug) cout << " Number of Combinations: " << ncomb << endl;
  ////vector<float> prob;
  prob.clear();
  float TOTprob = 0;
  for(int i=0;i<ncomb;i++){
    prob.push_back(1);
    for(int j=0;j<fNumJets;j++){
      if(comb[i][j]) prob[i]*=fJetBtagEff[j][fBtagReqWPidx];
      else prob[i]*=(1 - fJetBtagEff[j][fBtagReqWPidx]);
    }
    TOTprob += prob[i];
  }
  fTRFweight = TOTprob;
  // now:
  //  - comb[][] contains all the allowed combinations of tagged jets (0,1,0,0 means only the 2nd jet is tagged and so on...)
  //  - TOTprob is the sum of the probabilities for each allowed combination (i.e. with the required number of tags)
  //  - TOTprob is then also the prbability of having the reuqired number of tags, i.e. the TRF weight
  //
  // -- step 2 --
  // choose which combination, according to the probability
  rand = myRand->Uniform(0,TOTprob);  ///gRandom->Uniform(0,TOTprob); // RANDOM NUMBER !! (full event)
  float probSTART = 0;
  idx=0;
  for(int i=0;i<ncomb;i++){
    if(rand < probSTART+prob[i]){
      idx = i;
      // assign the "bjets"
      for(int j=0;j<fNumJets;j++){
        if(comb[i][j]){
          fJetIsTag[j][fBtagReqWPidx] = 1;
        }
      }
      break;
    }
    probSTART += prob[i];
  }
  if(fDebug){
    cout << " Total probability: " << TOTprob << endl;
    cout << " Random number: " << rand << endl;
    cout << " Considered combinations: " << endl;
    for(int i=0;i<ncomb;i++){
      cout << "  ";
      for(int j=0;j<fNumJets;j++) cout << comb[i][j] << " ";
      cout << " " << prob[i];
      if(i==idx) cout << " x";
      cout << endl;
    }
  }
  // now:
  //  - fJetIsTag[][fBtagReqWPidx] contains the chosen b-tag configuration for the chosen w.p.
  //
  // -- step 3 --
  // get the b-tag configuration for the other working points
  for(int j=0;j<fNumJets;j++){
    rand = myRand->Uniform(0,1);  //gRandom->Uniform(0,1); // RANDOM NUMBER !! (per jet)
    //cout << " jet: " << j << "  has random: " << rand << endl;
    // loop on the higher w.p.'s (if any) [from the highest back, to save time...]
    for(int wp_index=fJetBtagEff[j].size()-1;wp_index>=0;wp_index--){
      if ( wp_index==fBtagReqWPidx ) continue; // for the default WP the decision is already set.
      // now on I will assume the WP are ordered from tightest to loosest
      
      // looser WP .....
      if ( wp_index>fBtagReqWPidx ) { 
        if(fJetIsTag[j][fBtagReqWPidx]) {
          // those are looser WP so are passed by default!!!
          fJetIsTag[j][wp_index] = 1;
        } else {
          // this is the probability of the tagweight to be smaller than the cut i.e. fail
          // so if rand is bigger .... it passes
          float theProb=(1-fJetBtagEff[j][wp_index])/(1-fJetBtagEff[j][fBtagReqWPidx]);
          if (rand>theProb) fJetIsTag[j][wp_index] = 1;
        }
          } else {
        if(!fJetIsTag[j][fBtagReqWPidx]) {
          // those are tighter WP so are failed by default!!!
          fJetIsTag[j][wp_index] = 0;
        } else {
          // this is the probability of having a valus higher than the cut (i.e.) to pass
          // if rand is smaller .... it pass
          float theProb=fJetBtagEff[j][wp_index] / fJetBtagEff[j][fBtagReqWPidx];
          if (rand<theProb) fJetIsTag[j][wp_index] = 1;
          else              fJetIsTag[j][wp_index] = 0;
        }
      }
    }
  }
  for(int i=0;i<pow((double)2,(double)fNumJets);i++) delete comb[i];
  delete comb;
}

// Utility: Combinations
// ------------------------------------------------------------
// bool **comb = new bool*[pow(2,num_jets)];
// for(int i=0;i<pow(2,num_jets);i++){
//  comb[i] = new bool[num_jets];
// }
// ------------------------------------------------------------
int Combinations(const int num_jets,bool **comb,int min_btags,bool inclusive){
  bool val;
  int bt;
  int ncomb = (int)pow((double)2,(double)num_jets);
//  bool combTEMP[(int)pow((double)2,(double)num_jets)][num_jets];
  bool **combTEMP = new bool*[(int)pow((double)2,(double)num_jets)];
  for (int i = 0; i < (int)pow((double)2,(double)num_jets); ++i)
    combTEMP[i] = new bool[num_jets];
  //

  int ii = 0;

  for(int i=0;i<ncomb;i++){
    bt = 0;
    for(int j=0;j<num_jets;j++){
      val = (i & 1<<(num_jets-j-1));
      if(val) bt ++;
      combTEMP[i][j] = val;
    }
    if((inclusive && bt>=min_btags) || (!inclusive && bt==min_btags) ){
      for(int j=0;j<num_jets;j++){
        comb[ii][j] = combTEMP[i][j];
      }
      ii++;
    }
  }

  //
  for (int i = 0; i < (int)pow((double)2,(double)num_jets); ++i)
    delete[] combTEMP[i];
  delete[] combTEMP;
  //

  return ii;
}
// ---------------------------------------------------------------

// Get TRF weight, based on required b-tags
float CustomTRF::GetTRFweight(){
  return fTRFweight;
}

bool CustomTRF::GetJetIsTagged(int jet_index, int wp_index){
  return fJetIsTag[jet_index][wp_index]>0;
}

bool CustomTRF::GetJetIsTagged(int jet_index, string wp_name){
  int wp_index = find(fWorkingPoint.begin(),fWorkingPoint.end(),wp_name)-fWorkingPoint.begin();
  return GetJetIsTagged(jet_index, wp_index);
}

int CustomTRF::GetNumBtagJets(int wp_index){
  int num = 0;
  for(int j=0;j<fNumJets;j++){
    if(fJetIsTag[j][wp_index]>0) num ++;
  }
  return num;
}

int CustomTRF::GetNumBtagJets(string wp_name){
  int wp_index = find(fWorkingPoint.begin(),fWorkingPoint.end(),wp_name)-fWorkingPoint.begin();
  return GetNumBtagJets(wp_index);
}

int CustomTRF::GetSumBtagBins(){
  int sum = 0;
  for(int j=0;j<fNumJets;j++){
    for(int wp_index=0;wp_index<(int)fJetBtagEff[j].size();wp_index++){
      if(fJetIsTag[j][wp_index]>0) sum ++;
    }
  }
  return sum;
}

//float CustomTRF::GetJetDiscreteBtagWeight(int jet_index, int wp_index){
//  return 
//}

//float CustomTRF::GetJetDiscreteBtagWeight(int jet_index, string wp_name){
//  
//}

void CustomTRF::SetDebug(bool debug){
  fDebug = debug;
}

void CustomTRF::PrintJets(){
  for(int jet_index=0;jet_index<(int)fJetBtagEff.size();jet_index++){
    cout << " jet[" << jet_index << "]:";
    for(int wp_index=0;wp_index<(int)fJetBtagEff[jet_index].size();wp_index++){
      cout << "\t" << fJetBtagEff[jet_index][wp_index];
    }
    cout << endl;
  }
}

void CustomTRF::PrintChosenConfig(){
  for(int jet_index=0;jet_index<(int)fJetBtagEff.size();jet_index++){
    cout << " jet[" << jet_index << "]:";
    for(int wp_index=0;wp_index<(int)fJetBtagEff[jet_index].size();wp_index++){
      if(fJetIsTag[jet_index][wp_index]) cout << "\tx";
      else cout << "\t-";
    }
    cout << endl;
  }
}
