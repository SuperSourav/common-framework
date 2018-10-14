/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LikelihoodDiscriminantTool_HH
#define LikelihoodDiscriminantTool_HH

//see later
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"


#include "TTHbbMVAInterfaces/RegionInterface.h"
#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Containers.h"

using namespace TTHbb;

class LikelihoodDiscriminantTool : public RegionInterface{

public:

  LikelihoodDiscriminantTool();
  virtual ~LikelihoodDiscriminantTool();


struct Options{

  bool Run3b;
  bool Mttbb;
  bool MttbbAndMtt;
  bool Bbttbb;
  bool jetpermcut8;
  
};
struct TrainingSig{
  TH1* hmbb;  TH1* hmqq;  TH1* hmqqb; TH1* hmqqbminusmqq; TH1* hmqqbregminusmqq; 
  TH1* hmtt0; TH1* hmtt1; TH1* hmtt2;
  TH1* hmttminusmtminusmt0; TH1* hmttminusmtminusmt1; TH1* hmttminusmtminusmt2;
  TH1* hmttregminusmtregminusmt0; TH1* hmttregminusmtregminusmt1; TH1* hmttregminusmtregminusmt2;
  TH1* hmtth0; TH1* hmtth1; TH1* hmtth2;
  TH1* hmtthminusmttminusmh0; TH1* hmtthminusmttminusmh1; TH1* hmtthminusmttminusmh2;
  TH1* hmtthregminusmttregminusmh0; TH1* hmtthregminusmttregminusmh1; TH1* hmtthregminusmttregminusmh2;
  TH1* hmtthregminusmtregminusmtregminusmh0; TH1* hmtthregminusmtregminusmtregminusmh1; TH1* hmtthregminusmtregminusmtregminusmh2;
  TH1* hmlvb1;  TH1* hmlvb2;  TH1* hmlvb0; 
  float fWll; float fWlc;
  TH1* truthcosthetabhRF;
  TH1* truthcosthetabbttbbRF0; TH1* truthcosthetabbttbbRF1;  TH1* truthcosthetabbttbbRF2;  
};
struct TrainingBkg{
  TH1* hmbb;  TH1* hmbj; TH1* hmqq; TH1* hmqqb; TH1* hmqqbminusmqq; TH1* hmqqbregminusmqq;
  TH1* hmqq_bb; TH1* hmqq_bj;
  TH1* hmqqbregminusmqq_bb; TH1* hmqqbregminusmqq_bj;
  TH1* hmtt0; TH1* hmtt1; TH1* hmtt2;
  TH1* hmttminusmtminusmt0; TH1* hmttminusmtminusmt1; TH1* hmttminusmtminusmt2;
  TH1* hmttregminusmtregminusmt0; TH1* hmttregminusmtregminusmt1; TH1* hmttregminusmtregminusmt2;
  TH1* hmttbb0; TH1* hmttbb1; TH1* hmttbb2;
  TH1* hmttbbminusmttminusmbb0; TH1* hmttbbminusmttminusmbb1; TH1* hmttbbminusmttminusmbb2;
  TH1* hmttbbregminusmttregminusmbb0; TH1* hmttbbregminusmttregminusmbb1; TH1* hmttbbregminusmttregminusmbb2;
  TH1* hmttbj0; TH1* hmttbj1; TH1* hmttbj2;
  TH1* hmttbjminusmttminusmbj0; TH1* hmttbjminusmttminusmbj1; TH1* hmttbjminusmttminusmbj2;
  TH1* hmttbjregminusmttregminusmbj0; TH1* hmttbjregminusmttregminusmbj1; TH1* hmttbjregminusmttregminusmbj2;
  TH1* hmttbbregminusmtregminusmtregminusmbb0; TH1* hmttbbregminusmtregminusmtregminusmbb1; TH1* hmttbbregminusmtregminusmtregminusmbb2;
  TH1* hmttbjregminusmtregminusmtregminusmbj0; TH1* hmttbjregminusmtregminusmtregminusmbj1; TH1* hmttbjregminusmtregminusmtregminusmbj2;

  TH1* hmlvb1;  TH1* hmlvb2;  TH1* hmlvb0; float fbb; float fbj; float flight; float fc; float fWll_bb; float fWlc_bb; float fWll_bj; float fWlc_bj;
  TH1* truthcosthetabhRF_bb; TH1* truthcosthetabhRF_bj;
  TH1* truthcosthetabbttbbRF_bb0; TH1* truthcosthetabbttbbRF_bb1;  TH1* truthcosthetabbttbbRF_bb2;
  TH1* truthcosthetabbttbbRF_bj0; TH1* truthcosthetabbttbbRF_bj1;  TH1* truthcosthetabbttbbRF_bj2;
};
  //data members

  Options* m_opt;
  
  TrainingSig* m_sig_6jex4bin;
  TrainingSig* m_sig_7jin4bin;
  TrainingSig* m_sig_6jex3bex;
  TrainingSig* m_sig_7jin3bex;

  TrainingSig* m_sig_5jex4bin;


  TrainingBkg* m_bkg_6jex4bin;
  TrainingBkg* m_bkg_7jin4bin;
  TrainingBkg* m_bkg_6jex3bex;
  TrainingBkg* m_bkg_7jin3bex;

  TrainingBkg* m_bkg_5jex4bin;

  //5matched
  TrainingSig* m_sig5matched_6jex4bin;
  TrainingSig* m_sig5matched_7jin4bin;
  TrainingSig* m_sig5matched_6jex3bex;
  TrainingSig* m_sig5matched_7jin3bex;

  TrainingBkg* m_bkg5matched_6jex4bin;
  TrainingBkg* m_bkg5matched_7jin4bin;
  TrainingBkg* m_bkg5matched_6jex3bex;
  TrainingBkg* m_bkg5matched_7jin3bex;

  /* float m_ProbSigNum_6matched=-9; */
  /* float m_ProbSigDen_6matched=-9; */
  /* float m_ProbSigNum_5matched=-9; */
  /* float m_ProbSigDen_5matched=-9; */

  /* float m_ProbBkgNum_6matched=-9; */
  /* float m_ProbBkgDen_6matched=-9; */
  /* float m_ProbBkgNum_5matched=-9; */
  /* float m_ProbBkgDen_5matched=-9; */
  
  float m_log10ProbSig_6matched=-9;
  float m_log10ProbSig_5matched=-9;
  float m_log10ProbSig_merged=-9;
  
  float m_log10ProbBkg_6matched=-9;
  float m_log10ProbBkg_5matched=-9;
  float m_log10ProbBkg_merged=-9;

  float m_Discriminant_6matched=-9;
  float m_Discriminant_5matched=-9;
  float m_Discriminant_6matchedAnd5matched=-9;
  float m_Discriminant_merged=-9;

  std::vector <float> m_probB; 
  std::vector <float> m_probC; 
  std::vector <float> m_probL;

  
  //see later
  //BTaggingEfficiencyTool *tool;
  /* std::vector< AnalysisObject* > vectorAO_Jets_; */
  /* std::vector< AnalysisObject* > vectorAO_JetsMV2Ordered; */
  /* std::vector< AnalysisObject* > vectorAO_JetsOrdered; */
  /* std::vector< AnalysisObject* > vectorAO_JetsTruth; */
  /* std::vector< AnalysisObject* > vectorAO_ExtraJets; */
  /* std::vector< AnalysisObject* > vectorAO_ExtraJetsTruthb; */
  /* std::vector< AnalysisObject* > vectorAO_ExtraJetsTruthnotb; */
  /* std::vector<std::string> ExtraJetCombinations; */


  

  //functions
  
  void Init();
  void GetTrainingSig(TFile* f, const std::string& Reg, TrainingSig* tr);
  void GetTrainingSig5matched(TFile* f, const std::string& Reg, TrainingSig* tr);
  void GetTrainingSig5jex4bin(TFile* f, const std::string& Reg, TrainingSig* tr);

  void GetTrainingBkg(TFile* f, const std::string& Reg, TrainingBkg* tr);
  void GetTrainingBkg5matched(TFile* f, const std::string& Reg, TrainingBkg* tr);
  void GetTrainingBkg5jex4bin(TFile* f, const std::string& Reg, TrainingBkg* tr);
  void Clear();
  void Run(int njets, int nbjets, JetContainer* jets,  LeptonContainer* sleptons, float met_met, float met_phi);
  void GetDiscriminant(TrainingSig* tr_sig, TrainingSig* tr_sig5matched, TrainingBkg* tr_bkg, TrainingBkg* tr_bkg5matched, const std::string& Reg, JetContainer* jets, std::vector<TLorentzVector> NuSolutions, const TLorentzVector& tlv_lep);
  void GetDiscriminant5jex4bin(TrainingSig* tr_sig, TrainingBkg* tr_bkg, JetContainer* jets, std::vector<TLorentzVector> NuSolutions, const TLorentzVector& tlv_lep); 
  std::vector <float> GetBTagWeightsVarOP(const float& jetmv2c20);
  //see later
  //std::vector <float> GetBTagWeightsFromMaps(const float& pt, const float& eta, const float& mv2);


  std::vector<TLorentzVector> NuSolution(const TLorentzVector& l, const TLorentzVector& nu );
  float GetCosTheta(const TLorentzVector& combined, const TLorentzVector& part);

};















#endif
