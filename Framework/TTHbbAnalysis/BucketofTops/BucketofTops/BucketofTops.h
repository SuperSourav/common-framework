
#ifndef BucketofTops_HH
#define BucketofTops_HH

#include <stdio.h>
#include <math.h>
#include <string>
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/Bucket.h"
#include "PathResolver/PathResolver.h"

class BucketofTops {
  
  public:
    BucketofTops(std::vector<shared_ptr<TTHbb::Jet> > specbjets, std::vector<shared_ptr<TTHbb::Jet> > specnonbjets);
    ~BucketofTops();
    double mTop = 173.5; //GeV
    double mW = 80.4; //GeV
    std::vector<TTHbb::Bucket> B;
    std::vector <shared_ptr<TTHbb::Jet> > compvec(std::vector <shared_ptr<TTHbb::Jet> > EVT, std::vector <shared_ptr<TTHbb::Jet> > set);
    std::vector <std::vector <int> > pSet(std::vector <int> set, int offset);
    std::vector <int> cSet(std::vector <int> set, std::vector <int> subset);
    std::vector <shared_ptr<TTHbb::Jet> > extra(std::vector <shared_ptr<TTHbb::Jet> > EVT, std::vector <TTHbb::Bucket> B);
    std::vector<TTHbb::Bucket> doublebucket(std::vector<shared_ptr<TTHbb::Jet> > specbjets, std::vector<shared_ptr<TTHbb::Jet> > specnonbjets, double MbucketMax, double MbucketMin, string target_label, double B1weight);
    TTHbb::Bucket singlebucket(std::vector<shared_ptr<TTHbb::Jet> > specbjets, std::vector<shared_ptr<TTHbb::Jet> > specnonbjets, TTHbb::Bucket twbucket, double MbucketMax, double MbucketMin);
  private:

};

#endif
