
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
    std::vector<bucketAlgo::bucket> Blist; //
    std::vector<TLorentzVector> Xtra; //
    std::vector<float> mWcand, mBucketPrim, mratio; //GeV
    std::vector<float> twmass, twPt, tweta; //GeV
    std::vector<float> tminmass, tminPt, tmineta; //GeV
    std::vector<float> t0mass, t0Pt, t0eta; //GeV
    std::vector<float> Xmass, XPt, Xeta; //GeV
    float twcount, tmincount, t0count; //per event

    BucketofTops(std::vector<TLorentzVector> specbjets, std::vector<TLorentzVector> specnonbjets);
    ~BucketofTops();

    std::vector<bucketAlgo::bucket> * returnbucketlistptr();

    //
  private:

};

#endif
