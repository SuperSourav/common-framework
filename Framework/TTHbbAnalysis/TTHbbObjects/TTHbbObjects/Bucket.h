#ifndef _BUCKET_H_
#define _BUCKET_H_

#include <stdio.h>
#include <math.h>
#include <string>
#include "TLorentzVector.h"
#include "TTHbbObjects/Jet.h"
#include "TTHbbObjects/Event.h"

using namespace std;

namespace TTHbb{
  class Bucket
  {
    private:
    public:
      string bucket_label;
      double Mbucket, pTbucket, etabucket;
      double mpairnum, ratio_min;
      std::vector<shared_ptr<Jet> > members;
      std::vector<shared_ptr<Jet> > nonBJETS;
      shared_ptr<Jet> BJET;
      double mTop = 173.5; //GeV
      double mW = 80.4; //GeV
  
    ~Bucket() {} //destructor
  
    Bucket()
    {
      bucket_label = "tx"; //tx means label unassigned
      Mbucket = -9999; //GeV
      pTbucket = -9999; //GeV
      etabucket = -9999;
      mpairnum = -9999; //GeV
    }
  
    Bucket(std::vector <shared_ptr<TTHbb::Jet> > nonbjets, shared_ptr<TTHbb::Jet> bjet)
    {
      nonBJETS = nonbjets;
      BJET = bjet;
      members.push_back(bjet);
      TLorentzVector b = bjet->p4();
      for(int i =0; i < nonbjets.size(); ++i)
      {
        b = b + nonbjets[i]->p4();
        members.push_back(nonbjets[i]);
      }
      Mbucket = b.M();
      pTbucket = b.Pt();
      etabucket = b.Et();
    } 
  
    void setBucketLabel(string label)  {bucket_label=label;}
  
    string getBucketLabel() {return bucket_label;}
  
    double getBucketMass() {return Mbucket;}
  
    double getBucketPt() {return pTbucket;}
  
    double getBucketEta() {return etabucket;}
  
    bool twflag() //true : tw ; false : not tw
    {
      ratio_min = pow(10, 10); 
      bool flag = false; //defalt bucket not tw
      int sizeB = members.size();
  
      for (int i = 0; i < sizeB; ++i)
      {
        for (int j = 0; j < sizeB; ++j)
        {
          if (j < i) {continue;}
          TLorentzVector temp;
         	temp = members[i]->p4() + members[j]->p4();
          double dd = abs((temp.M()/Mbucket) - (mW/mTop));
          if (ratio_min > dd) //
          {
            ratio_min = dd;
            mpairnum = temp.M();
          }
        }
      }
      //^//cout << "final_mass_ratio: " << ratio_min << endl;
      if ( (ratio_min) < 0.15 ) {flag = true;}
      return flag;
    }
  
    double WcandMnum() {return mpairnum;}
   
    double WcandRatio() {return ratio_min;}
  
    double twOptMetric() {return (Mbucket - mTop)*(Mbucket - mTop);}
  
    double tminusOptMetric()
    {
      if (Mbucket > 155.0) {return pow(10,10);}
      else {return std::abs(Mbucket - 145.0);}
    }
  
  };

}

#endif
