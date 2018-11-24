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
        specnonbjets.push_back(vecnonb((0.001)); //converting MeV to GeV
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
      std::cout << "init bucket mass: " << bucklist[0].Mbucket << "\t" << bucklist[1].Mbucket << std::endl;
      std::cout << "init bucket label: " << bucklist[0].bucket_label << "\t" << bucklist[1].bucket_label << std::endl;
      std::cout << "init bucket pT: " << bucklist[0].pTbucket << "\t" << bucklist[1].pTbucket << std::endl;
      std::cout << "init bucket eta: " << bucklist[0].etabucket << "\t" << bucklist[1].etabucket << std::endl;
      std::cout << "init bucket mpairnum: " << bucklist[0].mpairnum << "\t" << bucklist[1].mpairnum << std::endl;

      if (m_buckets == NULL) {std::cout << "bucket ptr is null" << std::endl;}
	std::vector<bucketAlgo::bucket>& blistp = *m_buckets->returnbucketlistptr();
        std::cout << "get blist size: " << blistp.size() << std::endl;
	std::cout <<"get blist mass: " << blistp[0].Mbucket  << "\t" << blistp[1].Mbucket  << std::endl;
	std::cout <<"get blist label: " << blistp[0].bucket_label  << "\t" << blistp[1].bucket_label  << std::endl;
	std::cout <<"get blist pT: " << blistp[0].pTbucket  << "\t" << blistp[1].pTbucket  << std::endl;
	std::cout <<"get blist eta: " << blistp[0].etabucket  << "\t" << blistp[1].etabucket  << std::endl;
	std::cout <<"get blist mpairnum: " << blistp[0].mpairnum << "\t" << blistp[1].mpairnum << std::endl;
      event->intVariable("blistsize") = blistp.size();
      event->floatVariable("Mbucket1") = blistp[0].Mbucket;
      event->floatVariable("Mbucket2") = blistp[1].Mbucket;
      event->floatVariable("pTbucket1") = blistp[0].pTbucket;
      event->floatVariable("pTbucket2") = blistp[1].pTbucket;
      event->floatVariable("etabucket1") = blistp[0].etabucket;
      event->floatVariable("etabucket2") = blistp[1].etabucket;
      event->floatVariable("mpairnum1") = blistp[0].mpairnum;
      event->floatVariable("mpairnum2") = blistp[1].mpairnum;

      std::cout << "inside DoBuckets after call" << std::endl;
      //tbuck = m_buckets->B;
      //event->intVariable("B1mass") = tbuck.size();
    }
    std::cout << "HELLO WORLD FROM BUCKETS" << std::endl;
  }
}
