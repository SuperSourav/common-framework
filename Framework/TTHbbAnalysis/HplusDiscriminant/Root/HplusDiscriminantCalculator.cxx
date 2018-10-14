/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HplusDiscriminant/HplusDiscriminantCalculator.h"
#include <iostream>
#include <TFile.h>
#include <TString.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include "PathResolver/PathResolver.h"

namespace TTHbb{
  
  constexpr long double HplusDiscriminantCalculator::vop[];
  constexpr long double HplusDiscriminantCalculator::eff_vop_b[];
  constexpr long double HplusDiscriminantCalculator::eff_vop_c[];
  constexpr long double HplusDiscriminantCalculator::eff_vop_l[];
  constexpr double HplusDiscriminantCalculator::WMass;
  
  HplusDiscriminantCalculator::HplusDiscriminantCalculator(std::vector<int> masspoints) : m_masspoints(masspoints){
    // hard-coded settings
    m_channels={"5jetex3btagex","5jetex4btagin","6jetin3btagex","6jetin4btagin"};
    
    //read the tempaltes

    TString TemplatesPath=PathResolverFindCalibDirectory("HplusDiscriminant/DiscriminantTemplates/");
    TString cmstring="13TeVTemplates";
    // get H+ templates
    for(unsigned int imass=0; imass<m_masspoints.size(); imass++){
      TString rootfilename=TemplatesPath+cmstring+"/templates_hp"; rootfilename+=m_masspoints[imass]; rootfilename+="_nofit.root";
      TFile *rootfile=TFile::Open(rootfilename);
      m_templates_Hplus_APM.push_back(std::vector<HplusDiscriminantTemplateHolder>());
      m_templates_Hplus_MJ.push_back(std::vector<HplusDiscriminantTemplateHolder>());
      for(unsigned int ichannel=0; ichannel!=m_channels.size(); ichannel++){
	TString plotname="h"+m_channels[ichannel]; plotname+="_APMvsMJ";
	TH1F* apmvsmjhist = (TH1F *) rootfile->Get(plotname);
	int apmbin = apmvsmjhist->GetXaxis()->FindBin("APM");
	double apmprob = apmvsmjhist->GetBinContent(apmbin);
	int mjbin = apmvsmjhist->GetXaxis()->FindBin("MJ");
	double mjprob = apmvsmjhist->GetBinContent(mjbin);
	m_templates_Hplus_APM[imass].push_back(HplusDiscriminantTemplateHolder(rootfile,m_channels[ichannel],true, m_masspoints[imass], apmprob));
	m_templates_Hplus_MJ[imass].push_back(HplusDiscriminantTemplateHolder(rootfile,m_channels[ichannel],false, m_masspoints[imass], mjprob));
      }
      rootfile->Close();
    }
    
    // get ttbar templates
    TFile *rootfile=TFile::Open(TemplatesPath+cmstring+"/templates_ttbar_nofit.root");
    for(unsigned int ichannel=0; ichannel!=m_channels.size(); ichannel++){
      TString plotname="h"+m_channels[ichannel]; plotname+="_APMvsMJ";
      TH1F* apmvsmjhist = (TH1F *) rootfile->Get(plotname);
      int apmbin = apmvsmjhist->GetXaxis()->FindBin("APM");
      double apmprob = apmvsmjhist->GetBinContent(apmbin);
      int mjbin = apmvsmjhist->GetXaxis()->FindBin("MJ");
      double mjprob = apmvsmjhist->GetBinContent(mjbin);
      m_templates_WbWb_APM.push_back(HplusDiscriminantTemplateHolder(rootfile,m_channels[ichannel],true, -1, apmprob));
      m_templates_WbWb_MJ.push_back(HplusDiscriminantTemplateHolder(rootfile,m_channels[ichannel],false, -1, mjprob));
    }
    rootfile->Close();
    
    std:: cout << "HplusDiscriminantCalculator: Relative APM and MJ template weigths"  << std::endl;
    std:: cout << "HplusDiscriminantCalculator: mass  Hplus_APM  Hplus_MJ  tt_APM  tt_MJ "  << std::endl;
    Int_t index = 0;
    for(unsigned int imass=0; imass<m_masspoints.size(); imass++){
      while (index<4) {
	std::cout <<  "HplusDiscriminantCalculator: "<<m_masspoints[imass] << " " << m_templates_Hplus_APM[imass][index].getProb_APMvsMJ() << "       " << m_templates_Hplus_MJ[imass][index].getProb_APMvsMJ() << "      " 
		  <<  m_templates_WbWb_APM[index].getProb_APMvsMJ() << "   " << m_templates_WbWb_MJ[index].getProb_APMvsMJ() << std::endl;
	++index;
      }
    }
  }

  void HplusDiscriminantCalculator::InitializeWeights(HplusDiscriminantData* data) {
    /////////////////////////////////////////
    // Initializing the flavour probabilities
    /////////////////////////////////////////
    /* from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarks#MV2c10_tagger_added_on_11th_May
       name	weight cut	b-jet efficiency [%]	purity [%]	c RR	tau RR	light RR
       FixedCutBEff_30	0.9977155	29.99	99.95	1406.40	4891.95	51290.94
       FixedCutBEff_50	0.9769329	50.05	99.62	106.41	614.79	5701.28
       FixedCutBEff_60	0.934906	60.03	99.00	34.54	183.98	1538.78
       FixedCutBEff_70	0.8244273	69.97	97.46	12.17	54.72	381.32
       FixedCutBEff_77	0.645925	76.97	95.17	6.21	22.04	134.34
       FixedCutBEff_85	0.1758475	84.95	89.66	3.10	8.17	33.53
       
    */
    
    /*
      float vop[6]       = {-1., -0.7887, -0.4434, -0.0436, 0.4496,1.};
      float eff_vop_b[6] = {1., 0.85, 0.77, 0.7, 0.6, 0.};
      float eff_vop_c[6] = {1., 0.385, 0.222, 0.125, 0.047, 0.};
      float eff_vop_l[6] = {1., 0.0368, 0.008, 0.0025, 0.0006, 0.};
    */

    /////////////////////////////////////////////////////////
    //  Initializing btag, ctag and ltag weigths for all jets
    /////////////////////////////////////////////////////////
    m_btag_weight_prob.clear();
    m_ctag_weight_prob.clear();
    m_ltag_weight_prob.clear();
    
    
    ////////////////////////////////////////////////
    // Filling the flavour probabilities of each jet
    ////////////////////////////////////////////////
    int NVop = sizeof(vop)/sizeof(vop[0]);
   
    for (int i = 0; i < data->m_njets; i++) {
      
      float btag_weight = data->m_jets_mv2c10.at(i);
      
      for (int j=0; j<NVop-1; j++) {
        if ( vop[j] < btag_weight && btag_weight <= vop[j+1] ) {
	  m_btag_weight_prob.push_back(eff_vop_b[j]-eff_vop_b[j+1]); 
	  m_ctag_weight_prob.push_back(eff_vop_c[j]-eff_vop_c[j+1]); 
	  m_ltag_weight_prob.push_back(eff_vop_l[j]-eff_vop_l[j+1]); 
        }
      }
    }
  }

  std::vector<TLorentzVector> HplusDiscriminantCalculator::NuSolution_Rescale( TLorentzVector& l, TLorentzVector& nu ){
    //this functions calculates a rescale factor for MET so that exactly one solution exists. If no such rescale factor is found, it returns no solutions

    
    std::vector<TLorentzVector> nuSolutions;
    
    std::pair<double,double> dl=Calculate_lambda_delta(l,nu);
    double lambda=dl.first;
    double delta=dl.second;
    
    if(delta < 0) {
      double L = sqrt(l.E()*l.E()-l.Pz()*l.Pz());
      double M = sqrt( nu.Px()*nu.Px()+nu.Py()*nu.Py() );
      double N = nu.Px()*l.Px()+nu.Py()*l.Py();
      double alpha  = 0.5*(WMass*WMass-l.M()*l.M());
      double Alfa1 = -alpha*(N+M*L)/(N*N-M*M*L*L);
      double Alfa2 = -alpha*(N-M*L)/(N*N-M*M*L*L);
      double Alfa = 0.;  // cllm initialized to 0 to avoid compilation warning
      if (Alfa1 > 0) Alfa = Alfa1;
      if (Alfa2 > 0) Alfa = Alfa2;
      if(Alfa1<0 && Alfa2<0) {
	std::cout << "ERROR: Alfa1<0 and Alfa2<0 (when delta<0). Returning no neutrino solutions" << std::endl;
	return nuSolutions;
      }
      TLorentzVector nu2;
      nu2.SetPxPyPzE(nu.Px()*Alfa, nu.Py()*Alfa, nu.Pz()*Alfa, nu.E()*Alfa);
      double betaPrime2   = alpha+nu2.Px()*l.Px()+nu2.Py()*l.Py();
      double lambdaPrime2 = 2.*betaPrime2*l.Pz()/(l.E()*l.E()-l.Pz()*l.Pz());
      double Pz2 = lambdaPrime2/2;
      double E2 = sqrt(nu2.Px()*nu2.Px() + nu2.Py()*nu2.Py() + Pz2*Pz2);
      TLorentzVector nuSolution;
      nuSolution.SetPxPyPzE(nu2.Px(), nu2.Py(), Pz2, E2);
      nuSolutions.push_back(nuSolution);
      return nuSolutions;
    }
    
    double Pz1 = (lambda-sqrt(delta))/2.;
    double Pz2 = (lambda+sqrt(delta))/2.;
    double E1  = sqrt(nu.Px()*nu.Px() + nu.Py()*nu.Py() + Pz1*Pz1);
    double E2  = sqrt(nu.Px()*nu.Px() + nu.Py()*nu.Py() + Pz2*Pz2);
    if (TMath::Abs(Pz1)<TMath::Abs(Pz2)) {
      TLorentzVector nuSolution;
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz1, E1);
      nuSolutions.push_back(nuSolution);
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz2, E2);
      nuSolutions.push_back(nuSolution);
    }
    else {
      TLorentzVector nuSolution;
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz2, E2);
      nuSolutions.push_back(nuSolution);
      nuSolution.SetPxPyPzE(nu.Px(), nu.Py(), Pz1, E1);
    nuSolutions.push_back(nuSolution);
    }
    
    return nuSolutions;
  }
  
  std::pair<double,double> HplusDiscriminantCalculator::Calculate_lambda_delta( TLorentzVector& lepton, TLorentzVector& neutrino ){
    
    double MissingPt = sqrt( neutrino.Px()*neutrino.Px()+neutrino.Py()*neutrino.Py() );
    double alpha  = 0.5*(WMass*WMass-lepton.M()*lepton.M());
    double beta   = alpha+neutrino.Px()*lepton.Px()+neutrino.Py()*lepton.Py();
    double gamma  = -(beta*beta-(lepton.E()*lepton.E()*MissingPt*MissingPt))/(lepton.E()*lepton.E()-lepton.Pz()*lepton.Pz());
    double lambda = 2.*beta*lepton.Pz()/(lepton.E()*lepton.E()-lepton.Pz()*lepton.Pz());
    double delta  = lambda*lambda-4.*gamma;
    return std::pair<double,double>(lambda,delta);
  }
  
  void HplusDiscriminantCalculator::CalculateDiscriminant(HplusDiscriminantData* data){
    const bool usembb_ttbar=true;
    const bool debug=false;
    const int usemaxjets=8; 

    InitializeWeights(data);
    m_NuSolutions = NuSolution_Rescale(data->m_lepton, data->m_neutrino);

    int ChannelIndex=-1;
    if (data->m_njets == 5  &&  data->m_nbjets <= 3) {
      ChannelIndex=0; //5jetex3btagex
    }
    else if (data->m_njets == 5  &&  data->m_nbjets >= 4) {
      ChannelIndex=1; //5jetex4btagin
    }
    else if (data->m_njets >= 6  &&  data->m_nbjets <= 3) {
      ChannelIndex=2; //6jetin3btagex
    }
    else if (data->m_njets >= 6  &&  data->m_nbjets >=4 ) {
      ChannelIndex=3; //6jetin4btagin
    }
    else
      std::cout << "HplusDiscriminiantCalculator: ERROR: Requested calculation of H+ discrimininant in undefined channel njets=" << data->m_njets << " nbjets=" << data->m_nbjets << std::endl;
    
    ////////////////////////////////////////////
    // Intialize the global event probabilities
    ///////////////////////////////////////////
    for(std::vector<int>::iterator mass=m_masspoints.begin(); mass!=m_masspoints.end(); mass++){
      m_HplustbprobJWP[*mass] = 0.;   // JetWeightProbability terms   
      m_HplustbprobDkin[*mass]  = 0.;   // Kinematic Terms = MP*JWP
      m_HplustbprobDkin_APM[*mass]  = 0.;   // Kinematic Terms = MP*JWP
      m_HplustbprobDkin_MJ[*mass]  = 0.;   // Kinematic Terms = MP*JWP   
    }
    m_WbWbprobJWP = 0.;
    m_WbWbprobDkin = 0.;
    m_WbWbprobDkin_APM = 0.;
    m_WbWbprobDkin_MJ = 0.;
    

    //if(useonlycorrectcombination){
    //c_trueMatch->ComputeKinematics(data->m_njets);
    //c_trueMatch->ComputeMatching(data-m_njets);
    //}

    //////////////////////////
    //
    // Main loop over jets
    //
    /////////////////////////

    int usedjets;
    if (usemaxjets == 0) 
      usedjets = data->m_njets;
    else
      usedjets = std::min(usemaxjets,data->m_njets);

    // To use within ttbar
    int nj_6jloop = 1;
    if (usembb_ttbar && data->m_njets >= 6) nj_6jloop = usedjets;

    for (int jblep = 0; jblep < usedjets; jblep++) { // jblep is the b from the leptonic top
      for (int jq1 = 0; jq1 < usedjets; jq1++) { //quark1 from W decay
	for (int jq2 = 0; jq2 < usedjets; jq2++) { //quark 2 from W decay
	  for (int jbhad = 0; jbhad < usedjets; jbhad++){ //jbhad is the b from the hadronic top
	    for (int jb2h = 0; jb2h < usedjets; jb2h++){ //jb2h is the b from the H+ decay
	      if (jblep!=jq1 && jblep!=jq2 && jblep!=jbhad && jblep!=jb2h && jq1!=jq2 && jq1!=jbhad && jq1!=jb2h && jq2!=jbhad && jq2!=jb2h && jbhad!=jb2h ){
		std::vector<int> unusedjetindices;
		for(int i=0; i<data->m_njets; i++) unusedjetindices.push_back(i);
		unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jblep), unusedjetindices.end());
		unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jq1), unusedjetindices.end());
		unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jq2), unusedjetindices.end());
		unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jbhad), unusedjetindices.end());
		unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jb2h), unusedjetindices.end());

		if(data->m_useonlycorrectcombination){
		  if(jblep != data->m_bl_JetIndex ||
		     jq1 != data->m_q1_JetIndex ||
		     jq2 != data->m_q2_JetIndex ||
		     jbhad != data->m_bh_JetIndex ||
		     jb2h != data->m_b2h_JetIndex )
		    continue;
		}

		//////////////////////////////////
		//
		//    H+ case  (Ll.M. and Julian)
		//
		///////////////////////////////// 
		std::map<int,long double> HplustbpermprobMP_APM;
		std::map<int,long double> HplustbpermprobMP_MJ;
	      
		//////////////////////////////
		// case of 1 neutrino solution
		//////////////////////////////
		if (m_NuSolutions.size() == 1) {
		  long double Mlvb = (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //leptonic top mass
		  long double Mlvbb2_Mlvb =    (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)+ data->m_jets.at(jb2h)).M() -
		    (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //H+ mass - m_tl assuming H+ decays to leptonic top 
		  long double Mq1q2bb2_Mq1q2b= (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)+ data->m_jets.at(jb2h)).M() -
		    (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)).M(); //H+ mass - m_th assuming H+ decays to hadronic top
		
		  for(unsigned int imass=0; imass<m_masspoints.size(); imass++){
		    /////////////////////////////
		    // all partons matched
		    /////////////////////////////
		    // template probability for leptonic top
		    long double ProbMlvb_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mlvb_NoSolM(Mlvb);
		    HplustbpermprobMP_APM[m_masspoints[imass]] = ProbMlvb_APM;
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		    long double ProbMlvbb2_Mlvb_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mlvbb2_Mlvb_NoSolM(Mlvbb2_Mlvb);
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top
		    long double ProbMq1q2bb2_Mq1q2b_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		  
		    HplustbpermprobMP_APM[m_masspoints[imass]] *= 
		      m_templates_Hplus_APM[imass][ChannelIndex].getProb_tlep()*ProbMlvbb2_Mlvb_APM + 
		      m_templates_Hplus_APM[imass][ChannelIndex].getProb_thad()*ProbMq1q2bb2_Mq1q2b_APM;

		    if (debug) std::cout << "DEBUG: 1 neutrino solution (mH="<< m_masspoints[imass] << ", " << jblep << ","<<
				 jq1<<","<<jq2<<","<<jbhad<<","<<jb2h<<") APM Mlvb=" << Mlvb << " -> " << ProbMlvb_APM << 
				 " Mlvbb2_Mlvb="<< Mlvbb2_Mlvb << " -> " << ProbMlvbb2_Mlvb_APM << " Mq1q2bb2_Mq1q2b=" << 
				 Mq1q2bb2_Mq1q2b << " -> " << ProbMq1q2bb2_Mq1q2b_APM << " Probtlep: "<<
				 m_templates_Hplus_APM[imass][ChannelIndex].getProb_tlep()<< " Probthad: " << 
				 m_templates_Hplus_APM[imass][ChannelIndex].getProb_thad() << std::endl;
		       
		    /////////////////////////////
		    // case of 1 missing jet
		    /////////////////////////////
		    // template probability for leptonic top
		    long double ProbMlvb_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mlvb_NoSolM(Mlvb);
		    HplustbpermprobMP_MJ[m_masspoints[imass]] = ProbMlvb_MJ;
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		    long double ProbMlvbb2_Mlvb_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mlvbb2_Mlvb_NoSolM(Mlvbb2_Mlvb);
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top 
		    long double ProbMq1q2bb2_Mq1q2b_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		  
		    HplustbpermprobMP_MJ[m_masspoints[imass]] *= m_templates_Hplus_MJ[imass][ChannelIndex].getProb_tlep()*ProbMlvbb2_Mlvb_MJ + 
		      m_templates_Hplus_MJ[imass][ChannelIndex].getProb_thad()*ProbMq1q2bb2_Mq1q2b_MJ;
		  
		    if(debug) std::cout << "DEBUG: 1 neutrino solution (mH="<<m_masspoints[imass]<<", "<<jblep<<","<<jq1<<
				","<<jq2<<","<<jbhad<<","<<jb2h<<") MJ Mlvb=" << Mlvb << " -> " << ProbMlvb_MJ << " Mlvbb2_Mlvb="<< 
				Mlvbb2_Mlvb << " -> " << ProbMlvbb2_Mlvb_MJ << " Mq1q2bb2_Mq1q2b=" << Mq1q2bb2_Mq1q2b << " -> " << 
				ProbMq1q2bb2_Mq1q2b_MJ <<" Probtlep: "<<m_templates_Hplus_MJ[imass][ChannelIndex].getProb_tlep()<< 
				" Probthad: " << m_templates_Hplus_MJ[imass][ChannelIndex].getProb_thad() << std::endl;
		  }
		}  // if one neutrino solution     
	      
		/////////////////////////////////
		//// case of 2 neutrino solutions
		//////////////////////////////////
		else if (m_NuSolutions.size() == 2) {
		  long double Mlvb_NuSol1 = (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //leptonic top mass for solution 1
		  long double Mlvb_NuSol2 = (data->m_lepton + m_NuSolutions.at(1) + data->m_jets.at(jblep)).M(); //leptonic top mass for solution 2
		  long double Mlvbb2_Mlvb_NuSol1 =      (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)+ data->m_jets.at(jb2h)).M() -
		    (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //H+ mass^2 - m_tl assuming H+ decays to leptonic top for solution 1
		  long double Mlvbb2_Mlvb_NuSol2 =      (data->m_lepton + m_NuSolutions.at(1) + data->m_jets.at(jblep)+ data->m_jets.at(jb2h)).M() -
		    (data->m_lepton + m_NuSolutions.at(1) + data->m_jets.at(jblep)).M(); //H+ mass^2 - m_tl assuming H+ decays to leptonic top for solution 2
		  long double Mq1q2bb2_Mq1q2b =         (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)+ data->m_jets.at(jb2h)).M() -
		    (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)).M(); //H+ mass^2 - m_th assuming H+ decays to hadronic top
		
		  for(unsigned int imass=0; imass<m_masspoints.size(); imass++){
		    /////////////////////////////
		    //// all partons matched
		    /////////////////////////////
		    // template probability for leptonic top
		    long double ProbMlvb_NuSol1_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mlvb_NuSol1(Mlvb_NuSol1);
		    long double ProbMlvb_NuSol2_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mlvb_NuSol2(Mlvb_NuSol2);

		    long double Sol1_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_NuSol1();
		    long double Sol2_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_NuSol2();
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		    long double ProbMlvbb2_Mlvb_NuSol1_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol1(Mlvbb2_Mlvb_NuSol1);
		    long double ProbMlvbb2_Mlvb_NuSol2_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol2(Mlvbb2_Mlvb_NuSol2);
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top 
		    long double ProbMq1q2bb2_Mq1q2b_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		  
		    long double Probtlep_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_tlep();
		    long double Probthad_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_thad();

		    HplustbpermprobMP_APM[m_masspoints[imass]] =
		      Sol1_APM * ProbMlvb_NuSol1_APM *
		      (Probtlep_APM * ProbMlvbb2_Mlvb_NuSol1_APM + Probthad_APM * ProbMq1q2bb2_Mq1q2b_APM) 
		      +
		      Sol2_APM * ProbMlvb_NuSol2_APM *
		      (Probtlep_APM * ProbMlvbb2_Mlvb_NuSol2_APM + Probthad_APM * ProbMq1q2bb2_Mq1q2b_APM);

		    /////////////////////////////
		    //// case of 1 missing jet
		    /////////////////////////////
		    // template probability for leptonic top
		    long double ProbMlvb_NuSol1_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mlvb_NuSol1(Mlvb_NuSol1);
		    long double ProbMlvb_NuSol2_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mlvb_NuSol2(Mlvb_NuSol2);
		  
		    long double Sol1_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_NuSol1();
		    long double Sol2_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_NuSol2();
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		    long double ProbMlvbb2_Mlvb_NuSol1_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol1(Mlvbb2_Mlvb_NuSol1);
		    long double ProbMlvbb2_Mlvb_NuSol2_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol2(Mlvbb2_Mlvb_NuSol2);
		  
		    // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top 
		    long double ProbMq1q2bb2_Mq1q2b_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		  
		    long double Probtlep_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_tlep();
		    long double Probthad_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_thad();

		    HplustbpermprobMP_MJ[m_masspoints[imass]] =
		      Sol1_MJ * ProbMlvb_NuSol1_MJ *
		      (Probtlep_MJ * ProbMlvbb2_Mlvb_NuSol1_MJ + Probthad_MJ * ProbMq1q2bb2_Mq1q2b_MJ) 
		      +
		      Sol2_MJ * ProbMlvb_NuSol2_MJ *
		      (Probtlep_MJ * ProbMlvbb2_Mlvb_NuSol2_MJ + Probthad_MJ * ProbMq1q2bb2_Mq1q2b_MJ);

		  }
		}  // if two neutrino solutions    
	      
		/////////////////////////////////
		// hadronic W and hadronic top
		////////////////////////////////////
		long double Mq1q2 = (data->m_jets.at(jq1)+data->m_jets.at(jq2)).M(); // hadronic W mass
		long double Mq1q2b_Mq1q2 =(data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)).M() -
		  (data->m_jets.at(jq1)+data->m_jets.at(jq2)).M(); // (m_th - m_Wh)		   

		for(unsigned int imass=0; imass<m_masspoints.size(); imass++){
		  /////////////////////////////
		  ////// all partons matched
		  ///////////////////////////////
		  // template probability for hadronic W mass
		  long double ProbMq1q2_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mq1q2(Mq1q2);
		  HplustbpermprobMP_APM[m_masspoints[imass]] *= ProbMq1q2_APM;
		
		  // template probability for (m_th^2 - m_Wh^2)
		  long double ProbMq1q2b_Mq1q2_APM = m_templates_Hplus_APM[imass][ChannelIndex].getProb_Mq1q2b_Mq1q2(Mq1q2b_Mq1q2);
		  HplustbpermprobMP_APM[m_masspoints[imass]] *= ProbMq1q2b_Mq1q2_APM;

		  /////////////////////////////
		  ////// case of 1 missing jet
		  /////////////////////////////
		  // template probability for hadronic W mass
		  long double ProbMq1q2_MJ = m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mq1q2(Mq1q2);
		  HplustbpermprobMP_MJ[m_masspoints[imass]] *= ProbMq1q2_MJ;
		
		  // template probability for (m_th^2 - m_Wh^2)
		  long double ProbMq1q2b_Mq1q2_MJ =  m_templates_Hplus_MJ[imass][ChannelIndex].getProb_Mq1q2b_Mq1q2(Mq1q2b_Mq1q2);		
		  HplustbpermprobMP_MJ[m_masspoints[imass]] *= ProbMq1q2b_Mq1q2_MJ;

		}

	      
		/////////////////////////////////
		// calculating flavour weights
		/////////////////////////////////
	      
		//JWP terms
		long double HplustbpermprobJWP = m_btag_weight_prob.at(jblep);
		HplustbpermprobJWP *= m_ltag_weight_prob.at(jq1);
		HplustbpermprobJWP *= m_ltag_weight_prob.at(jq2);
		HplustbpermprobJWP *= m_btag_weight_prob.at(jbhad);
		HplustbpermprobJWP *= m_btag_weight_prob.at(jb2h);

		// Adding light tag weights to all unused jets for events with >= 6 jets
		if (usembb_ttbar && data->m_njets >= 6) {
		  if (ChannelIndex == 2)
		    HplustbpermprobJWP *= m_ltag_weight_prob.at(unusedjetindices.at(0));    //6jetin3btagex
		  else
		    HplustbpermprobJWP *= m_btag_weight_prob.at(unusedjetindices.at(0));
		  
		  for (unsigned int ijet=1;ijet<unusedjetindices.size();ijet++) 
		    HplustbpermprobJWP *= m_ltag_weight_prob.at(unusedjetindices.at(ijet));
		}

		for(unsigned int imass=0; imass<m_masspoints.size(); imass++){
		  // Dkin terms
		  long double HplustbpermprobDkin_APM = HplustbpermprobMP_APM[m_masspoints[imass]] * HplustbpermprobJWP;
		  long double HplustbpermprobDkin_MJ = HplustbpermprobMP_MJ[m_masspoints[imass]] * HplustbpermprobJWP;
		  long double HplustbpermprobDkin = 
		    HplustbpermprobDkin_APM * m_templates_Hplus_APM[imass][ChannelIndex].getProb_APMvsMJ() + 
		    HplustbpermprobDkin_MJ * m_templates_Hplus_MJ[imass][ChannelIndex].getProb_APMvsMJ();
		
		  //////////////////////////////////
		  // Final Probabilities
		  //////////////////////////////////
		  m_HplustbprobJWP[m_masspoints[imass]]  += HplustbpermprobJWP;
		  m_HplustbprobDkin[m_masspoints[imass]] += HplustbpermprobDkin;
		  m_HplustbprobDkin_APM[m_masspoints[imass]] += HplustbpermprobDkin_APM;
		  m_HplustbprobDkin_MJ[m_masspoints[imass]] += HplustbpermprobDkin_MJ;
		}	     

	      
		//////////////////////////////////
		//
		//    ttbar case 
		//
		/////////////////////////////////      

		for (int jotherb = 0; jotherb < nj_6jloop; jotherb++){ 
		  if (nj_6jloop==1 ||(jblep!=jotherb && jq1!=jotherb && jq2!=jotherb && jbhad!=jotherb && jb2h!=jotherb)){

		    long double WbWbpermprobMP_APM=1.;
		    long double WbWbpermprobMP_MJ=1.;
		    
		    // template probability for thad and tlep
		    long double Probtlep_APM = m_templates_WbWb_APM[ChannelIndex].getProb_tlep();
		    long double Probthad_APM = m_templates_WbWb_APM[ChannelIndex].getProb_thad();

		    long double Probtlep_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_tlep();
		    long double Probthad_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_thad();

		    ////////////////////////////////
		    //// case of 1 neutrino solution
		    ////////////////////////////////
		    if (m_NuSolutions.size() == 1) {
		      long double Mlvb = (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //leptonic top mass
		      long double Mlvbb2_Mlvb = (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)+ data->m_jets.at(jb2h)).M() -
			(data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M();  //H+ mass - m_tl assuming H+ decays to leptonic top 
		      long double Mq1q2bb2_Mq1q2b = (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)+ data->m_jets.at(jb2h)).M() -
			(data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)).M(); //H+ mass - m_th assuming H+ decays to hadronic top
		      
		      ////////////////////////////////
		      // all partons matched
		      ////////////////////////////////
		      // template probability for leptonic top
		      WbWbpermprobMP_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mlvb_NoSolM(Mlvb);
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		      long double ProbMlvbb2_Mlvb_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mlvbb2_Mlvb_NoSolM(Mlvbb2_Mlvb);
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top 
		      long double ProbMq1q2bb2_Mq1q2b_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		      
		      // IR:  removing the use of the sqrt(H+ mass^2 - m_th^2) template in the background hypothesis only for the >=6j channels
		      if (!usembb_ttbar || (usembb_ttbar && data->m_njets < 6))
			WbWbpermprobMP_APM *= 
			  m_templates_WbWb_APM[ChannelIndex].getProb_tlep()*ProbMlvbb2_Mlvb_APM + 
			  m_templates_WbWb_APM[ChannelIndex].getProb_thad()*ProbMq1q2bb2_Mq1q2b_APM;
		      

		      ///////////////////////////////
		      //   case of 1 missing jet
		      ///////////////////////////////
		      // template probability for leptonic top
		      WbWbpermprobMP_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mlvb_NoSolM(Mlvb);
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		      long double ProbMlvbb2_Mlvb_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mlvbb2_Mlvb_NoSolM(Mlvbb2_Mlvb);
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top 
		      long double ProbMq1q2bb2_Mq1q2b_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		      
		      // IR:  removing the use of the sqrt(H+ mass^2 - m_th^2) template in the background hypothesis only for the >=6j channels
		      if (!usembb_ttbar || (usembb_ttbar && data->m_njets < 6))
			WbWbpermprobMP_MJ *= 
			  m_templates_WbWb_MJ[ChannelIndex].getProb_tlep()*ProbMlvbb2_Mlvb_MJ + 
			  m_templates_WbWb_MJ[ChannelIndex].getProb_thad()*ProbMq1q2bb2_Mq1q2b_MJ;

		      if (debug)
			std::cout << "JULIAN: jblep=" << jblep << " jq1=" << jq1 << " jq2=" << jq2 << " jbhad=" << jbhad 
				  << " jb2h=" << jb2h << " -> mjnosolm "<<m_templates_WbWb_MJ[ChannelIndex].getProb_tlep() 
				  <<","<<ProbMlvbb2_Mlvb_MJ <<","<< m_templates_WbWb_MJ[ChannelIndex].getProb_thad()<<","
				  <<ProbMq1q2bb2_Mq1q2b_MJ<< std::endl;
		      
		    }  // if one neutrino solution  
		    
		    //////////////////////////////////
		    //  case of 2 neutrino solutions
		    //////////////////////////////////
		    else if (m_NuSolutions.size() == 2) {
		      long double Mlvb_NuSol1 = (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //leptonic top mass for solution 1
		      long double Mlvb_NuSol2 = (data->m_lepton + m_NuSolutions.at(1) + data->m_jets.at(jblep)).M(); //leptonic top mass for solution 2
		      long double Mlvbb2_Mlvb_NuSol1 =      (data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)+ data->m_jets.at(jb2h)).M() -
			(data->m_lepton + m_NuSolutions.at(0) + data->m_jets.at(jblep)).M(); //H+ mass - m_tl assuming H+ decays to leptonic top for solution 1
		      long double Mlvbb2_Mlvb_NuSol2 =      (data->m_lepton + m_NuSolutions.at(1) + data->m_jets.at(jblep)+ data->m_jets.at(jb2h)).M() -
			(data->m_lepton + m_NuSolutions.at(1) + data->m_jets.at(jblep)).M(); //H+ mass - m_tl assuming H+ decays to leptonic top for solution 2
		      long double Mq1q2bb2_Mq1q2b =         (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)+ data->m_jets.at(jb2h)).M() -
			(data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)).M(); //H+ mass - m_th assuming H+ decays to hadronic top
		      
		      ///////////////////////////////
		      //   all partons matched
		      ///////////////////////////////
		      // template probability for leptonic top
		      long double ProbMlvb_NuSol1_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mlvb_NuSol1(Mlvb_NuSol1);
		      long double ProbMlvb_NuSol2_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mlvb_NuSol2(Mlvb_NuSol2);
		      
		      long double Sol1_APM = m_templates_WbWb_APM[ChannelIndex].getProb_NuSol1();
		      long double Sol2_APM = m_templates_WbWb_APM[ChannelIndex].getProb_NuSol2();
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		      long double ProbMlvbb2_Mlvb_NuSol1_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol1(Mlvbb2_Mlvb_NuSol1);
		      long double ProbMlvbb2_Mlvb_NuSol2_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol2(Mlvbb2_Mlvb_NuSol2);
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top 
		      long double ProbMq1q2bb2_Mq1q2b_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		      
		      // IR:  removing the use of the sqrt(H+ mass^2 - m_th^2) template in the background hypothesis only for the >=6j channels
		      if (!usembb_ttbar || (usembb_ttbar && data->m_njets < 6))
			WbWbpermprobMP_APM =
			  Sol1_APM * ProbMlvb_NuSol1_APM *
			  (Probtlep_APM * ProbMlvbb2_Mlvb_NuSol1_APM + 
			   Probthad_APM * ProbMq1q2bb2_Mq1q2b_APM)
			  + 
			  Sol2_APM * ProbMlvb_NuSol2_APM *
			  (Probtlep_APM * ProbMlvbb2_Mlvb_NuSol2_APM + 
			   Probthad_APM * ProbMq1q2bb2_Mq1q2b_APM);
		      else
			WbWbpermprobMP_APM =
			  Sol1_APM * ProbMlvb_NuSol1_APM + 
			  Sol2_APM * ProbMlvb_NuSol2_APM;

		      ///////////////////////////////
		      //    case of 1 missing jet
		      ///////////////////////////////
		      // template probability for leptonic top
		      long double ProbMlvb_NuSol1_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mlvb_NuSol1(Mlvb_NuSol1);
		      long double ProbMlvb_NuSol2_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mlvb_NuSol2(Mlvb_NuSol2);
		      
		      long double Sol1_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_NuSol1();
		      long double Sol2_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_NuSol2();

		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to leptonic top
		      long double ProbMlvbb2_Mlvb_NuSol1_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol1(Mlvbb2_Mlvb_NuSol1);
		      long double ProbMlvbb2_Mlvb_NuSol2_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mlvbb2_Mlvb_NuSol2(Mlvbb2_Mlvb_NuSol2);
		      
		      // template probability for (H+ mass^2 - m_th^2) assuming H+ decays to hadronic top		     
		      long double ProbMq1q2bb2_Mq1q2b_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mq1q2bb2_Mq1q2b(Mq1q2bb2_Mq1q2b);
		      
		      // IR:  removing the use of the sqrt(H+ mass^2 - m_th^2) template in the background hypothesis only for the >=6j channels
		      if (!usembb_ttbar || (usembb_ttbar && data->m_njets < 6))
			WbWbpermprobMP_MJ =
			  Sol1_MJ * ProbMlvb_NuSol1_MJ *
			  (Probtlep_MJ * ProbMlvbb2_Mlvb_NuSol1_MJ + 
			   Probthad_MJ * ProbMq1q2bb2_Mq1q2b_MJ)
			  + 
			  Sol2_MJ * ProbMlvb_NuSol2_MJ *
			  (Probtlep_MJ * ProbMlvbb2_Mlvb_NuSol2_MJ + 
			   Probthad_MJ * ProbMq1q2bb2_Mq1q2b_MJ);
		      else 
			WbWbpermprobMP_MJ =
			  Sol1_MJ * ProbMlvb_NuSol1_MJ + 
			  Sol2_MJ * ProbMlvb_NuSol2_MJ;

		    }    // if two neutrino solutions 
		    
		    /////////////////////////////////
		    // hadronic W, hadronic top and event variables
		    ////////////////////////////////////
		    
		    long double Mq1q2_ttbar = (data->m_jets.at(jq1)+data->m_jets.at(jq2)).M(); // hadronic W mass
		    long double Mq1q2b_Mq1q2_ttbar = (data->m_jets.at(jq1)+data->m_jets.at(jq2)+data->m_jets.at(jbhad)).M() -
		      (data->m_jets.at(jq1)+data->m_jets.at(jq2)).M(); // m_th - m_Wh

		    ///////////////////////////////
		    //   all partons matched
		    ///////////////////////////////
		    // template probability for hadronic W mass
		    long double ProbMq1q2_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mq1q2(Mq1q2_ttbar);	   
		    WbWbpermprobMP_APM *= ProbMq1q2_APM;
		    
		    // template probability for (m_th^2 - m_Wh^2)
		    long double ProbMq1q2b_Mq1q2_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mq1q2b_Mq1q2(Mq1q2b_Mq1q2_ttbar);
		    WbWbpermprobMP_APM *= ProbMq1q2b_Mq1q2_APM;
		    
		    ///////////////////////////////
		    //   case of 1 missing jet
		    ///////////////////////////////
		    // template probability for hadronic W mass
		    long double ProbMq1q2_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mq1q2(Mq1q2_ttbar);
		    WbWbpermprobMP_MJ *= ProbMq1q2_MJ;
		    
		    // template probability for (m_th^2 - m_Wh^2)
		    long double ProbMq1q2b_Mq1q2_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mq1q2b_Mq1q2(Mq1q2b_Mq1q2_ttbar);
		    WbWbpermprobMP_MJ *= ProbMq1q2b_Mq1q2_MJ;
		    
		    // IR:  adding the use of mbb only in the background hypothesis and only in the >=6jets channels
		    if (usembb_ttbar && data->m_njets >= 6) {
		      
		      // Calculation of mbb
		      long double mbb=(data->m_jets.at(jb2h) + data->m_jets.at(jotherb)).M();
		      
		      ///////////////////////////////
		      //   all partons matched
		      ///////////////////////////////
		      
		      // template probability for (mbb)
		      long double ProbMbb_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mb2hbotherb(mbb);
		      long double ProbMjj_APM = m_templates_WbWb_APM[ChannelIndex].getProb_Mj2hj(mbb);
		      if (ChannelIndex == 2)
			WbWbpermprobMP_APM *= ProbMjj_APM;
		      else
			WbWbpermprobMP_APM *= ProbMbb_APM;
		      
		      ///////////////////////////////
		      //   case of 1 missing jet
		      ///////////////////////////////
		      
		      // template probability for (mbb)
		      long double ProbMbb_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mb2hbotherb(mbb);
		      long double ProbMjj_MJ = m_templates_WbWb_MJ[ChannelIndex].getProb_Mj2hj(mbb);
		      if (ChannelIndex == 2)
			WbWbpermprobMP_MJ *= ProbMjj_MJ;
		      else
			WbWbpermprobMP_MJ *= ProbMbb_MJ;
		    }		      


		    /////////////////////////////////
		    //  calculating flavour weights
		    /////////////////////////////////
		    
		    //JWP terms
		    long double WbWbpermprobJWP = m_btag_weight_prob.at(jblep);
		    WbWbpermprobJWP *= m_ltag_weight_prob.at(jq1);
		    WbWbpermprobJWP *= m_ltag_weight_prob.at(jq2);
		    WbWbpermprobJWP *= m_btag_weight_prob.at(jbhad);
		    
		    if (usembb_ttbar && data->m_njets >= 6) {
		      
		      if (ChannelIndex == 2) { 
			WbWbpermprobJWP *= m_ltag_weight_prob.at(jb2h);
			WbWbpermprobJWP *= m_ltag_weight_prob.at(jotherb);		      
		      }
		      else {
			WbWbpermprobJWP *= m_btag_weight_prob.at(jb2h);
			WbWbpermprobJWP *= m_btag_weight_prob.at(jotherb);		      
		      }
		    }
		    else 
		      WbWbpermprobJWP *= m_ltag_weight_prob.at(jb2h);

		    
		    if (data->m_njets >= 6) {
		      std::vector<int> unusedjetindices;
		      for(int i=0; i<data->m_njets; i++) unusedjetindices.push_back(i);
		      unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jblep), unusedjetindices.end());
		      unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jq1), unusedjetindices.end());
		      unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jq2), unusedjetindices.end());
		      unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jbhad), unusedjetindices.end());
		      unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jb2h), unusedjetindices.end());
		      unusedjetindices.erase( std::remove (unusedjetindices.begin(), unusedjetindices.end(), jotherb), unusedjetindices.end());
		      
		      for (unsigned int ijet=0;ijet<unusedjetindices.size();ijet++)  
			WbWbpermprobJWP *= m_ltag_weight_prob.at(unusedjetindices.at(ijet));
		    }

		    // Dkin terms
		    long double WbWbpermprobDkin_APM = WbWbpermprobMP_APM * WbWbpermprobJWP;
		    long double WbWbpermprobDkin_MJ = WbWbpermprobMP_MJ * WbWbpermprobJWP;
		    long double WbWbpermprobDkin = 
		      WbWbpermprobDkin_APM * m_templates_WbWb_APM[ChannelIndex].getProb_APMvsMJ() + 
		      WbWbpermprobDkin_MJ * m_templates_WbWb_MJ[ChannelIndex].getProb_APMvsMJ();
		    
		    ///////////////////////////////////
		    //// Final Probabilities
		    //// //////////////////////////////
		    m_WbWbprobJWP  += WbWbpermprobJWP;
		    m_WbWbprobDkin += WbWbpermprobDkin;
		    
		    m_WbWbprobDkin_MJ += WbWbpermprobDkin_MJ;
		    m_WbWbprobDkin_APM += WbWbpermprobDkin_APM;  

		  } // no indices overlap
		} // loop over the 6th jet
	      } // no indices overlap	
	    }  // Loop over jet 5
	  }  // Loop over jet 4
	}  // Loop over jet 3
      }  // Loop over jet 2
    }  // Loop over jet 1

    if (debug) {
      int mass=400;
      std::cout << "JULIAN: " << data->m_eventNumber << " -> " << data->m_njets << " " 
		<< (m_HplustbprobDkin[mass] / m_HplustbprobJWP[mass]) / (m_HplustbprobDkin[mass] / m_HplustbprobJWP[mass] + m_WbWbprobDkin / m_WbWbprobJWP) 
		<< " h+dkinAPM="<<m_HplustbprobDkin_APM[mass] <<" h+dkinMJ="<<m_HplustbprobDkin_MJ[mass] 
		<< " h+jwp="<< m_HplustbprobJWP[mass] << " ttdkinAPM=" << m_WbWbprobDkin_APM << " ttdkinMJ=" 
		<< m_WbWbprobDkin_MJ<< " ttjwp="<< m_WbWbprobJWP << std::endl;
    }
  }
      
  double HplusDiscriminantCalculator::GetDiscriminant(int mass){ //mass in GeV
    if( m_HplustbprobDkin.find(mass)!=m_HplustbprobDkin.end() && m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      if(m_HplustbprobDkin[mass]<=0 && m_WbWbprobDkin<=0)
	return 0;
      return (m_HplustbprobDkin[mass] / m_HplustbprobJWP[mass]) / (m_HplustbprobDkin[mass] / m_HplustbprobJWP[mass] + m_WbWbprobDkin / m_WbWbprobJWP);
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: Discriminant for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }
  
  double HplusDiscriminantCalculator::GetDiscriminantAPM(int mass){ //mass in GeV
    if( m_HplustbprobDkin_APM.find(mass)!=m_HplustbprobDkin_APM.end() && m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      if(m_HplustbprobDkin_APM[mass]<=0 && m_WbWbprobDkin<=0)
	return 0;
      return (m_HplustbprobDkin_APM[mass] / m_HplustbprobJWP[mass]) / (m_HplustbprobDkin_APM[mass] / m_HplustbprobJWP[mass] + m_WbWbprobDkin_APM / m_WbWbprobJWP);
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: Discriminant (APM) for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }
  
  double HplusDiscriminantCalculator::GetDiscriminantMJ(int mass){ //mass in GeV
    if( m_HplustbprobDkin_MJ.find(mass)!=m_HplustbprobDkin_MJ.end() && m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      if(m_HplustbprobDkin_MJ[mass]<=0 && m_WbWbprobDkin<=0)
	return 0;
      return (m_HplustbprobDkin_MJ[mass] / m_HplustbprobJWP[mass]) / (m_HplustbprobDkin_MJ[mass] / m_HplustbprobJWP[mass] + m_WbWbprobDkin_MJ / m_WbWbprobJWP);
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: Discriminant (MJ) for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }
  
  double HplusDiscriminantCalculator::GetLikelihood(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobDkin/m_WbWbprobJWP;
    if( m_HplustbprobDkin.find(mass)!=m_HplustbprobDkin.end() && m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      return m_HplustbprobDkin[mass]/m_HplustbprobJWP[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: Likelihood for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }

  double HplusDiscriminantCalculator::GetLikelihoodAPM(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobDkin_APM/m_WbWbprobJWP;
    if( m_HplustbprobDkin_APM.find(mass)!=m_HplustbprobDkin_APM.end() && m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      return m_HplustbprobDkin_APM[mass]/m_HplustbprobJWP[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: Likelihood (APM) for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }

  double HplusDiscriminantCalculator::GetLikelihoodMJ(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobDkin_MJ/m_WbWbprobJWP;
    if( m_HplustbprobDkin_MJ.find(mass)!=m_HplustbprobDkin_MJ.end() && m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      return m_HplustbprobDkin_MJ[mass]/m_HplustbprobJWP[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: Likelihood (MJ) for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }

  double HplusDiscriminantCalculator::GetLikelihoodDkin(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobDkin;
    if( m_HplustbprobDkin.find(mass)!=m_HplustbprobDkin.end() ){
      return m_HplustbprobDkin[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: LikelihoodDkin for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }

  double HplusDiscriminantCalculator::GetLikelihoodDkinAPM(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobDkin_APM;
    if( m_HplustbprobDkin_APM.find(mass)!=m_HplustbprobDkin_APM.end() ){
      return m_HplustbprobDkin_APM[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: LikelihoodDkin (APM) for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }

  double HplusDiscriminantCalculator::GetLikelihoodDkinMJ(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobDkin_MJ;
    if( m_HplustbprobDkin_MJ.find(mass)!=m_HplustbprobDkin_MJ.end() ){
      return m_HplustbprobDkin_MJ[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: LikelihoodDkin (MJ) for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }

  double HplusDiscriminantCalculator::GetLikelihoodJWP(int mass){ //mass in GeV
    if(mass<0)
      return m_WbWbprobJWP;
    if( m_HplustbprobJWP.find(mass)!=m_HplustbprobJWP.end() ){
      return m_HplustbprobJWP[mass];
    }
    else{
      std::cout << "HplusDiscriminant.cxx: ERROR: LikelihoodJWP for mass " << mass << " GeV requested, but not calculated!" << std::endl;
      return -9999999.;
    }
  }
  
}
