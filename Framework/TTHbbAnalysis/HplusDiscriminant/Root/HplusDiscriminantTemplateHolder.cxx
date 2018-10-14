/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HplusDiscriminant/HplusDiscriminantTemplateHolder.h"

namespace TTHbb{
HplusDiscriminantTemplateHolder::HplusDiscriminantTemplateHolder(TFile* rootfile, TString channelname, bool APM, int mass, double APMvsMJprob):
  m_channelname(channelname),
  m_mass(mass),
  m_APMvsMJprob(APMvsMJprob){
  TString plotnameprefix="h"+m_channelname;
  if(APM) plotnameprefix+="_APM_";
  else plotnameprefix+="_MJ_";
  m_hist_Mlvb_NoSolM=(TH1F *) rootfile->Get(plotnameprefix+"Mlvb_NoSolM_template");
  m_hist_Mlvb_NoSolM->SetDirectory(0);
  m_hist_Mlvb_NuSol1=(TH1F *) rootfile->Get(plotnameprefix+"Mlvb_NuSol1_template");
  m_hist_Mlvb_NuSol1->SetDirectory(0);
  m_hist_Mlvb_NuSol2=(TH1F *) rootfile->Get(plotnameprefix+"Mlvb_NuSol2_template");
  m_hist_Mlvb_NuSol2->SetDirectory(0);
  m_hist_Mlvbb2_Mlvb_NoSolM=(TH1F *) rootfile->Get(plotnameprefix+"Mlvbb2_Mlvb_NoSolM_template");
  m_hist_Mlvbb2_Mlvb_NoSolM->SetDirectory(0);
  m_hist_Mlvbb2_Mlvb_NuSol1=(TH1F *) rootfile->Get(plotnameprefix+"Mlvbb2_Mlvb_NuSol1_template");
  m_hist_Mlvbb2_Mlvb_NuSol1->SetDirectory(0);
  m_hist_Mlvbb2_Mlvb_NuSol2=(TH1F *) rootfile->Get(plotnameprefix+"Mlvbb2_Mlvb_NuSol2_template");
  m_hist_Mlvbb2_Mlvb_NuSol2->SetDirectory(0);
  m_hist_Mq1q2=(TH1F *) rootfile->Get(plotnameprefix+"Mq1q2_template");
  m_hist_Mq1q2->SetDirectory(0);
  m_hist_Mq1q2b_Mq1q2=(TH1F *) rootfile->Get(plotnameprefix+"Mq1q2b_Mq1q2_template");
  m_hist_Mq1q2b_Mq1q2->SetDirectory(0);
  m_hist_Mq1q2bb2_Mq1q2b=(TH1F *) rootfile->Get(plotnameprefix+"Mq1q2bb2_Mq1q2b_template");
  m_hist_Mq1q2bb2_Mq1q2b->SetDirectory(0);
  m_hist_NuSol_Ratio=(TH1F *) rootfile->Get(plotnameprefix+"NuSol_Ratio_template");
  m_hist_NuSol_Ratio->SetDirectory(0);
  m_hist_tlepvsthad=(TH1F *) rootfile->Get(plotnameprefix+"tlepvsthad");
  m_hist_tlepvsthad->SetDirectory(0);

  m_nusol1prob=getProb_NuSol1_FromHist();
  m_nusol2prob=getProb_NuSol2_FromHist();
  m_tlepprob=getProb_tlep_FromHist();
  m_thadprob=getProb_thad_FromHist();
  
  m_hist_botherbfoundvsnotfound=(TH1F *) rootfile->Get(plotnameprefix+"botherbfoundvsnotfound");
  m_hist_botherbfoundvsnotfound->SetDirectory(0);
  m_botherbfoundprob=getProb_botherbfound_FromHist();
  m_botherbnotfoundprob=getProb_botherbnotfound_FromHist();

  m_hist_Mb2hbotherb=(TH1F *) rootfile->Get(plotnameprefix+"Mb2hbotherb_template");
  m_hist_Mb2hbotherb->SetDirectory(0);
  m_hist_Mb2hj=(TH1F *) rootfile->Get(plotnameprefix+"Mb2hj_template");
  m_hist_Mb2hj->SetDirectory(0);
  if(mass<0){
    m_hist_Mj2hj=(TH1F *) rootfile->Get(plotnameprefix+"Mj2hj_template");
    m_hist_Mj2hj->SetDirectory(0);
  }
  else
    m_hist_Mj2hj=0;
}

HplusDiscriminantTemplateHolder::~HplusDiscriminantTemplateHolder(){
/*if(m_hist_Mlvb_NoSolM!=0) delete m_hist_Mlvb_NoSolM;
  if(m_hist_Mlvb_NuSol1!=0) delete m_hist_Mlvb_NuSol1;
  if(m_hist_Mlvb_NuSol2!=0) delete m_hist_Mlvb_NuSol2;
  if(m_hist_Mlvbb2_Mlvb_NoSolM!=0) delete m_hist_Mlvbb2_Mlvb_NoSolM;
  if(m_hist_Mlvbb2_Mlvb_NuSol1!=0) delete m_hist_Mlvbb2_Mlvb_NuSol1;
  if(m_hist_Mlvbb2_Mlvb_NuSol2!=0) delete m_hist_Mlvbb2_Mlvb_NuSol2;
  if(m_hist_Mq1q2!=0) delete m_hist_Mq1q2;
  if(m_hist_Mq1q2b_Mq1q2!=0) delete m_hist_Mq1q2b_Mq1q2;
  if(m_hist_Mq1q2bb2_Mq1q2b!=0) delete m_hist_Mq1q2bb2_Mq1q2b;
  if(m_hist_NuSol_Ratio!=0) delete m_hist_NuSol_Ratio;
  if(m_hist_tlepvsthad!=0) delete m_hist_tlepvsthad;
  if(m_hist_Mtt_Mq1q2b_Mlvb_NoSolM!=0) delete m_hist_Mtt_Mq1q2b_Mlvb_NoSolM;
  if(m_hist_Mtt_Mq1q2b_Mlvb_NuSol1!=0) delete m_hist_Mtt_Mq1q2b_Mlvb_NuSol1;
  if(m_hist_Mtt_Mq1q2b_Mlvb_NuSol2!=0) delete m_hist_Mtt_Mq1q2b_Mlvb_NuSol2;
  if(m_hist_Mttb_Mthadb_Mtlep_NoSolM!=0) delete m_hist_Mttb_Mthadb_Mtlep_NoSolM;
  if(m_hist_Mttb_Mthadb_Mtlep_NuSol1!=0) delete m_hist_Mttb_Mthadb_Mtlep_NuSol1;
  if(m_hist_Mttb_Mthadb_Mtlep_NuSol2!=0) delete m_hist_Mttb_Mthadb_Mtlep_NuSol2;
  if(m_hist_Mttb_Mtlepb_Mthad_NoSolM!=0) delete m_hist_Mttb_Mtlepb_Mthad_NoSolM;
  if(m_hist_Mttb_Mtlepb_Mthad_NuSol1!=0) delete m_hist_Mttb_Mtlepb_Mthad_NuSol1;
  if(m_hist_Mttb_Mtlepb_Mthad_NuSol2!=0) delete m_hist_Mttb_Mtlepb_Mthad_NuSol2;
  if(m_hist_Mttbb_NoSolM!=0) delete m_hist_Mttbb_NoSolM;
  if(m_hist_Mttbb_NuSol1!=0) delete m_hist_Mttbb_NuSol1;
  if(m_hist_Mttbb_NuSol2!=0) delete m_hist_Mttbb_NuSol2;
  if(m_hist_Mttbj_NoSolM!=0) delete m_hist_Mttbj_NoSolM;
  if(m_hist_Mttbj_NuSol1!=0) delete m_hist_Mttbj_NuSol1;
  if(m_hist_Mttbj_NuSol2!=0) delete m_hist_Mttbj_NuSol2;
  if(m_hist_Mb2hbotherb!=0) delete m_hist_Mb2hbotherb;
  if(m_hist_Mb2hj!=0) delete m_hist_Mb2hj; */
}
    
long double HplusDiscriminantTemplateHolder::getProb_Mlvb_NoSolM(double value){
  int bin = m_hist_Mlvb_NoSolM->FindBin(value);
  return m_hist_Mlvb_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mlvb_NuSol1(double value){
  int bin = m_hist_Mlvb_NuSol1->FindBin(value);
  return m_hist_Mlvb_NuSol1->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mlvb_NuSol2(double value){
  int bin = m_hist_Mlvb_NuSol2->FindBin(value);
  return m_hist_Mlvb_NuSol2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mlvbb2_Mlvb_NoSolM(double value){
  int bin = m_hist_Mlvbb2_Mlvb_NoSolM->FindBin(value);
  return m_hist_Mlvbb2_Mlvb_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mlvbb2_Mlvb_NuSol1(double value){
  int bin = m_hist_Mlvbb2_Mlvb_NuSol1->FindBin(value);
  return m_hist_Mlvbb2_Mlvb_NuSol1->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mlvbb2_Mlvb_NuSol2(double value){
  int bin = m_hist_Mlvbb2_Mlvb_NuSol2->FindBin(value);
  return m_hist_Mlvbb2_Mlvb_NuSol2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mq1q2(double value){
  int bin = m_hist_Mq1q2->FindBin(value);
  return m_hist_Mq1q2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mq1q2b_Mq1q2(double value){
  int bin = m_hist_Mq1q2b_Mq1q2->FindBin(value);
  return m_hist_Mq1q2b_Mq1q2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mq1q2bb2_Mq1q2b(double value){
  int bin = m_hist_Mq1q2bb2_Mq1q2b->FindBin(value);
  return m_hist_Mq1q2bb2_Mq1q2b->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_NuSol1_FromHist(){
  int bin = m_hist_NuSol_Ratio->FindBin(1);
  return m_hist_NuSol_Ratio->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_NuSol2_FromHist(){
  int bin = m_hist_NuSol_Ratio->FindBin(2);
  return m_hist_NuSol_Ratio->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_tlep_FromHist(){
  int bin = m_hist_tlepvsthad->GetXaxis()->FindBin("tlep");
  return m_hist_tlepvsthad->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_thad_FromHist(){
  int bin = m_hist_tlepvsthad->GetXaxis()->FindBin("thad");
  return m_hist_tlepvsthad->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mtt_Mq1q2b_Mlvb_NoSolM(double value){
  int bin = m_hist_Mtt_Mq1q2b_Mlvb_NoSolM->FindBin(value);
  return m_hist_Mtt_Mq1q2b_Mlvb_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mtt_Mq1q2b_Mlvb_NuSol1(double value){
  int bin = m_hist_Mtt_Mq1q2b_Mlvb_NuSol1->FindBin(value);
  return m_hist_Mtt_Mq1q2b_Mlvb_NuSol1->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mtt_Mq1q2b_Mlvb_NuSol2(double value){
  int bin = m_hist_Mtt_Mq1q2b_Mlvb_NuSol2->FindBin(value);
  return m_hist_Mtt_Mq1q2b_Mlvb_NuSol2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttb_Mthadb_Mtlep_NoSolM(double value){
  int bin = m_hist_Mttb_Mthadb_Mtlep_NoSolM->FindBin(value);
  return m_hist_Mttb_Mthadb_Mtlep_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttb_Mthadb_Mtlep_NuSol1(double value){
  int bin = m_hist_Mttb_Mthadb_Mtlep_NuSol1->FindBin(value);
  return m_hist_Mttb_Mthadb_Mtlep_NuSol1->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttb_Mthadb_Mtlep_NuSol2(double value){
  int bin = m_hist_Mttb_Mthadb_Mtlep_NuSol2->FindBin(value);
  return m_hist_Mttb_Mthadb_Mtlep_NuSol2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttb_Mtlepb_Mthad_NoSolM(double value){
  int bin = m_hist_Mttb_Mtlepb_Mthad_NoSolM->FindBin(value);
  return m_hist_Mttb_Mtlepb_Mthad_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttb_Mtlepb_Mthad_NuSol1(double value){
  int bin = m_hist_Mttb_Mtlepb_Mthad_NuSol1->FindBin(value);
  return m_hist_Mttb_Mtlepb_Mthad_NuSol1->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttb_Mtlepb_Mthad_NuSol2(double value){
  int bin = m_hist_Mttb_Mtlepb_Mthad_NuSol2->FindBin(value);
  return m_hist_Mttb_Mtlepb_Mthad_NuSol2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_botherbfound_FromHist(){
  int bin = m_hist_botherbfoundvsnotfound->GetXaxis()->FindBin("found");
  return m_hist_botherbfoundvsnotfound->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_botherbnotfound_FromHist(){
  int bin = m_hist_botherbfoundvsnotfound->GetXaxis()->FindBin("notfound");
  return m_hist_botherbfoundvsnotfound->GetBinContent(bin);
}
  
long double HplusDiscriminantTemplateHolder::getProb_Mttbb_NoSolM(double value){
  int bin = m_hist_Mttbb_NoSolM->FindBin(value);
  return m_hist_Mttbb_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttbb_NuSol1(double value){
  int bin = m_hist_Mttbb_NuSol1->FindBin(value);
  return m_hist_Mttbb_NuSol1->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttbb_NuSol2(double value){
  int bin = m_hist_Mttbb_NuSol2->FindBin(value);
  return m_hist_Mttbb_NuSol2->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttbj_NoSolM(double value){
  int bin = m_hist_Mttbj_NoSolM->FindBin(value);
  return m_hist_Mttbj_NoSolM->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mttbj_NuSol1(double value){
  int bin = m_hist_Mttbj_NuSol1->FindBin(value);
  return m_hist_Mttbj_NuSol1->GetBinContent(bin);
}
  
long double HplusDiscriminantTemplateHolder::getProb_Mttbj_NuSol2(double value){
  int bin = m_hist_Mttbj_NuSol2->FindBin(value);
  return m_hist_Mttbj_NuSol2->GetBinContent(bin);
}
  
long double HplusDiscriminantTemplateHolder::getProb_Mb2hbotherb(double value){
  int bin = m_hist_Mb2hbotherb->FindBin(value);
  return m_hist_Mb2hbotherb->GetBinContent(bin);
}
  
long double HplusDiscriminantTemplateHolder::getProb_Mb2hj(double value){
  int bin = m_hist_Mb2hj->FindBin(value);
  return m_hist_Mb2hj->GetBinContent(bin);
}

long double HplusDiscriminantTemplateHolder::getProb_Mj2hj(double value){
  int bin = m_hist_Mj2hj->FindBin(value);
  return m_hist_Mj2hj->GetBinContent(bin);
}

}
