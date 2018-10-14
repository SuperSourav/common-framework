/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRFFakes/TRFFakesTool.h"

namespace TTHbb{

  //___________________________________________________________________________
  TRFFakesTool::TRFFakesTool()
  {
    m_verbose = false;
  }


  //___________________________________________________________________________
  TRFFakesTool::~TRFFakesTool()
  {
  }


  //___________________________________________________________________________
  void TRFFakesTool::setBtaggingWPs(std::vector<std::string> wps)
  {
    for (auto wp : wps){
      if (wp.find("TRFfakes")!=string::npos)
        continue;
      m_btagWPs.push_back(wp);
    }
  }


  //___________________________________________________________________________
  void TRFFakesTool::setEffParam(std::string effparam)
  {
    m_parametrization = std::bitset<64>(effparam).to_ullong();
    std::cout << "TRFFakesTool::TRFFakesTool : Using cut " << m_parametrization << std::endl;

    m_param_JETPT      = (m_parametrization >> 0) & 1;
    m_param_JETETA     = (m_parametrization >> 1) & 1;
    m_param_JETPHI     = (m_parametrization >> 2) & 1;
    m_param_DRJETLEP   = (m_parametrization >> 3) & 1;
    m_param_DPHIJETMET = (m_parametrization >> 4) & 1;

    m_nPar = 0;
    if (m_param_JETPT){
      m_nPar++;
    }
    if (m_param_JETETA){
      m_nPar++;
    }
    if (m_param_JETPHI){
      m_nPar++;
    }
    if (m_param_DRJETLEP){
      m_nPar++;
    }
    if (m_param_DPHIJETMET){
      m_nPar++;
    }
    std::cout<<"Fakes TRF parametrization: "<<std::endl;
    std::cout<<"  - JETPT      "<<m_param_JETPT<<std::endl;
    std::cout<<"  - JETETA     "<<m_param_JETETA<<std::endl;
    std::cout<<"  - JETPHI     "<<m_param_JETPHI<<std::endl;
    std::cout<<"  - DRJETLEP   "<<m_param_DRJETLEP<<std::endl;
    std::cout<<"  - DPHIJETMET "<<m_param_DPHIJETMET<<std::endl;
  }


  //___________________________________________________________________________
  void TRFFakesTool::setExtrapolatingRegion(std::string region)
  {
    if(region=="I0B")
      m_extrapRegion = TRFfakesExtrapRegion::I0B;
    else if(region=="I1B")
      m_extrapRegion = TRFfakesExtrapRegion::I1B;
    else if(region=="I2B")
      m_extrapRegion = TRFfakesExtrapRegion::I2B;
    else if(region=="I3B")
      m_extrapRegion = TRFfakesExtrapRegion::I3B;
    else if(region=="I4B")
      m_extrapRegion = TRFfakesExtrapRegion::I4B;

    std::cout<<"Fakes TRF extrapolation region: "<<m_extrapRegion<<std::endl;
  }


  //___________________________________________________________________________
  void TRFFakesTool::setBTaggingHypotheses(std::vector<std::string> hypos)
  {
    m_btagHypo.clear();
    m_btagHypo = hypos;
    std::cout<<"B-tagging hypotheses: "<<std::endl;
    for (auto hypo : m_btagHypo){
      std::cout<<"  - "<<hypo<<std::endl;
    }
  }


  //___________________________________________________________________________
  void TRFFakesTool::setEffMap(std::string effmapPath)
  {
    TFile *efile = new TFile(effmapPath.c_str());
    TIter next(efile->GetListOfKeys());
    TKey *key;
    while((key = (TKey*)next())){
      std::string name(key->GetName());
      std::string classname(key->GetClassName());

      if (classname.find("TH1")!=std::string::npos){
        TH1F *h = (TH1F*)key->ReadObj();
        h->SetDirectory(0);
        m_histos1D[name] = (TH1F*)h->Clone();
        m_histos1D[name]->SetDirectory(0);
        delete h;
      }
      else if (classname.find("TH2")!=std::string::npos){
        TH2F *h = (TH2F*)key->ReadObj();
        m_histos2D[name] = (TH2F*)h->Clone();
        delete h;
      }
    }
    efile->Close();
  }


  //___________________________________________________________________________
  void TRFFakesTool::initialise()
  {
    m_simpleconfig_id = Form("r%d_p%lld", m_extrapRegion, m_parametrization);
    for (auto wp : m_btagWPs){
      m_config_id[wp] = Form("b%s_r%d_p%lld", wp.c_str(), m_extrapRegion, m_parametrization);
    }

    this->initTRF();
  }


  //___________________________________________________________________________
  void TRFFakesTool::apply(TTHbb::Event* event)
  {
    //--- Copying the event to data member
    m_event = event;

    //--- Init variables
    this->initVars();

    //--- Require 1 lepton
    if (m_event->m_leptons.size()<1)
      return;

    //--- Compute TRF weights on events to consider
    this->runTRF();

  }

  //___________________________________________________________________________
  void TRFFakesTool::finalise()
  {
  }



  //___________________________________________________________________________
  void TRFFakesTool::initTRF()
  {
    for (auto map_key : m_btagHypo){

      std::string hypo = "";
      if (map_key.find("1in")!=std::string::npos) hypo = "1in";
      if (map_key.find("2in")!=std::string::npos) hypo = "2in";
      if (map_key.find("3in")!=std::string::npos) hypo = "3in";
      if (map_key.find("4in")!=std::string::npos) hypo = "4in";
      if (map_key.find("1ex")!=std::string::npos) hypo = "1ex";
      if (map_key.find("2ex")!=std::string::npos) hypo = "2ex";
      if (map_key.find("3ex")!=std::string::npos) hypo = "3ex";
      if (map_key.find("4ex")!=std::string::npos) hypo = "4ex";

      std::string baseWP = "";
      if (map_key.find("85")!=std::string::npos) baseWP = "85";
      if (map_key.find("77")!=std::string::npos) baseWP = "77";
      if (map_key.find("70")!=std::string::npos) baseWP = "70";
      if (map_key.find("60")!=std::string::npos) baseWP = "60";

      TRF_map[map_key] = std::make_shared<CustomTRF>();

      //--- Add b-tagging working points
      int wp_counter = 0;
      int wp_index = 0;
      for (auto wp : m_btagWPs){
        TRF_map[map_key]->AddWorkingPoint(Form("MV2c10-%s%%", wp.c_str()));
        if(baseWP==wp) wp_index = wp_counter;
        wp_counter ++;
      }

      //--- Initial b-tag requirement
      bool isIn = false;
      if (hypo.find("in")!=string::npos){
        isIn = true;
      }

      //--- B-tagging hypothesis
      int bhyp = 0;
      if (m_extrapRegion==I0B){
        if (hypo.find("1")!=string::npos) bhyp = 1;
        if (hypo.find("2")!=string::npos) bhyp = 2;
        if (hypo.find("3")!=string::npos) bhyp = 3;
        if (hypo.find("4")!=string::npos) bhyp = 4;
      }
      else if (m_extrapRegion==I1B){
        if (hypo.find("1")!=string::npos) bhyp = 0;
        if (hypo.find("2")!=string::npos) bhyp = 1;
        if (hypo.find("3")!=string::npos) bhyp = 2;
        if (hypo.find("4")!=string::npos) bhyp = 3;
      }
      else if (m_extrapRegion==I2B){
        if (hypo.find("1")!=string::npos) bhyp = 0;
        if (hypo.find("2")!=string::npos) bhyp = 0;
        if (hypo.find("3")!=string::npos) bhyp = 1;
        if (hypo.find("4")!=string::npos) bhyp = 2;
      }

      TRF_map[map_key]->SetBtagRequirement(bhyp, isIn, wp_index); // e.g. 3ex true MC2c10-77% WP
    }
  }


  //___________________________________________________________________________
  void TRFFakesTool::initVars()
  {
    for (auto map_key : m_btagHypo){

      std::string baseWP = "";
      if (map_key.find("85")!=std::string::npos) baseWP = "85";
      if (map_key.find("77")!=std::string::npos) baseWP = "77";
      if (map_key.find("70")!=std::string::npos) baseWP = "70";
      if (map_key.find("60")!=std::string::npos) baseWP = "60";

      m_event->floatVariable(Form("weight_TRFfakes_%s_%s", m_simpleconfig_id.c_str(), map_key.c_str())) = 1.;

      for(auto jet : m_event->m_jets){
        jet->charVariable(Form("jet_ignore_TRFfakes_%s", m_config_id[baseWP].c_str())) = 0x0;
        if (!jet->checkFloatVariable("jet_dRlep"))    jet->floatVariable("jet_dRlep") = -999;
        if (!jet->checkFloatVariable("jet_dPhimet"))  jet->floatVariable("jet_dPhimet") = -999;

        for (auto wp : m_btagWPs){
          jet->charVariable(Form("isbtagged_TRFfakes_%s_%s", m_config_id[wp].c_str(), map_key.c_str())) = 0x0;
        }
      }
    }
  }


  //___________________________________________________________________________
  void TRFFakesTool::runTRF()
  {

    //--- Creating new jet collection ordered in MV2
    jets_ordered_MV2.clear();
    for(auto jet : m_event->m_jets){
      jets_ordered_MV2.push_back(jet);
    }
    JetOrderingTool::sortFloatDecorUp(jets_ordered_MV2, "mv2");

    //--- Loop over b-tagging hypotheses
    for (auto map_key : m_btagHypo){

      //--- Get baseWP from the list
      std::string baseWP = "";
      if (map_key.find("85")!=std::string::npos) baseWP = "85";
      if (map_key.find("77")!=std::string::npos) baseWP = "77";
      if (map_key.find("70")!=std::string::npos) baseWP = "70";
      if (map_key.find("60")!=std::string::npos) baseWP = "60";

      //--- Get number of b-jets under the hypothesis
      int bjet_n = 0;
      for (auto jet : m_event->m_jets){
        if (jet->charVariable(Form("isbtagged_%s", baseWP.c_str())))
          bjet_n++;
      }
      //--- Don't loop over events with b-tag mult. smaller than n when the extrapolation is not made from 0bi
      if (m_extrapRegion==I1B && bjet_n<1){
        return;
      }
      else if (m_extrapRegion==I2B && bjet_n<2){
        return;
      }
      else if (m_extrapRegion==I3B && bjet_n<3){
        return;
      }
      else if (m_extrapRegion==I4B && bjet_n<4){
        return;
      }

      //--- Reset TRF calculator
      TRF_map[map_key]->Reset();

      //--- Loop over ordered jets
      int jet_counter = 0;
      for (unsigned int ijet=0; ijet<jets_ordered_MV2.size(); ++ijet){

        //--- Compute variables before ignoring (to avoid overwritting initialization)
        TLorentzVector jet = jets_ordered_MV2.at(ijet)->p4();
        TLorentzVector lep = m_event->m_leptons.at(0)->p4();
        TLorentzVector met; met.SetPtEtaPhiE(m_event->met_met, 0, m_event->met_phi, 0);

        float dRjetlep   = jet.DeltaR(lep);
        float dPhijetmet = fabs(jet.DeltaPhi(met));
        jets_ordered_MV2.at(ijet)->floatVariable("jet_dRlep") = dRjetlep;
        jets_ordered_MV2.at(ijet)->floatVariable("jet_dPhimet") = dPhijetmet;


        //--- Check n jets with largest MV2 for the extrapolation region
        if (this->ignoreJet(ijet)){
          jets_ordered_MV2.at(ijet)->charVariable(Form("jet_ignore_TRFfakes_%s", m_config_id[baseWP].c_str())) = true;
          continue;
        }
        //--- Proceed with remaining jets
        jets_ordered_MV2.at(ijet)->charVariable(Form("jet_ignore_TRFfakes_%s", m_config_id[baseWP].c_str())) = false;

        //--- Loop over WPs
        int wp_counter = 0;
        for (auto wp : m_btagWPs){
          //--- Retrieve efficiency
          float efficiency = 1.;
          if (m_param_JETPT){
            efficiency *= getEfficiency1D(jet.Pt()/1e3, "jetpt", jets_ordered_MV2.size(), wp, (m_event->m_leptons.at(0)->type()==leptonType::electron));
          }
          if (m_param_JETETA){
            efficiency *= getEfficiency1D(jet.Eta(), "jeteta", jets_ordered_MV2.size(), wp, (m_event->m_leptons.at(0)->type()==leptonType::electron));
          }
          if (m_param_JETPHI){
            efficiency *= getEfficiency1D(jet.Phi(), "jetphi", jets_ordered_MV2.size(), wp, (m_event->m_leptons.at(0)->type()==leptonType::electron));
          }
          if (m_param_DRJETLEP){
            efficiency *= getEfficiency1D(dRjetlep, "deltaRjetlep", jets_ordered_MV2.size(), wp, (m_event->m_leptons.at(0)->type()==leptonType::electron));
          }
          if (m_param_DPHIJETMET){
            efficiency *= getEfficiency1D(dPhijetmet, "deltaPhijetmet", jets_ordered_MV2.size(), wp, (m_event->m_leptons.at(0)->type()==leptonType::electron));
          }
          efficiency /= pow(getEfficiency1D(0.5, "av", jets_ordered_MV2.size(), wp, (m_event->m_leptons.at(0)->type()==leptonType::electron)), m_nPar-1);

          //--- Input efficiency to TRF tool
          TRF_map[map_key]->SetJetBtagEff(jet_counter, wp_counter, efficiency);
          wp_counter++;
        }//wp
        jet_counter++;
      } //jets

      //-- Evaluate and compute the TRF weight
      TRF_map[map_key]->Evaluate();

      m_event->floatVariable(Form("weight_TRFfakes_%s_%s", m_simpleconfig_id.c_str(), map_key.c_str())) = TRF_map[map_key]->GetTRFweight();
    } // m_btagHypo

    //--- Get individual jet isTagged flag
    int jet_counter = 0;
    for (unsigned int ijet=0; ijet<jets_ordered_MV2.size(); ++ijet){
      if (this->ignoreJet(ijet)){
        for (auto map_key : m_btagHypo){
          //--- Loop over WPs
          for (auto wp : m_btagWPs){
            jets_ordered_MV2.at(ijet)->charVariable(Form("isbtagged_TRFfakes_%s_%s", m_config_id[wp].c_str(), map_key.c_str())) = true;
          }
        }
      }
      else{
        for (auto map_key : m_btagHypo){
          //--- Loop over WPs
          int wp_counter = 0;
          for (auto wp : m_btagWPs){
            jets_ordered_MV2.at(ijet)->charVariable(Form("isbtagged_TRFfakes_%s_%s", m_config_id[wp].c_str(), map_key.c_str())) = TRF_map[map_key]->GetJetIsTagged(jet_counter, wp_counter);
            wp_counter++;
          }
        } // m_btagHypo
        jet_counter++;
      }
    } //jets

    //--- Verbose - print jet isbtagged, only to debug
    if (m_verbose){
      jet_counter=0;
      for(auto jet : m_event->m_jets){
        for (auto wp : m_btagWPs){
          for (auto map_key : m_btagHypo){
            std::cout<<m_config_id[wp]<<"Jet #"<<jet_counter<<", "<<wp<<", "<<map_key<<" ==> "<<(jet->charVariable(Form("isbtagged_TRFfakes_%s_%s", m_config_id[wp].c_str(), map_key.c_str()))==true)<<std::endl;
          }
        }
        jet_counter++;
      }
    }
  }


  //_________________________________________________________________________________________
  bool TRFFakesTool::ignoreJet(int ijet)
  {
    if (m_extrapRegion==I1B && ijet<1)
      return true;
    if (m_extrapRegion==I2B && ijet<2)
      return true;
    if (m_extrapRegion==I3B && ijet<3)
      return true;
    if (m_extrapRegion==I4B && ijet<4)
      return true;
    return false;
  }


  //_________________________________________________________________________________________
  float TRFFakesTool::getEfficiency1D(float x, std::string var, int jet_n_raw, std::string wp, bool isEl)
  {
    std::string jet_n = std::string(Form("%dje", jet_n_raw));
    if (jet_n_raw>5){
      jet_n = std::string("5ji");
    }

    std::string btagStr = "0bi";
    if (m_extrapRegion==I1B)
      btagStr = "1bi";
    else if (m_extrapRegion==I2B)
      btagStr = "2bi";
    else if (m_extrapRegion==I3B)
      btagStr = "3bi";
    else if (m_extrapRegion==I4B)
      btagStr = "4bi";

    std::string histoName;
    if (isEl){
      histoName = Form("eff_1e%s%s_%s___%s", jet_n.c_str(), btagStr.c_str(), wp.c_str(), var.c_str());
    }
    else{
      histoName = Form("eff_1m%s%s_%s___%s", jet_n.c_str(), btagStr.c_str(), wp.c_str(), var.c_str());
    }

    float efficiency = m_histos1D[histoName]->GetBinContent( m_histos1D[histoName]->FindBin(x) );
    return efficiency;
  }
}
