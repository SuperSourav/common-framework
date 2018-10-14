#include "TTHbbToolManager/TtTRF.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include <iostream>

namespace TTHbb{

  TtTRF::TtTRF(std::string name) : ToolBase(true){
    std::cout << "Initialising Tool TtTRF" << std::endl;
    m_name = name;
    
    auto* config = TTHbb::GlobalConfiguration::get();

    int ntags = 4;
    if     ( TTHbb::util::iequals((*config)("TtTRF.nBTags"),"3b")   ) ntags = 3;
    else if( TTHbb::util::iequals((*config)("TtTRF.nBTags"),"ge4b") ) ntags = 4;
    else{
      std::cout << "TtTRF::WARNING Unrecognised TtTRF.nBTags option: please use 3b or ge4b. Assuming you meant ge4b." << std::endl;
    }
    
    std::string path = "/gpfs/atlas/pinamont/TRF_evaluation";
    if( (*config)("TtTRF.Path") != "" ) path = (*config)("TtTRF.Path");
    
    std::string sample = "mc";
    if( TTHbb::util::iequals((*config)("TtTRF.Sample"),"data") ) sample = "data";
    
    std::string selection = "5j6j";
    if( (*config)("TtTRF.Selection") != "" ) selection = (*config)("TtTRF.Selection");
    
    std::string systematic = "nominal";
    if( (*config)("TtTRF.Systematic") != "" ) systematic = (*config)("TtTRF.Systematic");
    
    bool split0Jge1J=false;
    std::string splitting = "split3b4b";
    if( (*config)("TtTRF.Splitting") != "" ) splitting = (*config)("TtTRF.Splitting");
    if (splitting.find("_")!=std::string::npos){
       std::vector<std::string> splittings = TTHbb::util::vectoriseString(splitting, "_");
       splitting=splittings.at(0);
       split0Jge1J=true;
    }
    
    std::string parametrisation = "pt/dRj"; // can use pt, pt/eta/dRj, pt/eta...
    if( (*config)("TtTRF.Parametrisation") != "" ) parametrisation = (*config)("TtTRF.Parametrisation");
    
    bool debug = false;
    if( TTHbb::util::iequals((*config)("TtTRF.Debug"),"true") ) debug = true;
    
    m_ttTRFapp = new TtTRFapplication(ntags,path,sample,systematic,selection,splitting,parametrisation,debug);
    
    if (split0Jge1J) {
      m_ttTRFapp_0J =   new TtTRFapplication(ntags,path,sample,systematic,selection+"_0J"  ,splitting,parametrisation,debug);
      m_ttTRFapp_ge1J = new TtTRFapplication(ntags,path,sample,systematic,selection+"_ge1J",splitting,parametrisation,debug);
    }
    else{
      m_ttTRFapp_0J=0x0;
      m_ttTRFapp_ge1J=0x0;
    }    
  }

  void TtTRF::apply(TTHbb::Event* event){
    
    auto * config = TTHbb::GlobalConfiguration::get();
    
//     if(event->floatVariable("nBTags")!=2){
//       event->floatVariable("weight_ttTRF") = 0.;
//       return;
//     }
    
//     /// Need to reset nbtag BEFORE computing every kind of flag
//     if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF"),"true")){
//       m_nBTags = 0;
//       for(unsigned int i = 0; i < jets_n; i++){
//         std::shared_ptr<Jet> jet = event->m_jets.at(i);
//         if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF.nBTags"),"2b"))
//           if(jet->charVariable("truthTagisB_2bex")) m_nBTags++;
//         if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF.nBTags"),"3b"))
//           if(jet->charVariable("truthTagisB_3bex")) m_nBTags++;
//         if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF.nBTags"),"ge4b"))
//           if(jet->charVariable("truthTagisB_4bin")) m_nBTags++;
//       }
//     }
    
    m_ttTRFapp->Reset();
    if (m_ttTRFapp_0J!=0x0) {
      m_ttTRFapp_0J  ->Reset();
      m_ttTRFapp_ge1J->Reset();
    }
    
    // Loop on jets
    unsigned int jets_n = event->m_jets.size();
    for(unsigned int i = 0; i < jets_n; i++){
      std::shared_ptr<Jet> jet = event->m_jets.at(i);
      bool isb = jet->charVariable("isbtagged_MV2c10_77");
/*      if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF"),"true")){
        isb = false;
        if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF.nBTags"),"2b"  ) && jet->charVariable("truthTagisB_2bex")) isb = true;
        if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF.nBTags"),"3b"  ) && jet->charVariable("truthTagisB_3bex")) isb = true;
        if(TTHbb::util::iequals((*config)("HtXAnalysis.TRF.nBTags"),"ge4b") && jet->charVariable("truthTagisB_4bin")) isb = true;
      }
*/
//       m_ttTRFapp->AddJet(i,jet->Pt()*0.001,jet->Eta(),jet->Phi(),jet->E(),jet->floatVariable("mv2")>0.645925);
      m_ttTRFapp->AddJet(i,jet->Pt()*0.001,jet->Eta(),jet->Phi(),jet->E(),isb);
      if (m_ttTRFapp_0J!=0x0) {
        m_ttTRFapp_0J  ->AddJet(i,jet->Pt()*0.001,jet->Eta(),jet->Phi(),jet->E(),isb);
        m_ttTRFapp_ge1J->AddJet(i,jet->Pt()*0.001,jet->Eta(),jet->Phi(),jet->E(),isb);
      }
    }
    
    m_ttTRFapp->Evaluate();
    if (m_ttTRFapp_0J!=0x0) {
      m_ttTRFapp_0J  ->Evaluate();
      m_ttTRFapp_ge1J->Evaluate();
    }

    event->floatVariable("weight_ttTRF") = m_ttTRFapp->GetTRFweight();
    if (m_ttTRFapp_0J!=0x0) {
      event->floatVariable("weight_ttTRF_0J") = m_ttTRFapp_0J  ->GetTRFweight();
      event->floatVariable("weight_ttTRF_ge1J") = m_ttTRFapp_ge1J->GetTRFweight();
    }

    //    event->floatVariable("nBTags") = m_ttTRFapp->GetNBTags();
    event->intVariable("nBTags_MV2c10_77") = m_ttTRFapp->GetNBTags();
    if (m_ttTRFapp_0J!=0x0) {
      event->intVariable("nBTags_0J") = m_ttTRFapp_0J->GetNBTags();
      event->intVariable("nBTags_ge1J") = m_ttTRFapp_ge1J->GetNBTags();
    }
    for(unsigned int i = 0; i < jets_n; i++){
      std::shared_ptr<Jet> jet = event->m_jets.at(i);
      jet->intVariable("jets_is_btagged") = m_ttTRFapp->GetJetIsTagged(i);
    }
    
  }

} 
