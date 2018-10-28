#include <stdio.h>
#include <math.h>
#include <string>
#include "TTHbbObjects/Event.h"
#include "BucketofTops/BucketofTops.h"
#include "BucketofTops/Bucket.h"
using namespace std;

BucketofTops::BucketofTops(std::vector<shared_ptr<TTHbb::Jet> > specjets){
  double bucketP1massMax = 200; //GeV
  double bucketP1massMin = 155; //GeV
  double firstP1Bucketwt = 100;
  B = doublebucket(specjets, bucketP1massMax, bucketP1massMin, "tw", firstP1Bucketwt);
  vector <Bucket> tmincand; //fill tmin candidates
  int telseindex; //tw or t0 bucket
  for (int i = 0; i < B.size(); ++i) {
    if (B[i].getBucketMass() > -1) {
      mW = B[i].WcandMnum();
	mBucketPrim = B[i].getBucketMass();
	mratio = B[i].WcandRatio();
    }
    if (B[i].getBucketLabel() == "t-") {
      tmincand.push_back(B[i]);
    }
    else {
      telseindex = i;
    }

    //redo both buckets for t-
    double bucketP2massMax = 155; //GeV
    double bucketP2massMin = 75; //GeV
    double firstP2Bucketwt = 1;

    if (tmincand.size() == 2) {
      B = doublebucket(specjets, bucketP2massMax, bucketP2massMin, "t-", firstP2Bucketwt);
    }

    else if (tmincand.size() == 1){
      B[1-telseindex] = singlebucket(specjets, B[telseindex], bucketP2massMax, bucketP2massMin);
    }

    return B;
  }
};

std::vector <shared_ptr<TTHbb::Jet> > compvec(std::vector <shared_ptr<TTHbb::Jet> > EVT, std::vector <shared_ptr<TTHbb::Jet> > set) //another bug
{
  std::vector <shared_ptr<TTHbb::Jet> > compset;
  for (int i = 0; i < EVT.size(); ++i)
  {
    bool findflag = false;
    for (int j = 0; j < set.size(); ++j)
    {
      findflag = findflag || (EVT[i]==set[j]); //check if the "==" works for TTHbb::Jet
    }
    if (!findflag) {compset.push_back(EVT[i]);}
  }
  return compset;
};

//power set generator
std::vector <std::vector <int> > BucketofTops::pSet(std::vector <int> set, int offset=0)
{
  //power set size : 2^n
  unsigned int pow_set_size = pow(2, set.size());
  int counter;
  std::vector <std::vector <int> > pVec;
  //Run counter from 000..1 and stop before 111..1 (111..1 excluded)
  for(counter = 1; counter < pow_set_size-offset; counter++)
  {
    std::vector <int> v;
    for(int j = 0; j < set.size(); j++)
    {
      //if jth bit in the counter is 1 print the jth element
      if(counter & (1<<j))
      v.push_back(set[j]);
    }
    pVec.push_back(v); 
    v.clear();
  }
  return pVec;
};

//complementary set generator
std::vector <int> BucketofTops::cSet(std::vector <int> set, std::vector <int> subset)
{
  std::vector <int> cVec;
  for (int i = 0; i < set.size(); ++i)
  {
     if (find(subset.begin(), subset.end(), set[i]) != subset.end())
     {
       continue;
     }
     cVec.push_back(set[i]);
  }
  return cVec;
};


//function for the extra jets
std::vector <shared_ptr<TTHbb::Jet> > extra(std::vector <shared_ptr<TTHbb::Jet> > EVT, std::vector <Bucket> B)
{
  std::vector <> Xmembers = EVT;
  for (int i = 0; i < B.size(); ++i)
  {
    Xmembers = compvec(Xmembers, B[i].members);
  }
  return Xmembers;
};

//function to get two top buckets
std::vector<Bucket> BucketofTops::doublebucket(std::vector<shared_ptr<TTHbb::Jet> > specjets, double MbucketMax, double MbucketMin, string target_label, double B1weight) {
  std::vector <Bucket> B; //B1 and B2
  Bucket B1, B2;
  std::vector<shared_ptr<TTHbb::Jet> > nonbjets, bjets;
  for (auto &jet: specjets){
    CHECK( jet->checkCharVariable("isbtagged_MV2c10_85") );
    bool isbtagged_MV2c10_85 = jet->charVariable("isbtagged_MV2c10_85") == 1;
    if (isbtagged_MV2c10_85) {bjets.push_back(jet);}
    else {nonbjets.push_back(jet);}
  }
  int nonbjetsize = nonbjets.size();
  std::vector <std::vector <int> > nonbindexset1;
  std::vector <int> nonbset;
  for (int j = 0; j < nonbjetsize; ++j) {nonbset.push_back(j);}
  if (target_label == "tw")
  {
    nonbindexset1 = pSet(nonbset); //no offset for tw
  }
  else //for t- pair search 
  {
    for (int l1 = 0; l1 < nonbjetsize; ++l1)
    {
      std::vector <int> tempPar1;
      tempPar1.push_back(nonbset[l1]);
      nonbindexset1.push_back(tempPar1);
      tempPar1.clear();
    }
  }
  double Deltatw = pow(10,10); //arbit large number
  for (int i = 0; i < nonbindexset1.size(); ++i) //looping over all possible buckets
  {
    vector <vector <int> > nonbindexset2;
    vector <int> restjetset = cSet(nonbset, nonbindexset1[i]);
    /*switching of second bucket's freedom for now//nonbindexset2 = pSet(restjetset); // no offset as all the jets can now be used*/
    if (target_label == "tw") 
    {
      nonbindexset2.push_back(restjetset);
    }
    else //for t- pair search
    {
      for (int l2 = 0; l2 < restjetset.size(); ++l2)
      {
        std::vector <int> tempPar2;
        tempPar2.push_back(restjetset[l2]);
        nonbindexset2.push_back(tempPar2);
        tempPar2.clear();
      }
    }
    std::vector<shared_ptr<TTHbb::Jet> > nonbA;
    for (int k = 0; k < nonbindexset1[i].size(); ++k)
    {
      nonbA.push_back(nonbjets[nonbindexset1[i][k]]);
    }
    for (int i1 = 0; i1 < nonbindexset2.size(); ++i1)
    {
      std::vector<shared_ptr<TTHbb::Jet> >  nonbB;
      for (int k1 = 0; k1 < nonbindexset2[i1].size(); ++k1)
      {
        nonbB.push_back(nonbjets[nonbindexset2[i1][k1]]);
      }
      Bucket Afirst(nonbA, bjets[0]);
      double AfirstDistance = (target_label == "tw") ? Afirst.twOptMetric() : Afirst.tminusOptMetric();
      Bucket Asecond(nonbA, bjets[1]);
	double AsecondDistance = (target_label == "tw") ? Asecond.twOptMetric() : Asecond.tminusOptMetric();
      Bucket Bfirst(nonbB, bjets[0]);
	double BfirstDistance = (target_label == "tw") ? Bfirst.twOptMetric() : Bfirst.tminusOptMetric();
      Bucket Bsecond(nonbB, bjets[1]);
	double BsecondDistance = (target_label == "tw") ? Bsecond.twOptMetric() : Bsecond.tminusOptMetric();
      
      double del1 = (B1weight*AfirstDistance) + BsecondDistance;
      double del2 = (B1weight*BfirstDistance) + AsecondDistance;
      double del3 = AfirstDistance + (B1weight*BsecondDistance);
      double del4 = BfirstDistance + (B1weight*AsecondDistance);
      if ((del1 < del2) && (del1 < del3) && (del1 < del4))
      {
        if (del1 < Deltatw)
        {
          Deltatw = del1;
          B1 = Afirst;
          B2 = Bsecond;
        }
      }
      else if ((del2 < del1) && (del2 < del3) && (del2 < del4))
      {
        if (del2 < Deltatw)
        {
          Deltatw = del2;
          B1 = Bfirst;
          B2 = Asecond;
        }
      }
      else if ((del3 < del1) && (del3 < del2) && (del3 < del4))
      {
        if (del3 < Deltatw)
        {
          Deltatw = del3;
          B1 = Bsecond;
          B2 = Afirst;
        }
      }
      else 
      {
        if (del4 < Deltatw)
        {
          Deltatw = del4;
          B1 = Asecond;
          B2 = Bfirst;
        }
      }
    }
  } // loop over all possible buckets ends
  B.push_back(B1);
  B.push_back(B2);
  for (int i = 0; i < B.size(); ++i)
  {
    string label; //label assignement , 
    double Bm = B[i].getBucketMass();
    if (target_label == "tw")
    {
      label = (B[i].twflag())?"tw":"t-";
    }
    else {label = "t-";}
    if ((Bm > MbucketMax) || (Bm < MbucketMin))
    {
      label = "t0";
    }
    B[i].setBucketLabel(label);
  }
  return B;
};

//function to get one top bucket
Bucket singlebucket(std::std::vector<shared_ptr<TTHbb::Jet> > specjets, Bucket twbucket, double MbucketMax, double MbucketMin)
{

  Bucket newbucket; 
  double Deltamin = pow(10,10); //arbit large number
  std::std::vector<shared_ptr<TTHbb::Jet> > nonbjets, bjets;
  for (auto &jet: specjets){
  CHECK( jet->checkCharVariable("isbtagged_MV2c10_85") );
  bool isbtagged_MV2c10_85 = jet->charVariable("isbtagged_MV2c10_85") == 1;
  if (isbtagged_MV2c10_85) {bjets.push_back(jet);}
  else {nonbjets.push_back(jet);}
}

  shared_ptr<TTHbb::Jet> bucketBjet = (bjets[0] == twbucket.BJET) ? bjets[1] : bjets[0]; //bjet to be used in the bucket

  int Evnonbjetsize = nonbjets.size();
  int twnonbjetsize = twbucket.nonBJETS.size();
  std::vector <int> plist = twbucket.getPIDlist();
  for (int i = 0; i < Evnonbjetsize; ++i)
  {
    bool findtw = false;
    for (int j = 0; j < twnonbjetsize; ++j)
    {
      bool findtemp = (nonbjets[i]==twbucket.nonBJETS[j]); //particle part of tw bucket; can be added to extra
      findtw = findtw || findtemp;
    }
    if (!findtw)  //no match
    {
      std::vector <shared_ptr<TTHbb::Jet> > tempnb;
      tempnb.push_back(nonbjets[i]);
      Bucket tempbucket(tempnb, bucketBjet);
      if (Deltamin > tempbucket.tminusOptMetric())
      {
        Deltamin = tempbucket.tminusOptMetric();
        newbucket = tempbucket;
      }
    }
  }
  
  //label assignement
  double massbucket = newbucket.getBucketMass();
  string label = ((massbucket < MbucketMax) && (massbucket  > MbucketMin)) ? "t-" : "t0";
  newbucket.setBucketLabel(label);
  return newbucket;
};

