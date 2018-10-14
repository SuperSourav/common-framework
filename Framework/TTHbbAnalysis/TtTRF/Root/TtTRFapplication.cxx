#include "TtTRF/TtTRFapplication.h"

TtTRFapplication::TtTRFapplication(int ntags,std::string path,std::string sample,std::string systematic,std::string selection,std::string splitting,std::string parametrisation,bool debug){  // sample = data or mc, syst = nominal, ...
  m_debug = debug;
  m_ntags = ntags;
  m_nBtags_ttTRF = 2;
  sel = selection;
  std::string syst = systematic;
  m_split3b = splitting.find("3b")!=std::string::npos;
  m_split4b = splitting.find("4b")!=std::string::npos;
  
  if(m_debug){
    if(m_split3b)                std::cout << "Opening file " << path+"/ttTRFmaps2018_"+sample+"_"+sel+"_3b__"+syst+".root" << std::endl;
    if(m_split4b)                std::cout << "Opening file " << path+"/ttTRFmaps2018_"+sample+"_"+sel+"_4b__"+syst+".root" << std::endl;
    if(!m_split3b || !m_split4b) std::cout << "Opening file " << path+"/ttTRFmaps2018_"+sample+"_"+sel+"__"   +syst+".root" << std::endl;
  }
  
  if(m_split3b) m_TRFreader0 = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"_3b__"+syst+".root",parametrisation);
  else          m_TRFreader0 = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"__"   +syst+".root",parametrisation);
  if(m_split4b) m_TRFreader1 = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"_4b__"+syst+".root",parametrisation);
  else          m_TRFreader1 = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"__"   +syst+".root",parametrisation);
//   // create other instances for weight systematics, only in case of nominal tree
//   if(syst=="nominal"){
//     for(auto systName : m_systNames){
//       if(m_debug) std::cout << "Opening " << "ttTRFmaps_"+sample+"_"+sel+"__"   +systName+".root" << std::endl;
//       m_systTRFreader0.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"__"   +systName+".root"));
//       m_systTRFreader1.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"_4b__"+systName+".root"));
//     }
//     for(unsigned int i_syst=0;i_syst<m_systVecNames.size();i_syst++){
//       for(unsigned int ii_syst=0;ii_syst<m_weight_systVec[i_syst].size();ii_syst++){
//         std::string systName = systVecNames[i_syst]+Form("%d",ii_syst);
//         systTRFreader0.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"__"   +systName+".root"));
//         systTRFreader1.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"_4b__"+systName+".root"));
//       }
//     }
//   }
  
  // 2i->3e/3i
  m_TRF = new CustomTRF();
  m_TRF->AddWorkingPoint("mv2c10_77");
//   std::vector<CustomTRF*> m_systTRF;
//   for(auto trfReader : m_systTRFreader0){
//     m_systTRF.push_back(new CustomTRF());
//     m_systTRF[systTRF.size()-1]->AddWorkingPoint("mv2c10_77");
//   }

  // 2i->2e
  m_TRF0 = new CustomTRF();
  m_TRF0->AddWorkingPoint("mv2c10_77");
//   std::vector<CustomTRF*> m_systTRF0;
//   for(auto trfReader : m_systTRFreader0){
//     m_systTRF0.push_back(new CustomTRF());
//     m_systTRF0[systTRF0.size()-1]->AddWorkingPoint("mv2c10_77");
//   }

  // 3i->4i
  m_TRF1 = new CustomTRF();
  m_TRF1->AddWorkingPoint("mv2c10_77");
//   std::vector<CustomTRF*> m_systTRF1;
//   for(auto trfReader : m_systTRFreader1){
//     m_systTRF1.push_back(new CustomTRF());
//     m_systTRF1[systTRF1.size()-1]->AddWorkingPoint("mv2c10_77");
//   }
  
  // 3i->3e
  m_TRF2 = new CustomTRF();
  m_TRF2->AddWorkingPoint("mv2c10_77");
//   std::vector<CustomTRF*> m_systTRF2;
//   for(auto trfReader : m_systTRFreader1){
//     m_systTRF2.push_back(new CustomTRF());
//     m_systTRF2[systTRF2.size()-1]->AddWorkingPoint("mv2c10_77");
//   }
}

// TtTRFapplication::AddJet(int idx,float pt,float eta,float phi,float e,float weight,int istagged){
void TtTRFapplication::AddJet(int idx,float pt,float eta,float phi,float e,int istagged){
  m_jet_index.push_back(idx);
  TLorentzVector jet(0,0,0,0);
  jet.SetPtEtaPhiE(pt,eta,phi,e);
  m_jet.push_back(jet);
//   m_jet_weight.push_back(weight);
  m_jet_istagged.push_back(istagged);
}

void TtTRFapplication::Reset(){
  m_jet_index.clear();
  m_jet.clear();
//   m_jet_weight.clear();
  m_jet_istagged.clear();
}

void TtTRFapplication::Evaluate(){
  int nJets = (int)m_jet.size();
  if(m_debug) std::cout << "-------\n" << "nJets = " << nJets << std::endl;
  //
  // get the first two tagged jets (there should be just two of them)
  int j1st = -1;
  int j2nd = -1;
  for(int j=0;j<nJets;j++){
    if(m_jet_istagged[j]){
      if(j1st<0)      j1st = j;
      else if(j2nd<0) j2nd = j;
      else{
        if(m_debug) 
          std::cout << "ttTRF::TtTRFapplication: WARNING: Trying to apply ttTRF to an event with != 2 b-tagged jets. Returning weight = 0." << std::endl;
        m_weight = 0.;
        return;
      }
    }
  }
  // G.
  if (j1st<0 || j2nd < 0) {
       m_weight =0.;
        if(m_debug) 
          std::cout << "ttTRF::TtTRFapplication: WARNING: btagged jets not found, returning weight = 0." << std::endl;
       // save correct direct nBTags, sparing storage space (events will be skipped when requiring nBTags >= 2)
       if (j1st*j2nd>0) m_nBtags_ttTRF=0;
       else m_nBtags_ttTRF=1;
       return;
  }
  //
  m_TRF->Reset();
  m_TRF0->Reset();
  m_TRF1->Reset();
  m_TRF2->Reset();
  //
  int jj = 0;
  float dRmin_all = 1000;
  for(int j=0;j<nJets;j++){
    if(j == j1st || j == j2nd) continue;
    // find min dR within all jets
    ////////////////////////////
    if (sel.find("nofix")==std::string::npos) dRmin_all=100;
    ////////////////////////////
    for(int jjj=0;jjj<nJets;jjj++){
      if(j==jjj) continue;
      float dRj = m_jet[j].DeltaR(m_jet[jjj]);
      if(dRj<dRmin_all){
        dRmin_all = dRj;
      }
    }
    //
    float jetPt = m_jet[j].Pt();
    float jetEta = TMath::Abs(m_jet[j].Eta());
    if(m_debug) std::cout << "..jet " << jj << " -> "<< jetPt << "; ->" ;
    float btageff = m_TRFreader0->GetEff(jetPt,jetEta,dRmin_all*nJets,nJets);
    if(m_debug) std::cout << "btageff = " << btageff << std::endl;
    m_TRF ->SetJetBtagEff(jj,0,btageff);
    m_TRF0->SetJetBtagEff(jj,0,btageff);
    //
    jj ++;
  }
  if(m_ntags>=4 && !m_split4b) m_TRF ->SetBtagRequirement(m_ntags-2,true,0);
  else                         m_TRF ->SetBtagRequirement(1,false,0);
  m_TRF0->SetBtagRequirement(0,false,0);
  m_TRF ->Evaluate();
  m_TRF0->Evaluate();
  if(m_debug) std::cout << "-------\n" << "Nominal: " << m_TRF->GetTRFweight() << " " << m_TRF0->GetTRFweight() << std::endl;
  m_weight = m_TRF->GetTRFweight() / m_TRF0->GetTRFweight();
  if(m_debug) std::cout << " -> " << m_weight << std::endl;
  //
  // ...
  int j3rd = -1;
  if(m_ntags>=4 && m_split4b){
    jj = 0;
    int jjjj = 0;
    dRmin_all = 1000;
    for(int j=0;j<nJets;j++){
      // skip the usual two real b-tags
      if(j == j1st || j == j2nd) continue;
      jjjj++;
      // skip the "promoted" jet as well
      if(m_TRF->GetJetIsTagged(jjjj-1,0)){
        j3rd = j;
        continue;
      }
      //
      // find min dR within all jets
      ////////////////////////////
      if (sel.find("nofix")==std::string::npos) dRmin_all=100;
      ////////////////////////////
      for(int jjj=0;jjj<nJets;jjj++){
        if(j==jjj) continue;
        float dRj = m_jet[j].DeltaR(m_jet[jjj]);
        if(dRj<dRmin_all){
          dRmin_all = dRj;
        }
      }
      //
      float jetPt = m_jet[j].Pt();
      float jetEta = TMath::Abs(m_jet[j].Eta());
      float btageff = m_TRFreader1->GetEff(jetPt,jetEta,dRmin_all*nJets,nJets);
      m_TRF1->SetJetBtagEff(jj,0,btageff);
      m_TRF2->SetJetBtagEff(jj,0,btageff);
      //
      jj ++;
    }
    //
//     m_TRF1->SetBtagRequirement(m_ntags-3,incl,0);
    m_TRF1->SetBtagRequirement(m_ntags-3,true, 0);
    m_TRF2->SetBtagRequirement(0,        false,0);
    m_TRF1->Evaluate();
    m_TRF2->Evaluate();
    m_weight *= m_TRF1->GetTRFweight() / m_TRF2->GetTRFweight();
    //
  }
  // save which jet is tagged:
  jj = 0;
  m_jet_istagged_ttTRF.clear();
  for(int j=0;j<nJets;j++){
    m_jet_istagged_ttTRF.push_back(false);
    if(j==j1st){ m_jet_istagged_ttTRF[j] = true; continue; }
    if(j==j2nd){ m_jet_istagged_ttTRF[j] = true; continue; }
    if(m_ntags>=4 && j==j3rd){ m_jet_istagged_ttTRF[j] = true; continue; }
    if(m_ntags<=3 && m_TRF ->GetJetIsTagged(jj,0)) m_jet_istagged_ttTRF[j] = true;
    if(m_ntags>=4 && m_TRF1->GetJetIsTagged(jj,0)) m_jet_istagged_ttTRF[j] = true;
    jj++;
  }
  //
  if(m_ntags<=3) m_nBtags_ttTRF = m_TRF ->GetNumBtagJets(0)+2;
  if(m_ntags>=4 && m_split4b)  m_nBtags_ttTRF = m_TRF1->GetNumBtagJets(0)+3;  
  if(m_ntags>=4 && !m_split4b) m_nBtags_ttTRF = m_TRF ->GetNumBtagJets(0)+3;  
  //
}

float TtTRFapplication::GetTRFweight(){
  return m_weight;
}

bool TtTRFapplication::GetJetIsTagged(int idx){
  if((int)m_jet_istagged_ttTRF.size()<=idx) return false;
  return m_jet_istagged_ttTRF[idx];
}

int TtTRFapplication::GetNBTags(){
  return m_nBtags_ttTRF;
}

