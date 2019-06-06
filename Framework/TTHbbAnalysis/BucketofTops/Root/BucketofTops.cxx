#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>
//#include "BucketofTops/bucket_all.h"
#include "BucketofTops/BucketofTops.h"
using namespace std;
//**//
//BucketofTops::BucketofTops(std::vector<TLorentzVector> specbjets, std::vector<TLorentzVector> specnonbjets, TLorentzVector lepton){
//**//
BucketofTops::BucketofTops(std::vector<TLorentzVector> specbjets, std::vector<TLorentzVector> specnonbjets){
//**//
  double bucketP1massMax = 200; //GeV
  double bucketP1massMin = 155; //GeV
  double firstP1Bucketwt2 = 1;
  //double firstP1Bucketwt2 = 100;
  //double firstP1Bucketwt1 = 1;
  //double firstP1Bucketwt1 = 10;
  double firstP1Bucketwt1 = 100;
  //double firstP1Bucketwt1 = 1000;
  ///////std::cout << "inside buckets constructor " << specbjets.size() << std::endl;
  //for(auto tesbtjet : specbjets){
  //  std::cout << "print b jet px: " << tesbtjet.Px() << "\tpy: " << tesbtjet.Py() << "\tpz: " << tesbtjet.Pz() << "\tE: " << tesbtjet.E() << std::endl;
  //}
  //for(auto testjet: specnonbjets){
  //  std::cout << "print jet px: " << testjet.Px() << "\tpy: " << testjet.Py() << "\tpz: " << testjet.Pz() << "\tE: " << testjet.E() << std::endl;
  //}
  //**//
  //
  nonbinitcount = specnonbjets.size();
  //
  //
  allbucketpairstw = bucketAlgo::doublebucket(specbjets, specnonbjets, bucketP1massMax, bucketP1massMin, "tw", firstP1Bucketwt1, firstP1Bucketwt2);
  Blist = allbucketpairstw.Bpairs[allbucketpairstw.solutionIndex];

  vector <bucketAlgo::bucket> tmincand; //fill tmin candidates
  int telseindex; //tw or t0 bucket
  //
  B1bjet = Blist[0].BJET;
  B2bjet = Blist[1].BJET;
  B1nonbjets = Blist[0].nonBJETS;
  B2nonbjets = Blist[1].nonBJETS;

  nonb1count = Blist[0].nonBJETS.size();
  nonb2count = Blist[1].nonBJETS.size();
  nonbISRcount = nonbinitcount - (nonb1count + nonb2count);
  //
  for (int i = 0; i < Blist.size(); ++i) {
    if (Blist[i].getBucketMass() > -1) {
      mWcand.push_back(Blist[i].WcandMnum());
      mBucketPrim.push_back(Blist[i].getBucketMass());
      mratio.push_back(Blist[i].WcandRatio());
    }
    if (Blist[i].getBucketLabel() == "t-") {
      tmincand.push_back(Blist[i]);
    }
    else {
      telseindex = i;
    }
  }


  redo both buckets for t-
  double bucketP2massMax = 155; GeV
  double bucketP2massMin = 75; GeV
  double firstP2Bucketwt1 = 1;
  double firstP2Bucketwt2 = 1;

  std::vector<TLorentzVector> specnonbjetsWithLep = specnonbjets;
  **
  specnonbjetsWithLep.push_back(lepton);
  **

  if (tmincand.size() == 2) {
    allbucketpairstmin = bucketAlgo::doublebucket(specbjets, specnonbjets, bucketP1massMax, bucketP1massMin, "t-", firstP1Bucketwt1, firstP1Bucketwt2);
    Blist = allbucketpairstmin.Bpairs[allbucketpairstmin.solutionIndex];
  }

  else if (tmincand.size() == 1){
    Blist[1-telseindex] = bucketAlgo::singlebucket(specbjets, specnonbjetsWithLep, Blist[telseindex], bucketP2massMax, bucketP2massMin);
  }

  twcount = 0;
  tmincount = 0;
  t0count = 0;
  for (int i = 0; i < Blist.size(); ++i)
  {
    if (Blist[i].getBucketLabel() == "tw") {
      if (Blist[i].getBucketMass() > -1) {
	twcount++;
	std::cout << "i: " << i << "twcount: " << twcount << std::endl;
        twmass.push_back(Blist[i].getBucketMass());
        twPt.push_back(Blist[i].getBucketPt());
        tweta.push_back(Blist[i].getBucketEta()); 
      }
    }
    else if (Blist[i].getBucketLabel() == "t-") {
      if (Blist[i].getBucketMass() > -1) {
	tmincount++;
        tminmass.push_back(Blist[i].getBucketMass());
        tminPt.push_back(Blist[i].getBucketPt());
        tmineta.push_back(Blist[i].getBucketEta());
      }
    }
    else {
      if (Blist[i].getBucketMass() > -1) {
	t0count++;
        t0mass.push_back(Blist[i].getBucketMass());
        t0Pt.push_back(Blist[i].getBucketPt());
        t0eta.push_back(Blist[i].getBucketEta());
      }
    }
  }
  std::cout << "twcount final: " << twcount << std::endl;
  

  Xtra = bucketAlgo::extra(specbjets, specnonbjets, Blist);  extra bucket

  for (int nn=0; nn < Xtra.size(); ++nn)
  {
    Xmass.push_back(Xtra[nn].M());
    XPt.push_back(Xtra[nn].Pt());
    Xeta.push_back(Xtra[nn].Eta());
  } 

}

std::vector<bucketAlgo::bucket> * BucketofTops::returnbucketlistptr(){
  std::vector<bucketAlgo::bucket> * Blistptr = new  std::vector<bucketAlgo::bucket>;
  *Blistptr = Blist;
  return Blistptr;
}

BucketofTops::~BucketofTops(){}
