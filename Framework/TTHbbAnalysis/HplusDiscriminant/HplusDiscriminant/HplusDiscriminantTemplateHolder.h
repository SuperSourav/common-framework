/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HPLUSDISCRIMINANTTEMPLATEHOLDER_H
#define HPLUSDISCRIMINANTTEMPLATEHOLDER_H
#include <TFile.h>
#include <TString.h>
#include <TH1F.h>

namespace TTHbb {

class HplusDiscriminantTemplateHolder { //there's a lot of hard-coding here, but this leads to a major speed-up as it avoid many string comparisons

 public:
  HplusDiscriminantTemplateHolder(){};
  HplusDiscriminantTemplateHolder(TFile* rootfile, TString channelname, bool APM, int mass, double APMvsMJprob); //mass is set to -1 for ttbar
  ~HplusDiscriminantTemplateHolder();
  long double getProb_Mlvb_NoSolM(double value);
  long double getProb_Mlvb_NuSol1(double value);
  long double getProb_Mlvb_NuSol2(double value);
  long double getProb_Mlvbb2_Mlvb_NoSolM(double value);
  long double getProb_Mlvbb2_Mlvb_NuSol1(double value);
  long double getProb_Mlvbb2_Mlvb_NuSol2(double value);
  long double getProb_Mq1q2(double value);
  long double getProb_Mq1q2b_Mq1q2(double value);
  long double getProb_Mq1q2bb2_Mq1q2b(double value);
  long double getProb_NuSol1(){ return m_nusol1prob; }
  long double getProb_NuSol1_FromHist();
  long double getProb_NuSol2(){ return m_nusol2prob; }
  long double getProb_NuSol2_FromHist();
  long double getProb_thad(){ return m_thadprob; }
  long double getProb_thad_FromHist();
  long double getProb_tlep(){ return m_tlepprob; }
  long double getProb_tlep_FromHist();
  long double getProb_APMvsMJ(){ return m_APMvsMJprob; }
  long double getProb_Mtt_Mq1q2b_Mlvb_NoSolM(double value);
  long double getProb_Mtt_Mq1q2b_Mlvb_NuSol1(double value);
  long double getProb_Mtt_Mq1q2b_Mlvb_NuSol2(double value);
  long double getProb_Mttb_Mthadb_Mtlep_NoSolM(double value);
  long double getProb_Mttb_Mthadb_Mtlep_NuSol1(double value);
  long double getProb_Mttb_Mthadb_Mtlep_NuSol2(double value);
  long double getProb_Mttb_Mtlepb_Mthad_NoSolM(double value);
  long double getProb_Mttb_Mtlepb_Mthad_NuSol1(double value);
  long double getProb_Mttb_Mtlepb_Mthad_NuSol2(double value);
  long double getProb_botherbfound(){ return m_botherbfoundprob; }
  long double getProb_botherbfound_FromHist();
  long double getProb_botherbnotfound(){ return m_botherbnotfoundprob; }
  long double getProb_botherbnotfound_FromHist();
  long double getProb_Mttbb_NoSolM(double value);
  long double getProb_Mttbb_NuSol1(double value);
  long double getProb_Mttbb_NuSol2(double value);
  long double getProb_Mttbj_NoSolM(double value);
  long double getProb_Mttbj_NuSol1(double value);
  long double getProb_Mttbj_NuSol2(double value);
  long double getProb_Mb2hbotherb(double value);
  long double getProb_Mb2hj(double value);
  long double getProb_Mj2hj(double value);
 private:
  TString m_channelname;
  int m_mass;
  TH1F* m_hist_Mlvb_NoSolM;
  TH1F* m_hist_Mlvb_NuSol1;
  TH1F* m_hist_Mlvb_NuSol2;
  TH1F* m_hist_Mlvbb2_Mlvb_NoSolM;
  TH1F* m_hist_Mlvbb2_Mlvb_NuSol1;
  TH1F* m_hist_Mlvbb2_Mlvb_NuSol2;
  TH1F* m_hist_Mq1q2;
  TH1F* m_hist_Mq1q2b_Mq1q2;
  TH1F* m_hist_Mq1q2bb2_Mq1q2b;
  TH1F* m_hist_NuSol_Ratio;
  TH1F* m_hist_tlepvsthad;
  double m_nusol1prob;
  double m_nusol2prob;
  double m_tlepprob;
  double m_thadprob;
  double m_APMvsMJprob;

  //histograms for event variables
  TH1F* m_hist_Mtt_Mq1q2b_Mlvb_NoSolM;
  TH1F* m_hist_Mtt_Mq1q2b_Mlvb_NuSol1;
  TH1F* m_hist_Mtt_Mq1q2b_Mlvb_NuSol2;
  TH1F* m_hist_Mttb_Mthadb_Mtlep_NoSolM;
  TH1F* m_hist_Mttb_Mthadb_Mtlep_NuSol1;
  TH1F* m_hist_Mttb_Mthadb_Mtlep_NuSol2;
  TH1F* m_hist_Mttb_Mtlepb_Mthad_NoSolM;
  TH1F* m_hist_Mttb_Mtlepb_Mthad_NuSol1;
  TH1F* m_hist_Mttb_Mtlepb_Mthad_NuSol2;

  double m_botherbfoundprob;
  double m_botherbnotfoundprob;
  TH1F* m_hist_botherbfoundvsnotfound;
  
  //histograms for mttbb
  TH1F* m_hist_Mttbb_NoSolM;
  TH1F* m_hist_Mttbb_NuSol1;
  TH1F* m_hist_Mttbb_NuSol2;
  TH1F* m_hist_Mttbj_NoSolM;
  TH1F* m_hist_Mttbj_NuSol1;
  TH1F* m_hist_Mttbj_NuSol2;

  //histograms for mbb
  TH1F* m_hist_Mb2hbotherb;
  TH1F* m_hist_Mb2hj;
  TH1F* m_hist_Mj2hj;
};

}

#endif // HPLUSDISCRIMINANTTEMPLATEHOLDER_H
