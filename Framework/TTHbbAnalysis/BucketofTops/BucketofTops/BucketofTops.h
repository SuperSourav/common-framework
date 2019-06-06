
#ifndef BucketofTops_HH
#define BucketofTops_HH

#include <stdio.h>
#include <math.h>
#include <string>
//#include "TTHbbObjects/Event.h"
#include "BucketofTops/bucket_all.h"
//#include "PathResolver/PathResolver.h"

class BucketofTops {
  
  public:
    bucketAlgo::bucketpairs allbucketpairstw; // includes the intermediates too
    bucketAlgo::bucketpairs allbucketpairstmin; // includes the intermediates too
    std::vector<bucketAlgo::bucket> Blist; //
    std::vector<TLorentzVector> Xtra; //
    std::vector<float> mWcand, mBucketPrim, mratio; //GeV
    std::vector<float> twmass, twPt, tweta; //GeV
    std::vector<float> tminmass, tminPt, tmineta; //GeV
    std::vector<float> t0mass, t0Pt, t0eta; //GeV
    std::vector<float> Xmass, XPt, Xeta; //GeV
    float twcount, tmincount, t0count; //per event
    int nonbinitcount, nonb1count, nonb2count, nonbISRcount; //per event
    TLorentzVector B1bjet, B2bjet; //per event
    vector<TLorentzVector> B1nonbjets, B2nonbjets; //per event

    //**//
    BucketofTops(std::vector<TLorentzVector> specbjets, std::vector<TLorentzVector> specnonbjets);
    //**//BucketofTops(std::vector<TLorentzVector> specbjets, std::vector<TLorentzVector> specnonbjets, TLorentzVector lepton);
    ~BucketofTops();

    std::vector<bucketAlgo::bucket> * returnbucketlistptr();

    //
  private:

};

#endif
