/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CUSTOMTRF_H_
#define _CUSTOMTRF_H_

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TRandom3.h"

using namespace std;

class CustomTRF : public TObject{
  public:
    CustomTRF();
    virtual ~CustomTRF();
    
    void SetWorkingPoints(vector<string> wp_names);
    void AddWorkingPoint(string name);
    
    void SetJetBtagEff(int jet_index,int wp_index,float eff);
    void SetJetBtagEff(int jet_index,string wp_name,float eff);
    void SetJetBtagEffs(int jet_index,vector<float> eff_vec);
    void SetJetsBtagEff(int wp_index,vector<float> eff_vec);
    void SetJetsBtagEff(string wp_name,vector<float> eff_vec);
    void SetJetsBtagEffs(vector<vector<float> > eff_vec);

    void SetBtagRequirement(int number_of_tags, bool inclusive, int wp_index=0);
    void SetBtagRequirement(int number_of_tags, bool inclusive, string wp_name);
    
    void Reset();
    void Evaluate(int EventNumber=0);

    float GetTRFweight();
    
    bool GetJetIsTagged(int jet_index,int wp_index=0);
    bool GetJetIsTagged(int jet_index,string wp_name);
    
    int GetNumBtagJets(int wp_index=0);
    int GetNumBtagJets(string wp_name);
    
    int GetSumBtagBins();
    float GetChosenPermutationProb();
    float GetPermutationProb(bool *perm);
    bool* GetChosenPermutation();

    //    float GetJetDiscreteBtagWeight(int wp_index);
    //    float GetJetDiscreteBtagWeight(string wp_name);

    // ...
    
    void SetDebug(bool debug=true);
    void PrintJets();
    void PrintChosenConfig();

  private:
    vector<string>         fWorkingPoint; // [wp_index]
    vector<vector<float> > fJetBtagEff;   // [jet_index][wp_index]
    vector<vector<float> > fJetIsTag;     // [jet_index][wp_index] // FIXME:  vector<vector<bool> > creating problems...
    
    int fNumJets;
    int fNumReqBtag;
    bool fBtagReqIncl;
    int fBtagReqWPidx;
    float fTRFweight;
    
    bool fDebug;  
    TRandom3* myRand;
    
    // for chosen permutation
    vector<float> prob;
    int idx;
    bool **comb;
    vector<bool> tempTest; //nasty hack to keep track of the previous number of permutations
};

// Utility
int Combinations(const int num_jets,bool **comb,int min_btags,bool inclusive);

#endif
