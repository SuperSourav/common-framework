/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/OfflineFakes.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

namespace TTHbb{

  OfflineFakes::OfflineFakes(std::string name) : ToolBase(true){
    m_name = name;
    auto* config = TTHbb::GlobalConfiguration::get();
    std::string dataPath2015 = (*config)(m_name + ".DataPath2015");
    std::string dataPath2016 = (*config)(m_name + ".DataPath2016");
    m_use2016Prescaled = TTHbb::util::iequals((*config)(m_name + ".Use2016PrescaledTriggers"),"true");
    m_applyPrescales   = TTHbb::util::iequals((*config)(m_name + ".ApplyPrescalesToMMweights"),"true");

    m_weight_ejets_2015  = new FakesWeights();
    m_weight_ejets_2016  = new FakesWeights();
    m_weight_mujets_2015 = new FakesWeights();
    m_weight_mujets_2016 = new FakesWeights();

    m_weight_ejets_2015->SetDataPath(dataPath2015);
    m_weight_ejets_2016->SetDataPath(dataPath2016);
    m_weight_mujets_2015->SetDataPath(dataPath2015);
    m_weight_mujets_2016->SetDataPath(dataPath2016);

    m_weight_ejets_2015->SetPeriod(1);
    m_weight_ejets_2016->SetPeriod(2);
    m_weight_mujets_2015->SetPeriod(1);
    m_weight_mujets_2016->SetPeriod(2);

    //m_weight_ejets_2015->SetupWeighter(FakesWeights::EJETS, "2015Test:NS:pt:dPhi", "2015Test:NS:pt:dPhi");
    m_weight_ejets_2015->SetupWeighter(FakesWeights::EJETS, "2015Test:NS:eta:jetpt:dR", "2015Test:NS:eta:jetpt:dR");
    //m_weight_mujets_2015->SetupWeighter(FakesWeights::MUJETS,"2015Test:NS:pt:dPhi", "2015Test:NS:pt:dPhi");
    m_weight_mujets_2015->SetupWeighter(FakesWeights::MUJETS,"2015Test:NS:eta:jetpt:dR", "2015Test:NS:eta:jetpt:dR");

    if (m_use2016Prescaled){
      //m_weight_ejets_2016->SetupWeighter(FakesWeights::EJETS, "2016PSTest:NS:pt:dPhi", "2016PSTest:NS:pt:dPhi");
      m_weight_ejets_2016->SetupWeighter(FakesWeights::EJETS, "2016PSTest:NS:eta:jetpt:dR", "2016PSTest:NS:eta:jetpt:dR");
      //m_weight_mujets_2016->SetupWeighter(FakesWeights::MUJETS,"2016PSTest:NS:pt:dPhi", "2016PSTest:NS:pt:dPhi");
      m_weight_mujets_2016->SetupWeighter(FakesWeights::MUJETS,"2016PSTest:NS:eta:jetpt:dR", "2016PSTest:NS:eta:jetpt:dR");
    }
    else {
      //m_weight_ejets_2016->SetupWeighter(FakesWeights::EJETS, "2016Test:NS:pt:dPhi", "2016Test:NS:pt:dPhi");
      m_weight_ejets_2016->SetupWeighter(FakesWeights::EJETS, "2016Test:NS:eta:jetpt:dR", "2016Test:NS:eta:jetpt:dR");
      //m_weight_mujets_2016->SetupWeighter(FakesWeights::MUJETS,"2016Test:NS:pt:dPhi", "2016Test:NS:pt:dPhi");
      m_weight_mujets_2016->SetupWeighter(FakesWeights::MUJETS,"2016Test:NS:eta:jetpt:dR", "2016Test:NS:eta:jetpt:dR");
    }
  }

  void OfflineFakes::initialise(){
  }


  void OfflineFakes::apply(TTHbb::Event* event){

    std::shared_ptr<Lepton> lep = event->m_leptons[0];
    float lep_pt = lep->Pt();
    float lep_eta = lep->Eta();
    TTHbb::leptonType lepType = lep->type();
    int tight = 0;
    if(lepType==leptonType::electron) tight = lep->charVariable("el_isTight");
    if(lepType==leptonType::muon)     tight = lep->charVariable("mu_isTight");

    int jet_n = (int)event->m_jets.size();
    float jet_pt = event->m_jets[0]->Pt();
    int nJet_tagged = 0;
    for(auto jet : event->m_jets){
      if (jet->charVariable("isbtagged_77"))
        nJet_tagged++;
    }

    // get dR_lj_min
    float dR_lj_min = minDeltaRljet(lep,event->m_jets);

    // get dPhi
    float dPhi = deltaPhi(lep->Phi(),event->met_phi);

    // get trigger flag
    int trigger = 0;
    if(event->charVariable("ejets_2015_Loose")){
      if( event->charVariable("HLT_e24_lhmedium_L1EM20VH") && lep->charVariable("el_trigMatch_HLT_e24_lhmedium_L1EM20VH") && lep->Pt()<=61*1e3) trigger |= 0x1 << 0;
      if( ( (event->charVariable("HLT_e60_lhmedium") && lep->charVariable("el_trigMatch_HLT_e60_lhmedium")) || 
            (event->charVariable("HLT_e120_lhloose") && lep->charVariable("el_trigMatch_HLT_e120_lhloose")) ) && lep->Pt()>61*1e3)              trigger |= 0x1 << 1;
    }
    if(event->charVariable("mujets_2015_Loose")){
      if( event->charVariable("HLT_mu20_iloose_L1MU15") && lep->charVariable("mu_trigMatch_HLT_mu20_iloose_L1MU15") && lep->Pt()<=51*1e3) trigger |= 0x1 << 0;
      if( event->charVariable("HLT_mu50") && lep->charVariable("mu_trigMatch_HLT_mu50") && lep->Pt()>51*1e3)                              trigger |= 0x1 << 1;
    }
    if(event->charVariable("ejets_2016_Loose")){
      if( m_use2016Prescaled  && event->charVariable("HLT_e24_lhmedium_nod0_L1EM18VH") && lep->charVariable("el_trigMatch_HLT_e24_lhmedium_nod0_L1EM18VH") && lep->Pt()<=61*1e3) trigger |= 0x1 << 0;
      if( !m_use2016Prescaled && event->charVariable("HLT_e26_lhtight_nod0_ivarloose") && lep->charVariable("el_trigMatch_HLT_e26_lhtight_nod0_ivarloose") && lep->Pt()<=61*1e3) trigger |= 0x1 << 0;
      if( ( (event->charVariable("HLT_e60_lhmedium_nod0") && lep->charVariable("el_trigMatch_HLT_e60_lhmedium_nod0")) || 
            (event->charVariable("HLT_e140_lhloose_nod0") && lep->charVariable("el_trigMatch_HLT_e140_lhloose_nod0")) ) && lep->Pt()>61*1e3)                                     trigger |= 0x1 << 1;
    }
    if(event->charVariable("mujets_2016_Loose")){
      if( m_use2016Prescaled  && event->charVariable("HLT_mu24") && lep->charVariable("mu_trigMatch_HLT_mu24") && lep->Pt()<=51*1e3)                       trigger |= 0x1 << 0;
      if( !m_use2016Prescaled && event->charVariable("HLT_mu26_ivarmedium") && lep->charVariable("mu_trigMatch_HLT_mu26_ivarmedium") && lep->Pt()<=51*1e3) trigger |= 0x1 << 0;
      if( event->charVariable("HLT_mu50") && lep->charVariable("mu_trigMatch_HLT_mu50") && lep->Pt()>51*1e3)                                               trigger |= 0x1 << 1;
    }

    MMLepton lepton; 
    lepton.pt = lep_pt/1e3;      // pt of the lepton
    lepton.eta = TMath::Abs(lep_eta);  // eta of the lepton, use the cluster eta for electrons!
    lepton.dR = dR_lj_min; // distance between the lepton and the closest jet
    lepton.dPhi  = dPhi;   // deltaPhi between lepton and ETmiss
    lepton.trigger = trigger;

    MMEvent mmevent; 
    mmevent.njets = jet_n; // jet multiplicity
    mmevent.ntag = nJet_tagged;  // b-jet multiplicity
    mmevent.jetpt = jet_pt/1e3; // pt of the leading jet

    if(lepType==leptonType::electron){
      if(event->charVariable("ejets_2015_Loose")){
        m_weight_ejets_2015->SetLepton(mmevent,lepton); 
        m_weight_mm = m_weight_ejets_2015->GetFakesWeightLJets(tight);
      }
      if(event->charVariable("ejets_2016_Loose")){
        m_weight_ejets_2016->SetLepton(mmevent,lepton); 
        m_weight_mm = m_weight_ejets_2016->GetFakesWeightLJets(tight);
      }
    }
    else if(lepType==leptonType::muon){
      if(event->charVariable("mujets_2015_Loose")){
        m_weight_mujets_2015->SetLepton(mmevent,lepton); 
        m_weight_mm = m_weight_mujets_2015->GetFakesWeightLJets(tight);
      }
      if(event->charVariable("mujets_2016_Loose")){
        m_weight_mujets_2016->SetLepton(mmevent,lepton); 
        m_weight_mm = m_weight_mujets_2016->GetFakesWeightLJets(tight);
      }
    }

    //--- Effectively remove events with high pT leptons that don't pass high pT trigger
    //    or low pT leptons that don't pass the low pT trigger
    if (trigger==0){
      m_weight_mm = 0;
    }

    //--- Multiply weight by prescale when necessary if requested.
    if (m_applyPrescales && m_use2016Prescaled){
      if (trigger==1){
        if(lepType==leptonType::electron && event->charVariable("ejets_2016_Loose")){
          m_weight_mm *= event->floatVariable("PS_HLT_e24_lhmedium_nod0_L1EM18VH");
        }
        if(lepType==leptonType::muon && event->charVariable("mujets_2016_Loose")){
          m_weight_mm *= event->floatVariable("PS_HLT_mu24");
        }
      }
    }
    event->floatVariable("weight_mm")  = m_weight_mm;
  }


  void OfflineFakes::finalise(){
  }

  // utility functions

  float deltaPhi(float phi1, float phi2) {
    float dPhi=fabs(phi1-phi2);
    if (dPhi>TMath::Pi()) dPhi=2*TMath::Pi()-dPhi;
    return dPhi;
  }

  float minDeltaRljet(std::shared_ptr<Lepton> lep,std::vector<std::shared_ptr<Jet> > jets){
    float dRmin = 1000.;
    float dR;
    for(int j=0;j<(int)jets.size();j++){
      TLorentzVector Lep = (*lep);
      TLorentzVector Jet = (*jets[j]);
      dR = lep->DeltaR(Jet);
      if(dR<dRmin) dRmin = dR;
    }
    return dRmin;
  }

} 
