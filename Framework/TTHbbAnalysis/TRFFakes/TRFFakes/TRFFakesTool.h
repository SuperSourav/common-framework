/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRFFAKES_H
#define _TRFFAKES_H

#include "MVAVariables/JetOrderingTool.h"

#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/Lepton.h"
#include "TTHbbObjects/TTHbbUtils.h"


#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <functional>
#include <algorithm>
#include <bitset>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TKey.h"

#include "CustomTRF/CustomTRF.h"

namespace TTHbb{

  enum TRFfakesParametrization{JETPT, JETETA, JETPHI, DRJETLEP, DPHIJETMET, JETPT_JETETA, JETPT_DRJETLEP, JETPT_DPHIJETMET};
  enum TRFfakesExtrapRegion{I0B, I1B, I2B, I3B, I4B};

  class TRFFakesTool{
    public:

      TRFFakesTool();
      ~TRFFakesTool();
      void initialise();
      void apply(TTHbb::Event* event);
      void finalise();

      void setBtaggingWPs(std::vector<std::string> wp);
      void setEffParam(std::string effParam);
      void setEffMap(std::string effmapPath);
      void setExtrapolatingRegion(std::string region);
      void setBTaggingHypotheses(std::vector<std::string> hypos);

    private:
      bool m_verbose;

      ULong64_t   m_parametrization;
      std::string m_effs_path;
      std::vector<std::string>           m_btagWPs;
      std::vector<std::string>           m_btagHypo;
      std::map<std::string, std::string> m_config_id;
      std::string                        m_simpleconfig_id;

      TTHbb::Event* m_event;
      std::vector<std::shared_ptr<TTHbb::Jet>> jets_ordered_MV2;

      void initTRF();
      void initVars();
      void runTRF();
      float getEfficiency1D(float x, std::string var, int jet_n_raw, std::string wp, bool isEl=true);
      bool ignoreJet(int ijet);

      std::string m_outputFileName;
      int  m_nPar;
      bool m_param_JETPT;
      bool m_param_JETETA;
      bool m_param_JETPHI;
      bool m_param_DRJETLEP;
      bool m_param_DPHIJETMET;
      TRFfakesExtrapRegion m_extrapRegion;

      std::unordered_map<std::string, std::shared_ptr<CustomTRF> > TRF_map;
//      std::shared_ptr<CustomTRF> TRF_1ex;
//      std::shared_ptr<CustomTRF> TRF_2ex;
//      std::shared_ptr<CustomTRF> TRF_3ex;
//      std::shared_ptr<CustomTRF> TRF_4ex;
//      std::shared_ptr<CustomTRF> TRF_1in;
//      std::shared_ptr<CustomTRF> TRF_2in;
//      std::shared_ptr<CustomTRF> TRF_3in;
//      std::shared_ptr<CustomTRF> TRF_4in;

      std::unordered_map<std::string, int> m_bjet_n;
      std::unordered_map<std::string, TH1F* > m_histos1D;
      std::unordered_map<std::string, TH2F* > m_histos2D;
  };
}

#endif
