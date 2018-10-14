#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TStyle.h"
#include "TRandom.h"
#include "TRandom2.h"
#include "TF1.h"

#include "AtlasStyle.C"

const float MW = 80.4;

using namespace std;


bool isTagged(float bweight){
  return bweight>0.645925;
}




// Extract tagging efficiencies to be used for ttTRF
// Can run on MC (all the ttbar) and data (after subtracting non-ttbar MC)
// Need to create two different output files (MC and data) for each systematic (weight or not)

// Adding tree systematics: 
//     To make clear what systematics are or not from trees, use the convention to add "tree_"
//     in front of its name, which HAS to be the name of the corresponding Tree. 
//     The prefix will be removed afterwards. This has the advantage not to change the signature of the function,
//     for backward compatibility.

void Get_ttTRF_2018(string sample="data",string syst="nominal",string sel="5j6j",bool showall=false, bool protectLweights=false, string camp="mc16a"){
  void ShowMaps(std::map<std::string,float> , std::map<std::string,float> );
  SetAtlasStyle();
  
  string channel = "ljets";
//   if(sel.find("dilep")!=string::npos) channel ="dilep";
  if(sel.find("dilep")!=string::npos) channel ="dilep_sf_cut";
  
  string syst_directory="/gpfs/atlas/gpanizzo/DataAnalysis/Release21/ljetsOS-framework/TTHbbAnalysis/run/test/";
  string directory_nom=syst_directory; // they were different at the beginning ..
  string directory_qcd=syst_directory;

  // We could just hadd nominal and others and use only one "directory", but managing it with two different folders 
  // allows to skip the hadding step (during which one needs twice the disk space needed ...)

  string syst_name = syst,directory=directory_nom,tree_name="nominal_Loose";
  bool skip_nnlo_rew = true;
  if (syst.find("_wow") != std::string::npos) {
      skip_nnlo_rew = true;
      size_t mpos = syst.find("_wow");
      syst=syst.erase(mpos,4);
  }


  bool is_syst_tree = false;
  if (syst.find("tree_") == 0) {
      syst_name.erase(0,5);
      tree_name=syst_name;
      if (skip_nnlo_rew) {
           size_t mpos = tree_name.find("_wow");
           tree_name=tree_name.erase(mpos,4);
      }
      is_syst_tree=true; 
      directory=syst_directory+tree_name+"/";
  }

  //std::cout << syst<<","<< syst_name<<","<< tree_name<<","<< std::endl;
  
  float lumi = 36100;
  
  vector<string> njSel; njSel.clear();
  for(int i_nj=4;i_nj<=10;i_nj++){
    string njString = Form("%dj",i_nj);
    if ( (i_nj==10 && channel.find("ljets")!=std::string::npos) || (i_nj==8 && channel.find("dilep")!=std::string::npos) ) njString="ge"+njString;
    if(sel.find(njString)!=string::npos || sel.find("incl")!=string::npos) njSel.push_back(njString);
  }
  
  TChain *t0 = new TChain(tree_name.c_str());  
  TChain *t1 = new TChain(tree_name.c_str()); 
  TChain *t2 = new TChain(tree_name.c_str()); 

  // alias to manage three trees
  TChain *t[3];
  t[0]=t0; t[1]=t1; t[2]=t2;

  std::map<std::string, int> edges; 
  
  // Define samples on which should run. Normally ttbar_powpyt6+bfil+Ht slices, 
  // but when running on ttbar syst choose proper ones
  if(sample=="mc"){
    if(syst.find("ttPs") != std::string::npos){
      for(auto nj : njSel){
        is_syst_tree = true;
        if (!t0->Add((directory+channel+nj+"/ttbar_herpp.root").c_str()) )          std::cout << "ERROR: ttbar_herpp.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_herpp_HT_6c_1k.root").c_str()))  std::cout << "ERROR: ttbar_herpp_HT_6c_1k.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_herpp_HT_1k_1k5.root").c_str())) std::cout << "ERROR: ttbar_herpp_HT_1k_1k5.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_herpp_HT_1k5.root").c_str()))    std::cout << "ERROR: ttbar_herpp_HT_1k5.root not found !" << std::endl;
      }
    }
    else if(syst.find("ttGen") != std::string::npos  ){
      for(auto nj : njSel){
        is_syst_tree = true;  
        if (!t0->Add((directory+channel+nj+"/ttbar_amcatnlo.root").c_str()) )       std::cout << "ERROR: ttbar_amcatnlo.root not found !" << std::endl;
      }
    }
    else if(syst.find("ttPy8") != std::string::npos  ){
      for(auto nj : njSel){
        is_syst_tree = true;  
        if (!t0->Add((directory+channel+nj+"/ttbar_pyt8_hdamp258."+camp+".root").c_str()) )  std::cout << "ERROR: ttbar_pyt8_hdamp258."<< camp <<".root not found !" << std::endl;
      }
    }
    else if(syst.find("ttRadUp") != std::string::npos){
      for(auto nj : njSel){
        is_syst_tree = true;  
        if (!t0->Add((directory+channel+nj+"/ttbar_radHi.root").c_str()) )            std::cout << "ERROR: ttbar_radHi.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_radHi_HT_6c_1k.root").c_str()) )   std::cout << "ERROR: ttbar_radHi_HT_6c_1k.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_radHi_HT_1k_1k5.root").c_str()) )  std::cout << "ERROR: ttbar_radHi_HT_1k_1k5.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_radHi_HT_1k5.root").c_str()) )     std::cout << "ERROR: ttbar_radHi_HT_1k5.root not found !" << std::endl;
      }
    }
    else if(syst.find("ttRadDown") != std::string::npos ){
      for(auto nj : njSel){
        is_syst_tree = true;  
        if (!t0->Add((directory+channel+nj+"/ttbar_radLo.root").c_str()) )            std::cout << "ERROR: ttbar_radLo.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_radLo_HT_1k_1k5.root").c_str()) )  std::cout << "ERROR: ttbar_radLo_HT_1k_1k5.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_radLo_HT_6c_1k.root" ).c_str()) )  std::cout << "ERROR: ttbar_radLo_HT_6c_1k.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_radLo_HT_1k5.root"   ).c_str()) )  std::cout << "ERROR: ttbar_radLo_HT_1k5.root not found !" << std::endl;
      }
    }
    else{
      for(auto nj : njSel){
        if (!t0->Add((directory+channel+nj+"/ttbar_powpyt6.root"  ).c_str()) )   std::cout << "ERROR: ttbar_powpyt6.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_HT_6c_1k.root" ).c_str()) )   std::cout << "ERROR: ttbar_HT_6c_1k.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_HT_1k_1k5.root").c_str()) )   std::cout << "ERROR: ttbar_HT_1k_1k5.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_HT_1k5.root"   ).c_str()) )   std::cout << "ERROR: ttbar_HT_1k5.root not found !" << std::endl;
        if (!t0->Add((directory+channel+nj+"/ttbar_bfilter.root"  ).c_str()) )   std::cout << "ERROR: ttbar_bfilter.root not found !" << std::endl;
      }
    }
  }
  else if(sample=="data"){ // ok, non-ttbar samples are in sample "data" (but in tree t1). 
                           // Fair. But remember to weight them at some point ...
    for(auto nj : njSel){
      // data. "/nominal_Loose" ensures the correct tree will be read in any case.
      string datacamp="15_16";
      if ( camp.find("mc16a") == std::string::npos) datacamp="17";
      if (!t0->Add((directory_nom+channel+nj+"/data."+datacamp+".root/nominal_Loose").c_str()) )  std::cout << "ERROR: data."<< datacamp <<".root not found !" << std::endl; 
      // add non-ttbar
      // Please, leave them in this order, or improve the systematics checks on them at the end ...
      if (!t1->Add((directory+channel+nj+"/wjets_221."+camp+".root").c_str()) )             std::cout << "ERROR: wjets_221."<< camp <<".root not found !" << std::endl;
      edges["wjets"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/zjets_221."+camp+".root").c_str()) )             std::cout << "ERROR: zjets_221."<< camp <<".root not found !" << std::endl; 
      edges["zjets"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/singleTop."+camp+".root").c_str()) )         std::cout << "ERROR: singleTop."<< camp <<".root not found !" << std::endl;
      edges["singleTop"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/ttW."+camp+".root").c_str()) )               std::cout << "ERROR: ttW."<< camp <<".root not found !" << std::endl;
      edges["ttW"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/ttZ."+camp+".root").c_str()) )               std::cout << "ERROR: ttZ."<< camp <<".root not found !" << std::endl;
      edges["ttZ"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/ttWW."+camp+".root").c_str()) )               std::cout << "ERROR: ttWW."<< camp <<".root not found !" << std::endl;
      edges["ttWW"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/ttH."+camp+".root").c_str()) )               std::cout << "ERROR: ttH."<< camp <<".root not found !" << std::endl;
      edges["ttH"] = t1->GetEntries();
      if (!t1->Add((directory+channel+nj+"/diboson."+camp+".root").c_str()) )           std::cout << "ERROR: diboson."<< camp <<".root not found !" << std::endl;
      edges["diboson"] = t1->GetEntries();
/*      if (channel.find("ljets")!=std::string::npos){ // qcd only for single lepton
        //for (int nsub=1; nsub <=18; nsub++){
        //     string subqcd = Form(    (directory_qcd+channel+nj+"/qcd_all_v%d.root/nominal_Loose").c_str() , nsub );
        //     //std::cout << "INFO: Adding " << subqcd << std::endl;
        //     t2->Add(  subqcd.c_str()   ); // qcd. "/nominal_Loose" ensures the correct tree will be read in any case.
        //}
             // qcd. "/nominal_Loose" ensures the correct tree will be read in any case.
             string subqcd = directory_qcd+channel+nj+"/qcd.root/nominal_Loose";
             if (!t2->Add(  subqcd.c_str()   ) )          std::cout << "ERROR: qcd.root not found !" << std::endl;
      }
*/
    }
  }
  else return;
  
  int rows=1;
  if (showall) rows=2;
  TCanvas *c = new TCanvas("c","c",1600,400*rows);
  c->Divide(4,rows);
  
  TFile *f;
    f = new TFile(("data/ttTRFmaps2018_"+sample+"_"+sel+"__"+syst_name+".root").c_str(),"RECREATE");
  
    
  //Define histograms and corresponding binnings:
  // 1) Averages
  TH1F *h_tot = new TH1F("h_tot","h_tot",1,0,1);
  TH1F *h_tag = new TH1F("h_tag","h_tag",1,0,1);
  TH1F *h_eff = new TH1F("h_eff","h_eff",1,0,1);
  h_tot->Sumw2();
  h_tag->Sumw2();
  
  // 2) Pt
  double pt_bins[100];
  int pt_nbins;
  if(sel.find("_dilep_4b")!=string::npos){
    double pt_bins_tmp[] = {25,50,100,300,700};
    pt_nbins = 4;
    memcpy(pt_bins,pt_bins_tmp,sizeof(pt_bins_tmp));
  }
  else if(sel.find("_dilep")!=string::npos){
    double pt_bins_tmp[] = {25,35,45,100,300,700};
    pt_nbins = 5;
    memcpy(pt_bins,pt_bins_tmp,sizeof(pt_bins_tmp));
  }
  else if(sel.find("_4b")!=string::npos){
    double pt_bins_tmp[] = {25,30,40,50,60,70,100,130,150,300,700};
    pt_nbins = 10;
    memcpy(pt_bins,pt_bins_tmp,sizeof(pt_bins_tmp));
  }
  else{
    double pt_bins_tmp[] = {25,27.5,30,32.5,35,37.5,40,42.5,45,47.5,50,55,60,65,70,85,100,115,130,145,160,190,300,700};
    pt_nbins = 23;
    memcpy(pt_bins,pt_bins_tmp,sizeof(pt_bins_tmp));
  }
  TH1F *h_tot_pt = new TH1F("h_tot_pt","h_tot_pt",pt_nbins,pt_bins);
  TH1F *h_tag_pt = new TH1F("h_tag_pt","h_tag_pt",pt_nbins,pt_bins);
  TH1F *h_eff_pt = new TH1F("h_eff_pt","h_eff_pt",pt_nbins,pt_bins);
  h_tot_pt->Sumw2();
  h_tag_pt->Sumw2();

  // 2b) get k
  double k_pt_bins_tmp[] = {25,75,175,700};
  double k_pt_bins[100];
  int k_pt_nbins=3;
  memcpy(k_pt_bins,k_pt_bins_tmp,sizeof(k_pt_bins_tmp));
  TH1F *k_non_tag_pt = new TH1F("k_non_tag_pt","k_non_tag_pt",k_pt_nbins,k_pt_bins);
  TH1F *k_non_tag_if_pt = new TH1F("k_non_tag_if_pt","k_non_tag_if_pt",k_pt_nbins,k_pt_bins);
  TH1F *k_eff_pt = new TH1F("k_eff_pt","k_eff_pt",k_pt_nbins,k_pt_bins);
  k_non_tag_pt->Sumw2();
  k_non_tag_if_pt->Sumw2();

  
  // 3) Eta
  double eta_bins[100];
  int eta_nbins;
  if(sel.find("_dilep_4b")!=string::npos){
    double eta_bins_tmp[] = {0.00,1.00,2.00,2.50};
    eta_nbins = 3;
    memcpy(eta_bins,eta_bins_tmp,sizeof(eta_bins_tmp));
  }
  else if(sel.find("_dilep")!=string::npos){
    double eta_bins_tmp[] = {0.00,0.50,1.00,1.50,2.00,2.50};
    eta_nbins = 5;
    memcpy(eta_bins,eta_bins_tmp,sizeof(eta_bins_tmp));
  }
  else if(sel.find("_4b")!=string::npos){
    double eta_bins_tmp[] = {0.00,0.50,1.00,1.50,2.00,2.50};
    eta_nbins = 5;
    memcpy(eta_bins,eta_bins_tmp,sizeof(eta_bins_tmp));
  }
  else{
    double eta_bins_tmp[] = {0.00,0.25,0.50,0.75,1.00,1.25,1.50,1.77,2.00,2.25,2.50};
    eta_nbins = 10;
    memcpy(eta_bins,eta_bins_tmp,sizeof(eta_bins_tmp));
  }
  TH1F *h_tot_eta = new TH1F("h_tot_eta","h_tot_eta",eta_nbins,eta_bins);
  TH1F *h_tag_eta = new TH1F("h_tag_eta","h_tag_eta",eta_nbins,eta_bins);
  TH1F *h_eff_eta = new TH1F("h_eff_eta","h_eff_eta",eta_nbins,eta_bins);
  h_tot_eta->Sumw2();
  h_tag_eta->Sumw2();
  
  // 4) dRj
  double dRj_bins[100];
  int dRj_nbins;
  if(sel.find("_dilep_4b")!=string::npos){
    double dRj_bins_tmp[] = {0,25};
    dRj_nbins = 1;
    memcpy(dRj_bins,dRj_bins_tmp,sizeof(dRj_bins_tmp));
  }
  else if(sel.find("_dilep")!=string::npos){
    double dRj_bins_tmp[] = {0,3,5,7,10,25};
    dRj_nbins = 5;
    memcpy(dRj_bins,dRj_bins_tmp,sizeof(dRj_bins_tmp));
  }
  else if(sel.find("_4b")!=string::npos){
    double dRj_bins_tmp[] = {0,2.5,5,7.5,10,12.5,15,25};
    dRj_nbins = 7;
    memcpy(dRj_bins,dRj_bins_tmp,sizeof(dRj_bins_tmp));
  }
  else{
    double dRj_bins_tmp[] = {0,0.5,1,2.5,3,3.5,4,4.5,5,6,7,8,9,10,11,14,25};
    dRj_nbins = 16;
    memcpy(dRj_bins,dRj_bins_tmp,sizeof(dRj_bins_tmp));
  }
  TH1F *h_tot_dRj = new TH1F("h_tot_dRj","h_tot_dRj",dRj_nbins,dRj_bins);
  TH1F *h_tag_dRj = new TH1F("h_tag_dRj","h_tag_dRj",dRj_nbins,dRj_bins);
  TH1F *h_eff_dRj = new TH1F("h_eff_dRj","h_eff_dRj",dRj_nbins,dRj_bins);
  h_tot_dRj->Sumw2();
  h_tag_dRj->Sumw2();
  
  // 5) Nj  
  double Nj_bins[100];
  int Nj_nbins;
  double Nj_bins_tmp[] = {4,5,6,7,8,9,10,11};
  Nj_nbins = 7;
  memcpy(Nj_bins,Nj_bins_tmp,sizeof(Nj_bins_tmp));
  TH1F *h_tot_Nj = new TH1F("h_tot_Nj","h_tot_Nj",Nj_nbins,Nj_bins);
  TH1F *h_tag_Nj = new TH1F("h_tag_Nj","h_tag_Nj",Nj_nbins,Nj_bins);
  TH1F *h_eff_Nj = new TH1F("h_eff_Nj","h_eff_Nj",Nj_nbins,Nj_bins);
  h_tot_Nj->Sumw2();
  h_tag_Nj->Sumw2();

  // Interlude: set th3 to find correlations between variables
  TH3F*h_corr_comp = new TH3F("h_corr_comp", "Histogram used to get correlations",pt_nbins,pt_bins,eta_nbins,eta_bins,dRj_nbins,dRj_bins);
  TH2F*h_corr = new TH2F("h_corr","Correlations",3,0,3,3,0,3);
  h_corr->GetYaxis()->SetBinLabel(1,"p_{T}");
  h_corr->GetYaxis()->SetBinLabel(2,"#eta");
  h_corr->GetYaxis()->SetBinLabel(3,"#Delta R_{j}");
  h_corr->GetXaxis()->SetBinLabel(1,"p_{T}");
  h_corr->GetXaxis()->SetBinLabel(2,"#eta");
  h_corr->GetXaxis()->SetBinLabel(3,"#Delta R_{j}");
  gStyle->SetPaintTextFormat(".1f");
  h_corr->SetMarkerSize(1000);

  // 6) pt vs eta  
  pt_nbins=20;
  if (syst.find("ttGen") != std::string::npos && syst.find("ttPy8") != std::string::npos) pt_nbins=10;
  TH2F *h_tot_pt_eta = new TH2F("h_tot_pt_eta","h_tot_pt_eta",pt_nbins,TMath::Log(25.0),TMath::Log(700),5,0,2.5);
  TH2F *h_tag_pt_eta = new TH2F("h_tag_pt_eta","h_tag_pt_eta",pt_nbins,TMath::Log(25.0),TMath::Log(700),5,0,2.5);
  TH2F *h_eff_pt_eta = new TH2F("h_eff_pt_eta","h_eff_pt_eta",pt_nbins,TMath::Log(25.0),TMath::Log(700),5,0,2.5);
  h_tot_pt_eta->Sumw2();
  h_tag_pt_eta->Sumw2();

 // 7) pt vs dRj  
  //TH2F *h_tot_pt_dRj = new TH2F("h_tot_pt_dRj","h_tot_pt_dRj",pt_nbins,TMath::Log(25.0),TMath::Log(700),5,0,2.5);
  //TH2F *h_tag_pt_dRj = new TH2F("h_tag_pt_dRj","h_tag_pt_dRj",pt_nbins,TMath::Log(25.0),TMath::Log(700),5,0,2.5);
  //TH2F *h_eff_pt_dRj = new TH2F("h_eff_pt_dRj","h_eff_pt_dRj",pt_nbins,TMath::Log(25.0),TMath::Log(700),5,0,2.5);
  //h_tot_pt_dRj->Sumw2();
  //h_tag_pt_dRj->Sumw2();
  
  // 8) dRjone
  int dRjone_bins=10;
  TH1F *h_tot_dRjone = new TH1F("h_tot_dRjone","h_tot_dRjone",dRjone_bins,0,log(5));
  TH1F *h_tag_dRjone = new TH1F("h_tag_dRjone","h_tag_dRjone",dRjone_bins,0,log(5));
  TH1F *h_eff_dRjone = new TH1F("h_eff_dRjone","h_eff_dRjone",dRjone_bins,0,log(5));
  h_tot_dRjone->Sumw2();
  h_tag_dRjone->Sumw2();

  // 9) dRj vs Nj  
  dRj_nbins=10; Nj_nbins=4;
  int low=5,high=9,N;
  if (channel.find("dilep") != std::string::npos) {
       low=4; high=6; dRj_nbins=5; Nj_nbins=3;
  }
  TH2F *h_tot_dRj_Nj = new TH2F("h_tot_dRj_Nj","h_tot_dRj_Nj",dRj_nbins, 0.0, 4,Nj_nbins,low,high);
  TH2F *h_tag_dRj_Nj = new TH2F("h_tag_dRj_Nj","h_tag_dRj_Nj",dRj_nbins, 0.0, 4,Nj_nbins,low,high);
  TH2F *h_eff_dRj_Nj = new TH2F("h_eff_dRj_Nj","h_eff_dRj_Nj",dRj_nbins, 0.0, 4,Nj_nbins,low,high);
  h_tot_dRj_Nj->Sumw2();
  h_tag_dRj_Nj->Sumw2();

  // 10) Fill a tree in order to reproduce results afterwards
  Double_t teff_dRjNj,teff_pt,teff_eta,teff_weight;
  Int_t teff_istagged,teff_Nj;
  std::string teff_sample=sample;
  TTree *teff=new TTree("teff","Jets building effective efficiencies");
  teff->Branch("teff_dRjNj",&teff_dRjNj,"teff_dRjNj/D");
  teff->Branch("teff_weight",&teff_weight,"teff_weight/D");
  teff->Branch("teff_pt",&teff_pt,"teff_pt/D");
  teff->Branch("teff_eta",&teff_eta,"teff_eta/D");
  teff->Branch("teff_istagged",&teff_istagged,"teff_istagged/I");
  teff->Branch("teff_Nj",&teff_Nj,"teff_Nj/I");
  teff->Branch("teff_sample",&teff_sample);

  // Link all useful branches (why didn't we create this as an Ht offline? To be completely independent?)
  
  vector<char>  *jet_isbtagged = 0; for (int i=0; i<3; i++) t[i]->SetBranchAddress("isbtagged_MV2c10_77",&jet_isbtagged); 
  vector<float> *jet_pt = 0;        for (int i=0; i<3; i++) t[i]->SetBranchAddress("jet_pt_GeV",&jet_pt);              // t1->SetBranchAddress("jets_pt",&jet_pt);
  vector<float> *jet_eta = 0;       for (int i=0; i<3; i++) t[i]->SetBranchAddress("jet_eta",&jet_eta);            // t1->SetBranchAddress("jets_eta",&jet_eta);
  vector<float> *jet_phi = 0;       for (int i=0; i<3; i++) t[i]->SetBranchAddress("jet_phi",&jet_phi);            // t1->SetBranchAddress("jets_phi",&jet_phi);
  vector<float> *jet_e = 0;         for (int i=0; i<3; i++) t[i]->SetBranchAddress("jet_e_GeV",&jet_e);                // t1->SetBranchAddress("jets_e",&jet_e);
  vector<float> *jet_mv2c10 = 0;   for (int i=0; i<3; i++) t[i]->SetBranchAddress("jet_mv2c10",&jet_mv2c10); // t1->SetBranchAddress("jets_btag_weight",&jet_mv2c10);
  int nJets;                        for (int i=0; i<3; i++) t[i]->SetBranchAddress("nJets",&nJets);                 // t1->SetBranchAddress("nJets",&nJets);
  int nBTags;                       for (int i=0; i<3; i++) t[i]->SetBranchAddress("nBTags_MV2c10_77",&nBTags);               // t1->SetBranchAddress("nBTags",&nBTags);
//  int nHOT;                         for (int i=0; i<3; i++) t[i]->SetBranchAddress("nHOT",&nHOT);                   // t1->SetBranchAddress("nBTags",&nBTags);
  
  
//  int ejets = 0;                    for (int i=0; i<3; i++) t[i]->SetBranchAddress("ejets",&ejets);                 // t1->SetBranchAddress("ejets",&ejets);
//  int mujets = 0;                   for (int i=0; i<3; i++) t[i]->SetBranchAddress("mujets",&mujets);               // t1->SetBranchAddress("mujets",&mujets);

/*
  int ejets_loose=0;                t2->SetBranchAddress("ejets_loose",&ejets_loose);
  int mujets_loose = 0;             t2->SetBranchAddress("mujets_loose",&mujets_loose);
  int is_central_QCD = 0;           t2->SetBranchAddress("is_central_QCD",&is_central_QCD);
*/
/*  int ee = 0;                       for (int i=0; i<3; i++) t[i]->SetBranchAddress("ee",&ee);                       // t1->SetBranchAddress("ee",&ee);
  int emu = 0;                      for (int i=0; i<3; i++) t[i]->SetBranchAddress("emu",&emu);                     // t1->SetBranchAddress("emu",&emu);
  int mumu = 0;                     for (int i=0; i<3; i++) t[i]->SetBranchAddress("mumu",&mumu);                   // t1->SetBranchAddress("mumu",&mumu);

  int dil_loose = 0;                t2->SetBranchAddress("dil_loose",&dil_loose);
*/

  // int trigger_el = 0;               for (int i=0; i<3; i++) t[i]->SetBranchAddress("trigger_el",&trigger_el);       // t1->SetBranchAddress("trigger_el",&trigger_el);
  // int trigger_muon = 0;             for (int i=0; i<3; i++) t[i]->SetBranchAddress("trigger_muon",&trigger_muon);   // t1->SetBranchAddress("trigger_muon",&trigger_muon);
  //int trgMatch_el = 0;              for (int i=0; i<3; i++) t[i]->SetBranchAddress("trgMatch_el",&trgMatch_el);     // t1->SetBranchAddress("trgMatch_el",&trgMatch_el);
  //int trgMatch_mu = 0;              for (int i=0; i<3; i++) t[i]->SetBranchAddress("trgMatch_mu",&trgMatch_mu);     // t1->SetBranchAddress("trgMatch_mu",&trgMatch_mu);
  int pass_triangle = 0;            for (int i=0; i<3; i++) t[i]->SetBranchAddress("pass_triangle",&pass_triangle); // t1->SetBranchAddress("pass_triangle",&pass_triangle);
  float met = 0;                    for (int i=0; i<3; i++) t[i]->SetBranchAddress("met_met",&met);                     // t1->SetBranchAddress("met",&met);  
  
/*  int pass_dilep = 0;               for (int i=0; i<3; i++) t[i]->SetBranchAddress("pass_dilep",&pass_dilep);       // t1->SetBranchAddress("pass_dilep",&pass_dilep);
  int pass_dilep_sf_cut = 0;        for (int i=0; i<3; i++) t[i]->SetBranchAddress("pass_dilep_sf_cut",&pass_dilep_sf_cut); // t1->SetBranchAddress("pass_dilep_sf_cut",&pass_dilep_sf_cut);
*/  
  float weight_normalise = 1.;      if(sample!="data") t0->SetBranchAddress("weight_normalise",&weight_normalise); t1->SetBranchAddress("weight_normalise",&weight_normalise);
                                    // t2 contains, if any, always data like events, so no branch address to weight_normalise


  std::map<std::string,float> NomWeights;
  std::map<std::string,float> QcdWeights;

  std::ifstream infile;
  std::string line;

  /// Open file with list of weights
  infile.open("weights.txt");

  /// Loop through all lines in the file
  while(std::getline(infile,line)) if (line.find("#")==std::string::npos) NomWeights[line]=1.;
  infile.close();

/*  if (sample=="mc" ){ // ttbar sample: link proper nominal weights
     NomWeights["weight_ttbar_NNLO_1L"]=1.0;
     NomWeights["weight_ttbb_ttbb_Nominal_weight"]=1.0;
  }
  else { 
       NomWeights["weight_WZ_2_2"]=1.0; // safely multiply it for every mc sample, correctly set to 1 in trees if non relevant (checked)
       QcdWeights["weight_QCD_1L_el"]=0.0; // set to zero, better in this case
       QcdWeights["weight_QCD_1L_mu"]=0.0;
  }
*/
  std::map<std::string, float>::iterator it = NomWeights.begin();
  while ( it!=NomWeights.end() ) { // non relying on C11
           if (sample!="data") t0->SetBranchAddress((it->first).c_str(),&NomWeights[it->first]); // if sample==data "t0" is indeed data!
           t1->SetBranchAddress((it->first).c_str(),&NomWeights[it->first]);
       it++; 
  }
  
   
  
  int HF_SimpleClassification;      //if(sample=="mc" && syst.find("Norm")!=string::npos){t0->SetBranchAddress("ttbar_class",&HF_SimpleClassification); t1->SetBranchAddress("ttbar_class",&HF_SimpleClassification);}
 
  if(sample=="mc"){t0->SetBranchAddress("HF_SimpleClassification",&HF_SimpleClassification); t1->SetBranchAddress("HF_SimpleClassification",&HF_SimpleClassification);}

 std::map<std::string,float> SystWeights; // to add a systematic weight, just add its name to the map

  // btag systs
 int nBEigen=6, nCEigen=3, nLightEigen=16;  // useful afterwards
/* std::string flavour="weight_btag_FT_EFF_Eigen_B_";
 for (int ie=0; ie<nBEigen; ie++) {
     SystWeights[flavour+std::to_string(ie)+"__1up"]=1.0;
     SystWeights[flavour+std::to_string(ie)+"__1down"]=1.0;
 }
 flavour="weight_btag_FT_EFF_Eigen_C_";
 for (int ie=0; ie<nCEigen; ie++) {
     SystWeights[flavour+std::to_string(ie)+"__1up"]=1.0;
     SystWeights[flavour+std::to_string(ie)+"__1down"]=1.0;
 }
 flavour="weight_btag_FT_EFF_Eigen_Light_";
 for (int ie=0; ie<nLightEigen; ie++) {
     SystWeights[flavour+std::to_string(ie)+"__1up"]=1.0;
     SystWeights[flavour+std::to_string(ie)+"__1down"]=1.0;
 } 

 SystWeights["weight_btag_FT_EFF_extrapolation__1up"]=1.0;
 SystWeights["weight_btag_FT_EFF_extrapolation__1down"]=1.0;
 SystWeights["weight_btag_FT_EFF_extrapolation_from_charm__1up"]=1.0;
 SystWeights["weight_btag_FT_EFF_extrapolation_from_charm__1down"]=1.0;
               
 // pile-up
 SystWeights["weight_pu_PRW_DATASF__1up"]=1.0;
 SystWeights["weight_pu_PRW_DATASF__1down"]=1.0;

 SystWeights["weight_jvt_JET_JvtEfficiency__1up"]=1.0;
 SystWeights["weight_jvt_JET_JvtEfficiency__1down"]=1.0;

 if (sample=="mc") { // here weights related to ttbar only
      SystWeights["weight_ttbb_ttbb_aMcAtNloHpp_weight"]=1.0;
      SystWeights["weight_ttbb_ttbb_aMcAtNloPy8_weight"]=1.0;
      SystWeights["weight_ttbb_ttbb_CSS_KIN_weight"]=1.0;
      SystWeights["weight_ttbb_ttbb_MPIfactor_weight"]=1.0;
      SystWeights["weight_ttbb_ttbb_defaultX2_weight"]=1.0;
      SystWeights["weight_ttbb_ttbb_defaultX05_weight"]=1.0;
  }
 
 if (sample=="data"){ // qcd only
      QcdWeights["weight_QCD_1L_mu_up"]=0.0;
      QcdWeights["weight_QCD_1L_el_up"]=0.0;
      QcdWeights["weight_QCD_1L_mu_down"]=0.0;
      QcdWeights["weight_QCD_1L_el_down"]=0.0;
      std::map<std::string, float>::iterator it = QcdWeights.begin();
      while ( it!=QcdWeights.end() ) { // non relying on C11
           t2->SetBranchAddress((it->first).c_str(),&QcdWeights[it->first]);
           it++;
      }

 }

  // Systematic weights have to be added only in case of nominal trees 
  // (which doesn't mean the tree name is "nominal", see e.g. ttbar systematics)
 if ( !is_syst_tree){
      std::map<std::string, float>::iterator it = SystWeights.begin();
      while ( it!=SystWeights.end() ) { // non relying on C11
             if (sample!="data") t0->SetBranchAddress((it->first).c_str(),&SystWeights[it->first]); 
             t1->SetBranchAddress((it->first).c_str(),&SystWeights[it->first]);
           it++; 
      }
  }
*/ 
  int Nev0 = t0->GetEntries();
  int Nev1 = t1->GetEntries();
  int Nev2 = t2->GetEntries();
  int Nsel = 0; // counter
  int Nsel0 = 0; // counter
  int Nsel1 = 0; // counter
  int Nsel2 = 0; // counter
  float totweight0=0,totweight1=0,totweight2=0;
  int Nmax = -1;  
  
  TLorentzVector Jet(0,0,0,0);
  vector<TLorentzVector> Jets;
  
  bool req4b = false;
  if(sel.find("4b")!=string::npos) req4b = true;
  bool req3b = false;
  if(sel.find("3b")!=string::npos) req3b = true;
  bool least3 = false;
  if(sel.find("least3")!=string::npos) least3 = true;
  
  int bjetExclusionMode = 0; // 0: highest weight, 1: highest pT, 2: random choice
  if(sel.find("hpt")!=string::npos) bjetExclusionMode = 1;
  if(sel.find("rnd")!=string::npos) bjetExclusionMode = 2;
//   bjetExclusionMode = 2;
  
  int lightjetExclusionMode = -1; // 0: lowest weight
//   if(sel.find("exW")!=string::npos) lightjetExclusionMode = 0;
  if(sel.find("exW")!=string::npos) lightjetExclusionMode = 1;
  
  
  // ----------------------------------------------
  // ---               MAIN LOOP                ---
  // ----------------------------------------------
  for(int i=0;i<Nev0+Nev1+Nev2;i++){
    if(i%10000==0) cout << i << "/" << Nev0+Nev1+Nev2 << endl;
  // TRandom2*ran1=new TRandom2();
  // int NSimuEvents=50;
  // for(int jvir=0;jvir<NSimuEvents;jvir++){
  //   int i = ran1->Integer(Nev0+Nev1+Nev2);
  //   if(jvir%(NSimuEvents/10)==0) cout << jvir << "/" << NSimuEvents << endl;

    if(i<Nev0) t0->GetEntry(i);
    else if(i<Nev0+Nev1)       t1->GetEntry(i-Nev0);
    else  {                     
         t2->GetEntry(i -Nev0 -Nev1);
         // std::cout << "Nominal qcd:  El:" << QcdWeights["weight_QCD_1L_el"] << std::endl;
         // std::cout << "              Mu:" << QcdWeights["weight_QCD_1L_mu"] << std::endl;
         // std::cout << "Syst qcd:     El:" << QcdWeights["weight_QCD_1L_el_up"] << std::endl;
         // std::cout << "              Mu:" << QcdWeights["weight_QCD_1L_mu_up"] << std::endl;
    }
    //ShowMaps(NomWeights, SystWeights);
    
    if(Nmax>0 && Nsel>Nmax) break;
    

    // select events with >=2 tags
    if(nBTags<2) continue;
    // or >=3 in case of sel == *4b*
    if(req4b && nBTags<3) continue;
    // or <=3 in case of sel == *3b*
    if(req3b && nBTags>3) continue;
    
    // dilepton cuts
    //if(pass_dilep && !pass_dilep_sf_cut) continue;
  
    // ljets cuts
    if (channel=="ljets") if (!( pass_triangle &&  met>20) ) continue;
  
    // qcd cut
//    if (sample=="data" && (   i >= Nev0 + Nev1 )){
//        if (!( ( ejets || ejets_loose || mujets || mujets_loose) && pass_triangle && met>20 ) ) continue;
//        if (!( ( ejets  || mujets ) && pass_triangle && met>20 ) ) continue;
//    }
      
    // Selection: ((ejets && trigger_el && trgMatch_el) || (mujets && trigger_muon && trgMatch_mu)) && pass_triangle && met>20 && nJets>=5 && nBTags>=2
//     if(!(
//       ((ejets && trigger_el && trgMatch_el) || (mujets && trigger_muon && trgMatch_mu)) && pass_triangle && met>20 && nJets>=5
//     )) continue;
    
    // select events with certain NJets
    if(sel.find("5j6j7j8j")!=string::npos) { if(nJets!=5 && nJets!=6 && nJets!=7 && nJets!=8) continue;}
    else if(sel.find("5j6j9jge10j")!=string::npos)   { if(nJets==4 || nJets==7 || nJets==8) continue;}
    else if(sel.find("4j5j6j")!=string::npos)   { if(nJets!=4 && nJets!=5 && nJets!=6) continue;}
    else if(sel.find("4j5j")!=string::npos){ if(nJets!=4 && nJets!=5) continue;}
    else if(sel.find("4j")!=string::npos)  { if(nJets!=4) continue;            }
    else if(sel.find("5j6j")!=string::npos){ if(nJets!=5 && nJets!=6) continue;}
    else if(sel.find("5j")!=string::npos)  { if(nJets!=5) continue;            }
    else if(sel.find("6j")!=string::npos)  { if(nJets!=6) continue;            }
    else if(sel.find("7j")!=string::npos)  { if(nJets!=7) continue;            }
    else if(sel.find("8j")!=string::npos)  { if(nJets!=8) continue;            }
    else if(sel.find("9j")!=string::npos)  { if(nJets!=9) continue;            }
    else if(sel.find("10j")!=string::npos) { if(nJets<10) continue;            }

/*    if      (sel.find("0J")!=string::npos)   { if(nHOT!=0) continue;           }
    else if (sel.find("ge1J")!=string::npos) { if(nHOT <1) continue;           }
    else if (sel.find("1J")!=string::npos)   { if(nHOT!=1) continue;           }
    else if (sel.find("2J")!=string::npos)   { if(nHOT <2) continue;           }
*/
    // add possibility to distinguish in flavour sub channels
/*    if (sel.find("dilep") != std::string::npos){
         if (sel.find("ee")   != std::string::npos) { if(!ee)   continue; } 
         if (sel.find("emu")  != std::string::npos) { if(!emu)  continue; } 
         if (sel.find("mumu") != std::string::npos) { if(!mumu) continue; }        
    }
    else {
         bool loosen = (i>=Nev0+Nev1) && sample=="data"; // double check, I don't want to think about if it is necessary or not
         if (sel.find("ejets") != std::string::npos) { 
             if(!ejets ||  (loosen && !ejets_loose) ) continue; 
         } 
         if (sel.find("mujets") != std::string::npos) {
             if(!mujets ||  (loosen && !mujets_loose) ) continue;  
         } 
    }
*/    
    Nsel++;
    if(i<Nev0)                Nsel0++;
    else if (i<Nev0+Nev1)     Nsel1++;
    else                      Nsel2++;
    //
    
    // build TLorentzVector of jets
    Jets.clear();
    for(int j=0;j<nJets;j++){
//       Jet.SetPtEtaPhiE(jet_pt->at(j)/1e3,jet_eta->at(j),jet_phi->at(j),jet_e->at(j)/1e3);
      Jet.SetPtEtaPhiE(jet_pt->at(j),jet_eta->at(j),jet_phi->at(j),jet_e->at(j));
      Jets.push_back(Jet);
    }

    // exclude 2 (or 3) b-jets
    int j1st = -1;
    int j2nd = -1;
    int j3rd = -1;
    
    // Highest b-tag weights
    if(bjetExclusionMode==0){
      float first_MV2  = -1.;
      float second_MV2 = -1.;
      float third_MV2  = -1.;
      if (jet_mv2c10->size()!=nJets) std::cout << "Ill defined event: mv2c10 != nJets sizes" << std::endl;
      for(int j=0;j<nJets;j++){
        if(jet_mv2c10->at(j)>first_MV2){
          first_MV2 = jet_mv2c10->at(j);
          j1st = j;
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == j1st) continue;
        if(jet_mv2c10->at(j)>second_MV2){
          second_MV2 = jet_mv2c10->at(j);
          j2nd = j;
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == j1st) continue;
        if(j == j2nd) continue;
        if(jet_mv2c10->at(j)>third_MV2){
          third_MV2 = jet_mv2c10->at(j);
          j3rd = j;
        }
      }
    }
    // Highest pT
    else if(bjetExclusionMode==1){
      float first_pT  = -1.;
      float second_pT = -1.;
      float third_pT  = -1.;
      if (jet_isbtagged->size()!=nJets) std::cout << "Ill defined event: isbtagged != nJets sizes" << std::endl;
      for(int j=0;j<nJets;j++){
         if(jet_isbtagged->at(j)){
//	if(isTagged(jet_mv2c10->at(j))){
          if(jet_pt->at(j)>first_pT){
            first_pT = jet_pt->at(j);
            j1st = j;
          }
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == j1st) continue;
         if(jet_isbtagged->at(j)){
//	if(isTagged(jet_mv2c10->at(j))){
          if(jet_pt->at(j)>second_pT){
            second_pT = jet_pt->at(j);
            j2nd = j;
          }
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == j1st) continue;
        if(j == j2nd) continue;
         if(jet_isbtagged->at(j)){
//	if(isTagged(jet_mv2c10->at(j))){
          if(jet_pt->at(j)>third_pT){
            third_pT = jet_pt->at(j);
            j3rd = j;
          }
        }
      }
    }
    // Random
    else if(bjetExclusionMode==2){
      float first_rnd  = -1.;
      float second_rnd = -1.;
      float third_rnd  = -1.;
      // assign random number
      if(nJets>100) nJets = 99;
      float random[100];
      for(int j=0;j<nJets;j++){
        random[j] = gRandom->Rndm();
      }
      for(int j=0;j<nJets;j++){
         if(jet_isbtagged->at(j)){
//	if(isTagged(jet_mv2c10->at(j))){
          if(random[j]>first_rnd){
            first_rnd = random[j];
            j1st = j;
          }
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == j1st) continue;
         if(jet_isbtagged->at(j)){
//	if(isTagged(jet_mv2c10->at(j))){
          if(random[j]>second_rnd){
            second_rnd = random[j];
            j2nd = j;
          }
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == j1st) continue;
        if(j == j2nd) continue;
         if(jet_isbtagged->at(j)){
//	if(isTagged(jet_mv2c10->at(j))){
          if(random[j]>third_rnd){
            third_rnd = random[j];
            j3rd = j;
          }
        }
      }
    }
    else{
      cout << "ERROR: Urecognised bjetExclusionMode " << bjetExclusionMode << endl;
      return;
    }
  
    //
    // Exclude two jets from W to get effs for dilep
    int jlast = -1;
    int jsecondlast = -1;
    // Lowest b-tag weights
    if(lightjetExclusionMode==0){
      float last_MV2  = 1.;
      float secondlast_MV2 = 1.;
      for(int j=0;j<nJets;j++){
        if(jet_mv2c10->at(j)<last_MV2){
          last_MV2 = jet_mv2c10->at(j);
          jlast = j;
        }
      }
      for(int j=0;j<nJets;j++){
        if(j == jlast) continue;
        if(jet_mv2c10->at(j)<secondlast_MV2){
          secondlast_MV2 = jet_mv2c10->at(j);
          jsecondlast = j;
        }
      }
    }
    // Pair closest to W mass
    else if(lightjetExclusionMode==1){
      float dm_min = 10000;
      for(int j1=0;j1<nJets;j1++){
        if(j1 == j1st) continue;
        if(j1 == j2nd) continue;
        if(j1 == j3rd) continue;
        if(nJets<6) break;
        for(int j2=j1+1;j2<nJets;j2++){
          if(j2 == j1st) continue;
          if(j2 == j2nd) continue;
          if(j2 == j3rd) continue;
          float dm = fabs((Jets[j1]+Jets[j2]).M()-MW);
          if(dm<dm_min){
            dm_min = dm;
            jlast = j1;
            jsecondlast = j2;
          }
        }
      }
      if(jsecondlast<0){
        float first_rnd = 0;
        if(nJets>100) nJets = 99;
        float random[100];
        for(int j=0;j<nJets;j++){
          random[j] = gRandom->Rndm();
        }
        for(int j=0;j<nJets;j++){
          if(j == j1st) continue;
          if(j == j2nd) continue;
          if(j == j3rd) continue;
          if(random[j]>first_rnd){
            first_rnd = random[j];
            jlast = j;
          }
        }
      }
    }
    
    bool is_ttb = HF_SimpleClassification== 1;
    bool is_ttc = HF_SimpleClassification==-1;
    
    double weight = 2.0 * weight_normalise;
    // std::cout << "Weight = " << weight<<std::endl; 
    double weight_qcd = 0;
    bool reweight=true;

/*
    if(sample=="data" && (   i >= Nev0 + Nev1 ) ){ //qcd land
       // nominal weight
       weight_qcd = QcdWeights["weight_QCD_1L_el"] + QcdWeights["weight_QCD_1L_mu"];
       if ( 
            (     (  syst.find("e_QCD_Up")  != std::string::npos && (ejets  || ejets_loose)  ) 
               || (  syst.find("mu_QCD_Up") != std::string::npos && (mujets || mujets_loose) ) 
            )  &&
            (     (  syst.find("_C_") != std::string::npos && is_central_QCD==1  )
               || (  syst.find("_F_") != std::string::npos && is_central_QCD==0  )
            )
          ) weight_qcd = QcdWeights["weight_QCD_1L_el_up"] + QcdWeights["weight_QCD_1L_mu_up"];
       if ( 
            (     (  syst.find("e_QCD_Down")  != std::string::npos && (ejets  || ejets_loose)  ) 
               || (  syst.find("mu_QCD_Down") != std::string::npos && (mujets || mujets_loose) ) 
            )  &&
            (     (  syst.find("_C_") != std::string::npos && is_central_QCD==1  )
               || (  syst.find("_F_") != std::string::npos && is_central_QCD==0  )
            )
          ) weight_qcd = QcdWeights["weight_QCD_1L_el_down"] + QcdWeights["weight_QCD_1L_mu_down"];
       // qcd has to be subtracted, exactly as non ttbar
       weight = -1*weight_qcd;
    }
*/
    if(sample!="data" || (  i>=Nev0 && i < Nev0 + Nev1 ) ){
      // save infos on sample name
      if (i>=Nev0){
        int iprime=i-Nev0;
        if (iprime < edges["wjets"]) teff_sample="wjets";
        else if (iprime < edges["zjets"]) teff_sample="zjets";
        else if (iprime < edges["singleTop"]) teff_sample="singleTop";
        else if (iprime < edges["ttW"]) teff_sample="ttW";
        else if (iprime < edges["ttZ"]) teff_sample="ttZ";
        else if (iprime < edges["ttWW"]) teff_sample="ttWW";
        else if (iprime < edges["ttH"]) teff_sample="ttH";
        else if (iprime < edges["diboson"]) teff_sample="diboson";
      }

      // nominal weight
      std::map<std::string, float>::iterator it = NomWeights.begin();
      while ( it!=NomWeights.end() ) { // non relying on C11. Skip nnlo reweighting, it will be done properly below 
           reweight= ( (it->first).find("weight_ttbar_NNLO_1L") == std::string::npos ) && ( (it->first).find("weight_ttbb_Norm") == std::string::npos  );
           if (reweight) {
               weight*= it->second; 
               // std::cout << "Weight = " << weight<< " after "<< it->first << std::endl; 
           }
           it++; 
      }
      if (sample=="mc" && !skip_nnlo_rew ){ // ttbar sample, add specific weights properly (NNLO_1L is not set to 1 for ttbb, checked!! ), if not asked not to do so
         if (!is_ttb) { 
             if (syst.find("TTBB_ONLY")==std::string::npos) weight*=NomWeights["weight_ttbar_NNLO_1L"];
         }
         else {
             if (syst.find("NNLO_ONLY")==std::string::npos) weight*=NomWeights["weight_ttbb_Norm_new"];
         }
     }

      // ... add here other nominal weight pieces
      // ---
      // systematic weights



      if(syst!="nominal" && syst!="ttPy8"){

         if( !is_syst_tree){// if the tree isn't a nominal one (which means either different name or different sample), skip to gain time

            // b-tagging
            for(int i_btag=0;i_btag<nBEigen;i_btag++) { 
                 if(syst==Form("BTag_B_Up_NP%d",i_btag))      weight*=SystWeights[Form("weight_btag_FT_EFF_Eigen_B_%d__1up",i_btag)]/NomWeights["weight_btag"];
                 if(syst==Form("BTag_B_Down_NP%d",i_btag))      weight*=SystWeights[Form("weight_btag_FT_EFF_Eigen_B_%d__1down",i_btag)]/NomWeights["weight_btag"]; 
            }
            for(int i_btag=0;i_btag<nCEigen;i_btag++) { 
                 if(syst==Form("BTag_C_Up_NP%d",i_btag))      weight*=SystWeights[Form("weight_btag_FT_EFF_Eigen_C_%d__1up",i_btag)]/NomWeights["weight_btag"];
                 if(syst==Form("BTag_C_Down_NP%d",i_btag))      weight*=SystWeights[Form("weight_btag_FT_EFF_Eigen_C_%d__1down",i_btag)]/NomWeights["weight_btag"]; 
            }
            for(int i_btag=0;i_btag<nLightEigen;i_btag++) { 
                 if(syst==Form("BTag_Light_Up_NP%d",i_btag))      weight*=SystWeights[Form("weight_btag_FT_EFF_Eigen_Light_%d__1up",i_btag)]/NomWeights["weight_btag"];
                 if(syst==Form("BTag_Light_Down_NP%d",i_btag))      weight*=SystWeights[Form("weight_btag_FT_EFF_Eigen_Light_%d__1down",i_btag)]/NomWeights["weight_btag"]; 
            }
    
            //
            if(syst=="BTag_extrapolation_Up")              weight*=SystWeights["weight_btag_FT_EFF_extrapolation__1up"]/NomWeights["weight_btag"];
            if(syst=="BTag_extrapolation_Down")            weight*=SystWeights["weight_btag_FT_EFF_extrapolation__1down"]/NomWeights["weight_btag"];
            if(syst=="BTag_extrapolation_from_charm_Up")   weight*=SystWeights["weight_btag_FT_EFF_extrapolation_from_charm__1up"]/NomWeights["weight_btag"];
            if(syst=="BTag_extrapolation_from_charm_Down") weight*=SystWeights["weight_btag_FT_EFF_extrapolation_from_charm__1down"]/NomWeights["weight_btag"];
            // ...
        
            // pile up
            if(syst=="pu_PRW_DATASF_Up") weight*=SystWeights["weight_pu_PRW_DATASF__1up"]/NomWeights["weight_pu"];
            if(syst=="pu_PRW_DATASF_Down") weight*=SystWeights["weight_pu_PRW_DATASF__1down"]/NomWeights["weight_pu"];

            // jvt
            if(syst=="JET_JvtEfficiency_Up") weight*=SystWeights["weight_jvt_JET_JvtEfficiency__1up"]/NomWeights["weight_jvt"];
            if(syst=="JET_JvtEfficiency_Down") weight*=SystWeights["weight_jvt_JET_JvtEfficiency__1down"]/NomWeights["weight_jvt"];       
            if (sample=="mc"){
              if (is_ttb){
                if(syst=="TTBB_AMCHpp" ) weight*=SystWeights["weight_ttbb_ttbb_aMcAtNloHpp_weight"]/NomWeights["weight_ttbb_ttbb_Nominal_weight"];
                if(syst=="TTBB_AMCPy8" ) weight*=SystWeights["weight_ttbb_ttbb_aMcAtNloPy8_weight"]/NomWeights["weight_ttbb_ttbb_Nominal_weight"];
                if(syst=="TTBB_CSS_KIN" ) weight*=SystWeights["weight_ttbb_ttbb_CSS_KIN_weight"]/NomWeights["weight_ttbb_ttbb_Nominal_weight"];
                if(syst=="TTBB_MPI" ){
                   if (!skip_nnlo_rew) weight*=SystWeights["weight_ttbb_ttbb_MPIfactor_weight"]/NomWeights["weight_ttbb_ttbb_Nominal_weight"];
                   else weight*=SystWeights["weight_ttbb_ttbb_MPIfactor_weight"];
                }
                if(syst=="TTBB_SCALE_Up" ) weight*=SystWeights["weight_ttbb_ttbb_defaultX2_weight"]/NomWeights["weight_ttbb_ttbb_Nominal_weight"];
                if(syst=="TTBB_SCALE_Down" ) weight*=SystWeights["weight_ttbb_ttbb_defaultX05_weight"]/NomWeights["weight_ttbb_ttbb_Nominal_weight"];
              }
            }
         }// end of systs that need syst weights
         if (sample=="mc"){
            if (is_ttb){
               // tt+b normalisation
               if(syst.find("ttbNormUp") != std::string::npos   ) weight*=1.5;
               if(syst.find("ttbNormDown") != std::string::npos ) weight*=0.5;
               if(syst=="dummy_ttPs") weight*=(1.30+nJets*(-0.085+0.0029*nJets));
               if(syst=="dummy_ttGen") weight*=(1.275+nJets*(0.00551));
               if(syst.find("resc")!=std::string::npos ){
                     if(syst.find("ttRadUp")!=std::string::npos )   weight*=0.75493;
                     if(syst.find("ttRadDown")!=std::string::npos ) weight*=1.21387;
                     if(syst.find("ttPs")!=std::string::npos )      weight*=0.987235;
                     if(syst.find("ttGen")!=std::string::npos )     weight*=1.28863;
               }
            }    
            else if (is_ttc){
               // tt+c normalisation
               if(syst.find("ttcNormUp") != std::string::npos   ) weight*=1.5;
               if(syst.find("ttcNormDown") != std::string::npos ) weight*=0.5;
               if(syst=="dummy_ttPs") weight*=(1.01+nJets*(-0.0046));
               if(syst=="dummy_ttGen") weight*=(1.145+nJets*(-0.0158));
               if(syst.find("TTC_NNLO_REWEIGHTING_OFF") != std::string::npos ) weight/=NomWeights["weight_ttbar_NNLO_1L"];
               if(syst.find("resc")!=std::string::npos ){
                     if(syst.find("ttRadUp")!=std::string::npos )   weight*=0.725134;
                     if(syst.find("ttRadDown")!=std::string::npos ) weight*=1.24817;
                     if(syst.find("ttPs")!=std::string::npos )      weight*=0.99349;
                     if(syst.find("ttGen")!=std::string::npos )     weight*=1.10544;
               }

            }
            else {
               if(syst.find("TTLIGHT_NNLO_REWEIGHTING_OFF") != std::string::npos ) weight/=NomWeights["weight_ttbar_NNLO_1L"];
               if(syst=="dummy_ttPs") weight*=(1.004+nJets*(-0.003));
               if(syst=="dummy_ttGen") weight*=(1.026+nJets*(-0.009496));
               if(syst.find("resc")!=std::string::npos ){
                     if(syst.find("ttRadUp")!=std::string::npos )   weight*=1.04137;
                     if(syst.find("ttRadDown")!=std::string::npos ) weight*=0.979424;
                     if(syst.find("ttPs")!=std::string::npos )      weight*=1.0009;
                     if(syst.find("ttGen")!=std::string::npos )     weight*=0.986151;
               }
            }
         }
         else {
             if (i>=Nev0) {
                if (syst=="LumiUp")               weight*=(1.0  +  0.021);
                if (syst=="LumiDown")             weight*=(1.0  -  0.021);
                if (i<(Nev0 + edges["zjets"])){ // takes both wjets and zjets, keep them in order !!!  
                  if (syst=="wzjetsNormUp")       weight*=(1.0  +  0.01*sqrt(5.0*5.0+nJets*24.0*24.0));
                  if (syst=="wzjetsNormDown")     weight*=(1.0  -  0.01*sqrt(5.0*5.0+nJets*24.0*24.0)); // hopefully no event will have > 17 jets ..
                }
                else if (i<(Nev0 + edges["singleTop"])){
                  if (syst=="singleTopNormUp" )   weight*=(1.0  +  0.05);
                  if (syst=="singleTopNormDown" ) weight*=(1.0  -  0.04);
                }
                else if (i<(Nev0 + edges["ttV"])){
                  if (syst=="ttVNormUp" )         weight*=(1.0  +  0.15);
                  if (syst=="ttVNormDown" )       weight*=(1.0  -  0.15);
                }
                else if (i<(Nev0 + edges["ttH"])){
                  if (syst=="ttHNormUp" )         weight*=(1.0  +  0.10);
                  if (syst=="ttHNormDown" )       weight*=(1.0  -  0.13);
                }
                else { // diboson
                  if (syst=="dibosonNormUp" )     weight*=(1.0  +  0.5);
                  if (syst=="dibosonNormDown" )   weight*=(1.0  -  0.5);
                }
             }
        }
      }
      // ---
      // correct by -lumi in case of the MC to be subtracted from data
      if(i>=Nev0 && i< Nev0+Nev1) weight*=-1*lumi; 
    }

    if (i>=Nev0+Nev1) teff_sample="qcd";

    // Protection from unphysical large weights
      if (protectLweights){
      Double_t tweight=NomWeights["weight_pu"]*NomWeights["weight_jvt"]*NomWeights["weight_mc"];
      if   ( teff_sample=="wjets")    if ( (TMath::Abs(tweight)>16)  ) continue; 
      // if   ( teff_sample=="zjets")    if ( (req4b && TMath::Abs(weight)>5) || TMath::Abs(weight)>50 ) continue; 
      // if   ( teff_sample=="diboson")  if ( (req4b && TMath::Abs(weight)>2) || TMath::Abs(weight)>3 ) continue; 
      // if   ( teff_sample=="qcd")      if ( TMath::Abs(weight)>20 ) continue; 
    }
   
    if(i<Nev0)                totweight0+=weight;
    else if (i<Nev0+Nev1)     totweight1+=weight;
    else                      totweight2+=weight;


    float dR1 = -1;
    float dR2 = -1;
    float dR3 = -1;
    float dRmin_all = 100, dRtemp;
    
    // prepare global dRmin, needed after to compute dRminone
    float dRmin_global = 100;
    for(int j=0;j<nJets;j++){
        for(int k=j+1;k<nJets;k++){
           dRtemp = Jets[j].DeltaR(Jets[k]);
           if (dRtemp<dRmin_global) dRmin_global=dRtemp;
        }
    }

    //std::cout << "nBTags = " << nBTags << std::endl;
    for(int j=0;j<nJets;j++){
      if(j==j1st || j==j2nd) continue;
      if(req4b && j==j3rd) continue;
      if(j==jlast || j==jsecondlast) continue;
//       if(j==jlast) continue;
      h_tot->Fill(0.5, weight);
      h_tot_pt->Fill(Jets[j].Pt(), weight);
      h_tot_eta->Fill(TMath::Abs(Jets[j].Eta()), weight);
      h_tot_pt_eta->Fill(TMath::Log(Jets[j].Pt()), TMath::Abs(Jets[j].Eta()), weight);
     // find min dR within all jets 
      // is this supposed to be the minimum distance between this jet and all the others?
      // if so, dRmin_all has to be initialised here below! 
      //In the other case (minimum dR between all jets in the event), it can't be filled inside the loop on jets ...
      // Leave the possibility to reproduce the bug adding "nofix" to the selection
      ////////////////////////////
      if (sel.find("nofix")==std::string::npos) dRmin_all=100;
      ////////////////////////////
      for(int jjj=0;jjj<nJets;jjj++){
        if(j==jjj) continue;
//       if(jjj==j1st || jjj==j2nd) continue;
//       if(req4b && jjj==j3rd) continue;
        float dRj = Jets[j].DeltaR(Jets[jjj]);
        if(dRj<dRmin_all){
          dRmin_all = dRj;
        }
      }
      h_tot_dRj->Fill(dRmin_all*nJets, weight);
      h_tot_dRjone->Fill(log(dRmin_all/dRmin_global), weight);
      h_tot_Nj->Fill(nJets, weight);
      h_tot_dRj_Nj->Fill(dRmin_all,nJets,weight); 
      h_corr_comp->Fill(Jets[j].Pt(), TMath::Abs(Jets[j].Eta()), dRmin_all*nJets ,weight);
      // Fill Tree branches
      teff_weight=weight;
      teff_dRjNj=dRmin_all*nJets; 
      teff_pt=Jets[j].Pt();
      teff_eta=TMath::Abs( Jets[j].Eta() );
      teff_istagged=0;
      teff_Nj=nJets;

//       h_tot_dRj->Fill(dRmin_all, weight);
//       h_tot_dRj->Fill(dRmin_all*pow(nJets,0.33)/pow(4.,0.33), weight);
      //
       if(jet_isbtagged->at(j) && (!req4b || nBTags>=4)){
//      if(isTagged(jet_mv2c10->at(j)) && (!req4b || nBTags>=4)){
        teff_istagged=1;
        h_tag->Fill(0.1, weight);
        h_tag_pt->Fill(Jets[j].Pt(), weight);
        h_tag_eta->Fill(TMath::Abs(Jets[j].Eta()), weight);
        h_tag_pt_eta->Fill(TMath::Log(Jets[j].Pt()), TMath::Abs(Jets[j].Eta()), weight);
        h_tag_dRj_Nj->Fill(dRmin_all, nJets, weight);
        h_tag_dRj->Fill(dRmin_all*nJets, weight);
        h_tag_dRjone->Fill(log(dRmin_all/dRmin_global), weight);
        h_tag_Nj->Fill(nJets, weight);
//         h_tag_dRj->Fill(dRmin_all, weight);
//         h_tag_dRj->Fill(dRmin_all*pow(nJets,0.33)/pow(4.,0.33), weight);
      }
      teff->Fill();
      if (least3 && nBTags>=3){
        if(jet_isbtagged->at(j) ){
//        if(isTagged(jet_mv2c10->at(j)) ){
          //std::cout << "Filling tagged " << j << std::endl;
          k_non_tag_pt->Fill(Jets[j].Pt(), weight);
          if (nBTags>=4){
            //std::cout << "Filling tagged if one other tagged" << j << std::endl;
            k_non_tag_if_pt->Fill(Jets[j].Pt(), weight);
          }
        }
      }
    }
  }
  cout << endl;
  cout << "Selected " << Nsel << " events." << endl;
  cout << "       ( " << Nsel0 << " data, " << Nsel1 << " background, " << Nsel2 << " qcd)" << endl;
  cout << "Total weights: "  << endl;
  cout << "       ( " << totweight0 << " data, " << totweight1 << " background, " << totweight2 << " qcd)" << endl;
  cout << endl;
  
  h_eff->Divide(h_tag,h_tot,1,1);
  h_eff->SetMaximum(0.1);
//   h_eff->SetMaximum(0.15);
//   h_eff->SetMaximum(0.2);
  h_eff->SetMinimum(0);

  h_eff_pt->Divide(h_tag_pt,h_tot_pt,1,1);
  h_eff_pt->SetMaximum(0.1);
//   h_eff_pt->SetMaximum(0.15);
//   h_eff_pt->SetMaximum(0.2);
  h_eff_pt->SetMinimum(0);

  k_eff_pt->Divide(k_non_tag_if_pt, k_non_tag_pt);
  TH1F*k_temp = (TH1F*)k_eff_pt->Clone("k_temp");
  for (int i=1;i<=k_temp->GetNbinsX(); i++) {
       k_temp->SetBinError(i,0);
       k_temp->SetBinContent(i,1);
  }
  //k_eff_pt->Add(k_temp,-1);
  //k_eff_pt->Scale(-1);
  k_eff_pt->SetMaximum(0.3);
  k_eff_pt->SetMinimum(0);     
          
  h_eff_eta->Divide(h_tag_eta,h_tot_eta,1,1);
  h_eff_eta->SetMaximum(0.1);
//   h_eff_eta->SetMaximum(0.15);
//   h_eff_eta->SetMaximum(0.2);
  h_eff_eta->SetMinimum(0);

  h_eff_pt_eta->Divide(h_tag_pt_eta,h_tot_pt_eta,1,1);
  h_eff_pt_eta->SetMaximum(0.1);
  h_eff_pt_eta->SetMinimum(0);

  h_eff_dRj_Nj->Divide(h_tag_dRj_Nj,h_tot_dRj_Nj,1,1);
  
  h_eff_dRj->Divide(h_tag_dRj,h_tot_dRj,1,1);
  h_eff_dRj->SetMaximum(0.1);
//   h_eff_dRj->SetMaximum(0.15);
//   h_eff_dRj->SetMaximum(0.2);
  h_eff_dRj->SetMinimum(0);

  h_eff_dRjone->Divide(h_tag_dRjone,h_tot_dRjone,1,1);
  h_eff_dRjone->SetMaximum(0.1);
  h_eff_dRjone->SetMinimum(0);
  
  h_eff_Nj->Divide(h_tag_Nj,h_tot_Nj,1,1);
  h_eff_Nj->SetMaximum(0.1);
//   h_eff_dRj->SetMaximum(0.15);
//   h_eff_dRj->SetMaximum(0.2);
  h_eff_Nj->SetMinimum(0);
  
  c->cd(1)->SetLogx();
  h_eff_pt->Draw("E");
  h_eff_pt->GetYaxis()->SetTitle("#epsilon_{b}^{effective}");
  h_eff_pt->GetYaxis()->SetTitleOffset(1.6);
  h_eff_pt->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
//   h_eff_pt->Smooth();
  c->cd(2);
  h_eff_eta->Draw("E");
  h_eff_eta->GetYaxis()->SetTitle("#epsilon_{b}^{effective}");
  h_eff_eta->GetYaxis()->SetTitleOffset(1.6);
  h_eff_eta->GetXaxis()->SetTitle("#eta^{jet}");
//   h_eff_eta->Smooth();
  c->cd(3);
  h_eff_dRj->Draw("E");
  h_eff_dRj->GetYaxis()->SetTitle("#epsilon_{b}^{effective}");
  h_eff_dRj->GetYaxis()->SetTitleOffset(1.6);
  h_eff_dRj->GetXaxis()->SetTitle("#Delta R^{min}_{jj} #times N^{jet}");
//   h_eff_dRj->Smooth();
  c->cd(4);
  h_eff_Nj->Draw("E");
  h_eff_Nj->GetYaxis()->SetTitle("#epsilon_{b}^{effective}");
  h_eff_Nj->GetYaxis()->SetTitleOffset(1.6);
  h_eff_Nj->GetXaxis()->SetTitle("N^{jet}");
//   h_eff_dRj->Smooth();

  // Nj extrapolation
  TH1F *h_eff_Nj_extrap = (TH1F*) h_eff_Nj->Clone("h_eff_Nj_extrap");
//  TF1*f1=new TF1("f1","pol1");
  TF1*f1=new TF1("f1","pol0");
  f1->SetLineStyle(2);
  f1->SetLineColor(4);
  h_eff_Nj_extrap->SetLineColor(4);
  h_eff_Nj_extrap->SetMarkerStyle(24);
  h_eff_Nj_extrap->Fit(f1,"N","",4,7);
  for (int i=1; i<= h_eff_Nj_extrap->GetNbinsX(); i++) h_eff_Nj_extrap->SetBinContent(i,f1->Eval(h_eff_Nj_extrap->GetBinCenter(i)));
  h_eff_Nj_extrap->Draw("same hist");

  if (showall) {
    c->cd(5);
    gStyle->SetPalette(53);
    h_eff_pt_eta->Draw("colz");
    h_eff_pt_eta->GetYaxis()->SetTitle("#eta^{jet}");
    h_eff_pt_eta->GetYaxis()->SetTitleOffset(1.6);
    h_eff_pt_eta->GetXaxis()->SetTitle("log(p_{T}^{jet}/1GeV) ");

    c->cd(6);
//    h_eff_dRjone->Draw("E");
//    h_eff_dRjone->GetYaxis()->SetTitle("#epsilon_{b}^{effective}");
//    h_eff_dRjone->GetYaxis()->SetTitleOffset(1.6);
//    h_eff_dRjone->GetXaxis()->SetTitle("#Delta R^{min,*}_{jj} ");
    h_eff_dRj_Nj->Draw("colz");
    h_eff_dRj_Nj->GetYaxis()->SetTitle("N^{jet}");
    h_eff_dRj_Nj->GetYaxis()->SetTitleOffset(1.6);
    h_eff_dRj_Nj->GetXaxis()->SetTitle("#Delta R^{min}_{jj}");


    c->cd(7);
    for (int i=1; i<4; i++)   for (int j=i; j<4; j++) h_corr->SetBinContent(i,j,100.0*h_corr_comp->GetCorrelationFactor(i,j));
    h_corr->Draw("TEXT");

    c->cd(8);
    k_eff_pt->Draw("E");
    k_eff_pt->GetYaxis()->SetTitle("corr factor k_{b}^{effective}");
    k_eff_pt->GetYaxis()->SetTitleOffset(1.6);
    k_eff_pt->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  }
  
  f->cd();
  teff->Write("",TObject::kOverwrite);
  h_eff    ->Write("",TObject::kOverwrite);
  h_tot    ->Write("",TObject::kOverwrite); // to keep track of statistics in source regions
  h_eff_pt ->Write("",TObject::kOverwrite);
  h_eff_eta->Write("",TObject::kOverwrite);
  h_eff_pt_eta->Write("",TObject::kOverwrite);
  h_eff_dRj->Write("",TObject::kOverwrite);
  h_eff_dRj_Nj->Write("",TObject::kOverwrite);
  h_eff_dRjone->Write("",TObject::kOverwrite);
  h_eff_Nj ->Write("",TObject::kOverwrite);
  h_eff_Nj_extrap ->Write("",TObject::kOverwrite);
  h_corr ->Write("",TObject::kOverwrite);
  k_eff_pt->Write("",TObject::kOverwrite);
  c->SaveAs(("plots/ttTRFmaps2018_"+sample+"_"+sel+"_"+syst_name+".png").c_str());
}


void ShowMaps(std::map<std::string,float> NMap, std::map<std::string,float> SMap){

  std::cout << " %----- Begin Map ---------% "<< std::endl;
  
  std::map<std::string,float>::iterator it = NMap.begin(),jt;
  while ( it != NMap.end() ){
     std::cout << "Nominal:" << it->first << " = "<< it->second << std::endl;
     jt=SMap.begin();
     while (jt != SMap.end() ) {
        if ( (jt->first).find(it->first) != std::string::npos   )   std::cout << "      " << jt->first << " = "<< jt->second << std::endl;
        jt++;
     }
     it++;
  }

  std::cout << std::endl;
  return;
}
