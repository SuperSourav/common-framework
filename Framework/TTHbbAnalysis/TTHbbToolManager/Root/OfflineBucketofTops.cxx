#include "TTHbbToolManager/OfflineBucketofTops.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include <iostream>
#include <string>

namespace TTHbb{

  OfflineBucketofTops::OfflineBucketofTops(std::string name) : ToolBase(true),
  m_buckets(nullptr)
  {
    m_name = name;
    auto * config = TTHbb::GlobalConfiguration::get();
    m_DoBuckets = TTHbb::util::iequals((*config)(m_name + ".UseBucketsAlgo"), "true");
    std::cout << "dobuckets flag being set to : " << m_DoBuckets << std::endl;
  }

  OfflineBucketofTops::~OfflineBucketofTops(){
    if (m_buckets)
      delete m_buckets;
  }

  void OfflineBucketofTops::initialise(){}

  void OfflineBucketofTops::finalise(){}

  void OfflineBucketofTops::apply(TTHbb::Event* event){
    auto rcjets = event->m_customObj["rcjet"];
    std::vector<TLorentzVector> specbjets, specnonbjets; //for now using all the jets as spectators
    //std::cout << "rcjetcontainer: " << rcjets << std::endl;
    for(auto jet : event->m_jets){
      bool ORfromRC = true;
      for (auto rcjet : rcjets){
        std::cout << "*********************************** inside overlap removal loop *******************" << std::endl;
        TLorentzVector j = jet->p4();
        TLorentzVector rcj = rcjet->p4();
        std::cout << "overlap dR: " << rcj.DeltaR(j) << std::endl;
        if (rcj.DeltaR(j) < 1.0) {ORfromRC = false; break;}
      }
      if (ORfromRC){
        bool isbtagged_MV2c10_77 = jet->charVariable("isbtagged_MV2c10_77");
        if (isbtagged_MV2c10_77) 
        {
          TLorentzVector vecb = jet->p4();
          specbjets.push_back(vecb*(0.001));} //converting MeV to GeV
        else 
        {
          TLorentzVector vecnonb = jet->p4();
          specnonbjets.push_back(vecnonb*(0.001)); //converting MeV to GeV
        }
      }
    } 
    std::cout << specbjets.size() << std::endl;
    for(auto btjet : specbjets){
      std::cout << "print b jet px: " << btjet.Px() << "\tpy: " << btjet.Py() << "\tpz: " << btjet.Pz() << "\tE: " << btjet.E() << std::endl;
    }
    for(auto tjet : specnonbjets){
      std::cout << "print nonb jet px: " << tjet.Px() << "\tpy: " << tjet.Py() << "\tpz: " << tjet.Pz() << "\tE: " << tjet.E() << std::endl;
    }
    std::cout << m_DoBuckets << std::endl;
    std::cout << (m_DoBuckets and (specbjets.size() == 2)) << std::endl;
    if (m_DoBuckets and (specbjets.size() == 2)) {
    //if (m_DoBuckets) {
      std::cout << "inside DoBuckets before call" << std::endl;
      m_buckets = new BucketofTops(specbjets, specnonbjets);
      std::vector<bucketAlgo::bucket> bucklist = m_buckets->Blist;
      std::cout << "buclet list size: " << bucklist.size() << std::endl;
      std::cout << "init bucket mass: " << bucklist[0].getBucketMass() << "\t" << bucklist[1].getBucketMass() << std::endl;
      std::cout << "init bucket label: " << bucklist[0].getBucketLabel() << "\t" << bucklist[1].getBucketLabel() << std::endl;
      std::cout << "init bucket pT: " << bucklist[0].getBucketPt() << "\t" << bucklist[1].getBucketPt() << std::endl;
      std::cout << "init bucket eta: " << bucklist[0].getBucketEta() << "\t" << bucklist[1].getBucketEta() << std::endl;
      std::cout << "init bucket WcandMnum(): " << bucklist[0].WcandMnum() << "\t" << bucklist[1].WcandMnum() << std::endl;

      if (m_buckets == NULL) {std::cout << "bucket ptr is null" << std::endl;}
        std::vector<bucketAlgo::bucket>& blistp = *m_buckets->returnbucketlistptr();
        std::cout << "get blist size: " << blistp.size() << std::endl;
	std::cout <<"get blist mass: " << blistp[0].getBucketMass()  << "\t" << blistp[1].getBucketMass()  << std::endl;
        std::cout <<"get blist label: " << blistp[0].getBucketLabel()  << "\t" << blistp[1].getBucketLabel()  << std::endl;
        std::cout <<"get blist pT: " << blistp[0].getBucketPt()  << "\t" << blistp[1].getBucketPt()  << std::endl;
	std::cout <<"get blist eta: " << blistp[0].getBucketEta()  << "\t" << blistp[1].getBucketEta()  << std::endl;
	std::cout <<"get blist WcandMnum(): " << blistp[0].WcandMnum() << "\t" << blistp[1].WcandMnum() << std::endl;
      event->intVariable("blistsize") = blistp.size();
      event->floatVariable("BucketMass1") = blistp[0].getBucketMass();
      event->floatVariable("BucketMass2") = blistp[1].getBucketMass();
      event->floatVariable("BucketPt1") = blistp[0].getBucketPt();
      event->floatVariable("BucketPt2") = blistp[1].getBucketPt();
      event->floatVariable("BucketEta1") = blistp[0].getBucketEta();
      event->floatVariable("BucketEta2") = blistp[1].getBucketEta();
      event->floatVariable("WcandMnum1") = blistp[0].WcandMnum();
      event->floatVariable("WcandMnum2") = blistp[1].WcandMnum();

      std::cout << "inside DoBuckets after call" << std::endl;
      //tbuck = m_buckets->B;
      //event->intVariable("B1mass") = tbuck.size();
      event->floatVariable("twcount") = m_buckets->twcount;
      event->floatVariable("t0count") = m_buckets->t0count;
      event->floatVariable("tmincount") = m_buckets->tmincount;

      std::vector<float> m_mW = m_buckets->mWcand;
      for(unsigned int iTarget = 0; iTarget < m_mW.size(); ++iTarget ) {
        std::string thisVarToSave = "mW"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_mW.at(iTarget);
      } 
      std::vector<float> m_mBucketPrim = m_buckets->mBucketPrim;
      for(unsigned int iTarget = 0; iTarget < m_mBucketPrim.size(); ++iTarget ) {
        std::string thisVarToSave = "mBucketPrim"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_mBucketPrim.at(iTarget);
      } 
      std::vector<float> m_mratio = m_buckets->mratio;
      for(unsigned int iTarget = 0; iTarget < m_mratio.size(); ++iTarget ) {
        std::string thisVarToSave = "mratio"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_mratio.at(iTarget);
      } 
      std::vector<float> m_twmass = m_buckets->twmass;
      for(unsigned int iTarget = 0; iTarget < m_twmass.size(); ++iTarget ) {
        std::string thisVarToSave = "twmass"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_twmass.at(iTarget);
      } 
      std::vector<float> m_twPt = m_buckets->twPt;
      for(unsigned int iTarget = 0; iTarget < m_twPt.size(); ++iTarget ) {
        std::string thisVarToSave = "twPt"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_twPt.at(iTarget);
      } 
      std::vector<float> m_tweta = m_buckets->tweta;
      for(unsigned int iTarget = 0; iTarget < m_tweta.size(); ++iTarget ) {
        std::string thisVarToSave = "tweta"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_tweta.at(iTarget);
      } 
      std::vector<float> m_tminmass = m_buckets->tminmass;
      for(unsigned int iTarget = 0; iTarget < m_tminmass.size(); ++iTarget ) {
        std::string thisVarToSave = "tminmass"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_tminmass.at(iTarget);
      } 
      std::vector<float> m_tminPt = m_buckets->tminPt;
      for(unsigned int iTarget = 0; iTarget < m_tminPt.size(); ++iTarget ) {
        std::string thisVarToSave = "tminPt"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_tminPt.at(iTarget);
      } 
      std::vector<float> m_tmineta = m_buckets->tmineta;
      for(unsigned int iTarget = 0; iTarget < m_tmineta.size(); ++iTarget ) {
        std::string thisVarToSave = "tmineta"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_tmineta.at(iTarget);
      } 
      std::vector<float> m_t0mass = m_buckets->t0mass;
      for(unsigned int iTarget = 0; iTarget < m_t0mass.size(); ++iTarget ) {
        std::string thisVarToSave = "t0mass"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_t0mass.at(iTarget);
      } 
      std::vector<float> m_t0Pt = m_buckets->t0Pt;
      for(unsigned int iTarget = 0; iTarget < m_t0Pt.size(); ++iTarget ) {
        std::string thisVarToSave = "t0Pt"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_t0Pt.at(iTarget);
      } 
      std::vector<float> m_t0eta = m_buckets->t0eta;
      for(unsigned int iTarget = 0; iTarget < m_t0eta.size(); ++iTarget ) {
        std::string thisVarToSave = "t0eta"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_t0eta.at(iTarget);
      } 
      std::vector<float> m_Xmass = m_buckets->Xmass;
      for(unsigned int iTarget = 0; iTarget < m_Xmass.size(); ++iTarget ) {
        std::string thisVarToSave = "Xmass"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_Xmass.at(iTarget);
      } 
      std::vector<float> m_XPt = m_buckets->XPt;
      for(unsigned int iTarget = 0; iTarget < m_XPt.size(); ++iTarget ) {
        std::string thisVarToSave = "XPt"+std::to_string(iTarget); 
	cout << "var_name: " << thisVarToSave;
	event->floatVariable(thisVarToSave) = m_XPt.at(iTarget);
      } 
      std::vector<float> m_Xeta = m_buckets->Xeta;
      for(unsigned int iTarget = 0; iTarget < m_Xeta.size(); ++iTarget ) {
        std::string thisVarToSave = "Xeta"+std::to_string(iTarget); 
        cout << "var_name: " << thisVarToSave;
        event->floatVariable(thisVarToSave) = m_Xeta.at(iTarget);
      } 
    }
    std::cout << "HELLO WORLD FROM BUCKETS" << std::endl;
  }
}
