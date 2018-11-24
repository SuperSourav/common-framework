#include "TTHbbToolManager/OfflineBucketofTops.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include <iostream>

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
    std::vector<TLorentzVector> specbjets, specnonbjets; //for now using all the jets as spectators
    //artificial bjet count
    int bjcount = 0;
    for(auto jet : event->m_jets){
      bool isbtagged_MV2c10_85 = jet->charVariable("isbtagged_MV2c10_85") == 0;
      bool isbtagged_MV2c10_77 = jet->charVariable("isbtagged_MV2c10_77") == 0;
      bool isbtagged_MV2c10_70 = jet->charVariable("isbtagged_MV2c10_70") == 0;
      bool isbtagged_MV2c10_60 = jet->charVariable("isbtagged_MV2c10_60") == 0;
      int jet_isbtagged_85 = jet->charVariable("isbtagged_85");
      int jet_isbtagged_77 = jet->charVariable("isbtagged_77");
      int jet_isbtagged_70 = jet->charVariable("isbtagged_70");
      int jet_isbtagged_60 = jet->charVariable("jetIsBtagged_60");
       
      if (jet->charVariable("isbtagged_70")) { std::cout << "70 btag" << std::endl;}
      if (jet->charVariable("isbtagged_60")) { std::cout << "60 btag" << std::endl;}


      std::cout << "85: " << isbtagged_MV2c10_85 << "\t77: " << isbtagged_MV2c10_77 << "\t70: " << isbtagged_MV2c10_70 << "\t60: " << isbtagged_MV2c10_60 << std::endl;
      std::cout << "85: " << jet_isbtagged_85 << "\t77: " << jet_isbtagged_77 << "\t70: " << jet->floatVariable("mv2") << "\t60: " << jet_isbtagged_60 << std::endl;
      if (bjcount < 2) 
      {
        bjcount++;
        TLorentzVector vecb = jet->p4();
        specbjets.push_back(vecb*(0.001));} //converting MeV to GeV
      else 
      {
        TLorentzVector vecnonb = jet->p4();
        specnonbjets.push_back(vecnonb*(0.001)); //converting MeV to GeV
      }
    } 
    std::cout << specbjets.size() << std::endl;
    for(auto btjet : specbjets){
      std::cout << "print b jet px: " << btjet.Px() << "\tpy: " << btjet.Py() << "\tpz: " << btjet.Pz() << "\tE: " << btjet.E() << std::endl;
    }
    for(auto tjet : specnonbjets){
      std::cout << "print jet px: " << tjet.Px() << "\tpy: " << tjet.Py() << "\tpz: " << tjet.Pz() << "\tE: " << tjet.E() << std::endl;
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
    }
    std::cout << "HELLO WORLD FROM BUCKETS" << std::endl;
  }
}
