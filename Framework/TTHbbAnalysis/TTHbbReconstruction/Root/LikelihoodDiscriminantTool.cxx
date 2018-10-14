/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/LikelihoodDiscriminantTool.h"
#include "PathResolver/PathResolver.h"

LikelihoodDiscriminantTool::LikelihoodDiscriminantTool(){
////
  m_opt = new Options;
    //see later: how to do with options
  m_opt->Run3b=true; 
  m_opt->Mttbb=true;
  m_opt->MttbbAndMtt=true;
  m_opt->Bbttbb=true;
  m_opt->jetpermcut8=true;
}


LikelihoodDiscriminantTool::~LikelihoodDiscriminantTool(){
  ////
  //see later delete tool etc

  
   delete m_sig_6jex4bin;
   delete m_sig_7jin4bin;
   delete m_sig_5jex4bin; 
   delete m_bkg_6jex4bin;
   delete m_bkg_7jin4bin;
   delete m_bkg_5jex4bin;
   
   delete m_sig5matched_6jex4bin;
   delete m_sig5matched_7jin4bin; 
   delete m_bkg5matched_6jex4bin;
   delete m_bkg5matched_7jin4bin;

   if(m_opt->Run3b){
     delete m_sig_6jex3bex;
     delete m_sig_7jin3bex;
     delete m_bkg_6jex3bex;
     delete m_bkg_7jin3bex;
     delete m_sig5matched_6jex3bex;
     delete m_sig5matched_7jin3bex;
     delete m_bkg5matched_6jex3bex;
     delete m_bkg5matched_7jin3bex;    
   }
   
   delete m_opt;
 
}
  


void LikelihoodDiscriminantTool::Init(){
  //see later: the exact path structure
  //std::string discriminant_path = std::string(SETUPDIR)+"/data/TTHbbReconstruction/";
  std::string discriminant_path = PathResolverFindCalibDirectory("TTHbbReconstruction/");

  //std::string filename_sig=discriminant_path+"sigPy8-02-04-15-01_training_withMttbb5matchedHist_Smoothed_Normalized.root";
  //std::string filename_bkg=discriminant_path+"bfilter-02-04-15-01_training_withMttbb5matchedHist_Smoothed_Normalized.root";
  //std::string filename_sig=discriminant_path+"sigPy8-02-04-15-01_training_5jex4binHist_Smoothed_Normalized.root";
  //std::string filename_bkg=discriminant_path+"bfilter-02-04-15-01_training_5jex4binHist_Smoothed_Normalized.root";
  std::string filename_sig=discriminant_path+"sig_trainingHist_Smoothed_Normalized.root";
  std::string filename_bkg=discriminant_path+"bfilter_trainingHist_Smoothed_Normalized.root";

  TFile* f_sig = TFile::Open(filename_sig.c_str());
  TFile* f_bkg = TFile::Open(filename_bkg.c_str());
  m_sig_6jex4bin=new TrainingSig;
  m_sig_7jin4bin=new TrainingSig;

  GetTrainingSig(f_sig, "6jex4bin", m_sig_6jex4bin);
  GetTrainingSig(f_sig, "7jin4bin", m_sig_7jin4bin);
 
  m_bkg_6jex4bin=new TrainingBkg;
  m_bkg_7jin4bin=new TrainingBkg;

  GetTrainingBkg(f_bkg, "6jex4bin", m_bkg_6jex4bin);
  GetTrainingBkg(f_bkg, "7jin4bin", m_bkg_7jin4bin);
  //5matched
  m_sig5matched_6jex4bin=new TrainingSig;
  m_sig5matched_7jin4bin=new TrainingSig;
  //std::cout << "Debug before GetTrainingSig5matched" << std::endl;

  GetTrainingSig5matched(f_sig, "6jex4bin_5matched", m_sig5matched_6jex4bin);
  //std::cout << "Debug after GetTrainingSig5matched 6jex4bin going to do with 7jin4bin" << std::endl;

  GetTrainingSig5matched(f_sig, "7jin4bin_5matched", m_sig5matched_7jin4bin);
  //std::cout << "Debug after GetTrainingSig5matched" << std::endl;

  m_bkg5matched_6jex4bin=new TrainingBkg;
  m_bkg5matched_7jin4bin=new TrainingBkg;

  GetTrainingBkg5matched(f_bkg, "6jex4bin_3matched", m_bkg5matched_6jex4bin);
  GetTrainingBkg5matched(f_bkg, "7jin4bin_3matched", m_bkg5matched_7jin4bin);

  
  if(m_opt->Run3b){//3b
    //std::cout << "going to set 3b trainings" << std::endl;

    m_sig_6jex3bex=new TrainingSig;
    m_sig_7jin3bex=new TrainingSig;
    GetTrainingSig(f_sig, "6jex3bex", m_sig_6jex3bex);
    GetTrainingSig(f_sig, "7jin3bex", m_sig_7jin3bex);

    m_bkg_6jex3bex=new TrainingBkg;
    m_bkg_7jin3bex=new TrainingBkg;
    GetTrainingBkg(f_bkg, "6jex3bex", m_bkg_6jex3bex);
    GetTrainingBkg(f_bkg, "7jin3bex", m_bkg_7jin3bex);
    
    //5matched
    m_sig5matched_6jex3bex=new TrainingSig;
    m_sig5matched_7jin3bex=new TrainingSig;
    GetTrainingSig5matched(f_sig, "6jex3bex_5matched", m_sig5matched_6jex3bex);
    GetTrainingSig5matched(f_sig, "7jin3bex_5matched", m_sig5matched_7jin3bex);
    
    m_bkg5matched_6jex3bex=new TrainingBkg;
    m_bkg5matched_7jin3bex=new TrainingBkg;
    GetTrainingBkg5matched(f_bkg, "6jex3bex_3matched", m_bkg5matched_6jex3bex);
    GetTrainingBkg5matched(f_bkg, "7jin3bex_3matched", m_bkg5matched_7jin3bex);
  }//3b

  
  m_sig_5jex4bin=new TrainingSig;
  GetTrainingSig5jex4bin(f_sig, "5jex4bin_5matched", m_sig_5jex4bin);
  m_bkg_5jex4bin=new TrainingBkg;
  GetTrainingBkg5jex4bin(f_bkg, "5jex4bin_3matched", m_bkg_5jex4bin);


  
  f_sig->Clear();
  f_sig->Close();
  f_bkg->Clear();
  f_bkg->Close();

  //see later: BTaggingEfficiencyTool


}


void LikelihoodDiscriminantTool::GetTrainingSig(TFile* f, const std::string& Reg, TrainingSig* tr){
  //std::cout << "Debug: in GetTrainingSig" << std::endl;
  std::string tit="";
  //std::cout << "Debug 1" << std::endl;
  tit="reg_"+Reg+"_Mbb";
  tr->hmbb=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "Debug 2" << std::endl;
  tit="reg_"+Reg+"_Mqq";
  tr->hmqq=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MqqbMinusMqq";
  tr->hmqqbminusmqq=(TH1*)f->Get(tit.c_str())->Clone();
  //new
  tit="reg_"+Reg+"_MqqbRegMinusMqq";
  tr->hmqqbregminusmqq=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "sig "<< std::endl;
 tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt0";
  tr->hmttregminusmtregminusmt0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt1";
  tr->hmttregminusmtregminusmt1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt2";
  tr->hmttregminusmtregminusmt2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttregminusmtregminusmt2->GetMean()= " <<  tr->hmttregminusmtregminusmt2->GetMean() << std::endl;

  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH0";
  tr->hmtthregminusmttregminusmh0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH1";
  tr->hmtthregminusmttregminusmh1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH2";
  tr->hmtthregminusmttregminusmh2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmtthregminusmttregminusmh2->GetMean()= " <<  tr->hmtthregminusmttregminusmh2->GetMean() << std::endl;

 
  //
  tit="reg_"+Reg+"_Mlvb0";
  tr->hmlvb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb1";
  tr->hmlvb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb2";
  tr->hmlvb2=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabhRF";
  tr->truthcosthetabhRF=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF0";
  tr->truthcosthetabbttbbRF0=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF1";
  tr->truthcosthetabbttbbRF1=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF2";
  tr->truthcosthetabbttbbRF2=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_subqfromWtruthflav_vs_leadqfromWtruthflav";
  TH2* hqfromWtruthflav= (TH2*)f->Get(tit.c_str());////flav of quarks from W
  tr->fWll= hqfromWtruthflav->GetBinContent(6)/(hqfromWtruthflav->GetBinContent(6)+hqfromWtruthflav->GetBinContent(7)+hqfromWtruthflav->GetBinContent(11));
  tr->fWlc= (hqfromWtruthflav->GetBinContent(7)+hqfromWtruthflav->GetBinContent(11))/(hqfromWtruthflav->GetBinContent(6)+hqfromWtruthflav->GetBinContent(7)+hqfromWtruthflav->GetBinContent(11));
  //std::cout << "debug 1 tr->fWll= " <<  tr->fWll << std::endl;
  //std::cout << "fWlc= " <<  fWlc << std::endl;
  delete hqfromWtruthflav;
  tr->hmbb->SetDirectory(0);
  tr->hmqq->SetDirectory(0);
  tr->hmqqbminusmqq->SetDirectory(0);
  tr->hmqqbregminusmqq->SetDirectory(0);
  tr->hmttregminusmtregminusmt0->SetDirectory(0);
  tr->hmttregminusmtregminusmt1->SetDirectory(0);
  tr->hmttregminusmtregminusmt2->SetDirectory(0);
  tr->hmtthregminusmttregminusmh0->SetDirectory(0);
  tr->hmtthregminusmttregminusmh1->SetDirectory(0);
  tr->hmtthregminusmttregminusmh2->SetDirectory(0);
  tr->hmlvb0->SetDirectory(0);
  tr->hmlvb1->SetDirectory(0);
  tr->hmlvb2->SetDirectory(0);
    tr->truthcosthetabhRF->SetDirectory(0);
    
    tr->truthcosthetabbttbbRF0->SetDirectory(0);
    tr->truthcosthetabbttbbRF1->SetDirectory(0);
    tr->truthcosthetabbttbbRF2->SetDirectory(0);
    

}
void LikelihoodDiscriminantTool::GetTrainingSig5matched(TFile* f, const std::string& Reg, TrainingSig* tr){
  //std::cout << "Debug: in GetTrainingSig 5 matched" << std::endl;
  std::string tit="";
  tit="reg_"+Reg+"_Mbb";
  tr->hmbb=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_Mqq_Jet_1";

  tr->hmqq=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_MqqbRegMinusMqq";
  tr->hmqqbminusmqq=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_MqqbRegMinusMqq_Jet_1";
  //std::cout << "hmqq tit= " << tit<< std::endl;

  tr->hmqqbregminusmqq=(TH1*)f->Get(tit.c_str())->Clone();
  // std::cout << "Debug 3" << std::endl;
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt0_Jet_1";
  tr->hmttregminusmtregminusmt0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt1_Jet_1";
  tr->hmttregminusmtregminusmt1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt2_Jet_1";
  tr->hmttregminusmtregminusmt2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttregminusmtregminusmt2>GetMean()= " <<  tr->hmttregminusmtregminusmt2->GetMean() << std::endl;
  
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH0_Jet_1";
  tr->hmtthregminusmttregminusmh0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH1_Jet_1";
  tr->hmtthregminusmttregminusmh1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH2_Jet_1";
  tr->hmtthregminusmttregminusmh2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmtthregminusmttregminusmh2>GetMean()= " <<  tr->hmtthregminusmttregminusmh2->GetMean() << std::endl;




  
  tit="reg_"+Reg+"_extrajet5matched_vs_jetFromW5matched";
  
  TH2* hqfromWtruthflav= (TH2*)f->Get(tit.c_str());////flav of quarks from W

  tr->fWll= hqfromWtruthflav->GetBinContent(6)/(hqfromWtruthflav->GetBinContent(6)+hqfromWtruthflav->GetBinContent(7)+hqfromWtruthflav->GetBinContent(11));

  tr->fWlc= (hqfromWtruthflav->GetBinContent(7)+hqfromWtruthflav->GetBinContent(11))/(hqfromWtruthflav->GetBinContent(6)+hqfromWtruthflav->GetBinContent(7)+hqfromWtruthflav->GetBinContent(11));

  delete hqfromWtruthflav;

  //
  tit="reg_"+Reg+"_Mlvb0";
  tr->hmlvb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb1";
  tr->hmlvb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb2";
  tr->hmlvb2=(TH1*)f->Get(tit.c_str())->Clone();


  tit="reg_"+Reg+"_truthcosthetabhRF";
  tr->truthcosthetabhRF=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_truthcosthetabbttbbRF0_5matched";
  tr->truthcosthetabbttbbRF0=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF1_5matched";
  tr->truthcosthetabbttbbRF1=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF2_5matched";
  tr->truthcosthetabbttbbRF2=(TH1*)f->Get(tit.c_str())->Clone();
  

  
  tr->hmbb->SetDirectory(0);
  tr->hmqq->SetDirectory(0);
  tr->hmqqbminusmqq->SetDirectory(0);
  tr->hmqqbregminusmqq->SetDirectory(0);
  tr->hmttregminusmtregminusmt0->SetDirectory(0);
  tr->hmttregminusmtregminusmt1->SetDirectory(0);
  tr->hmttregminusmtregminusmt2->SetDirectory(0);
  tr->hmtthregminusmttregminusmh0->SetDirectory(0);
  tr->hmtthregminusmttregminusmh1->SetDirectory(0);
  tr->hmtthregminusmttregminusmh2->SetDirectory(0);
  

  tr->hmlvb0->SetDirectory(0);
  tr->hmlvb1->SetDirectory(0);
  tr->hmlvb2->SetDirectory(0);
   tr->truthcosthetabhRF->SetDirectory(0);
      
    tr->truthcosthetabbttbbRF0->SetDirectory(0);
    tr->truthcosthetabbttbbRF1->SetDirectory(0);
    tr->truthcosthetabbttbbRF2->SetDirectory(0);
    

}

void LikelihoodDiscriminantTool::GetTrainingSig5jex4bin(TFile* f, const std::string& Reg, TrainingSig* tr){
  // std::cout << "Debug: in GetTrainingSig5jex4bin" << std::endl;
  std::string tit="";
  tit="reg_"+Reg+"_Mbb";
  tr->hmbb=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mqqb_Jet_1";
  tr->hmqqb=(TH1*)f->Get(tit.c_str())->Clone();
  // std::cout << "taking hmqqb tit= " << tit<< std::endl;
  // std::cout << "tr->hmqqb Get Mean = " << tr->hmqqb->GetMean()<< std::endl;
  
  tit="reg_"+Reg+"_jetFromW5matched";

  TH1* hqfromWtruthflav= (TH1*)f->Get(tit.c_str());////flav of quarks from W
 tr->fWll= hqfromWtruthflav->GetBinContent(1)/(hqfromWtruthflav->GetBinContent(1)+hqfromWtruthflav->GetBinContent(2));
  tr->fWlc= hqfromWtruthflav->GetBinContent(2)/(hqfromWtruthflav->GetBinContent(1)+hqfromWtruthflav->GetBinContent(2));
  delete hqfromWtruthflav;
  tit="reg_"+Reg+"_Mlvb0";
  tr->hmlvb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb1";
  tr->hmlvb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb2";
  tr->hmlvb2=(TH1*)f->Get(tit.c_str())->Clone();


  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt0_Jet_1";
  tr->hmttregminusmtregminusmt0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt1_Jet_1";
  tr->hmttregminusmtregminusmt1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt2_Jet_1";
  tr->hmttregminusmtregminusmt2=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH0_Jet_1";
  tr->hmtthregminusmttregminusmh0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH1_Jet_1";
  tr->hmtthregminusmttregminusmh1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttHRegMinusMttRegMinusMH2_Jet_1";
  tr->hmtthregminusmttregminusmh2=(TH1*)f->Get(tit.c_str())->Clone();
  // std::cout << "tr->hmtthregminusmttregminusmh2>GetMean()= " <<  tr->hmtthregminusmttregminusmh2->GetMean() << std::endl;

  
  tit="reg_"+Reg+"_truthcosthetabhRF";
  tr->truthcosthetabhRF=(TH1*)f->Get(tit.c_str())->Clone();

   
  tit="reg_"+Reg+"_truthcosthetabbttbbRF0";
  tr->truthcosthetabbttbbRF0=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF1";
  tr->truthcosthetabbttbbRF1=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_truthcosthetabbttbbRF2";
  tr->truthcosthetabbttbbRF2=(TH1*)f->Get(tit.c_str())->Clone();


 


  tr->hmttregminusmtregminusmt0->SetDirectory(0);
  tr->hmttregminusmtregminusmt1->SetDirectory(0);
  tr->hmttregminusmtregminusmt2->SetDirectory(0);
  tr->hmtthregminusmttregminusmh0->SetDirectory(0);
  tr->hmtthregminusmttregminusmh1->SetDirectory(0);
  tr->hmtthregminusmttregminusmh2->SetDirectory(0);

  tr->hmbb->SetDirectory(0);
  tr->hmqqb->SetDirectory(0);
  tr->hmlvb0->SetDirectory(0);
  tr->hmlvb1->SetDirectory(0);
  tr->hmlvb2->SetDirectory(0);



  
  tr->truthcosthetabhRF->SetDirectory(0);

    tr->truthcosthetabbttbbRF0->SetDirectory(0);
    tr->truthcosthetabbttbbRF1->SetDirectory(0);
    tr->truthcosthetabbttbbRF2->SetDirectory(0);
  


}

void LikelihoodDiscriminantTool::GetTrainingBkg(TFile* f, const std::string& Reg, TrainingBkg* tr){
  // std::cout << "Debug: in GetTrainingBkg" << std::endl;
 std::string tit=""; //std::string Reg="";

  tit="reg_"+Reg+"_2truthb_ExtraJetsbb";

  tr->hmbb=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_ExtraJetsbj";

  tr->hmbj=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_Mqq";
  tr->hmqq=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MqqbMinusMqq";
  tr->hmqqbminusmqq=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MqqbRegMinusMqq";
  // std::cout << "tit= " <<tit << std::endl;

  tr->hmqqbregminusmqq=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt0";
  // std::cout << "tit= " <<tit << std::endl;

  tr->hmttregminusmtregminusmt0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt1";
  tr->hmttregminusmtregminusmt1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt2";
  tr->hmttregminusmtregminusmt2=(TH1*)f->Get(tit.c_str())->Clone();
  // std::cout << "tr->hmttregminusmtregminusmt2->GetMean()= " <<  tr->hmttregminusmtregminusmt2->GetMean() << std::endl;


  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb0";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmttbbregminusmttregminusmbb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb1";
  tr->hmttbbregminusmttregminusmbb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb2";
  tr->hmttbbregminusmttregminusmbb2=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj0";
 // std::cout << "tit= " <<tit << std::endl;

  tr->hmttbjregminusmttregminusmbj0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj1";
  tr->hmttbjregminusmttregminusmbj1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj2";
  tr->hmttbjregminusmttregminusmbj2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttbjregminusmttregminusmbj1->GetMean()= " <<  tr->hmttbjregminusmttregminusmbj1->GetMean() << std::endl;
  
  tit="reg_"+Reg+"_Mlvb0";
  tr->hmlvb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb1";
  tr->hmlvb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb2";
  tr->hmlvb2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "debug 2 "  << std::endl;


  tit="reg_"+Reg+"_2truthb_truthcosthetabhRF_bb";
  tr->truthcosthetabhRF_bb=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabhRF_bj";
  tr->truthcosthetabhRF_bj=(TH1*)f->Get(tit.c_str())->Clone();


  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb0";
  tr->truthcosthetabbttbbRF_bb0=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj0";
  tr->truthcosthetabbttbbRF_bj0=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb1";
  tr->truthcosthetabbttbbRF_bb1=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj1";
  tr->truthcosthetabbttbbRF_bj1=(TH1*)f->Get(tit.c_str())->Clone();

   tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb2";
  tr->truthcosthetabbttbbRF_bb2=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj2";
  tr->truthcosthetabbttbbRF_bj2=(TH1*)f->Get(tit.c_str())->Clone();


  
 
  //
  tit="reg_"+Reg+"_ntruthbextrajet";
  TH1* hntruthbextrajet= (TH1*)f->Get(tit.c_str());
  tr->fbb = hntruthbextrajet->Integral(3,10)/hntruthbextrajet->Integral(2,10);///fraction of 2b case
  tr->fbj =hntruthbextrajet->GetBinContent(2)/hntruthbextrajet->Integral(2,10);///fraction of 1b case
  
  tit="reg_"+Reg+"_extrajetleadnotbtruthflav";
  TH1* hextrajetleadnotbtruthflav= (TH1*)f->Get(tit.c_str());
  tr->flight=hextrajetleadnotbtruthflav->GetBinContent(1)/hextrajetleadnotbtruthflav->Integral(); ///flavour of truth non b jet in 1b case
  tr->fc=hextrajetleadnotbtruthflav->GetBinContent(2)/hextrajetleadnotbtruthflav->Integral();///flavour of truth non b jet in 1b case

  tit="reg_"+Reg+"_2truthb_subqfromWtruthflav_vs_leadqfromWtruthflav";
  TH2* hqfromWtruthflavbb= (TH2*)f->Get(tit.c_str());////flav of quarks from W 2b case
  tr->fWll_bb=	hqfromWtruthflavbb->GetBinContent(6)/(hqfromWtruthflavbb->GetBinContent(6)+hqfromWtruthflavbb->GetBinContent(7)+hqfromWtruthflavbb->GetBinContent(11));
  tr->fWlc_bb=	(hqfromWtruthflavbb->GetBinContent(7)+hqfromWtruthflavbb->GetBinContent(11))/(hqfromWtruthflavbb->GetBinContent(6)+hqfromWtruthflavbb->GetBinContent(7)+hqfromWtruthflavbb->GetBinContent(11));
  
  tit="reg_"+Reg+"_1truthb_subqfromWtruthflav_vs_leadqfromWtruthflav";
  TH2* hqfromWtruthflavbj= (TH2*)f->Get(tit.c_str());
  tr->fWll_bj=hqfromWtruthflavbj->GetBinContent(6)/(hqfromWtruthflavbj->GetBinContent(6)+hqfromWtruthflavbj->GetBinContent(7)+hqfromWtruthflavbj->GetBinContent(11));
  tr-> fWlc_bj=(hqfromWtruthflavbj->GetBinContent(7)+hqfromWtruthflavbj->GetBinContent(11))/(hqfromWtruthflavbj->GetBinContent(6)+hqfromWtruthflavbj->GetBinContent(7)+hqfromWtruthflavbj->GetBinContent(11));
  //std::cout << "debug 3 "  << std::endl;

  delete hntruthbextrajet;
  delete hextrajetleadnotbtruthflav;
  delete hqfromWtruthflavbb;
  delete hqfromWtruthflavbj;
  tr->hmbb->SetDirectory(0);
  tr->hmbj->SetDirectory(0);
  tr->hmqq->SetDirectory(0);
  tr->hmqqbminusmqq->SetDirectory(0);
  tr->hmqqbregminusmqq->SetDirectory(0);
  tr->hmttregminusmtregminusmt0->SetDirectory(0);
  tr->hmttregminusmtregminusmt1->SetDirectory(0);
  tr->hmttregminusmtregminusmt2->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb0->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb1->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb2->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj0->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj1->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj2->SetDirectory(0);
  tr->hmlvb0->SetDirectory(0);
  tr->hmlvb1->SetDirectory(0);
  tr->hmlvb2->SetDirectory(0);

  tr->truthcosthetabhRF_bb->SetDirectory(0);
  tr->truthcosthetabhRF_bj->SetDirectory(0);
  
  tr->truthcosthetabbttbbRF_bb0->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj0->SetDirectory(0);

    tr->truthcosthetabbttbbRF_bb1->SetDirectory(0);
    tr->truthcosthetabbttbbRF_bj1->SetDirectory(0);
    
    tr->truthcosthetabbttbbRF_bb2->SetDirectory(0);
    tr->truthcosthetabbttbbRF_bj2->SetDirectory(0);


}
void LikelihoodDiscriminantTool::GetTrainingBkg5matched(TFile* f, const std::string& Reg, TrainingBkg* tr){
  //std::cout << "Debug: in GetTrainingBkg 5 matched" << std::endl;

  std::string tit="";
  
  tit="reg_"+Reg+"_2truthb_ExtraJetsbb";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmbb=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "Debug 2" << std::endl;
  tit="reg_"+Reg+"_1truthb_ExtraJetsbj";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmbj=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_Mqq_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmqq_bj=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_1truthb_MqqbRegMinusMqq_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmqqbregminusmqq_bj=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_2truthb_Mqq_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmqq_bb=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_2truthb_MqqbRegMinusMqq_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmqqbregminusmqq_bb=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt0_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmttregminusmtregminusmt0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt1_Jet_1";
  tr->hmttregminusmtregminusmt1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt2_Jet_1";
  tr->hmttregminusmtregminusmt2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttregminusmtregminusmt2->GetMean()= " <<  tr->hmttregminusmtregminusmt2->GetMean() << std::endl;


  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb0_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmttbbregminusmttregminusmbb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb1_Jet_1";
  tr->hmttbbregminusmttregminusmbb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb2_Jet_1";
  tr->hmttbbregminusmttregminusmbb2=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj0_Jet_1";

  tr->hmttbjregminusmttregminusmbj0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj1_Jet_1";
  tr->hmttbjregminusmttregminusmbj1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj2_Jet_1";
  tr->hmttbjregminusmttregminusmbj2=(TH1*)f->Get(tit.c_str())->Clone();
 

  //
  //
 tit="reg_"+Reg+"_2truthb_extrajet5matched_vs_jetFromW5matched";

  TH2* hqfromWtruthflavbb= (TH2*)f->Get(tit.c_str());////flav of quarks from W 2b case
  tr->fWll_bb=	hqfromWtruthflavbb->GetBinContent(6)/(hqfromWtruthflavbb->GetBinContent(6)+hqfromWtruthflavbb->GetBinContent(7)+hqfromWtruthflavbb->GetBinContent(11));
  tr->fWlc_bb=	(hqfromWtruthflavbb->GetBinContent(7)+hqfromWtruthflavbb->GetBinContent(11))/(hqfromWtruthflavbb->GetBinContent(6)+hqfromWtruthflavbb->GetBinContent(7)+hqfromWtruthflavbb->GetBinContent(11));

  tit="reg_"+Reg+"_1truthb_extrajet5matched_vs_jetFromW5matched";

  TH2* hqfromWtruthflavbj= (TH2*)f->Get(tit.c_str());
  tr->fWll_bj=hqfromWtruthflavbj->GetBinContent(6)/(hqfromWtruthflavbj->GetBinContent(6)+hqfromWtruthflavbj->GetBinContent(7)+hqfromWtruthflavbj->GetBinContent(11));
  tr-> fWlc_bj=(hqfromWtruthflavbj->GetBinContent(7)+hqfromWtruthflavbj->GetBinContent(11))/(hqfromWtruthflavbj->GetBinContent(6)+hqfromWtruthflavbj->GetBinContent(7)+hqfromWtruthflavbj->GetBinContent(11));
  //std::cout << "debug 3 "  << std::endl;
  tit="reg_"+Reg+"_Mlvb0";
  tr->hmlvb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb1";
  tr->hmlvb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb2";
  tr->hmlvb2=(TH1*)f->Get(tit.c_str())->Clone();
  // std::cout << "debug 2 "  << std::endl;
  tit="reg_"+Reg+"_2truthb_truthcosthetabhRF_bb";
  tr->truthcosthetabhRF_bb=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabhRF_bj";
  tr->truthcosthetabhRF_bj=(TH1*)f->Get(tit.c_str())->Clone();



  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb0_5matched";
  tr->truthcosthetabbttbbRF_bb0=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "5 matched truthcosthetabbttbbRF_bb0->GetMean()= " <<  tr->truthcosthetabbttbbRF_bb0->GetMean() << std::endl;

  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj0_5matched";
  tr->truthcosthetabbttbbRF_bj0=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb1_5matched";
  tr->truthcosthetabbttbbRF_bb1=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "5 matched truthcosthetabbttbbRF_bb1->GetMean()= " <<  tr->truthcosthetabbttbbRF_bb1->GetMean() << std::endl;

  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj1_5matched";
  tr->truthcosthetabbttbbRF_bj1=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb2_5matched";
  tr->truthcosthetabbttbbRF_bb2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "5 matched truthcosthetabbttbbRF_bb2->GetMean()= " <<  tr->truthcosthetabbttbbRF_bb2->GetMean() << std::endl;

  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj2_5matched";
  tr->truthcosthetabbttbbRF_bj2=(TH1*)f->Get(tit.c_str())->Clone();


  

//
  tit="reg_"+Reg+"_ntruthbextrajet";
  TH1* hntruthbextrajet= (TH1*)f->Get(tit.c_str());
  tr->fbb = hntruthbextrajet->Integral(3,10)/hntruthbextrajet->Integral(2,10);///fraction of 2b case
  tr->fbj =hntruthbextrajet->GetBinContent(2)/hntruthbextrajet->Integral(2,10);///fraction of 1b case
  
  tit="reg_"+Reg+"_extrajetleadnotbtruthflav";
  TH1* hextrajetleadnotbtruthflav= (TH1*)f->Get(tit.c_str());
  tr->flight=hextrajetleadnotbtruthflav->GetBinContent(1)/hextrajetleadnotbtruthflav->Integral(); ///flavour of truth non b jet in 1b case
  tr->fc=hextrajetleadnotbtruthflav->GetBinContent(2)/hextrajetleadnotbtruthflav->Integral();///flavour of truth non b jet in 1b case


  delete hntruthbextrajet;
  delete hextrajetleadnotbtruthflav;
  delete hqfromWtruthflavbb;
  delete hqfromWtruthflavbj;
  tr->hmbb->SetDirectory(0);
  tr->hmbj->SetDirectory(0);
  tr->hmqq_bb->SetDirectory(0);
  tr->hmqq_bj->SetDirectory(0);
  tr->hmqqbregminusmqq_bb->SetDirectory(0);
  tr->hmqqbregminusmqq_bj->SetDirectory(0);
  tr->hmttregminusmtregminusmt0->SetDirectory(0);
  tr->hmttregminusmtregminusmt1->SetDirectory(0);
  tr->hmttregminusmtregminusmt2->SetDirectory(0);
  
  tr->hmttbbregminusmttregminusmbb0->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb1->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb2->SetDirectory(0);
  
  tr->hmttbjregminusmttregminusmbj0->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj1->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj2->SetDirectory(0);


  
  tr->hmlvb0->SetDirectory(0);
  tr->hmlvb1->SetDirectory(0);
  tr->hmlvb2->SetDirectory(0);

  tr->truthcosthetabhRF_bb->SetDirectory(0);
  tr->truthcosthetabhRF_bj->SetDirectory(0);

  tr->truthcosthetabbttbbRF_bb0->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj0->SetDirectory(0);

  tr->truthcosthetabbttbbRF_bb1->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj1->SetDirectory(0);
    
  tr->truthcosthetabbttbbRF_bb2->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj2->SetDirectory(0);
  

}

void LikelihoodDiscriminantTool::GetTrainingBkg5jex4bin(TFile* f, const std::string& Reg, TrainingBkg* tr){
  //std::cout << "Debug: in GetTrainingBkg5jex4bin" << std::endl;

  std::string tit=""; //std::string Reg="";
 
  tit="reg_"+Reg+"_2truthb_ExtraJetsbb";
  tr->hmbb=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_ExtraJetsbj";
  tr->hmbj=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmbb Get Mean = " << tr->hmbb->GetMean()<< std::endl;

  tit="reg_"+Reg+"_Mqqb_Jet_1";
  tr->hmqqb=(TH1*)f->Get(tit.c_str())->Clone();
 

  tit="reg_"+Reg+"_Mlvb0";
  tr->hmlvb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb1";
  tr->hmlvb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_Mlvb2";
  tr->hmlvb2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmlvb2 Get Mean = " << tr->hmlvb2->GetMean()<< std::endl;

  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt0_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmttregminusmtregminusmt0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt1_Jet_1";
  tr->hmttregminusmtregminusmt1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_MttRegMinusMtRegMinusMt2_Jet_1";
  tr->hmttregminusmtregminusmt2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttregminusmtregminusmt2->GetMean()= " <<  tr->hmttregminusmtregminusmt2->GetMean() << std::endl;


  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb0_Jet_1";
  //std::cout << "tit= " <<tit << std::endl;

  tr->hmttbbregminusmttregminusmbb0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb1_Jet_1";
  tr->hmttbbregminusmttregminusmbb1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_2truthb_MttbbRegMinusMttRegMinusMbb2_Jet_1";
  tr->hmttbbregminusmttregminusmbb2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttbbregminusmttregminusmbb0->GetMean()= " <<  tr->hmttbbregminusmttregminusmbb0->GetMean() << std::endl;

  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj0_Jet_1";
  // std::cout << "tit= " <<tit << std::endl;

  tr->hmttbjregminusmttregminusmbj0=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj1_Jet_1";
  tr->hmttbjregminusmttregminusmbj1=(TH1*)f->Get(tit.c_str())->Clone();
  tit="reg_"+Reg+"_1truthb_MttbjRegMinusMttRegMinusMbj2_Jet_1";
  tr->hmttbjregminusmttregminusmbj2=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << "tr->hmttbjregminusmttregminusmbj1->GetMean()= " <<  tr->hmttbjregminusmttregminusmbj1->GetMean() << std::endl;


    
  // std::cout << "debug 2 "  << std::endl;
  tit="reg_"+Reg+"_2truthb_truthcosthetabhRF_bb";
  tr->truthcosthetabhRF_bb=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_1truthb_truthcosthetabhRF_bj";
  tr->truthcosthetabhRF_bj=(TH1*)f->Get(tit.c_str())->Clone();




  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb0";
  tr->truthcosthetabbttbbRF_bb0=(TH1*)f->Get(tit.c_str())->Clone();
  //std::cout << " 5 matched tr->truthcosthetabbttbbRF_bb0 Get Mean = " << tr->truthcosthetabbttbbRF_bb0->GetMean()<< std::endl;

  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj0";
  tr->truthcosthetabbttbbRF_bj0=(TH1*)f->Get(tit.c_str())->Clone();

  tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb1";
  tr->truthcosthetabbttbbRF_bb1=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj1";
  tr->truthcosthetabbttbbRF_bj1=(TH1*)f->Get(tit.c_str())->Clone();

   tit="reg_"+Reg+"_2truthb_truthcosthetabbttbbRF_bb2";
  tr->truthcosthetabbttbbRF_bb2=(TH1*)f->Get(tit.c_str())->Clone();
  
  tit="reg_"+Reg+"_1truthb_truthcosthetabbttbbRF_bj2";
  tr->truthcosthetabbttbbRF_bj2=(TH1*)f->Get(tit.c_str())->Clone();


  //
  tit="reg_"+Reg+"_ntruthbextrajet";
  //std::cout << "tit= " << tit<< std::endl;

  TH1* hntruthbextrajet= (TH1*)f->Get(tit.c_str());
  tr->fbb = hntruthbextrajet->Integral(3,10)/hntruthbextrajet->Integral(2,10);///fraction of 2b case
  tr->fbj =hntruthbextrajet->GetBinContent(2)/hntruthbextrajet->Integral(2,10);///fraction of 1b case
  
  tit="reg_"+Reg+"_extrajetleadnotbtruthflav";
  //std::cout << "tit= " << tit<< std::endl;

  TH1* hextrajetleadnotbtruthflav= (TH1*)f->Get(tit.c_str());
  tr->flight=hextrajetleadnotbtruthflav->GetBinContent(1)/hextrajetleadnotbtruthflav->Integral(); ///flavour of truth non b jet in 1b case
  tr->fc=hextrajetleadnotbtruthflav->GetBinContent(2)/hextrajetleadnotbtruthflav->Integral();///flavour of truth non b jet in 1b case

  tit="reg_"+Reg+"_2truthb_jetFromW5matched";
  //std::cout << "tit= " << tit<< std::endl;

  TH1* hqfromWtruthflavbb= (TH1*)f->Get(tit.c_str());////flav of quarks from W

  tr->fWll_bb= hqfromWtruthflavbb->GetBinContent(1)/(hqfromWtruthflavbb->GetBinContent(1)+hqfromWtruthflavbb->GetBinContent(2));
  
  tr->fWlc_bb= hqfromWtruthflavbb->GetBinContent(2)/(hqfromWtruthflavbb->GetBinContent(1)+hqfromWtruthflavbb->GetBinContent(2));


  tit="reg_"+Reg+"_1truthb_jetFromW5matched";
  //std::cout << "tit= " << tit<< std::endl;

  TH1* hqfromWtruthflavbj= (TH1*)f->Get(tit.c_str());////flav of quarks from W


  tr->fWll_bj= hqfromWtruthflavbj->GetBinContent(1)/(hqfromWtruthflavbj->GetBinContent(1)+hqfromWtruthflavbj->GetBinContent(2));
  

  tr->fWlc_bj= hqfromWtruthflavbj->GetBinContent(2)/(hqfromWtruthflavbj->GetBinContent(1)+hqfromWtruthflavbj->GetBinContent(2));
    
  delete hntruthbextrajet;
  delete hextrajetleadnotbtruthflav;
  delete hqfromWtruthflavbb;
  delete hqfromWtruthflavbj;
  tr->hmbb->SetDirectory(0);
  tr->hmbj->SetDirectory(0);
  tr->hmqqb->SetDirectory(0);

  tr->hmttregminusmtregminusmt0->SetDirectory(0);
  tr->hmttregminusmtregminusmt1->SetDirectory(0);
  tr->hmttregminusmtregminusmt2->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb0->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb1->SetDirectory(0);
  tr->hmttbbregminusmttregminusmbb2->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj0->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj1->SetDirectory(0);
  tr->hmttbjregminusmttregminusmbj2->SetDirectory(0);



  
  tr->truthcosthetabhRF_bb->SetDirectory(0);
  tr->truthcosthetabhRF_bj->SetDirectory(0);

  
  tr->truthcosthetabbttbbRF_bb0->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj0->SetDirectory(0);

  tr->truthcosthetabbttbbRF_bb1->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj1->SetDirectory(0);
    
  tr->truthcosthetabbttbbRF_bb2->SetDirectory(0);
  tr->truthcosthetabbttbbRF_bj2->SetDirectory(0);
   // //
  tr->hmlvb0->SetDirectory(0);
  tr->hmlvb1->SetDirectory(0);
  tr->hmlvb2->SetDirectory(0);
  //   std::cout << "debug 8 "  << std::endl;


  
}


void LikelihoodDiscriminantTool::Clear(){
  m_probB.clear();
  m_probC.clear();
  m_probL.clear();
  
  
  m_log10ProbSig_6matched=-99;
  m_log10ProbSig_5matched=-99;
  m_log10ProbSig_merged=-99;

  m_log10ProbBkg_6matched=-99;
  m_log10ProbBkg_5matched=-99;
  m_log10ProbBkg_merged=-99;

  m_Discriminant_6matched=-9;
  m_Discriminant_5matched=-9;
  m_Discriminant_6matchedAnd5matched=-9;
  m_Discriminant_merged=-9;

}
				     
void LikelihoodDiscriminantTool::Run(int njets, int nbjets, JetContainer* jets,  LeptonContainer* sleptons, float met_met, float met_phi){

  Clear();
  //int njets = eventvar->nJets;
  //int nbjets = eventvar->nBJets;
  //float neut_pt = eventvar->met_met;
  //float neut_phi = eventvar->met_phi;
  
  //jr
  TLorentzVector lep = (*sleptons)[0]->p4();
  TLorentzVector met;
  //met.SetPtEtaPhiM(neut_pt, 0., neut_phi, 0.);
  met.SetPtEtaPhiM(met_met, 0., met_phi, 0.); 
  std::vector<TLorentzVector> NuSolutions;
  NuSolutions=NuSolution(lep, met);

  m_probL.resize(jets->size());
  m_probC.resize(jets->size());
  m_probB.resize(jets->size());
  
   for (unsigned int j = 0; j < jets->size(); j++){
     std::vector <float> flavprob=GetBTagWeightsVarOP(jets->at(j)->floatVariable("mv2"));
    //std::cout << "flavprob.at(0)= " <<flavprob.at(0) << std::endl;

     m_probL[j]=flavprob.at(0);
     m_probC[j]=flavprob.at(1);
     m_probB[j]=flavprob.at(2);
  }

  //std::cout << "debug 2" <<std::endl;

  
  //std::cout << "I am in Discr Run going to check the channels" << "njets "<< njets << "nbjets " << nbjets<<std::endl;

  if(njets==6 && nbjets>=4){
    // std::cout << "I am in 6jex4bin going to calculate it"<< std::endl;

    GetDiscriminant(m_sig_6jex4bin, m_sig5matched_6jex4bin, m_bkg_6jex4bin, m_bkg5matched_6jex4bin, "6jex4bin", jets, NuSolutions, lep);
  }
  else if(njets>=7 && nbjets>=4){
    //  std::cout << "I am in 7jin4bin going to calculate it"<< std::endl;

    GetDiscriminant(m_sig_7jin4bin, m_sig5matched_7jin4bin, m_bkg_7jin4bin, m_bkg5matched_7jin4bin, "7jin4bin", jets, NuSolutions, lep);
  }

  if(njets==5 && nbjets>=4){
    //std::cout << "I am in 5j4b going to calculate it"<< std::endl;

    GetDiscriminant5jex4bin(m_sig_5jex4bin, m_bkg_5jex4bin, jets, NuSolutions, lep);
  }

  if(m_opt->Run3b){//3b

    if(njets==6 && nbjets==3){
      //  std::cout << "I am in 6jex3bex going to calculate it"<< std::endl;

      GetDiscriminant(m_sig_6jex3bex, m_sig5matched_6jex3bex, m_bkg_6jex3bex, m_bkg5matched_6jex3bex, "6jex3bex", jets, NuSolutions, lep);
    }
    else if(njets>=7 && nbjets==3){
      //  std::cout << "I am in 7jin3bex going to calculate it"<< std::endl;

      GetDiscriminant(m_sig_7jin3bex, m_sig5matched_7jin3bex, m_bkg_7jin3bex, m_bkg5matched_7jin3bex, "7jin3bex", jets, NuSolutions, lep);
    }
	
  }//3b

  
}
void LikelihoodDiscriminantTool::GetDiscriminant(TrainingSig* tr_sig, TrainingSig* tr_sig5matched, TrainingBkg* tr_bkg, TrainingBkg* tr_bkg5matched, const std::string& Reg, JetContainer* jets, std::vector<TLorentzVector> NuSolutions, const TLorentzVector& tlv_lep){
  //std::cout<<"I am in GetDiscr " <<std::endl;
  bool btag7jettests = true;
  //btag7jettests = false;
  
  float Discriminant = -9;  float ProbSig = -9; float ProbBkg = -9;
  float Discriminant_5matched = -9;  float ProbSig_5matched = -9; float ProbBkg_5matched = -9;
  float Discriminant_merged = -9;  float ProbSig_merged = -9; float ProbBkg_merged = -9;

  float ProbSigNum = 0.;  float ProbSigDen = 0.;
  float ProbSigNum_5matched = 0.;  float ProbSigDen_5matched = 0.;
  float ProbSigNum_merged = 0.;  float ProbSigDen_merged = 0.;
  
  float ProbBkgNum_bb = 0.;  float ProbBkgNum_bj = 0.; float ProbBkgDen_bb = 0.; float ProbBkgDen_bj = 0.;
  float ProbBkgNum_bb_5matched = 0.;  float ProbBkgNum_bj_5matched = 0.; float ProbBkgDen_bb_5matched = 0.; float ProbBkgDen_bj_5matched = 0.;
  float ProbBkgNum_bb_merged = 0.;  float ProbBkgNum_bj_merged = 0.; float ProbBkgDen_bb_merged = 0.; float ProbBkgDen_bj_merged = 0.;

  float ProbBkgNum = 0.;  float ProbBkgDen = 0.;
  float ProbBkgNum_5matched = 0.;  float ProbBkgDen_5matched = 0.;
  float ProbBkgNum_merged = 0.;  float ProbBkgDen_merged = 0.;
  float f6matched = 0.53; float f5matched = 0.47;
  
  TLorentzVector j1_vector, j2_vector, j3_vector, j4_vector, j5_vector, j6_vector;
  j1_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j2_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j3_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j4_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j5_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j6_vector.SetPtEtaPhiE(0., 0., 0., 0.);

  //Masses
  float Mbb=-9; float Mqqb=-9; float Mqq=-9;  float MqqbMinusMqq=-9;  float Mlvb1=-9; float Mlvb2=-9; float Mlvb0=-9;
  //new
  float Mtt0=-9; float Mtt1=-9; float Mtt2=-9;
  float MttMinusMtMinusMt0=-9; float MttMinusMtMinusMt1=-9; float MttMinusMtMinusMt2=-9;
  float Mttbb0=-9; float Mttbb1=-9; float Mttbb2=-9;
  float MttbbMinusMttMinusMbb0=-9; float MttbbMinusMttMinusMbb1=-9; float MttbbMinusMttMinusMbb2=-9;

  //Prob sig
  float PMlvb0=-9; float  PMlvb1=-9; float  PMlvb2=-9; float PMbb=-9; float PMqq=-9;	float  PMqqbMinusMqq=-9;
  //5matched 
  float PMqq_5matched = -9; float PMqqbMinusMqq_5matched = -9; 
  
  //new
  float PMttMinusMtMinusMt0=-9; float PMttMinusMtMinusMt1=-9; float PMttMinusMtMinusMt2=-9;
  float PMttbbMinusMttMinusMbb0=-9; float PMttbbMinusMttMinusMbb1=-9; float PMttbbMinusMttMinusMbb2=-9;
  //5matched
  float PMttMinusMtMinusMt0_5matched=-9; float PMttMinusMtMinusMt1_5matched=-9; float PMttMinusMtMinusMt2_5matched=-9; 
  float PMttbbMinusMttMinusMbb0_5matched=-9; float PMttbbMinusMttMinusMbb1_5matched=-9; float PMttbbMinusMttMinusMbb2_5matched=-9;


  //angular
  float CosThetab1 = -9;
  float PCosThetab1 = -9;  float PCosThetab1_bb = -9; float PCosThetab1_bj = -9;
  float CosThetabbttbb0 = -9;
  float PCosThetabbttbb0 = -9;  float PCosThetabbttbb_bb0 = -9; float PCosThetabbttbb_bj0 = -9;
    float PCosThetabbttbb0_5matched = -9;  float PCosThetabbttbb_bb0_5matched = -9; float PCosThetabbttbb_bj0_5matched = -9;

  float CosThetabbttbb1 = -9;
  float PCosThetabbttbb1 = -9;  float PCosThetabbttbb_bb1 = -9; float PCosThetabbttbb_bj1 = -9;
  float PCosThetabbttbb1_5matched = -9;  float PCosThetabbttbb_bb1_5matched = -9; float PCosThetabbttbb_bj1_5matched = -9;
  
  float CosThetabbttbb2 = -9;
  float PCosThetabbttbb2 = -9;  float PCosThetabbttbb_bb2 = -9; float PCosThetabbttbb_bj2 = -9;
    float PCosThetabbttbb2_5matched = -9;  float PCosThetabbttbb_bb2_5matched = -9; float PCosThetabbttbb_bj2_5matched = -9;

  float PermProbMPSig = 1;  float PermProbBtagSig = 1;
  float PermProbMPSig_5matched = 1;  float PermProbBtagSig_5matched = 1;
  
  //Prob bkg
  float PMbb_bkg = -9; float PMbj_bkg = -9; float PMqq_bkg = -9; float PMqqbMinusMqq_bkg = -9; float PMlvb0_bkg = -9; float PMlvb1_bkg = -9; float PMlvb2_bkg = -9;
  //5matched
  float PMqq_bb_bkg5matched = -9; float PMqq_bj_bkg5matched = -9; float PMqqbMinusMqq_bb_bkg5matched = -9; float PMqqbMinusMqq_bj_bkg5matched = -9;
  //new bkg
  float  PMttMinusMtMinusMt0_bkg=-9; float  PMttMinusMtMinusMt1_bkg=-9; float  PMttMinusMtMinusMt2_bkg=-9;
  float PMttbbMinusMttMinusMbb0_bkg=-9; float PMttbbMinusMttMinusMbb1_bkg=-9; float PMttbbMinusMttMinusMbb2_bkg=-9;
  float PMttbjMinusMttMinusMbj0_bkg=-9; float PMttbjMinusMttMinusMbj1_bkg=-9; float PMttbjMinusMttMinusMbj2_bkg=-9;
  //5matched
  float  PMttMinusMtMinusMt0_bkg_5matched=-9; float  PMttMinusMtMinusMt1_bkg_5matched=-9; float  PMttMinusMtMinusMt2_bkg_5matched=-9;
  float PMttbbMinusMttMinusMbb0_bkg_5matched=-9; float PMttbbMinusMttMinusMbb1_bkg_5matched=-9; float PMttbbMinusMttMinusMbb2_bkg_5matched=-9;
  float PMttbjMinusMttMinusMbj0_bkg_5matched=-9; float PMttbjMinusMttMinusMbj1_bkg_5matched=-9; float PMttbjMinusMttMinusMbj2_bkg_5matched=-9;

  float PermProbMPBkg = 1; float PermProbBtagBkg_bb = 1; float PermProbBtagBkg_bj = 1;
  //5matched
  float PermProbMPBkg_5matched = 1; float PermProbBtagBkg_bb_5matched = 1; float PermProbBtagBkg_bj_5matched = 1;




  
  unsigned int niter=jets->size();
  if(m_opt->jetpermcut8 && jets->size()>9) niter=9;
  
  for (unsigned int j1 = 0; j1 < niter; j1++){//j1 and j2: Higgs
	    j1_vector=jets->at(j1)->p4();
    
     for (unsigned int j2 = 0; j2 < niter; j2++){
       if(j2==j1) continue;
	     j2_vector=jets->at(j2)->p4();
      
          Mbb=(j1_vector+j2_vector).M()/1000.;
          PMbb = tr_sig -> hmbb -> GetBinContent(tr_sig->hmbb ->FindBin(Mbb));
      if(PMbb<0) PMbb=0;
      PMbb_bkg = tr_bkg->hmbb -> GetBinContent(tr_bkg->hmbb ->FindBin(Mbb));

      PMbj_bkg = tr_bkg->hmbj -> GetBinContent(tr_bkg->hmbj ->FindBin(Mbb));
      if(Mbb>1000){
	PMbb_bkg=0.;
	PMbj_bkg=0.;
      }
      if(PMbb_bkg<0) PMbb_bkg=0;
      if(PMbj_bkg<0) PMbj_bkg=0;

      //boosted 
       TLorentzVector higgs=j1_vector+j2_vector;
        CosThetab1=GetCosTheta(higgs, j1_vector);
      
      PCosThetab1 = tr_sig -> truthcosthetabhRF -> GetBinContent(tr_sig->truthcosthetabhRF ->FindBin(CosThetab1));
      PCosThetab1_bb = tr_bkg -> truthcosthetabhRF_bb -> GetBinContent(tr_bkg->truthcosthetabhRF_bb ->FindBin(CosThetab1));
      PCosThetab1_bj = tr_bkg -> truthcosthetabhRF_bj -> GetBinContent(tr_bkg->truthcosthetabhRF_bj ->FindBin(CosThetab1));
      if(PCosThetab1<0) PCosThetab1=0;
      if(PCosThetab1_bb<0) PCosThetab1_bb=0;
      if(PCosThetab1_bj<0) PCosThetab1_bj=0;
      //

      for (unsigned int j6 = 0; j6 < niter; j6++){//j6: b from lep top
	if(j6==j1 || j6==j2) continue;
	j6_vector=jets->at(j6)->p4();
	
	if (NuSolutions.size() == 1){
	   Mlvb0=(j6_vector+tlv_lep+NuSolutions.at(0)).M()/1000.;
	    PMlvb0=tr_sig->hmlvb0 -> GetBinContent(tr_sig-> hmlvb0 ->FindBin(Mlvb0));
	  if(PMlvb0<0) PMlvb0=0;
	    PMlvb0_bkg=tr_bkg->hmlvb0 -> GetBinContent(tr_bkg->hmlvb0 ->FindBin(Mlvb0));
	  if(PMlvb0_bkg<0) PMlvb0_bkg=0;
	}     
	if (NuSolutions.size() == 2){
	  Mlvb1=(j6_vector+tlv_lep+NuSolutions.at(0)).M()/1000.;
	  Mlvb2=(j6_vector+tlv_lep+NuSolutions.at(1)).M()/1000.;
	  PMlvb1=tr_sig->hmlvb1 -> GetBinContent(tr_sig->hmlvb1 ->FindBin(Mlvb1));		  
	  PMlvb2=tr_sig->hmlvb2 -> GetBinContent(tr_sig->hmlvb2 ->FindBin(Mlvb2));
	  if(PMlvb1<0) PMlvb1=0;
	  if(PMlvb2<0) PMlvb2=0;
	  PMlvb1_bkg=tr_bkg->hmlvb1 -> GetBinContent(tr_bkg->hmlvb1 ->FindBin(Mlvb1));
	  PMlvb2_bkg=tr_bkg->hmlvb2 -> GetBinContent(tr_bkg->hmlvb2 ->FindBin(Mlvb2));
	  if(PMlvb1_bkg<0) PMlvb1_bkg=0;
	  if(PMlvb2_bkg<0) PMlvb2_bkg=0;

	}


     
 	for (unsigned int j3 = 0; j3 < niter; j3++){ //j3 and j4: had W 
	  if(j3==j1 || j3==j2 || j3==j6) continue;
	  j3_vector=jets->at(j3)->p4();
	  
	  for (unsigned int j4 = 0; j4 < niter; j4++){
	    if(j4==j1 || j4==j2 || j4==j6 || j4==j3) continue;
			j4_vector=jets->at(j4)->p4();

	     Mqq=(j3_vector+j4_vector).M()/1000.;
	    	    PMqq = tr_sig -> hmqq -> GetBinContent(tr_sig -> hmqq ->FindBin(Mqq));
	    if(PMqq<0) PMqq=0;
	    	    PMqq_bkg= tr_bkg->hmqq -> GetBinContent(tr_bkg->hmqq ->FindBin(Mqq));
	    if(PMqq_bkg<0) PMqq_bkg=0;

	    //5matched
	    	    PMqq_5matched = tr_sig5matched -> hmqq -> GetBinContent(tr_sig5matched -> hmqq ->FindBin(Mqq));
	    if(PMqq_5matched<0) PMqq_5matched=0;
	    	    PMqq_bb_bkg5matched= tr_bkg5matched->hmqq_bb -> GetBinContent(tr_bkg5matched ->hmqq_bb ->FindBin(Mqq));
	    if(PMqq_bb_bkg5matched<0) PMqq_bb_bkg5matched=0;
	    	    PMqq_bj_bkg5matched= tr_bkg5matched->hmqq_bj -> GetBinContent(tr_bkg5matched ->hmqq_bj ->FindBin(Mqq));
	    if(PMqq_bj_bkg5matched<0) PMqq_bj_bkg5matched=0;

	    
 	    for (unsigned int j5 = 0; j5 <niter; j5++){//j5: b from had top
 	      if(j5==j1 || j5==j2 || j5==j6 || j5==j3 || j5==j4) continue;
 	      j5_vector=jets->at(j5)->p4();

	      //mass
	       Mqqb=(j3_vector+j4_vector+j5_vector).M()/1000.;
	      MqqbMinusMqq=Mqqb - Mqq;
	      
	      //sig
	      	      PMqqbMinusMqq  = tr_sig -> hmqqbregminusmqq -> GetBinContent(tr_sig -> hmqqbregminusmqq ->FindBin(MqqbMinusMqq));
	      if(PMqqbMinusMqq<0) PMqqbMinusMqq=0;
	      
	      //sig 5matched
	      	      PMqqbMinusMqq_5matched = tr_sig5matched -> hmqqbregminusmqq -> GetBinContent(tr_sig5matched -> hmqqbregminusmqq ->FindBin(MqqbMinusMqq));
	      if(PMqqbMinusMqq_5matched<0) PMqqbMinusMqq_5matched=0;

	      //bkg
	      	      PMqqbMinusMqq_bkg = tr_bkg->hmqqbregminusmqq -> GetBinContent(tr_bkg->hmqqbregminusmqq ->FindBin(MqqbMinusMqq));
	      if(PMqqbMinusMqq_bkg<0) PMqqbMinusMqq_bkg=0;

	      //bkg 5matched
	       PMqqbMinusMqq_bb_bkg5matched= tr_bkg5matched->hmqqbregminusmqq_bb -> GetBinContent(tr_bkg5matched ->hmqqbregminusmqq_bb ->FindBin(MqqbMinusMqq));
	       PMqqbMinusMqq_bj_bkg5matched= tr_bkg5matched->hmqqbregminusmqq_bj -> GetBinContent(tr_bkg5matched ->hmqqbregminusmqq_bj ->FindBin(MqqbMinusMqq));
	      if(PMqqbMinusMqq_bb_bkg5matched<0) PMqqbMinusMqq_bb_bkg5matched=0;      
	      if(PMqqbMinusMqq_bj_bkg5matched<0) PMqqbMinusMqq_bj_bkg5matched=0;

	      //-----------------additional masses: Mttbb and Mtt and PermProbMPSig calculation (per NuSolutions case)----------------------------//
	     
	      if (NuSolutions.size() == 1){//Mttbb and Mtt: 1 NuSol
		//masses
		//Mtt
			Mtt0=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j4_vector+j5_vector).M()/1000.;
	      	MttMinusMtMinusMt0= Mtt0 - Mqqb - Mlvb0;
		
		//Mttbb
			Mttbb0=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j4_vector+j5_vector+j1_vector+j2_vector).M()/1000.;
	      	MttbbMinusMttMinusMbb0 = Mttbb0 - Mtt0 - Mbb;
		
		//Probabilities
		//PMtt
		//sig
			      	PMttMinusMtMinusMt0=tr_sig -> hmttregminusmtregminusmt0  -> GetBinContent(tr_sig -> hmttregminusmtregminusmt0 ->FindBin(MttMinusMtMinusMt0));
		if(PMttMinusMtMinusMt0<0) PMttMinusMtMinusMt0=0;
	      
		//sig 5matched
			      	PMttMinusMtMinusMt0_5matched=tr_sig5matched -> hmttregminusmtregminusmt0  -> GetBinContent(tr_sig5matched -> hmttregminusmtregminusmt0 ->FindBin(MttMinusMtMinusMt0));
		if(PMttMinusMtMinusMt0_5matched<0) PMttMinusMtMinusMt0_5matched=0;
		
		//bkg
				PMttMinusMtMinusMt0_bkg=tr_bkg -> hmttregminusmtregminusmt0  -> GetBinContent(tr_bkg -> hmttregminusmtregminusmt0 ->FindBin(MttMinusMtMinusMt0));
	      	if(PMttMinusMtMinusMt0_bkg<0) PMttMinusMtMinusMt0_bkg=0;
		
		//bkg 5matched
			      	PMttMinusMtMinusMt0_bkg_5matched=tr_bkg5matched -> hmttregminusmtregminusmt0  -> GetBinContent(tr_bkg5matched -> hmttregminusmtregminusmt0 ->FindBin(MttMinusMtMinusMt0));		
	      	if(PMttMinusMtMinusMt0_bkg_5matched<0) PMttMinusMtMinusMt0_bkg_5matched=0;

	 

		//PMttbb
		//sig
			      	PMttbbMinusMttMinusMbb0 = tr_sig -> hmtthregminusmttregminusmh0 -> GetBinContent(tr_sig ->hmtthregminusmttregminusmh0->FindBin(MttbbMinusMttMinusMbb0));
		if(PMttbbMinusMttMinusMbb0<0) PMttbbMinusMttMinusMbb0=0;

		//sig 5matched
			      	PMttbbMinusMttMinusMbb0_5matched = tr_sig5matched -> hmtthregminusmttregminusmh0 -> GetBinContent(tr_sig5matched ->hmtthregminusmttregminusmh0->FindBin(MttbbMinusMttMinusMbb0));	       
		if(PMttbbMinusMttMinusMbb0_5matched<0) PMttbbMinusMttMinusMbb0_5matched=0;
		 
	      	//bkg
			      	PMttbbMinusMttMinusMbb0_bkg = tr_bkg -> hmttbbregminusmttregminusmbb0 -> GetBinContent(tr_bkg ->hmttbbregminusmttregminusmbb0->FindBin(MttbbMinusMttMinusMbb0));
			      	PMttbjMinusMttMinusMbj0_bkg = tr_bkg -> hmttbjregminusmttregminusmbj0 -> GetBinContent(tr_bkg ->hmttbjregminusmttregminusmbj0->FindBin(MttbbMinusMttMinusMbb0));
		if(PMttbbMinusMttMinusMbb0_bkg<0) PMttbbMinusMttMinusMbb0_bkg=0;
		if(PMttbjMinusMttMinusMbj0_bkg<0) PMttbjMinusMttMinusMbj0_bkg=0;

		//bkg 5matched
			PMttbbMinusMttMinusMbb0_bkg_5matched = tr_bkg5matched -> hmttbbregminusmttregminusmbb0 -> GetBinContent(tr_bkg5matched ->hmttbbregminusmttregminusmbb0->FindBin(MttbbMinusMttMinusMbb0));
		 	PMttbjMinusMttMinusMbj0_bkg_5matched = tr_bkg5matched -> hmttbjregminusmttregminusmbj0 -> GetBinContent(tr_bkg5matched ->hmttbjregminusmttregminusmbj0->FindBin(MttbbMinusMttMinusMbb0));	 	       
		if(PMttbbMinusMttMinusMbb0_bkg_5matched<0) PMttbbMinusMttMinusMbb0_bkg_5matched=0;
		if(PMttbjMinusMttMinusMbj0_bkg_5matched<0) PMttbjMinusMttMinusMbj0_bkg_5matched=0;


		//boosted bb in ttbb
			TLorentzVector ttbb0=j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j4_vector+j5_vector+j1_vector+j2_vector;
			CosThetabbttbb0=GetCosTheta(ttbb0, higgs);
		PCosThetabbttbb0 = tr_sig -> truthcosthetabbttbbRF0 -> GetBinContent(tr_sig->truthcosthetabbttbbRF0 ->FindBin(CosThetabbttbb0));
		 PCosThetabbttbb_bb0 = tr_bkg -> truthcosthetabbttbbRF_bb0 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bb0 ->FindBin(CosThetabbttbb0));
		 PCosThetabbttbb_bj0 = tr_bkg -> truthcosthetabbttbbRF_bj0 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bj0 ->FindBin(CosThetabbttbb0));
		if(PCosThetabbttbb0<0) PCosThetabbttbb0=0;
		if(PCosThetabbttbb_bb0<0) PCosThetabbttbb_bb0=0;
		if(PCosThetabbttbb_bj0<0) PCosThetabbttbb_bj0=0;
		//5 matched
		PCosThetabbttbb0_5matched = tr_sig5matched -> truthcosthetabbttbbRF0 -> GetBinContent(tr_sig5matched->truthcosthetabbttbbRF0 ->FindBin(CosThetabbttbb0));
		PCosThetabbttbb_bb0_5matched = tr_bkg5matched -> truthcosthetabbttbbRF_bb0 -> GetBinContent(tr_bkg5matched->truthcosthetabbttbbRF_bb0 ->FindBin(CosThetabbttbb0));
		PCosThetabbttbb_bj0_5matched = tr_bkg5matched -> truthcosthetabbttbbRF_bj0 -> GetBinContent(tr_bkg5matched->truthcosthetabbttbbRF_bj0 ->FindBin(CosThetabbttbb0));
		if(PCosThetabbttbb0_5matched<0) PCosThetabbttbb0_5matched=0;
		if(PCosThetabbttbb_bb0_5matched<0) PCosThetabbttbb_bb0_5matched=0;
		if(PCosThetabbttbb_bj0_5matched<0) PCosThetabbttbb_bj0_5matched=0;
		
		//
		//---------------------PermProbMPSig/Bkg: default, with Mttbb and with MttbbAndMtt---------------------//
		
		if(!m_opt->Mttbb && ! m_opt->MttbbAndMtt){//default without Mttbb/Mtt
		  //test boosted PCosThetab1
		  PermProbMPSig=PMbb*PMqqbMinusMqq*PMqq*PMlvb0*PCosThetab1;
		  PermProbMPSig_5matched=PMbb*PMqqbMinusMqq_5matched*PMqq_5matched*PMlvb0*PCosThetab1;
		  PermProbMPBkg=PMqqbMinusMqq_bkg*PMqq_bkg*PMlvb0_bkg;//for bkg here everything except bh (separately for bb and bj)
		  PermProbMPBkg_5matched=PMlvb0_bkg;//		  
		}
			
		if( m_opt->MttbbAndMtt){//test both Mttbb and Mtt
		  
		  PermProbMPSig=PMbb*PMqqbMinusMqq*PMqq*PMlvb0*PMttbbMinusMttMinusMbb0*PMttMinusMtMinusMt0*PCosThetab1*PCosThetabbttbb0;
		  PermProbMPSig_5matched=PMbb*PMqqbMinusMqq_5matched*PMqq_5matched*PMlvb0*PMttbbMinusMttMinusMbb0_5matched*PMttMinusMtMinusMt0_5matched*PCosThetab1*PCosThetabbttbb0_5matched;		  
		  PermProbMPBkg=PMqqbMinusMqq_bkg*PMqq_bkg*PMlvb0_bkg*PMttMinusMtMinusMt0_bkg;//bkg: here just Mtt, Mttbb added after
		  PermProbMPBkg_5matched=PMlvb0_bkg*PMttMinusMtMinusMt0_bkg_5matched;
		}

		//
	      }	      
	      else if (NuSolutions.size() == 2){//Mttbb and Mtt: 2 NuSol
		//masses
		//Mtt
		 Mtt1=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j4_vector+j5_vector).M()/1000.;
		 Mtt2=(j6_vector+tlv_lep+NuSolutions.at(1)+j3_vector+j4_vector+j5_vector).M()/1000.;
		MttMinusMtMinusMt1= Mtt1 - Mqqb - Mlvb1;
		MttMinusMtMinusMt2= Mtt2 - Mqqb - Mlvb2;
		
		// std::cout << "Mtt1= " <<Mtt1 <<" Mqqb= " << Mqqb <<" Mlvb1= "<< Mlvb1<< std::endl;
		// std::cout << "Mtt2= " <<Mtt2 <<" Mqqb= " << Mqqb <<" Mlvb2= "<< Mlvb2<< std::endl;

		//Mttbb
		 Mttbb1=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j4_vector+j5_vector+j1_vector+j2_vector).M()/1000.;
		 Mttbb2=(j6_vector+tlv_lep+NuSolutions.at(1)+j3_vector+j4_vector+j5_vector+j1_vector+j2_vector).M()/1000.;
		MttbbMinusMttMinusMbb1 = Mttbb1 - Mtt1 - Mbb;
		MttbbMinusMttMinusMbb2 = Mttbb2 - Mtt2 - Mbb;
		
		//Probabilities
		//sig PMtt
		PMttMinusMtMinusMt1=tr_sig -> hmttregminusmtregminusmt1  -> GetBinContent(tr_sig -> hmttregminusmtregminusmt1 ->FindBin(MttMinusMtMinusMt1));
		PMttMinusMtMinusMt2=tr_sig -> hmttregminusmtregminusmt2  -> GetBinContent(tr_sig -> hmttregminusmtregminusmt2 ->FindBin(MttMinusMtMinusMt2));
		if(PMttMinusMtMinusMt1<0) PMttMinusMtMinusMt1=0;
		if(PMttMinusMtMinusMt2<0) PMttMinusMtMinusMt2=0;
		
		//sig 5matched
		PMttMinusMtMinusMt1_5matched=tr_sig5matched -> hmttregminusmtregminusmt1  -> GetBinContent(tr_sig5matched -> hmttregminusmtregminusmt1 ->FindBin(MttMinusMtMinusMt1));
		PMttMinusMtMinusMt2_5matched=tr_sig5matched -> hmttregminusmtregminusmt2  -> GetBinContent(tr_sig5matched -> hmttregminusmtregminusmt2 ->FindBin(MttMinusMtMinusMt2));
		if(PMttMinusMtMinusMt1_5matched<0) PMttMinusMtMinusMt1_5matched=0;
		if(PMttMinusMtMinusMt2_5matched<0) PMttMinusMtMinusMt2_5matched=0;
		
		// std::cout << "MttMinusMtMinusMt1= " <<MttMinusMtMinusMt1 << std::endl;
		// std::cout << "MttMinusMtMinusMt2= " <<MttMinusMtMinusMt2 << std::endl;
		// std::cout << "PMttMinusMtMinusMt1_5matched= " <<PMttMinusMtMinusMt1_5matched << std::endl;
		// std::cout << "PMttMinusMtMinusMt2_5matched= " <<PMttMinusMtMinusMt2_5matched << std::endl;

		//bkg
		PMttMinusMtMinusMt1_bkg=tr_bkg -> hmttregminusmtregminusmt1  -> GetBinContent(tr_bkg -> hmttregminusmtregminusmt1 ->FindBin(MttMinusMtMinusMt1));
		PMttMinusMtMinusMt2_bkg=tr_bkg -> hmttregminusmtregminusmt2  -> GetBinContent(tr_bkg -> hmttregminusmtregminusmt2 ->FindBin(MttMinusMtMinusMt2));
		if(PMttMinusMtMinusMt1_bkg<0) PMttMinusMtMinusMt1_bkg=0;
		if(PMttMinusMtMinusMt2_bkg<0) PMttMinusMtMinusMt2_bkg=0;
		
		//bkg5matched
		PMttMinusMtMinusMt1_bkg_5matched=tr_bkg5matched -> hmttregminusmtregminusmt1  -> GetBinContent(tr_bkg5matched -> hmttregminusmtregminusmt1 ->FindBin(MttMinusMtMinusMt1));
		PMttMinusMtMinusMt2_bkg_5matched=tr_bkg5matched -> hmttregminusmtregminusmt2  -> GetBinContent(tr_bkg5matched -> hmttregminusmtregminusmt2 ->FindBin(MttMinusMtMinusMt2));
		if(PMttMinusMtMinusMt1_bkg_5matched<0) PMttMinusMtMinusMt1_bkg_5matched=0;
		if(PMttMinusMtMinusMt2_bkg_5matched<0) PMttMinusMtMinusMt2_bkg_5matched=0;
		
		//PMttbb
		//sig
		PMttbbMinusMttMinusMbb1 = tr_sig -> hmtthregminusmttregminusmh1 -> GetBinContent(tr_sig ->hmtthregminusmttregminusmh1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbbMinusMttMinusMbb2 = tr_sig -> hmtthregminusmttregminusmh2 -> GetBinContent(tr_sig ->hmtthregminusmttregminusmh2->FindBin(MttbbMinusMttMinusMbb2));
		if(PMttbbMinusMttMinusMbb1<0) PMttbbMinusMttMinusMbb1=0;
		if(PMttbbMinusMttMinusMbb2<0) PMttbbMinusMttMinusMbb2=0;
	
		//sig 5matched
		PMttbbMinusMttMinusMbb1_5matched = tr_sig5matched -> hmtthregminusmttregminusmh1 -> GetBinContent(tr_sig5matched ->hmtthregminusmttregminusmh1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbbMinusMttMinusMbb2_5matched = tr_sig5matched -> hmtthregminusmttregminusmh2 -> GetBinContent(tr_sig5matched ->hmtthregminusmttregminusmh2->FindBin(MttbbMinusMttMinusMbb2));
		if(PMttbbMinusMttMinusMbb1_5matched<0) PMttbbMinusMttMinusMbb1_5matched=0;
		if(PMttbbMinusMttMinusMbb2_5matched<0) PMttbbMinusMttMinusMbb2_5matched=0;

		//		   std::cout << "MttbbMinusMttMinusMbb1= " <<MttbbMinusMttMinusMbb1 << std::endl;
		//		   std::cout << "PMttbbMinusMttMinusMbb1= " <<PMttbbMinusMttMinusMbb1 << std::endl;
		//		   std::cout << "PMttbbMinusMttMinusMbb2= " <<PMttbbMinusMttMinusMbb2 << std::endl;

		
		//bkg
		PMttbbMinusMttMinusMbb1_bkg = tr_bkg -> hmttbbregminusmttregminusmbb1 -> GetBinContent(tr_bkg ->hmttbbregminusmttregminusmbb1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbbMinusMttMinusMbb2_bkg = tr_bkg -> hmttbbregminusmttregminusmbb2 -> GetBinContent(tr_bkg ->hmttbbregminusmttregminusmbb2->FindBin(MttbbMinusMttMinusMbb2));
		PMttbjMinusMttMinusMbj1_bkg = tr_bkg -> hmttbjregminusmttregminusmbj1 -> GetBinContent(tr_bkg ->hmttbjregminusmttregminusmbj1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbjMinusMttMinusMbj2_bkg = tr_bkg -> hmttbjregminusmttregminusmbj2 -> GetBinContent(tr_bkg ->hmttbjregminusmttregminusmbj2->FindBin(MttbbMinusMttMinusMbb2));
		if(PMttbbMinusMttMinusMbb1_bkg<0) PMttbbMinusMttMinusMbb1_bkg=0;
		if(PMttbbMinusMttMinusMbb2_bkg<0) PMttbbMinusMttMinusMbb2_bkg=0;
		if(PMttbjMinusMttMinusMbj1_bkg<0) PMttbjMinusMttMinusMbj1_bkg=0;
		if(PMttbjMinusMttMinusMbj2_bkg<0) PMttbjMinusMttMinusMbj2_bkg=0;

		//bkg 5matched
		PMttbbMinusMttMinusMbb1_bkg_5matched = tr_bkg5matched -> hmttbbregminusmttregminusmbb1 -> GetBinContent(tr_bkg5matched ->hmttbbregminusmttregminusmbb1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbbMinusMttMinusMbb2_bkg_5matched = tr_bkg5matched -> hmttbbregminusmttregminusmbb2 -> GetBinContent(tr_bkg5matched ->hmttbbregminusmttregminusmbb2->FindBin(MttbbMinusMttMinusMbb2));
		PMttbjMinusMttMinusMbj1_bkg_5matched = tr_bkg5matched -> hmttbjregminusmttregminusmbj1 -> GetBinContent(tr_bkg5matched ->hmttbjregminusmttregminusmbj1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbjMinusMttMinusMbj2_bkg_5matched = tr_bkg5matched -> hmttbjregminusmttregminusmbj2 -> GetBinContent(tr_bkg5matched ->hmttbjregminusmttregminusmbj2->FindBin(MttbbMinusMttMinusMbb2));
		if(PMttbbMinusMttMinusMbb1_bkg_5matched<0) PMttbbMinusMttMinusMbb1_bkg_5matched=0;
		if(PMttbbMinusMttMinusMbb2_bkg_5matched<0) PMttbbMinusMttMinusMbb2_bkg_5matched=0;
		if(PMttbjMinusMttMinusMbj1_bkg_5matched<0) PMttbjMinusMttMinusMbj1_bkg_5matched=0;
		if(PMttbjMinusMttMinusMbj2_bkg_5matched<0) PMttbjMinusMttMinusMbj2_bkg_5matched=0;




		//boosted bb in ttbb
			TLorentzVector ttbb1=j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j4_vector+j5_vector+j1_vector+j2_vector;
			CosThetabbttbb1=GetCosTheta(ttbb1, higgs);
		PCosThetabbttbb1 = tr_sig -> truthcosthetabbttbbRF1 -> GetBinContent(tr_sig->truthcosthetabbttbbRF1 ->FindBin(CosThetabbttbb1));
		PCosThetabbttbb_bb1 = tr_bkg -> truthcosthetabbttbbRF_bb1 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bb1 ->FindBin(CosThetabbttbb1));
		PCosThetabbttbb_bj1 = tr_bkg -> truthcosthetabbttbbRF_bj1 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bj1 ->FindBin(CosThetabbttbb1));
		if(PCosThetabbttbb1<0) PCosThetabbttbb1=0;
		if(PCosThetabbttbb_bb1<0) PCosThetabbttbb_bb1=0;
		if(PCosThetabbttbb_bj1<0) PCosThetabbttbb_bj1=0;
		//5 matched
		PCosThetabbttbb1_5matched = tr_sig5matched -> truthcosthetabbttbbRF1 -> GetBinContent(tr_sig5matched->truthcosthetabbttbbRF1 ->FindBin(CosThetabbttbb1));
		PCosThetabbttbb_bb1_5matched = tr_bkg5matched -> truthcosthetabbttbbRF_bb1 -> GetBinContent(tr_bkg5matched->truthcosthetabbttbbRF_bb1 ->FindBin(CosThetabbttbb1));
		PCosThetabbttbb_bj1_5matched = tr_bkg5matched -> truthcosthetabbttbbRF_bj1 -> GetBinContent(tr_bkg5matched->truthcosthetabbttbbRF_bj1 ->FindBin(CosThetabbttbb1));
		if(PCosThetabbttbb1_5matched<0) PCosThetabbttbb1_5matched=0;
		if(PCosThetabbttbb_bb1_5matched<0) PCosThetabbttbb_bb1_5matched=0;
		if(PCosThetabbttbb_bj1_5matched<0) PCosThetabbttbb_bj1_5matched=0;
		//     std::cout << "PCosThetabbttbb_bb1 = " <<  PCosThetabbttbb_bb1 << std::endl;
		//     std::cout << "PCosThetabbttbb_bj1 = " <<  PCosThetabbttbb_bj1 << std::endl;

		//     std::cout << "PCosThetabbttbb_bb1_5matched = " <<  PCosThetabbttbb_bb1_5matched << std::endl;
		//     std::cout << "PCosThetabbttbb_bj1_5matched = " <<  PCosThetabbttbb_bj1_5matched << std::endl;


		
		

		TLorentzVector ttbb2=j6_vector+tlv_lep+NuSolutions.at(1)+j3_vector+j4_vector+j5_vector+j1_vector+j2_vector;
	CosThetabbttbb2=GetCosTheta(ttbb2, higgs);
		PCosThetabbttbb2 = tr_sig -> truthcosthetabbttbbRF2 -> GetBinContent(tr_sig->truthcosthetabbttbbRF2 ->FindBin(CosThetabbttbb2));
		PCosThetabbttbb_bb2 = tr_bkg -> truthcosthetabbttbbRF_bb2 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bb2 ->FindBin(CosThetabbttbb2));
		PCosThetabbttbb_bj2 = tr_bkg -> truthcosthetabbttbbRF_bj2 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bj2 ->FindBin(CosThetabbttbb2));
		if(PCosThetabbttbb2<0) PCosThetabbttbb2=0;
		if(PCosThetabbttbb_bb2<0) PCosThetabbttbb_bb2=0;
		if(PCosThetabbttbb_bj2<0) PCosThetabbttbb_bj2=0;
		//5 matched
		PCosThetabbttbb2_5matched = tr_sig5matched -> truthcosthetabbttbbRF2 -> GetBinContent(tr_sig5matched->truthcosthetabbttbbRF2 ->FindBin(CosThetabbttbb2));
		PCosThetabbttbb_bb2_5matched = tr_bkg5matched -> truthcosthetabbttbbRF_bb2 -> GetBinContent(tr_bkg5matched->truthcosthetabbttbbRF_bb2 ->FindBin(CosThetabbttbb2));
		PCosThetabbttbb_bj2_5matched = tr_bkg5matched -> truthcosthetabbttbbRF_bj2 -> GetBinContent(tr_bkg5matched->truthcosthetabbttbbRF_bj2 ->FindBin(CosThetabbttbb2));
		if(PCosThetabbttbb2_5matched<0) PCosThetabbttbb2_5matched=0;
		if(PCosThetabbttbb_bb2_5matched<0) PCosThetabbttbb_bb2_5matched=0;
		if(PCosThetabbttbb_bj2_5matched<0) PCosThetabbttbb_bj2_5matched=0;

		
	      	//---------------------PermProbMPSig/Bkg: default, with Mttbb and with MttbbAndMtt---------------------//
		
	      	if(!m_opt->Mttbb && ! m_opt->MttbbAndMtt){//default without Mttbb/Mtt
		  
	      	  //boosted testPCosThetab1
	      	  PermProbMPSig=PMbb*PMqqbMinusMqq*PMqq*(0.65*PMlvb1+0.35*PMlvb2)*PCosThetab1;
	      	  PermProbMPSig_5matched= PMbb*PMqqbMinusMqq_5matched*PMqq_5matched*(0.65*PMlvb1+0.35*PMlvb2)*PCosThetab1;
	      	  PermProbMPBkg=PMqqbMinusMqq_bkg*PMqq_bkg*(0.65*PMlvb1_bkg+0.35*PMlvb2_bkg);
	      	  PermProbMPBkg_5matched=(0.65*PMlvb1_bkg+0.35*PMlvb2_bkg);		  
      
	      	}//default without Mttbb

	      	if( m_opt->MttbbAndMtt){//test both Mttbb and Mtt


	      	    PermProbMPSig=PMbb*PMqqbMinusMqq*PMqq*(0.65*PMlvb1*PMttbbMinusMttMinusMbb1*PMttMinusMtMinusMt1*PCosThetabbttbb1+0.35*PMlvb2*PMttbbMinusMttMinusMbb2*PMttMinusMtMinusMt2*PCosThetabbttbb2)*PCosThetab1;
	      	    PermProbMPSig_5matched= PMbb*PMqqbMinusMqq_5matched*PMqq_5matched*(0.65*PMlvb1*PMttbbMinusMttMinusMbb1_5matched*PMttMinusMtMinusMt1_5matched*PCosThetabbttbb1_5matched+0.35*PMlvb2*PMttbbMinusMttMinusMbb2_5matched*PMttMinusMtMinusMt2_5matched*PCosThetabbttbb2_5matched)*PCosThetab1;
	      	    PermProbMPBkg=PMqqbMinusMqq_bkg*PMqq_bkg; //bkg: here just Mtt, Mttbb added after
	      	    PermProbMPBkg_5matched=1;

				  
	      	}//test both Mttbb and Mtt
	


	      }//NuSol2
	      
	      PermProbBtagSig=m_probB.at(j1)*m_probB.at(j2)*(tr_sig->fWlc*m_probC.at(j3)+tr_sig->fWll*m_probL.at(j3))*m_probL.at(j4)*m_probB.at(j5)*m_probB.at(j6);
	      //5matched
	      PermProbBtagSig_5matched=m_probB.at(j1)*m_probB.at(j2)*(tr_sig5matched->fWlc*m_probC.at(j3)+tr_sig5matched->fWll*m_probL.at(j3))*m_probL.at(j4)*m_probB.at(j5)*m_probB.at(j6);

		
	      // // std::cout <<  "PermProbBtagSig before btag7jettest= " <<PermProbBtagSig << std::endl;

	      //btag7jettest
	      if(btag7jettests){
	      	if (Reg=="7jin4bin" || Reg=="7jin3bex"){
	      	  for(unsigned int extra_j = 0; extra_j < niter; extra_j++){
	      	    if(extra_j==j1 || extra_j==j2 || extra_j==j3 || extra_j==j4 || extra_j==j5 || extra_j==j6) continue;
	      	    PermProbBtagSig=PermProbBtagSig*m_probL.at(extra_j);
	      	    PermProbBtagSig_5matched=PermProbBtagSig_5matched*m_probL.at(extra_j);
	      	    //std::cout << "extra_j= " <<extra_j << std::endl;
	      	    //std::cout << "iteration PermProbBtagSig= " <<PermProbBtagSig << std::endl;	
	      	  }//extra_j loop
	      	  //std::cout <<  "PermProbBtagSig after = " <<PermProbBtagSig << std::endl;
	      	}//7jin4bin and 7jin3bex
	      }//btag7jettests

	      // //sum up ProbSigNum and ProbSigDen
	      // //6matched
	      ProbSigNum+=PermProbBtagSig*PermProbMPSig;
	      ProbSigDen+=PermProbBtagSig;
	      //5matched
	      ProbSigNum_5matched+=PermProbBtagSig_5matched*PermProbMPSig_5matched;
	      ProbSigDen_5matched+=PermProbBtagSig_5matched;
	      //merged
	      ProbSigNum_merged+=(f6matched*PermProbBtagSig*PermProbMPSig+f5matched*PermProbBtagSig_5matched*PermProbMPSig_5matched);
	      ProbSigDen_merged+=(f6matched*PermProbBtagSig+f5matched*PermProbBtagSig_5matched);
	      


	      
	      // // // //bkg
	      
	      PermProbBtagBkg_bb=(tr_bkg->fWlc_bb*m_probC.at(j3)+tr_bkg->fWll_bb*m_probL.at(j3))*m_probL.at(j4)*m_probB.at(j5)*m_probB.at(j6);
	      //5matched
	      PermProbBtagBkg_bb_5matched=(tr_bkg5matched->fWlc_bb*m_probC.at(j3)+tr_bkg5matched->fWll_bb*m_probL.at(j3))*m_probL.at(j4)*m_probB.at(j5)*m_probB.at(j6);

	      PermProbBtagBkg_bj=(tr_bkg->fWlc_bj*m_probC.at(j3)+tr_bkg->fWll_bj*m_probL.at(j3))*m_probL.at(j4)*m_probB.at(j5)*m_probB.at(j6);
	      //5matched
	      PermProbBtagBkg_bj_5matched=(tr_bkg5matched->fWlc_bj*m_probC.at(j3)+tr_bkg5matched->fWll_bj*m_probL.at(j3))*m_probL.at(j4)*m_probB.at(j5)*m_probB.at(j6);
		
	      if(btag7jettests){
	      	if (Reg=="7jin4bin" || Reg=="7jin3bex"){
	      	  //btag7jettest
	      	  for(unsigned int extra_j = 0; extra_j < niter; extra_j++){
	      	    if(extra_j==j1 || extra_j==j2 || extra_j==j3 || extra_j==j4 || extra_j==j5 || extra_j==j6) continue;
	      	    PermProbBtagBkg_bb=PermProbBtagBkg_bb*m_probL.at(extra_j);
	      	    PermProbBtagBkg_bb_5matched=PermProbBtagBkg_bb_5matched*m_probL.at(extra_j);
	      	    PermProbBtagBkg_bj=PermProbBtagBkg_bj*m_probL.at(extra_j);
	      	    PermProbBtagBkg_bj_5matched=PermProbBtagBkg_bj_5matched*m_probL.at(extra_j);
	      	    // std::cout << "extra_j= " <<extra_j << std::endl;
	      	  }//extra_j loop
	      	}//Reg=="7jin4bin" || Reg=="7jin3bex"
	      }//btag7jettests
	      // //
			      
	      // //default
	      if(!m_opt->Mttbb && ! m_opt->MttbbAndMtt){//default without Mttbb
		
	      	ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*PCosThetab1_bb);
	      	//5matched
	      	ProbBkgNum_bb_5matched+=(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb_5matched*PermProbMPBkg_5matched*PMqq_bb_bkg5matched*PMqqbMinusMqq_bb_bkg5matched*PCosThetab1_bb);

	      	ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*PCosThetab1_bj);
	      	//5matched
	      	ProbBkgNum_bj_5matched+=(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj_5matched*PermProbMPBkg_5matched*PMqq_bj_bkg5matched*PMqqbMinusMqq_bj_bkg5matched*PCosThetab1_bj);
	      	//merged
	      	ProbBkgNum_bb_merged+=(f6matched*(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*PCosThetab1_bb)+f5matched*(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb_5matched*PermProbMPBkg_5matched*PMqq_bb_bkg5matched*PMqqbMinusMqq_bb_bkg5matched*PCosThetab1_bb));
				       
	      	ProbBkgNum_bj_merged+=(f6matched*(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*PCosThetab1_bj)+f5matched*(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj_5matched*PermProbMPBkg_5matched*PMqq_bj_bkg5matched*PMqqbMinusMqq_bj_bkg5matched*PCosThetab1_bj));
		
	      }
	      //
	      //test
	      if (NuSolutions.size() == 1){
	      	if(m_opt->Mttbb ||  m_opt->MttbbAndMtt){//test both Mttbb // or MttbbAndMtt
	      	  ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*PMttbbMinusMttMinusMbb0_bkg*PCosThetab1_bb*PCosThetabbttbb_bb0);
	      	  ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*PMttbjMinusMttMinusMbj0_bkg*PCosThetab1_bj*PCosThetabbttbb_bj0);
	      	  //5matched
	      	  ProbBkgNum_bb_5matched+=(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb_5matched*PermProbMPBkg_5matched*PMqq_bb_bkg5matched*PMqqbMinusMqq_bb_bkg5matched*PMttbbMinusMttMinusMbb0_bkg_5matched*PCosThetab1_bb*PCosThetabbttbb_bb0_5matched);//done
	   		
	      	  ProbBkgNum_bj_5matched+=(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj_5matched*PermProbMPBkg_5matched*PMqq_bj_bkg5matched*PMqqbMinusMqq_bj_bkg5matched*PMttbjMinusMttMinusMbj0_bkg_5matched*PCosThetab1_bj*PCosThetabbttbb_bj0_5matched);
	      	  //merged
	      	  ProbBkgNum_bb_merged+=(f6matched*(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*PMttbbMinusMttMinusMbb0_bkg*PCosThetab1_bb*PCosThetabbttbb_bb0)+f5matched*(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb_5matched*PermProbMPBkg_5matched*PMqq_bb_bkg5matched*PMqqbMinusMqq_bb_bkg5matched*PMttbbMinusMttMinusMbb0_bkg_5matched*PCosThetab1_bb*PCosThetabbttbb_bb0_5matched));
	      	  ProbBkgNum_bj_merged+=(f6matched*(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*PMttbjMinusMttMinusMbj0_bkg*PCosThetab1_bj*PCosThetabbttbb_bj0)+f5matched*(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj_5matched*PermProbMPBkg_5matched*PMqq_bj_bkg5matched*PMqqbMinusMqq_bj_bkg5matched*PMttbjMinusMttMinusMbj0_bkg_5matched*PCosThetab1_bj*PCosThetabbttbb_bj0_5matched));

		  
	      	}////test Mttbb - Mtt -Mbb  //with or	without Mtt				
	      }
	      else if (NuSolutions.size() == 2){
	      	if( m_opt->MttbbAndMtt){//test MttbbAndMtt
		  
	      	  ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*(0.65*PMttbbMinusMttMinusMbb1_bkg*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg*PCosThetabbttbb_bb1+0.35*PMttbbMinusMttMinusMbb2_bkg*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg*PCosThetabbttbb_bb2)*PCosThetab1_bb);
		  
	      	  ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*(0.65*PMttbjMinusMttMinusMbj1_bkg*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg*PCosThetabbttbb_bj1+0.35*PMttbjMinusMttMinusMbj2_bkg*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg*PCosThetabbttbb_bj2)*PCosThetab1_bj);
	      	  //5matched
	      	  ProbBkgNum_bb_5matched+=(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb_5matched*PermProbMPBkg_5matched*PMqq_bb_bkg5matched*PMqqbMinusMqq_bb_bkg5matched*(0.65*PMttbbMinusMttMinusMbb1_bkg_5matched*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg_5matched*PCosThetabbttbb_bb1_5matched+0.35*PMttbbMinusMttMinusMbb2_bkg_5matched*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg_5matched*PCosThetabbttbb_bb2_5matched)*PCosThetab1_bb);//done
	   		
	      	  ProbBkgNum_bj_5matched+=(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj_5matched*PermProbMPBkg_5matched*PMqq_bj_bkg5matched*PMqqbMinusMqq_bj_bkg5matched*(0.65*PMttbjMinusMttMinusMbj1_bkg_5matched*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg_5matched*PCosThetabbttbb_bj1_5matched+0.35*PMttbjMinusMttMinusMbj2_bkg_5matched*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg_5matched)*PCosThetab1_bj*PCosThetabbttbb_bj2_5matched);
	      	  //merged
	      	  ProbBkgNum_bb_merged+=(f6matched*(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*(0.65*PMttbbMinusMttMinusMbb1_bkg*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg*PCosThetabbttbb_bb1+0.35*PMttbbMinusMttMinusMbb2_bkg*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg*PCosThetabbttbb_bb2)*PCosThetab1_bb)+f5matched*(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb_5matched*PermProbMPBkg_5matched*PMqq_bb_bkg5matched*PMqqbMinusMqq_bb_bkg5matched*(0.65*PMttbbMinusMttMinusMbb1_bkg_5matched*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg_5matched*PCosThetabbttbb_bb1_5matched+0.35*PMttbbMinusMttMinusMbb2_bkg_5matched*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg_5matched*PCosThetabbttbb_bb2_5matched)*PCosThetab1_bb));
	      	  ProbBkgNum_bj_merged+=(f6matched*(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*(0.65*PMttbjMinusMttMinusMbj1_bkg*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg*PCosThetabbttbb_bj1+0.35*PMttbjMinusMttMinusMbj2_bkg*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg*PCosThetabbttbb_bj1)*PCosThetab1_bj)+f5matched*(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj_5matched*PermProbMPBkg_5matched*PMqq_bj_bkg5matched*PMqqbMinusMqq_bj_bkg5matched*(0.65*PMttbjMinusMttMinusMbj1_bkg_5matched*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg_5matched*PCosThetabbttbb_bj1_5matched+0.35*PMttbjMinusMttMinusMbj2_bkg_5matched*PMlvb2_bkg*PMttbbMinusMttMinusMbb2_bkg_5matched*PCosThetabbttbb_bj1_5matched)*PCosThetab1_bj));
	
	      	}//test
		
	      }//Nu Sol 2

	      ProbBkgDen_bb+=tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PermProbBtagBkg_bb;
	      ProbBkgDen_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PermProbBtagBkg_bj);
	      //5matched
	      ProbBkgDen_bb_5matched+=tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PermProbBtagBkg_bb_5matched;
	      ProbBkgDen_bj_5matched+=(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PermProbBtagBkg_bj_5matched);

	      ProbBkgDen_bb_merged+=(f6matched*(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PermProbBtagBkg_bb)+f5matched*(tr_bkg5matched->fbb*m_probB.at(j1)*m_probB.at(j2)*PermProbBtagBkg_bb_5matched));
	      ProbBkgDen_bj_merged+=(f6matched*(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PermProbBtagBkg_bj)+f5matched*(tr_bkg5matched->fbj*m_probB.at(j1)*(tr_bkg5matched->flight*m_probL.at(j2)+ tr_bkg5matched->fc*m_probC.at(j2))*PermProbBtagBkg_bj_5matched));

		
	    
	      ProbBkgNum=ProbBkgNum_bb+ProbBkgNum_bj;
	      ProbBkgDen=ProbBkgDen_bb+ProbBkgDen_bj;
	      //5matched
	      ProbBkgNum_5matched=ProbBkgNum_bb_5matched+ProbBkgNum_bj_5matched;
	      ProbBkgDen_5matched=ProbBkgDen_bb_5matched+ProbBkgDen_bj_5matched;
	      //merged
	      ProbBkgNum_merged=ProbBkgNum_bb_merged+ProbBkgNum_bj_merged;
	      ProbBkgDen_merged=ProbBkgDen_bb_merged+ProbBkgDen_bj_merged;
	    }////j6	      
	  }	   
 	}
       }     
     }    
  }////////j1


  

  // std::cout << "-----------------------------------------ProbSigNum 6 matched " << ProbSigNum << std::endl;
  // std::cout << "-----------------------------------------ProbSigDen 6 matched " << ProbSigDen << std::endl;
  // std::cout << "-----------------------------------------ProbBkgNum 6 matched " << ProbBkgNum << std::endl;
  // std::cout << "-----------------------------------------ProbBkgDen 6 matched " << ProbBkgDen << std::endl;
  // std::cout << "-----------------------------------------ProbSigNum 5 matched " << ProbSigNum_5matched << std::endl;
  // std::cout << "-----------------------------------------ProbSigDen 5 matched " << ProbSigDen_5matched << std::endl;
  // std::cout << "-----------------------------------------ProbBkgNum 5 matched " << ProbBkgNum_5matched << std::endl;
  // std::cout << "-----------------------------------------ProbBkgDen 5 matched " << ProbBkgDen_5matched << std::endl;
  // std::cout << "-----------------------------------------ProbSigNum merged " << ProbSigNum_merged << std::endl;
  // std::cout << "-----------------------------------------ProbSigDen merged " << ProbSigDen_merged << std::endl;
  // std::cout << "-----------------------------------------ProbBkgNum merged " << ProbBkgNum_merged << std::endl;
  // std::cout << "-----------------------------------------ProbBkgDen merged " << ProbBkgDen_merged << std::endl;

    
  if(ProbSigNum<=0)  {
    std::cout << "!!! ProbSigNum = " <<  ProbSigNum << std::endl;
    m_log10ProbSig_6matched = -28;    
  }
  
  if (ProbSigDen<=0){
    std::cout << "!!! ProbSigDen 4 = " <<  ProbSigDen << std::endl;
    ProbSig=-0.1;
    m_log10ProbSig_6matched = -28;
  }
  else {
    ProbSig=ProbSigNum/ProbSigDen;
    m_log10ProbSig_6matched = log10(ProbSig);    
  }

  if(ProbSigNum_5matched<=0)  {
    std::cout << "!!! ProbSigNum_5matched = " <<  ProbSigNum_5matched << std::endl;
    ProbSig_5matched=-0.1;
    m_log10ProbSig_5matched = -28;
  }
  
  if (ProbSigDen_5matched<=0){
    std::cout << "!!! ProbSigDen_5matched = " <<  ProbSigDen_5matched << std::endl;
    ProbSig_5matched=-0.1;
    m_log10ProbSig_5matched = -28;
  }
  else {
    ProbSig_5matched=ProbSigNum_5matched/ProbSigDen_5matched;    
    m_log10ProbSig_5matched = log10(ProbSig_5matched);
  }

  
  //bkg
  if(ProbBkgNum<=0)  {
    std::cout << "!!! ProbBkgNum = " <<  ProbBkgNum << std::endl;
    m_log10ProbBkg_6matched = -28;

  }
  if (ProbBkgDen<=0) {
    std::cout << "!!! ProbBkgDen = " <<  ProbBkgDen << std::endl;
    ProbBkg=-0.1;
    m_log10ProbBkg_6matched = -28;

  }
  else {
    ProbBkg=ProbBkgNum/ProbBkgDen;
    m_log10ProbBkg_6matched = log10(ProbBkg);
  }
  
  if(ProbBkgNum_5matched<=0)  {
    std::cout << "!!! ProbBkgNum_5matched = " <<  ProbBkgNum_5matched << std::endl;
    m_log10ProbBkg_5matched = -28;

  }
  if (ProbBkgDen_5matched<=0) {
    std::cout << "!!! ProbBkgDen_5matched = " <<  ProbBkgDen_5matched << std::endl;
    ProbBkg_5matched=-0.1;
    m_log10ProbBkg_5matched = -28;

  }
  else {
    ProbBkg_5matched=ProbBkgNum_5matched/ProbBkgDen_5matched;
    m_log10ProbBkg_5matched = log10(ProbBkg_5matched);
  }



  //merged sig
  if(ProbSigNum_merged<=0)  {
    std::cout << "!!! ProbSigNum_merged = " <<  ProbSigNum_merged << std::endl;
    ProbSig_merged=-0.1;
    m_log10ProbSig_merged = -28;

  }

  if (ProbSigDen_merged<=0){
    std::cout << "!!! ProbSigDen_merged = " <<  ProbSigDen_merged << std::endl;
    ProbSig_merged=-0.1;
    m_log10ProbSig_merged = -28;

  }
  else {
    ProbSig_merged=ProbSigNum_merged/ProbSigDen_merged;    
    m_log10ProbSig_merged = log10(ProbSig_merged);
  }



  //merged bkg
  if(ProbBkgNum_merged<=0)  {
    std::cout << "!!! ProbBkgNum_merged = " <<  ProbBkgNum_merged << std::endl;
    m_log10ProbBkg_merged = -28;


  }
  if (ProbBkgDen_merged<=0) {
    std::cout << "!!! ProbBkgDen_merged = " <<  ProbBkgDen_merged << std::endl;
    ProbBkg_merged=-0.1;
    m_log10ProbBkg_merged = -28;

  }
  else {
    ProbBkg_merged=ProbBkgNum_merged/ProbBkgDen_merged;
    m_log10ProbBkg_merged = log10(ProbBkg_merged);

  }
  //
   
  // if (ProbSig<0 || ProbBkg<0) Discriminant=-9;
  // else if (ProbSig==0 && ProbBkg==0) Discriminant=-3;
  if (ProbSig<0 || ProbBkg<0) Discriminant=-0.3;
  else if (ProbSig==0 && ProbBkg==0) Discriminant=-0.1;
  else Discriminant=ProbSig/(ProbSig+ProbBkg);
  if(Discriminant>-0.1){
    //m_outData -> o_Discriminant=Discriminant;
    //std::cout << "//" << std::endl;
    // std::cout << "Reg= " <<Reg << std::endl;

    // std::cout << "ProbSig= " <<ProbSig << std::endl;
    // std::cout << "ProbBkg= " <<ProbBkg << std::endl;
    m_Discriminant_6matched=Discriminant;
    //     std::cout << "//" << std::endl;

    //  std::cout << "Reg= " <<Reg << std::endl;

    //std::cout << "Final Discriminant 6matched= " <<m_Discriminant_6matched << std::endl;
  }
  
  // if (ProbSig_5matched<0 || ProbBkg_5matched<0) Discriminant_5matched=-9;
  // else if (ProbSig_5matched==0 && ProbBkg_5matched==0) Discriminant_5matched=-3;
  if (ProbSig_5matched<0 || ProbBkg_5matched<0) Discriminant_5matched=-0.3;
  else if (ProbSig_5matched==0 && ProbBkg_5matched==0) Discriminant_5matched=-0.1;
  else Discriminant_5matched=ProbSig_5matched/(ProbSig_5matched+ProbBkg_5matched);
  if(Discriminant_5matched>-0.1){
    //m_outData -> o_Discriminant_5matched=Discriminant_5matched;
    // std::cout << "//" << std::endl;
    // std::cout << "ProbSig= " <<ProbSig << std::endl;
    // std::cout << "ProbBkg= " <<ProbBkg << std::endl;
      m_Discriminant_5matched=Discriminant_5matched;

      // std::cout << "Final Discriminant 5matched= " << m_Discriminant_5matched << std::endl;
  }
  if(Discriminant>-0.1 && Discriminant_5matched>-0.1){
    m_Discriminant_6matchedAnd5matched=0.53*Discriminant+0.47*Discriminant_5matched;
  }


  //merged
  if (ProbSig_merged<0 || ProbBkg_merged<0) Discriminant_merged=-0.3;
  else if (ProbSig_merged==0 && ProbBkg_merged==0) Discriminant_merged=-0.1;
  else Discriminant_merged=ProbSig_merged/(ProbSig_merged+ProbBkg_merged);
  if(Discriminant_merged>-0.1){
    //m_outData -> o_Discriminant_merged=Discriminant_merged;
    m_Discriminant_merged=Discriminant_merged;
    //std::cout << "Reg= " <<Reg << std::endl;

    //std::cout << "Final Discriminant_merged= " <<m_Discriminant_merged << std::endl;
  }


  //std::cout << "Reg= " <<Reg << std::endl;
  //std::cout << "Final Discriminant_merged= " <<m_Discriminant_merged << std::endl;

  // std::cout << "ProbSig= " <<ProbSig << std::endl;
  // std::cout << "ProbBkg= " <<ProbBkg << std::endl;
 
}
void LikelihoodDiscriminantTool::GetDiscriminant5jex4bin(TrainingSig* tr_sig, TrainingBkg* tr_bkg, JetContainer* jets, std::vector<TLorentzVector> NuSolutions, const TLorentzVector& tlv_lep){
  //std::cout<<"I am in GetDiscr 5jex4bin " <<std::endl;
  
  float Discriminant = -9;  float ProbSig = -9; float ProbBkg = -9;

  float ProbSigNum = 0.;  float ProbSigDen = 0.;
  
  float ProbBkgNum_bb = 0.;  float ProbBkgNum_bj = 0.; float ProbBkgDen_bb = 0.; float ProbBkgDen_bj = 0.;

  float ProbBkgNum = 0.;  float ProbBkgDen = 0.;
  
  TLorentzVector j1_vector, j2_vector, j3_vector, j4_vector, j5_vector, j6_vector;
  j1_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j2_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j3_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j4_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j5_vector.SetPtEtaPhiE(0., 0., 0., 0.);
  j6_vector.SetPtEtaPhiE(0., 0., 0., 0.);

  //Masses
  float Mbb=-9; float Mlvb1=-9; float Mlvb2=-9; float Mlvb0=-9;
  float Mqb=-9;
  float Mtt0=-9; float Mtt1=-9; float Mtt2=-9;
  float MttMinusMtMinusMt0=-9; float MttMinusMtMinusMt1=-9; float MttMinusMtMinusMt2=-9;

  float Mttbb0=-9; float Mttbb1=-9; float Mttbb2=-9;
  float MttbbMinusMttMinusMbb0=-9; float MttbbMinusMttMinusMbb1=-9; float MttbbMinusMttMinusMbb2=-9;

  //Prob sig
  float  PMlvb0=-9; float  PMlvb1=-9; float  PMlvb2=-9; float PMbb=-9;
  float PMqb=-9;
  
  //new
  float PMttMinusMtMinusMt0=-9; float PMttMinusMtMinusMt1=-9; float PMttMinusMtMinusMt2=-9;
  float PMttbbMinusMttMinusMbb0=-9; float PMttbbMinusMttMinusMbb1=-9; float PMttbbMinusMttMinusMbb2=-9;

    //angular
  float CosThetab1 = -9;
  float PCosThetab1 = -9;  float PCosThetab1_bb = -9; float PCosThetab1_bj = -9;
  float CosThetabbttbb0 = -9;
  float PCosThetabbttbb0 = -9;  float PCosThetabbttbb_bb0 = -9; float PCosThetabbttbb_bj0 = -9;

  float CosThetabbttbb1 = -9;
  float PCosThetabbttbb1 = -9;  float PCosThetabbttbb_bb1 = -9; float PCosThetabbttbb_bj1 = -9;
  
  float CosThetabbttbb2 = -9;
  float PCosThetabbttbb2 = -9;  float PCosThetabbttbb_bb2 = -9; float PCosThetabbttbb_bj2 = -9;

  float PermProbMPSig = 1;  float PermProbBtagSig = 1;
  //5matched
  
  //Prob bkg
  float PMbb_bkg = -9; float PMbj_bkg = -9; float PMlvb0_bkg = -9; float PMlvb1_bkg = -9; float PMlvb2_bkg = -9;  float PMqb_bkg=-9;
  
  //new bkg
  float  PMttMinusMtMinusMt0_bkg=-9; float  PMttMinusMtMinusMt1_bkg=-9; float  PMttMinusMtMinusMt2_bkg=-9;
  float PMttbbMinusMttMinusMbb0_bkg=-9; float PMttbbMinusMttMinusMbb1_bkg=-9; float PMttbbMinusMttMinusMbb2_bkg=-9;
  float PMttbjMinusMttMinusMbj0_bkg=-9; float PMttbjMinusMttMinusMbj1_bkg=-9; float PMttbjMinusMttMinusMbj2_bkg=-9;
   
  float PermProbMPBkg = 1; float PermProbBtagBkg_bb = 1; float PermProbBtagBkg_bj = 1;
  unsigned int niter=jets->size();

  for (unsigned int j1 = 0; j1 < niter; j1++){
	
    //j1_vector.SetPtEtaPhiE(jets->at(j1)->p4().Pt(), jets->at(j1)->p4().Eta(), jets->at(j1)->p4().Phi(), jets->at(j1)->p4().E());
	    j1_vector=jets->at(j1)->p4();

    for (unsigned int j2 = 0; j2 < niter; j2++){
      if(j2==j1) continue;
      
      //j2_vector.SetPtEtaPhiE(jets->at(j2)->p4().Pt(), jets->at(j2)->p4().Eta(), jets->at(j2)->p4().Phi(), jets->at(j2)->p4().E());
      	     j2_vector=jets->at(j2)->p4();

      Mbb=(j1_vector+j2_vector).M()/1000.;

      PMbb = tr_sig -> hmbb -> GetBinContent(tr_sig->hmbb ->FindBin(Mbb));

      if(PMbb<0) PMbb=0;
      PMbb_bkg = tr_bkg->hmbb -> GetBinContent(tr_bkg->hmbb ->FindBin(Mbb));

      PMbj_bkg = tr_bkg->hmbj -> GetBinContent(tr_bkg->hmbj ->FindBin(Mbb));
      if(Mbb>1000){
	PMbb_bkg=0.;
	PMbj_bkg=0.;
      }
      if(PMbb_bkg<0) PMbb_bkg=0;
      if(PMbj_bkg<0) PMbj_bkg=0;
            //boosted 
      //boosted 
      TLorentzVector higgs=j1_vector+j2_vector;
      CosThetab1=GetCosTheta(higgs, j1_vector);



      
      PCosThetab1 = tr_sig -> truthcosthetabhRF -> GetBinContent(tr_sig->truthcosthetabhRF ->FindBin(CosThetab1));
      PCosThetab1_bb = tr_bkg -> truthcosthetabhRF_bb -> GetBinContent(tr_bkg->truthcosthetabhRF_bb ->FindBin(CosThetab1));
      PCosThetab1_bj = tr_bkg -> truthcosthetabhRF_bj -> GetBinContent(tr_bkg->truthcosthetabhRF_bj ->FindBin(CosThetab1));
            if(PCosThetab1<0) PCosThetab1=0;
            if(PCosThetab1_bb<0) PCosThetab1_bb=0;
            if(PCosThetab1_bj<0) PCosThetab1_bj=0;
	    // std::cout<<"PCosThetab1 " << PCosThetab1 <<std::endl;
	    // std::cout<<"PCosThetab1_bb " << PCosThetab1_bb <<std::endl;

      //

      for (unsigned int j6 = 0; j6 < niter; j6++){
	if(j6==j1 || j6==j2) continue;

	//j6_vector.SetPtEtaPhiE(jets->at(j6)->p4().Pt(), jets->at(j6)->p4().Eta(), jets->at(j6)->p4().Phi(), jets->at(j6)->p4().E());
		j6_vector=jets->at(j6)->p4();

	if (NuSolutions.size() == 1){
	  Mlvb0=(j6_vector+tlv_lep+NuSolutions.at(0)).M()/1000.;
	  PMlvb0=tr_sig->hmlvb0 -> GetBinContent(tr_sig-> hmlvb0 ->FindBin(Mlvb0));
	  if(PMlvb0<0) PMlvb0=0;
	  PMlvb0_bkg=tr_bkg->hmlvb0 -> GetBinContent(tr_bkg->hmlvb0 ->FindBin(Mlvb0));
	  if(PMlvb0_bkg<0) PMlvb0_bkg=0;
	}     
	if (NuSolutions.size() == 2){
	  Mlvb1=(j6_vector+tlv_lep+NuSolutions.at(0)).M()/1000.;
	  Mlvb2=(j6_vector+tlv_lep+NuSolutions.at(1)).M()/1000.;
	  PMlvb1=tr_sig->hmlvb1 -> GetBinContent(tr_sig->hmlvb1 ->FindBin(Mlvb1));
		  
	  PMlvb2=tr_sig->hmlvb2 -> GetBinContent(tr_sig->hmlvb2 ->FindBin(Mlvb2));
	  if(PMlvb1<0) PMlvb1=0;
	  if(PMlvb2<0) PMlvb2=0;
	  PMlvb1_bkg=tr_bkg->hmlvb1 -> GetBinContent(tr_bkg->hmlvb1 ->FindBin(Mlvb1));
	  PMlvb2_bkg=tr_bkg->hmlvb2 -> GetBinContent(tr_bkg->hmlvb2 ->FindBin(Mlvb2));
	  if(PMlvb1_bkg<0) PMlvb1_bkg=0;
	  if(PMlvb2_bkg<0) PMlvb2_bkg=0;
	}



	for (unsigned int j3 = 0; j3 < niter; j3++){
	  if(j3==j1 || j3==j2 || j3==j6) continue;
	  //j3_vector.SetPtEtaPhiE(jets->at(j3)->p4().Pt(), jets->at(j3)->p4().Eta(), jets->at(j3)->p4().Phi(), jets->at(j3)->p4().E());
	  j3_vector=jets->at(j3)->p4();

	  //for (unsigned int j4 = 0; j4 < niter; j4++){
	  //if(j4==j1 || j4==j2 || j4==j6 || j4==j3) continue;
	  //j4_vector.SetPtEtaPhiE(jets->at(j4)->p4().Pt(), jets->at(j4)->p4().Eta(), jets->at(j4)->p4().Phi(), jets->at(j4)->p4().E());
	  //Mqq=(j3_vector+j4_vector).M()/1000.;
	  //PMqq = tr_sig -> hmqq -> GetBinContent(tr_sig -> hmqq ->FindBin(Mqq));
	  //if(PMqq<0) PMqq=0;
	  // PMqq_bkg= tr_bkg->hmqq -> GetBinContent(tr_bkg->hmqq ->FindBin(Mqq));
	  // if(PMqq_bkg<0) PMqq_bkg=0;

	  // //5matched
	  // PMqq_5matched = tr_sig5matched -> hmqq -> GetBinContent(tr_sig5matched -> hmqq ->FindBin(Mqq));
	  // if(PMqq_5matched<0) PMqq_5matched=0;
	  // PMqq_bb_bkg5matched= tr_bkg5matched->hmqq_bb -> GetBinContent(tr_bkg5matched ->hmqq_bb ->FindBin(Mqq));
	  // if(PMqq_bb_bkg5matched<0) PMqq_bb_bkg5matched=0;
	  // PMqq_bj_bkg5matched= tr_bkg5matched->hmqq_bj -> GetBinContent(tr_bkg5matched ->hmqq_bj ->FindBin(Mqq));
	  // if(PMqq_bj_bkg5matched<0) PMqq_bj_bkg5matched=0;
   
	    
	  for (unsigned int j5 = 0; j5 <niter; j5++){
	    //if(j5==j1 || j5==j2 || j5==j6 || j5==j3 || j5==j4) continue;
	    if(j5==j1 || j5==j2 || j5==j6 || j5==j3) continue;
      

	    //j5_vector.SetPtEtaPhiE(jets->at(j5)->p4().Pt(), jets->at(j5)->p4().Eta(), jets->at(j5)->p4().Phi(), jets->at(j5)->p4().E());
	    j5_vector=jets->at(j5)->p4();

		
	    //Mqqb=(j3_vector+j4_vector+j5_vector).M()/1000.;
	    Mqb=(j3_vector+j5_vector).M()/1000.;//5jex4bin
	    //std::cout<<"debug 4" <<std::endl;
   
	    PMqb  = tr_sig -> hmqqb -> GetBinContent(tr_sig -> hmqqb ->FindBin(Mqb));
	    if(PMqb<0) PMqb=0;

	    PMqb_bkg  = tr_bkg -> hmqqb -> GetBinContent(tr_bkg -> hmqqb ->FindBin(Mqb));
	    if(PMqb_bkg<0) PMqb_bkg=0;


	    
	    	    // std::cout<<"PMbb= " << PMbb <<std::endl;
	    	    // std::cout<<"PMqb= " << PMqb <<std::endl;
	    	    // std::cout<<"PMlvb2= " << PMlvb2 <<std::endl;
	    	    // std::cout<<"PCosThetab1= " << PCosThetab1 <<std::endl;

	      //-----------------additional masses: Mttbb and Mtt and PermProbMPSig calculation (per NuSolutions case)----------------------------//

	    if (NuSolutions.size() == 1){

		//masses
		//Mtt
	      	Mtt0=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j5_vector).M()/1000.;
	      	MttMinusMtMinusMt0= Mtt0 - Mqb - Mlvb0;
		
		//Mttbb
	      	Mttbb0=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j5_vector+j1_vector+j2_vector).M()/1000.;
	      	MttbbMinusMttMinusMbb0 = Mttbb0 - Mtt0 - Mbb;
		
		//Probabilities
		//PMtt
		//sig
	      	PMttMinusMtMinusMt0=tr_sig -> hmttregminusmtregminusmt0  -> GetBinContent(tr_sig -> hmttregminusmtregminusmt0 ->FindBin(MttMinusMtMinusMt0));
		if(PMttMinusMtMinusMt0<0) PMttMinusMtMinusMt0=0;
		
		//bkg
		PMttMinusMtMinusMt0_bkg=tr_bkg -> hmttregminusmtregminusmt0  -> GetBinContent(tr_bkg -> hmttregminusmtregminusmt0 ->FindBin(MttMinusMtMinusMt0));
	      	if(PMttMinusMtMinusMt0_bkg<0) PMttMinusMtMinusMt0_bkg=0;
		 

		//PMttbb
		//sig
	      	PMttbbMinusMttMinusMbb0 = tr_sig -> hmtthregminusmttregminusmh0 -> GetBinContent(tr_sig ->hmtthregminusmttregminusmh0->FindBin(MttbbMinusMttMinusMbb0));
		if(PMttbbMinusMttMinusMbb0<0) PMttbbMinusMttMinusMbb0=0;

		 
	      	//bkg
	      	PMttbbMinusMttMinusMbb0_bkg = tr_bkg -> hmttbbregminusmttregminusmbb0 -> GetBinContent(tr_bkg ->hmttbbregminusmttregminusmbb0->FindBin(MttbbMinusMttMinusMbb0));
	      	PMttbjMinusMttMinusMbj0_bkg = tr_bkg -> hmttbjregminusmttregminusmbj0 -> GetBinContent(tr_bkg ->hmttbjregminusmttregminusmbj0->FindBin(MttbbMinusMttMinusMbb0));
		if(PMttbbMinusMttMinusMbb0_bkg<0) PMttbbMinusMttMinusMbb0_bkg=0;
		if(PMttbjMinusMttMinusMbj0_bkg<0) PMttbjMinusMttMinusMbj0_bkg=0;

		//boosted bb in ttbb
      		TLorentzVector ttbb0=j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j5_vector+j1_vector+j2_vector;
      		CosThetabbttbb0=GetCosTheta(ttbb0, higgs);
		PCosThetabbttbb0 = tr_sig -> truthcosthetabbttbbRF0 -> GetBinContent(tr_sig->truthcosthetabbttbbRF0 ->FindBin(CosThetabbttbb0));
		PCosThetabbttbb_bb0 = tr_bkg -> truthcosthetabbttbbRF_bb0 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bb0 ->FindBin(CosThetabbttbb0));
		PCosThetabbttbb_bj0 = tr_bkg -> truthcosthetabbttbbRF_bj0 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bj0 ->FindBin(CosThetabbttbb0));
		if(PCosThetabbttbb0<0) PCosThetabbttbb0=0;
		if(PCosThetabbttbb_bb0<0) PCosThetabbttbb_bb0=0;
		if(PCosThetabbttbb_bj0<0) PCosThetabbttbb_bj0=0;		
		//



	      		  if (m_opt->Bbttbb){//PCosThetab1 and bbttbbRF
	      PermProbMPSig=PMbb*PMqb*PMlvb0*PMttbbMinusMttMinusMbb0*PMttMinusMtMinusMt0*PCosThetab1*PCosThetabbttbb0;//5jex
	      PermProbMPBkg=PMqb_bkg*PMlvb0_bkg*PMttMinusMtMinusMt0_bkg;//5jex here just Mtt, Mttbb added after
			  }
			  else{
			    
              PermProbMPSig=PMbb*PMqb*PMlvb0*PCosThetab1;//5jex
              PermProbMPBkg=PMqb_bkg*PMlvb0_bkg;//5jex		
			  }
	    }     
	    if (NuSolutions.size() == 2){


		//masses
		//Mtt
		Mtt1=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j5_vector).M()/1000.;
		Mtt2=(j6_vector+tlv_lep+NuSolutions.at(1)+j3_vector+j5_vector).M()/1000.;
		MttMinusMtMinusMt1= Mtt1 - Mqb - Mlvb1;
		MttMinusMtMinusMt2= Mtt2 - Mqb - Mlvb2;
		
		// std::cout << "Mtt1= " <<Mtt1 <<" Mqb= " << Mqb <<" Mlvb1= "<< Mlvb1<< std::endl;
		// std::cout << "Mtt2= " <<Mtt2 <<" Mqb= " << Mqb <<" Mlvb2= "<< Mlvb2<< std::endl;

		//Mttbb
		Mttbb1=(j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j5_vector+j1_vector+j2_vector).M()/1000.;
		Mttbb2=(j6_vector+tlv_lep+NuSolutions.at(1)+j3_vector+j5_vector+j1_vector+j2_vector).M()/1000.;
		MttbbMinusMttMinusMbb1 = Mttbb1 - Mtt1 - Mbb;
		MttbbMinusMttMinusMbb2 = Mttbb2 - Mtt2 - Mbb;

		//Probabilities
		//sig PMtt
		PMttMinusMtMinusMt1=tr_sig -> hmttregminusmtregminusmt1  -> GetBinContent(tr_sig -> hmttregminusmtregminusmt1 ->FindBin(MttMinusMtMinusMt1));
		PMttMinusMtMinusMt2=tr_sig -> hmttregminusmtregminusmt2  -> GetBinContent(tr_sig -> hmttregminusmtregminusmt2 ->FindBin(MttMinusMtMinusMt2));
		if(PMttMinusMtMinusMt1<0) PMttMinusMtMinusMt1=0;
		if(PMttMinusMtMinusMt2<0) PMttMinusMtMinusMt2=0;

		// std::cout << "MttMinusMtMinusMt1= " <<MttMinusMtMinusMt1 << std::endl;
		// std::cout << "MttMinusMtMinusMt2= " <<MttMinusMtMinusMt2 << std::endl;
		// std::cout << "PMttMinusMtMinusMt1_5matched= " <<PMttMinusMtMinusMt1_5matched << std::endl;

		//bkg
		PMttMinusMtMinusMt1_bkg=tr_bkg -> hmttregminusmtregminusmt1  -> GetBinContent(tr_bkg -> hmttregminusmtregminusmt1 ->FindBin(MttMinusMtMinusMt1));
		PMttMinusMtMinusMt2_bkg=tr_bkg -> hmttregminusmtregminusmt2  -> GetBinContent(tr_bkg -> hmttregminusmtregminusmt2 ->FindBin(MttMinusMtMinusMt2));
		if(PMttMinusMtMinusMt1_bkg<0) PMttMinusMtMinusMt1_bkg=0;
		if(PMttMinusMtMinusMt2_bkg<0) PMttMinusMtMinusMt2_bkg=0;
		
		
		//PMttbb
		//sig
		PMttbbMinusMttMinusMbb1 = tr_sig -> hmtthregminusmttregminusmh1 -> GetBinContent(tr_sig ->hmtthregminusmttregminusmh1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbbMinusMttMinusMbb2 = tr_sig -> hmtthregminusmttregminusmh2 -> GetBinContent(tr_sig ->hmtthregminusmttregminusmh2->FindBin(MttbbMinusMttMinusMbb2));
		if(PMttbbMinusMttMinusMbb1<0) PMttbbMinusMttMinusMbb1=0;
		if(PMttbbMinusMttMinusMbb2<0) PMttbbMinusMttMinusMbb2=0;
	

		
		//bkg
		PMttbbMinusMttMinusMbb1_bkg = tr_bkg -> hmttbbregminusmttregminusmbb1 -> GetBinContent(tr_bkg ->hmttbbregminusmttregminusmbb1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbbMinusMttMinusMbb2_bkg = tr_bkg -> hmttbbregminusmttregminusmbb2 -> GetBinContent(tr_bkg ->hmttbbregminusmttregminusmbb2->FindBin(MttbbMinusMttMinusMbb2));
		PMttbjMinusMttMinusMbj1_bkg = tr_bkg -> hmttbjregminusmttregminusmbj1 -> GetBinContent(tr_bkg ->hmttbjregminusmttregminusmbj1->FindBin(MttbbMinusMttMinusMbb1));
		PMttbjMinusMttMinusMbj2_bkg = tr_bkg -> hmttbjregminusmttregminusmbj2 -> GetBinContent(tr_bkg ->hmttbjregminusmttregminusmbj2->FindBin(MttbbMinusMttMinusMbb2));
		if(PMttbbMinusMttMinusMbb1_bkg<0) PMttbbMinusMttMinusMbb1_bkg=0;
		if(PMttbbMinusMttMinusMbb2_bkg<0) PMttbbMinusMttMinusMbb2_bkg=0;
		if(PMttbjMinusMttMinusMbj1_bkg<0) PMttbjMinusMttMinusMbj1_bkg=0;
		if(PMttbjMinusMttMinusMbj2_bkg<0) PMttbjMinusMttMinusMbj2_bkg=0;



		//boosted bb in ttbb
      		TLorentzVector ttbb1=j6_vector+tlv_lep+NuSolutions.at(0)+j3_vector+j5_vector+j1_vector+j2_vector;
      		CosThetabbttbb1=GetCosTheta(ttbb1, higgs);
		PCosThetabbttbb1 = tr_sig -> truthcosthetabbttbbRF1 -> GetBinContent(tr_sig->truthcosthetabbttbbRF1 ->FindBin(CosThetabbttbb1));
		PCosThetabbttbb_bb1 = tr_bkg -> truthcosthetabbttbbRF_bb1 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bb1 ->FindBin(CosThetabbttbb1));
		PCosThetabbttbb_bj1 = tr_bkg -> truthcosthetabbttbbRF_bj1 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bj1 ->FindBin(CosThetabbttbb1));
		if(PCosThetabbttbb1<0) PCosThetabbttbb1=0;
		if(PCosThetabbttbb_bb1<0) PCosThetabbttbb_bb1=0;
		if(PCosThetabbttbb_bj1<0) PCosThetabbttbb_bj1=0;


		
		TLorentzVector ttbb2=j6_vector+tlv_lep+NuSolutions.at(1)+j3_vector+j5_vector+j1_vector+j2_vector;
      		CosThetabbttbb2=GetCosTheta(ttbb2, higgs);
		PCosThetabbttbb2 = tr_sig -> truthcosthetabbttbbRF2 -> GetBinContent(tr_sig->truthcosthetabbttbbRF2 ->FindBin(CosThetabbttbb2));
		PCosThetabbttbb_bb2 = tr_bkg -> truthcosthetabbttbbRF_bb2 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bb2 ->FindBin(CosThetabbttbb2));
		PCosThetabbttbb_bj2 = tr_bkg -> truthcosthetabbttbbRF_bj2 -> GetBinContent(tr_bkg->truthcosthetabbttbbRF_bj2 ->FindBin(CosThetabbttbb2));
		if(PCosThetabbttbb2<0) PCosThetabbttbb2=0;
		if(PCosThetabbttbb_bb2<0) PCosThetabbttbb_bb2=0;
		if(PCosThetabbttbb_bj2<0) PCosThetabbttbb_bj2=0;





	      
	      // PermProbMPSig=PMbb*PMqb*(0.65*PMlvb1+0.35*PMlvb2);//5jex
	      		  if (m_opt->Bbttbb){//PCosThetab1 and bbttbbRF

		      //boosted test
	      PermProbMPSig=PMbb*PMqb*(0.65*PMttbbMinusMttMinusMbb1*PMttMinusMtMinusMt1*PMlvb1*PCosThetabbttbb1+0.35*PMlvb2*PMttbbMinusMttMinusMbb2*PMttMinusMtMinusMt2*PCosThetabbttbb2)*PCosThetab1;//5jex
	      //test
	      //PermProbMPBkg=PMqb_bkg*(0.65*PMlvb1_bkg+0.35*PMlvb2_bkg);//5jex
	      //test now only below PMlvb
	      PermProbMPBkg=PMqb_bkg;//5jex
			  }
			  else{
             
                      //boosted test
              PermProbMPSig=PMbb*PMqb*(0.65*PMlvb1+0.35*PMlvb2)*PCosThetab1;//5jex
              PermProbMPBkg=PMqb_bkg*(0.65*PMlvb1_bkg+0.35*PMlvb2_bkg);//5jex

			  }

	    }
	    // 	    std::cout<<"PermProbMPSig= " << PermProbMPSig <<std::endl;
	    // std::cout<<"PermProbMPBkg= " << PermProbMPBkg <<std::endl;

	    PermProbBtagSig=m_probB.at(j1)*m_probB.at(j2)*(tr_sig->fWlc*m_probC.at(j3)+tr_sig->fWll*m_probL.at(j3))*m_probB.at(j5)*m_probB.at(j6);////5jex

		
	
	    ProbSigNum+=PermProbBtagSig*PermProbMPSig;
	    ProbSigDen+=PermProbBtagSig;
	    //bkg//5jex4bin
	    PermProbBtagBkg_bb=(tr_bkg->fWlc_bb*m_probC.at(j3)+tr_bkg->fWll_bb*m_probL.at(j3))*m_probB.at(j5)*m_probB.at(j6);
	

	    PermProbBtagBkg_bj=(tr_bkg->fWlc_bj*m_probC.at(j3)+tr_bkg->fWll_bj*m_probL.at(j3))*m_probB.at(j5)*m_probB.at(j6);
	

	    //5jex4bin
	    //
	    // ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg);

	    // ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg);
	    if(m_opt->Bbttbb){
	    	      if (NuSolutions.size() == 1){

	    //boosted test
	    ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*PMttbbMinusMttMinusMbb0_bkg*PCosThetab1_bb*PCosThetabbttbb_bb0);

	    ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*PMttbjMinusMttMinusMbj0_bkg*PCosThetab1_bj*PCosThetabbttbb_bj0);
		      }
	      else if (NuSolutions.size() == 2){
		ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*(0.65*PMttbbMinusMttMinusMbb1_bkg*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg*PCosThetabbttbb_bb1+0.35*PMttbbMinusMttMinusMbb2_bkg*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg*PCosThetabbttbb_bb2)*PCosThetab1_bb);

	    ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*(0.65*PMttbjMinusMttMinusMbj1_bkg*PMlvb1_bkg*PMttMinusMtMinusMt1_bkg*PCosThetabbttbb_bj1+0.35*PMttbjMinusMttMinusMbj2_bkg*PMlvb2_bkg*PMttMinusMtMinusMt2_bkg*PCosThetabbttbb_bj2)*PCosThetab1_bj);
	
		
	      }
	    }
	    else{
            ProbBkgNum_bb+=(tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PMbb_bkg*PermProbBtagBkg_bb*PermProbMPBkg*PCosThetab1_bb);
 
            ProbBkgNum_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PMbj_bkg*PermProbBtagBkg_bj*PermProbMPBkg*PCosThetab1_bj);
 
	    }


	      
	    ProbBkgDen_bb+=tr_bkg->fbb*m_probB.at(j1)*m_probB.at(j2)*PermProbBtagBkg_bb;
	    ProbBkgDen_bj+=(tr_bkg->fbj*m_probB.at(j1)*(tr_bkg->flight*m_probL.at(j2)+ tr_bkg->fc*m_probC.at(j2))*PermProbBtagBkg_bj);
			      
	    ProbBkgNum=ProbBkgNum_bb+ProbBkgNum_bj;
	    ProbBkgDen=ProbBkgDen_bb+ProbBkgDen_bj;

	  }////j5    
	  //}	//j4   
	}
      }     
    }    
  }////////j1

  //  std::cout << "///// " <<  std::endl;
  // std::cout << "-----------------------------------------ProbSigNum 5jex4bin " << ProbSigNum << std::endl;
  // std::cout << "-----------------------------------------ProbSigDen 5jex4bin " << ProbSigDen << std::endl;
  // std::cout << "-----------------------------------------ProbBkgNum 5jex4bind " << ProbBkgNum << std::endl;
  // std::cout << "-----------------------------------------ProbBkgDen 5jex4bin " << ProbBkgDen << std::endl;
 
    
  if(ProbSigNum<=0)  {
    std::cout << "!!! ProbSigNum 5jex4bin= " <<  ProbSigNum << std::endl;
    m_log10ProbSig_merged = -28;
    m_log10ProbSig_6matched = -28;

      
  }

  if (ProbSigDen<=0){
    std::cout << "!!! ProbSigDen 5jex4bin = " <<  ProbSigDen << std::endl;
    ProbSig=-0.1;
    m_log10ProbSig_merged = -28;
    m_log10ProbSig_6matched = -28;
  }
  else {
    ProbSig=ProbSigNum/ProbSigDen;
    m_log10ProbSig_merged = log10(ProbSig);
    m_log10ProbSig_6matched = log10(ProbSig);
  }
  
  //bkg
  if(ProbBkgNum<=0)  {
    std::cout << "!!! ProbBkgNum 5jex4bin = " <<  ProbBkgNum << std::endl;
    m_log10ProbBkg_6matched = -28; 
    m_log10ProbBkg_merged = -28;

  }
  if (ProbBkgDen<=0) {
    std::cout << "!!! ProbBkgDen 5jex4bin = " <<  ProbBkgDen << std::endl;
    ProbBkg=-0.1;
    m_log10ProbBkg_6matched = -28;
    m_log10ProbBkg_merged = -28;

  }
  else {
    ProbBkg=ProbBkgNum/ProbBkgDen;
    m_log10ProbBkg_6matched = log10(ProbBkg);
    m_log10ProbBkg_merged = log10(ProbBkg);
  }

  if (ProbSig<0 || ProbBkg<0) Discriminant=-0.3;
  else if (ProbSig==0 && ProbBkg==0) Discriminant=-0.1;
  else Discriminant=ProbSig/(ProbSig+ProbBkg);
  //std::cout << "I am in the end of Discr caLCULATION Final Discriminant 5jex4bin: = " <<Discriminant << std::endl;
  // std::cout << "ProbSig = " <<ProbSig << std::endl;
  // std::cout << "ProbBkg = " <<ProbBkg << std::endl;

  if(Discriminant>-0.1){
    //m_Discriminant_merged=Discriminant;
    m_Discriminant_6matched=Discriminant;
    m_Discriminant_merged=Discriminant;
    //std::cout << "Reg= " <<Reg << std::endl;

    //std::cout << "Final Discriminant 5jex4bin: = " <<m_Discriminant_merged << std::endl;
  }
  
}







 std::vector <float> LikelihoodDiscriminantTool::GetBTagWeightsVarOP(const float& mv2){
  //float vop[6] = {-1., -0.7887, -0.4434,-0.0436,0.4496,1.};//mv2c20
  float vop[6] = {-1., 0.1758475, 0.645925, 0.8244273, 0.934906, 1.};//mv2c10

  float eff_vop_b[6] = {1., 0.85, 0.77, 0.7, 0.6, 0.};
  //float eff_vop_b_mod[6] = {0.7, 0.7, 0.7, 0.7, 0.6, 0.};
  //float eff_vop_b_mod[6] = {0.85, 0.85, 0.77, 0.7, 0.6, 0.};
  float eff_vop_b_mod[6] = {1., 0.85, 0.77, 0.7, 0.6, 0.};

  //c:
  //60: 1/34.54=2.895194e-02
  //70: 1/12.17=8.216927e-02
  //77: 1/6.21=1.610306e-01
  //85: 1/3.10=3.225806e-01
  //float eff_vop_c[6] = {1., 0.385, 0.222, 0.125, 0.047, 0.};
  float eff_vop_c[6] = {1., 0.323, 0.161, 0.082, 0.029, 0.};
  //l:
  //60: 1/1538.78=6.498655e-04
  //70: 1/381.32=2.622469e-03 
  //77: 1/134.34=7.443799e-03
  //85: 1/33.53=2.982404e-02
  //float eff_vop_l[6] = {1., 0.0368, 0.008, 0.0025, 0.0006, 0.};
  float eff_vop_l[6] = {1., 0.0298, 0.0074, 0.0026, 0.00065, 0.};

  std::vector <float> flav_weight_prob;
  int    NVop = sizeof(vop)/sizeof(vop[0]);
  for(int j=0; j<NVop-1; j++) {
    if( vop[j] < mv2 && mv2 <= vop[j+1]){
      flav_weight_prob.push_back(eff_vop_l[j]-eff_vop_l[j+1]);
      flav_weight_prob.push_back(eff_vop_c[j]-eff_vop_c[j+1]);
      flav_weight_prob.push_back(eff_vop_b[j]-eff_vop_b[j+1]);
      flav_weight_prob.push_back(eff_vop_b_mod[j]-eff_vop_b_mod[j+1]);
    }
  }
  return flav_weight_prob;
}


//see later: decide on what to do with maps and uncomemnt this part


// std::vector <float> LikelihoodDiscriminantTool::GetBTagWeightsFromMaps(const float& pt, const float& eta, const float& mv2){//70OP mv2c10
//     std::vector <float> flav_weight_prob;
//   bool retval = true;


 
//   //std::cout << "Creating a jet" << std::endl;
//   //xAOD::JetFourMom_t p4(25000.,0.7,0.3,1000.);
 
//   //xAOD::Jet * jet = new xAOD::Jet();
//   // jet->makePrivateStore();
//   // std::cout << "Setting jet 4 momentum" << std::endl;
//   // jet->setJetP4(p4);
//   // std::cout << "Setting jet attribute" << std::endl;
//   // // light quark. Note that here and in the following, we fill both the Run-1 and Run-2 style flavour label variables
//   // jet->setAttribute("ConeTruthLabelID", 0);
//   // jet->setAttribute("HadronConeExclTruthLabelID", 0);
//     CorrectionCode result;

//   // float sfl=0;
//   // std::cout << "\nTesting function calls without systematics..." << std::endl;


//   // result = tool->getEfficiency(*jet,sfl);
//   // if( result!=CorrectionCode::Ok) { std::cout << "Light quark get efficiency failed"<<std::endl; retval=false;}
//   // else {
//   //   std::cout << "Light quark get efficiency succeeded: " << sfl << std::endl;
//   // }
//   // //b-quark
//   // jet->setAttribute("ConeTruthLabelID", 5);
//   // jet->setAttribute("HadronConeExclTruthLabelID", 5);
//   // float sfb=0;
//   // //CorrectionCode result;
//   // std::cout << "\nTesting function calls without systematics..." << std::endl;
//   // result = tool->getEfficiency(*jet,sfb);
//   // if( result!=CorrectionCode::Ok) { std::cout << "b quark get efficiency failed"<<std::endl; retval=false;}
//   // else {
//   //   std::cout << "b quark get efficiency succeeded: " << sfb << std::endl;
//   // }
//   // //c-quark
//   // jet->setAttribute("ConeTruthLabelID", 4);
//   // jet->setAttribute("HadronConeExclTruthLabelID", 4);
//   // float sfc=0;
//   // //CorrectionCode result;
//   // std::cout << "\nTesting function calls without systematics..." << std::endl;
//   // result = tool->getEfficiency(*jet,sfc);
//   // if( result!=CorrectionCode::Ok) { std::cout << "c quark get efficiency failed"<<std::endl; retval=false;}
//   // else {
//   //   std::cout << "c quark get efficiency succeeded: " << sfc << std::endl;
//   // }
//   //  std::cout << "Test CDV" << std::endl;

//       Analysis::CalibrationDataVariables *j = new Analysis::CalibrationDataVariables();
//     // j->jetPt=90000.;
//     // j->jetEta=0.3;
//     // j->jetTagWeight=0.9;
//     j->jetPt=pt;
//     j->jetEta=eta;
//     j->jetTagWeight=mv2;
//    // std::cout << "// " << std::endl;

//      // std::cout << "j->jetPt= " << j->jetPt << std::endl;
//      // std::cout << "j->jetEta= " << j->jetEta << std::endl;
//      // std::cout << "j->jetTagWeight= " << j->jetTagWeight << std::endl;

//     float eff_l=0;
//     result = tool->getEfficiency(0, *j,eff_l);
//   // if( result!=CorrectionCode::Ok) { std::cout << "CDV l quark get efficiency failed"<<std::endl; retval=false;}
//   // else {
//   // std::cout << "CDV l quark get efficiency succeeded: " << eff_l << std::endl;
//   // }
//   //std::cout << "eff_l = " << eff_l << std::endl;

//     float eff_c=0;
//     result = tool->getEfficiency(4, *j,eff_c);
//     //if( result!=CorrectionCode::Ok) { std::cout << "CDV c quark get efficiency failed"<<std::endl; retval=false;}
//   // else {
//   // std::cout << "CDV c quark get efficiency succeeded: " << eff_c << std::endl;
//   // }
//   // std::cout<<"eff_c = " <<eff_c<<std::endl;

//       float eff_b=0;

//     result = tool->getEfficiency(5, *j,eff_b);
//     //if( result!=CorrectionCode::Ok) { std::cout << "CDV b quark get efficiency failed"<<std::endl; retval=false;}
  
//   // else {
//   // std::cout << "CDV b quark get efficiency succeeded: " << eff_b << std::endl;
//   // }
//   // std::cout<<"eff_b = " <<eff_b<<std::endl;
//     if(m_opt->VarOP()){
//       flav_weight_prob.push_back(eff_l);
//       flav_weight_prob.push_back(eff_c);
//       flav_weight_prob.push_back(eff_b);
//       flav_weight_prob.push_back(eff_b);
//     }//VarOP
//   else{
//       if(mv2>0.8244273) {
//       flav_weight_prob.push_back(eff_l);
//       flav_weight_prob.push_back(eff_c);
//       flav_weight_prob.push_back(eff_b);
//       flav_weight_prob.push_back(eff_b);
//       }//tagged
//   else {
//       flav_weight_prob.push_back(1-eff_l);
//       flav_weight_prob.push_back(1-eff_c);
//       flav_weight_prob.push_back(1-eff_b);
//       flav_weight_prob.push_back(1-eff_b);
//   }//not tagged

//   }//70OP
//       return flav_weight_prob;

//   }






std::vector<TLorentzVector> LikelihoodDiscriminantTool::NuSolution(const TLorentzVector& l, const TLorentzVector& nu ){
  std::vector<TLorentzVector> nuSolutions;
  TLorentzVector nuSolution;
     nuSolution.SetPtEtaPhiE(0., 0., 0., 0.);

  double WMass = 80385;
  double MissingPt = sqrt( nu.Px()*nu.Px()+nu.Py()*nu.Py() );///////////////
  double alpha  = 0.5*(WMass*WMass-l.M()*l.M());
  double beta   = alpha+nu.Px()*l.Px()+nu.Py()*l.Py();
  double gamma  = -(beta*beta-(l.E()*l.E()*MissingPt*MissingPt))/(l.E()*l.E()-l.Pz()*l.Pz());
  double lambda = 2.*beta*l.Pz()/(l.E()*l.E()-l.Pz()*l.Pz());
  double delta  = lambda*lambda-4.*gamma;
 
 
  if(delta < 0) {
    double L = sqrt(l.E()*l.E()-l.Pz()*l.Pz());
    double M = sqrt( nu.Px()*nu.Px()+nu.Py()*nu.Py() );
    double N = nu.Px()*l.Px()+nu.Py()*l.Py();
    double Alfa1 = -alpha*(N+M*L)/(N*N-M*M*L*L);
    double Alfa2 = -alpha*(N-M*L)/(N*N-M*M*L*L);
    double Alfa=0;
    if (Alfa1 > 0 && Alfa1 < 25) Alfa = Alfa1;
    else if (Alfa2 > 0 && Alfa2 < 25) Alfa = Alfa2;
    TLorentzVector nu2;
    nu2.SetPxPyPzE(nu.Px()*Alfa, nu.Py()*Alfa, nu.Pz()*Alfa, nu.E()*Alfa);
    double betaPrime2   = alpha+nu2.Px()*l.Px()+nu2.Py()*l.Py();
    double lambdaPrime2 = 2.*betaPrime2*l.Pz()/(l.E()*l.E()-l.Pz()*l.Pz());
    double Pz2 = lambdaPrime2/2;
    double E2 = sqrt(nu2.Px()*nu2.Px() + nu2.Py()*nu2.Py() + Pz2*Pz2);
    nuSolution.SetPxPyPzE(nu2.Px(), nu2.Py(), Pz2, E2);
    nuSolutions.push_back(nuSolution);
    //return nuSolutions;
  }
 
  else{
    double Pz1 = (lambda-sqrt(delta))/2.;
    double Pz2 = (lambda+sqrt(delta))/2.;
    double E1  = sqrt(nu.Px()*nu.Px() + nu.Py()*nu.Py() + Pz1*Pz1);
    double E2  = sqrt(nu.Px()*nu.Px() + nu.Py()*nu.Py() + Pz2*Pz2);
    if (TMath::Abs(Pz1)<TMath::Abs(Pz2)) {
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz1, E1);
      nuSolutions.push_back(nuSolution);
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz2, E2);
      nuSolutions.push_back(nuSolution);
    }
    else {
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz2, E2);
      nuSolutions.push_back(nuSolution);
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz1, E1);
      nuSolutions.push_back(nuSolution);
    }
  }
  return nuSolutions;
}

float LikelihoodDiscriminantTool::GetCosTheta(const TLorentzVector& combined, const TLorentzVector& part){
  TVector3 boost = combined.BoostVector();
  TLorentzVector part_newSys = part;
  part_newSys.Boost(-boost);
  float CosTheta = (combined.Vect()*part_newSys.Vect())/(combined.Vect().Mag()*part_newSys.Vect().Mag());
  return CosTheta;
}
