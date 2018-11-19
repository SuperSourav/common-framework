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
  }

  OfflineBucketofTops::~OfflineBucketofTops(){
    if (m_buckets)
      delete m_buckets;
  }

  void OfflineBucketofTops::initialise(){}

  void OfflineBucketofTops::finalise(){}

  void OfflineBucketofTops::apply(TTHbb::Event* event){
    std::vector<shared_ptr<TTHbb::Jet> > specbjets, specnonbjets; //for now using all the jets as spectators
    for(auto jet : event->m_jets){
      bool isbtagged_MV2c10_85 = jet->charVariable("isbtagged_MV2c10_85") == 1;
      if (isbtagged_MV2c10_85) {specbjets.push_back(jet);}
      else {specnonbjets.push_back(jet);}
    } 
    if (m_DoBuckets) {
      m_buckets = new BucketofTops(specbjets, specnonbjets);
    }
    std::vector<TTHbb::Bucket> tbuck = m_buckets->B;
    //std::cout << B1.getBucketMass() << std::endl;
    event->intVariable("B1mass") = tbuck.size();
  }
}
