#include <stdio.h>
#include <math.h>
#include <string>
#include "TTHbbObjects/Event.h"

class BucketofTops {
  
  public:
    class Bucket; //forward declare Bucket
    BucketofTops(std::vector<shared_ptr<TTHbb::Jet> > specjets);
    ~BucketofTops();
    double mTop = 173.5; //GeV
    double mW = 80.4; //GeV
    std::vector<Bucket> B;
    std::vector <shared_ptr<TTHbb::Jet> > compvec(std::vector <shared_ptr<TTHbb::Jet> > EVT, std::vector <shared_ptr<TTHbb::Jet> > set);
    std::vector <std::vector <int> > BucketofTops::pSet(std::vector <int> set, int offset=0);
    std::vector <int> BucketofTops::cSet(std::vector <int> set, std::vector <int> subset);
    std::vector <shared_ptr<TTHbb::Jet> > extra(std::vector <shared_ptr<TTHbb::Jet> > EVT, std::vector <Bucket> B);
    std::vector<Bucket> doublebucket(std::vector<Bucket> specjets, double MbucketMax, double MbucketMin, string target_label, double B1weight);
    Bucket singlebucket(std::std::vector<shared_ptr<TTHbb::Jet> > specjets, Bucket twbucket, double MbucketMax, double MbucketMin);
  private:

};

#endif
