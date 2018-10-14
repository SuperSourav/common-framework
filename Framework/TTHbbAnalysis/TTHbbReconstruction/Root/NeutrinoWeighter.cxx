#include "TTHbbReconstruction/NeutrinoWeighter.h"
#include <iostream>
#include <ctime>
#include "TObject.h"
#include "TRandom3.h"
#include "TLorentzVector.h"
#include <iostream>
#include <string>
// #include <ctime>

void NeutrinoWeighter::Reset(){

  m_top   = TLorentzVector();
  m_tbar  = TLorentzVector();
  m_ttbar = TLorentzVector();
  m_nu    = TLorentzVector();
  m_nubar = TLorentzVector();
  m_b     = TLorentzVector();
  m_bbar  = TLorentzVector();
  m_weight_max = -1.;

}

NeutrinoWeighter::~NeutrinoWeighter(){
 ///...
}

void NeutrinoWeighter::RecalculateEtas(double pos_lep_eta, double neg_lep_eta){

  m_nu_eta.clear();
  m_nu_sinh.clear();
  m_nu_cosh.clear();

  m_nubar_eta.clear();
  m_nubar_sinh.clear();
  m_nubar_cosh.clear();

  UInt_t seed = 105200;
  m_random.SetSeed(seed);

  // for (int i = 0; i < 200; ++i){
  for (int i = 0; i < 50; ++i){
    double mean_nu = pos_lep_eta*0.7 - 0.1;
    double eta_nu = m_random.Gaus(mean_nu, 1.14);

    m_nu_eta.push_back(eta_nu);
    m_nu_sinh.push_back(sinh(eta_nu));
    m_nu_cosh.push_back(cosh(eta_nu));

    double mean_nubar = neg_lep_eta*0.7 - 0.1;
    double eta_nubar = m_random.Gaus(mean_nubar, 1.14);

    m_nubar_eta.push_back(eta_nubar);
    m_nubar_sinh.push_back(sinh(eta_nubar));
    m_nubar_cosh.push_back(cosh(eta_nubar));

  }
}

NeutrinoWeighter::NeutrinoWeighter( int setting, int event_number ){
  // std::cout << "Initialising Class" << std::endl;
  m_top   = TLorentzVector();
  m_tbar  = TLorentzVector();
  m_ttbar = TLorentzVector();
  m_b     = TLorentzVector();
  m_bbar  = TLorentzVector();
  m_nu    = TLorentzVector();
  m_nubar = TLorentzVector();
  m_random.SetSeed(event_number);
  m_weight_max = -1.;
  m_do_both_pairings = false;
  do_jetsmearing = false;


  m_include_x               = (setting & 1<<0)  ==  1<<0;  /// Use MET ex in weight function
  m_include_y               = (setting & 1<<1)  ==  1<<1;  /// Use MET ey in weight function
  m_include_t               = (setting & 1<<2)  ==  1<<2;  /// Use MET et in weight function
  m_include_phi             = (setting & 1<<3)  ==  1<<3;  /// Use MET phi in weight function
  m_include_mWp             = (setting & 1<<4)  ==  1<<4;  /// Use WMass+ smearing in weight function
  m_include_mWn             = (setting & 1<<5)  ==  1<<5;  /// Use WMass- smearing in weight function
  m_include_mtop            = (setting & 1<<6)  ==  1<<6;  /// Use MTop smearing in weight function
  m_include_mtbar           = (setting & 1<<7)  ==  1<<7;  /// Use MTbar smearing in weight function
  m_include_b_jvc           = (setting & 1<<8)  ==  1<<8;  /// Use b_jvc in weight function
  m_include_bbar_jvc        = (setting & 1<<9)  ==  1<<9;  /// Use bbar_jvc in weight function
  m_do_crystalball_smearing = (setting & 1<<10) ==  1<<10; /// Use crystal ball smearing instead of Gaussian
  m_scale_top_mass          = (setting & 1<<11) ==  1<<11; /// Scale the top mass to the scanned top mass
  m_scale_b_mass            = (setting & 1<<12) ==  1<<12; /// Scale the b mass to the scanned b mass

  ///-- Determine eta sampling --///
  for (double eta = -5.0; eta < 5.0001; eta += 0.2) {

    double sinh_eta = sinh(eta);
    double cosh_eta = cosh(eta);

    //std::cout << eta << " " << sinh_eta << " " << cosh_eta << std::endl;
    m_nu_eta.push_back(eta);
    m_nu_sinh.push_back(sinh_eta);
    m_nu_cosh.push_back(cosh_eta);

    m_nubar_eta.push_back(eta);
    m_nubar_sinh.push_back(sinh_eta);
    m_nubar_cosh.push_back(cosh_eta);
  }

  ///-- Do Top Mass Smearing --///
  //m_top_smears.push_back(170.5*1000);

  // m_top_smears.push_back(171.0*1000);
  // m_top_smears.push_back(171.5*1000);
  // m_top_smears.push_back(172.0*1000);
  m_top_smears.push_back(172.5*1000);
  // m_top_smears.push_back(173.0*1000);
  // m_top_smears.push_back(173.5*1000);
  // m_top_smears.push_back(174.0*1000);

  //m_top_smears.push_back(174.5*1000);

  //m_W_smears.push_back(80.3*1000);
  //m_W_smears.push_back(80.35*1000);
  m_W_smears.push_back(80.4*1000);
  //m_W_smears.push_back(80.45*1000);
  //m_W_smears.push_back(80.5*1000);

  ///-- Crystal Ball Setup --///

  ///-- This may cause some TH1 TKey errors, ingore them --///
  pt_25_50   = TH1F(Form("pt_25_50_%i",event_number)   , "pt_25_50",   20, -1, 1);
  pt_50_100  = TH1F(Form("pt_50_100_%i",event_number)  , "pt_50_100",  20, -1, 1);
  pt_100_200 = TH1F(Form("pt_100_200_%i",event_number) , "pt_100_200", 20, -1, 1);
  pt_200_inf = TH1F(Form("pt_200_inf_%i",event_number) , "pt_200_inf", 20, -1, 1);

  double pt_25_50_hist[20] = {
    0.0,              0.0,              0.0,              0.0,              0.0,
    0.0132411709055,  0.0581445209682,  0.125602453947,   0.203042730689,   0.230073928833,
    0.180299475789,   0.102910652757,   0.0455551184714,  0.0192785970867,  0.0100680924952,
    0.00444230996072, 0.00283004296944, 0.00197245413437, 0.00149220449384, 0.00104625837412
  };
  double pt_50_100_hist[20] = {
    0.0,              0.0,               0.000255721766734, 0.00337821920402,  0.0152019867674,
    0.0348387286067,  0.0619654245675,   0.107907861471,    0.186138540506,    0.262713760138,
    0.209348648787,   0.0825039222836,   0.0214066039771,   0.00712656229734,  0.00303501379676,
    0.00165546196513, 0.000982509925961, 0.000672952039167, 0.000491254962981, 0.000376853131456
  };
  double pt_100_200_hist[20] = {
    0.0,             0.17548828106e-05, 0.00141117942985,  0.00605132849887,  0.0151283219457,
    0.0292999111116, 0.0495826266706,   0.0811188966036,   0.159810096025,    0.322442531586,
    0.26414167881,   0.058121457696,    0.00797675177455,  0.00210480997339,  0.00117199646775,
    0.0006457939744, 0.000478365895106, 0.000227223805268, 0.000155468922458, 0.97957368882e-05
  };
  double pt_200_inf_hist[20] = {
    0.0,               0.00011296881712,  0.00192046992015, 0.00700406683609, 0.0172842293978,
    0.0262087658048,   0.0450745597482,   0.0707184821367,  0.126412108541,   0.36217802763,
    0.308065980673,    0.0283551737666,   0.00350203341804, 0.00135562580545, 0.00124265707564,
    0.000225937634241, 0.00011296881712,  0.00011296881712, 0.0,              0.00011296881712
  };

  for (int i=0; i < 20; i++){
    pt_25_50.SetBinContent(   i+1, pt_25_50_hist[i]);
    pt_50_100.SetBinContent(  i+1, pt_50_100_hist[i]);
    pt_100_200.SetBinContent( i+1, pt_100_200_hist[i]);
    pt_200_inf.SetBinContent( i+1, pt_200_inf_hist[i]);
  }

}

double NeutrinoWeighter::Reconstruct(TLorentzVector lepton_pos, TLorentzVector lepton_neg, TLorentzVector jet_1, TLorentzVector jet_2, double met_ex, double met_ey, double met_phi, double b_jvc, double bbar_jvc){

  //m_mt2 = mt2(jet_1, jet_2, leptons[0], leptons[1], m_met_et, 30);
  double width_1, width_2;
  if (jet_1.Pt() < 40.0*1000.) width_1 = 0.13;
  else if (jet_1.Pt() < 50.0*1000) width_1 = 0.14;
  else if (jet_1.Pt() < 60.0*1000) width_1 = 0.12;
  else if (jet_1.Pt() < 70.0*1000) width_1 = 0.11;
  else if (jet_1.Pt() < 80.0*1000) width_1 = 0.09;
  else if (jet_1.Pt() < 150.0*1000) width_1 = 0.08;
  else width_1 = 0.06;

  if (jet_2.Pt() < 40.0*1000) width_2 = 0.13;
  else if (jet_2.Pt() < 50.0*1000) width_2 = 0.14;
  else if (jet_2.Pt() < 60.0*1000) width_2 = 0.12;
  else if (jet_2.Pt() < 70.0*1000) width_2 = 0.11;
  else if (jet_2.Pt() < 80.0*1000) width_2 = 0.09;
  else if (jet_2.Pt() < 150.0*1000) width_2 = 0.08;
  else width_2 = 0.06;

  for(size_t mtop_counter = 0; mtop_counter < m_top_smears.size(); ++mtop_counter){
    for(size_t mtbar_counter = 0; mtbar_counter < m_top_smears.size(); ++mtbar_counter){
      for(size_t mWp_counter = 0; mWp_counter < m_W_smears.size(); ++mWp_counter){
	for(size_t mWn_counter = 0; mWn_counter < m_W_smears.size(); ++mWn_counter){

	  double met_ex_original = met_ex;
	  double met_ey_original = met_ey;

	  calculateWeight(lepton_pos, lepton_neg, jet_1, jet_2, met_ex, met_ey, met_phi, m_top_smears[mtop_counter], m_top_smears[mtbar_counter], m_W_smears[mWp_counter], m_W_smears[mWn_counter], b_jvc, bbar_jvc, true);
	  if( m_do_both_pairings)
	    calculateWeight(lepton_pos, lepton_neg, jet_2, jet_1, met_ex, met_ey, met_phi, m_top_smears[mtop_counter], m_top_smears[mtbar_counter], m_W_smears[mWp_counter], m_W_smears[mWn_counter], bbar_jvc, b_jvc, true);

	  ///-- Jet Smearing --///
	  //for (int smears = 0; smears < 15; ++smears){
    if (do_jetsmearing){
      for (int smears = 0; smears < 5; ++smears){
        TLorentzVector jet_1_smeared, jet_2_smeared;

        jet_1_smeared = jet_1;
        jet_2_smeared = jet_2;

        double scale_1 = 1.;
        double scale_2 = 2.;
        if(m_do_crystalball_smearing){
          scale_1 = GetCrystalBallWeight(jet_1.Pt());
          scale_2 = GetCrystalBallWeight(jet_2.Pt());
        } else {
          scale_1 = m_random.Gaus(jet_1.Pt(), width_1*jet_1.Pt())/jet_1.Pt();
          scale_2 = m_random.Gaus(jet_2.Pt(), width_2*jet_2.Pt())/jet_2.Pt();
        }

        jet_1_smeared = jet_1_smeared*scale_1;
        jet_2_smeared = jet_2_smeared*scale_2;

        met_ex = met_ex_original - jet_1.Px() + jet_1_smeared.Px() - jet_2.Px() + jet_2_smeared.Px();
        met_ey = met_ey_original - jet_1.Py() + jet_1_smeared.Py() - jet_2.Py() + jet_2_smeared.Py();

        if (jet_1_smeared.M() > 0.0 && jet_2_smeared.M() > 0.0){
          calculateWeight(lepton_pos, lepton_neg, jet_1_smeared, jet_2_smeared, met_ex, met_ey, met_phi, m_top_smears[mtop_counter], m_top_smears[mtbar_counter], m_W_smears[mWp_counter], m_W_smears[mWn_counter], b_jvc, bbar_jvc,false);
          if(m_do_both_pairings)
      calculateWeight(lepton_pos, lepton_neg, jet_2_smeared, jet_1_smeared, met_ex, met_ey, met_phi, m_top_smears[mtop_counter], m_top_smears[mtbar_counter], m_W_smears[mWp_counter], m_W_smears[mWp_counter], bbar_jvc, b_jvc,false);
        }

        met_ex = met_ex_original;
        met_ey = met_ey_original;
      }
    } // END OF JET SMEARING
	}// END OF Wn MASS SMEARING
      }// END OF Wp MASS SMEARING
    }// END OF TBAR MASS SMEARING
  }// END OF TOP MASS SMEARING
  return m_weight_max;
}

float NeutrinoWeighter::GetCrystalBallWeight(float jet_pt) {

  double scale = 1.;

  if (jet_pt > 25.*1000 && jet_pt < 50.*1000){
    scale = 1 + pt_25_50.GetRandom();
  }
  if (jet_pt > 50.*1000 && jet_pt < 100.*1000){
    scale = 1 + pt_50_100.GetRandom();
  }
  if (jet_pt > 100.*1000 && jet_pt < 200.*1000){
    scale = 1 + pt_100_200.GetRandom();
  }
  if (jet_pt > 200*1000){
    scale = 1 + pt_200_inf.GetRandom();
  }

  return scale;
}

void NeutrinoWeighter::calculateWeight(TLorentzVector lepton_pos, TLorentzVector lepton_neg, TLorentzVector b1, TLorentzVector b2, double met_ex, double met_ey, double met_phi, double mtop, double mtbar, double mWp, double mWn, double b_jvc, double bbar_jvc, bool print) {

  double weight = -1.;
  TLorentzVector top, tbar, ttbar, Wp, Wn;
  if(m_scale_b_mass){
    b1  = b1*(4.5*1000/b1.M());
    b2  = b2*(4.5*1000/b2.M());}
  // if(m_scale_l_mass){
  //   lepton_pos  = lepton_pos*(0/lepton_pos.M());
  //   lepton_neg  = lepton_neg*(0/lepton_neg.M());}

  //std::cout <<"Calc Weight " <<  mtop << " " << mtbar << " " << mWp << " " << mWn << std::endl;
  bool first =false;
  for (unsigned int nu_eta_index = 0; nu_eta_index < m_nu_eta.size(); ++nu_eta_index){

    //std::cout << "nu eta index " << nu_eta_index << std::endl;

    std::vector<TLorentzVector> solution_1 = solveForNeutrinoEta(&lepton_pos, &b1, nu_eta_index, 1, mtop, mWp);
    if(solution_1.size() == 0){
      continue; // if no solutions in sol1 then continue before calculating sol2;
    }

    for (unsigned int nubar_eta_index = 0; nubar_eta_index < m_nubar_eta.size(); ++nubar_eta_index){

      //std::cout << "nubar eta index " << nubar_eta_index << std::endl;

      std::vector<TLorentzVector> solution_2 = solveForNeutrinoEta(&lepton_neg, &b2, nubar_eta_index, -1, mtbar, mWn);
      if (solution_2.size() == 0){

	continue; // sol2 has no solutions, continue;
      }


      //// SOLUTION 0, 0 ////
      //weight = neutrino_weight(solution_1.at(0), solution_2.at(0), met_ex, met_ey, met_phi);
      weight = neutrino_weight(solution_1.at(0), solution_2.at(0), met_ex, met_ey, met_phi, lepton_pos, lepton_neg, b1, b2, mtop, mtbar, mWp, mWn, b_jvc, bbar_jvc,first && print );
      //std::cout << "weight 0,0" << weight << std::endl;
      first = false;
      if(weight > m_weight_max  && weight > 0.0){

        top   = (lepton_pos + b1 + solution_1.at(0));
	tbar  = (lepton_neg + b2 + solution_2.at(0));

	///-- Forces tops to have the mass we scanned with --///
  //std::cout << mtop << "   " << top.M() << "    " <<  mtbar << "    " <<tbar.M()<< std::endl;
  if(m_scale_top_mass){
    top  = top*(mtop/top.M());
    tbar = tbar*(mtbar/tbar.M());}

	ttbar = top + tbar;

	///-- No point saving non-physical solutons, even if they have high weights --///
        if (ttbar.M() > 300.*1000 && top.E() > 0. && tbar.E() > 0.){
	  m_weight_max = weight;
	  m_top        = top;
	  m_tbar       = tbar;
	  m_ttbar      = ttbar;
	  m_b          = b1;
	  m_bbar       = b2;
	  m_nu         = solution_1.at(0);
	  m_nubar      = solution_2.at(0);
	}
      }

      //// SOLUTION 1, 0 ////
      if (solution_1.size() > 1){

	//weight = neutrino_weight(solution_1.at(1), solution_2.at(0), met_ex, met_ey, met_phi);
	weight = neutrino_weight(solution_1.at(1), solution_2.at(0), met_ex, met_ey, met_phi, lepton_pos, lepton_neg, b1, b2, mtop, mtbar, mWp, mWn, b_jvc, bbar_jvc,first);

	if(weight > m_weight_max && weight > 0.0){
	  top   = (lepton_pos + b1 + solution_1.at(1));
	  tbar  = (lepton_neg + b2 + solution_2.at(0));

	  ///-- Forces tops to have the mass we scanned with --///
	  // top  = top*(mtop/top.M());
	  // tbar = tbar*(mtbar/tbar.M());

	  ttbar = top + tbar;

	  ///-- No point saving non-physical solutons, even if they have high weights --///
 	  if (ttbar.M() > 300.*1000 && top.E() > 0 && tbar.E() > 0){
	    m_weight_max = weight;
	    m_top        = top;
	    m_tbar       = tbar;
	    m_ttbar      = ttbar;
	    m_b          = b1;
	    m_bbar       = b2;
	    m_nu         = solution_1.at(1);
	    m_nubar      = solution_2.at(0);
	  }
	}
      }

      //// SOLUTION 0, 1 ////
      if (solution_2.size() > 1){

	//weight = neutrino_weight(solution_1.at(0), solution_2.at(1), met_ex, met_ey, met_phi);
	weight = neutrino_weight(solution_1.at(0), solution_2.at(1), met_ex, met_ey, met_phi, lepton_pos, lepton_neg, b1, b2, mtop, mtbar, mWp, mWn, b_jvc, bbar_jvc, first);

	if(weight > m_weight_max && weight > 0.0){
	  top   = (lepton_pos + b1 + solution_1.at(0));
	  tbar  = (lepton_neg + b2 + solution_2.at(1));

	  ///-- Forces tops to have the mass we scanned with --///
	  // top  = top*(mtop/top.M());
	  // tbar = tbar*(mtbar/tbar.M());

	  ttbar = top + tbar;

	  ///-- No point saving non-physical solutons, even if they have high weights --///
	  if (ttbar.M() > 300.*1000 && top.E() > 0. && tbar.E() > 0.){
	    m_weight_max = weight;
	    m_top        = top;
	    m_tbar       = tbar;
	    m_ttbar      = ttbar;
	    m_b          = b1;
	    m_bbar       = b2;
	    m_nu         = solution_1.at(0);
	    m_nubar      = solution_2.at(1);
	  }
	}
      }

      //// SOLUTION 1, 1 ////
      if (solution_1.size() > 1 && solution_2.size() > 1){

	//weight = neutrino_weight(solution_1.at(1), solution_2.at(1), met_ex, met_ey, met_phi);
	weight = neutrino_weight(solution_1.at(1), solution_2.at(1), met_ex, met_ey, met_phi, lepton_pos, lepton_neg, b1, b2, mtop, mtbar, mWp, mWn, b_jvc, bbar_jvc, first);

	if(weight > m_weight_max && weight > 0.000001){
	  top   = (lepton_pos + b1 + solution_1.at(1));
	  tbar  = (lepton_neg + b2 + solution_2.at(1));

	  ///-- Forces tops to have the mass we scanned with --///
	  // top  = top*(mtop/top.M());
	  // tbar = tbar*(mtbar/tbar.M());

	  ttbar = top + tbar;

	  ///-- No point saving non-physical solutons, even if they have high weights --///
	  if (ttbar.M() > 300.*1000 && top.E() > 0 && tbar.E() > 0){
	    m_weight_max = weight;
	    m_top        = top;
	    m_tbar       = tbar;
	    m_ttbar      = ttbar;
	    m_b          = b1;
	    m_bbar       = b2;
	    m_nu         = solution_1.at(1);
	    m_nubar      = solution_2.at(1);
	  }
	}
      }

    first = false;
    }//End of nubar eta index
  }// End of nu eta index


  return;
}

//double NeutrinoWeighter::neutrino_weight(TLorentzVector neutrino1, TLorentzVector neutrino2, double met_ex, double met_ey, double met_phi){
double NeutrinoWeighter::neutrino_weight(TLorentzVector neutrino1, TLorentzVector neutrino2, double met_ex, double met_ey, double met_phi, TLorentzVector lep_pos, TLorentzVector lep_neg, TLorentzVector b1, TLorentzVector b2, double mtop, double mtbar, double mWp, double mWn, double b_jvc, double bbar_jvc, bool print){


  double met_et = sqrt(pow(met_ex, 2) + pow(met_ey, 2));
  double pi = TMath::Pi();
  double dx = met_ex - neutrino1.Px() - neutrino2.Px();
  double dy = met_ey - neutrino1.Py() - neutrino2.Py();
  double dt = met_et - neutrino1.Pt() - neutrino2.Pt();

  // std::cout <<"MET_t: " << met_et <<"  neutrino1_PT: " << neutrino1.Pt()<< "    neutrino2_PT: " << neutrino2.Pt()<< "     Diff: " << dt <<  std::endl;
  double dphi = 1.;
  if(met_phi > -99.){
    TLorentzVector nunubar = neutrino1 + neutrino2;
    dphi = met_phi - nunubar.Phi();
    while (dphi >= pi) dphi -= 2 * pi;
    while (dphi < -pi) dphi += 2 * pi;
  }
  TLorentzVector Wp   = neutrino1 + lep_pos;
  TLorentzVector Wn   = neutrino2 + lep_neg;
  TLorentzVector top  = neutrino1 + lep_pos + b1;
  TLorentzVector tbar = neutrino2 + lep_neg + b2;

  double dmWp = mWp - Wp.M();
  double dmWn = mWn - Wn.M();
  double dmtop = 172.5*1000 - top.M();
  double dmtbar = 172.5*1000 - tbar.M();
  if(print){
  std::cout <<"mtop: " << mtop <<"mtop teconstructed : " << top.M()<< "\t\t\tdelta mtop: " << dmtop<< std::endl;
  std::cout <<"mtbar: " << mtbar <<"mtbar " << tbar.M()<< "dmtbar: " << dmtbar<< std::endl;
  }
  //double m_sigma_met_ex = m_met_sumet*0.023 + 6.5;
  //double m_sigma_met_ey = m_met_sumet*0.023 + 6.5;
  double m_sigma_met_ex  = 0.2*met_ex;
  double m_sigma_met_ey  = 0.2*met_ey;
  double m_sigma_met_phi = 0.05;//0.15
  double m_sigma_met_et  = 0.2*met_et;
  if(met_et < 5000)
    m_sigma_met_et = 1000;

  double m_sigma_mWp     = 0.015*1000;
  double m_sigma_mWn     = 0.015*1000;
  double m_sigma_mtop    = 1.5*1000;
  double m_sigma_mtbar   = 1.5*1000;

  double numerator_x = -dx*dx;
  double numerator_y = -dy*dy;
  double numerator_t = -dt*dt;
  double numerator_phi = 1.;
  if(met_phi > -99.)
    numerator_phi = -dphi*dphi;
  double numerator_mWp   = -dmWp*dmWp;
  double numerator_mWn   = -dmWn*dmWn;
  double numerator_mtop  = -dmtop*dmtop;
  double numerator_mtbar = -dmtbar*dmtbar;

  double denominator_x = 2.*m_sigma_met_ex*m_sigma_met_ex;
  double denominator_y = 2.*m_sigma_met_ey*m_sigma_met_ey;
  double denominator_t = 2.*m_sigma_met_et*m_sigma_met_et;
  double denominator_phi = 1.;
  if(met_phi > -99.)
    denominator_phi = 2.*m_sigma_met_phi*m_sigma_met_phi;
  double denominator_mWp   = 2.*m_sigma_mWp*m_sigma_mWp;
  double denominator_mWn   = 2.*m_sigma_mWn*m_sigma_mWn;
  double denominator_mtop  = 2.*m_sigma_mtop*m_sigma_mtop;
  double denominator_mtbar = 2.*m_sigma_mtbar*m_sigma_mtbar;


  double exp_x = exp(numerator_x/denominator_x);
  double exp_y = exp(numerator_y/denominator_y);
  double exp_t = exp(numerator_t/denominator_t);
  double exp_phi = 1.;
  if(met_phi > -99.)
    exp_phi = exp(numerator_phi/denominator_phi);
  double exp_mWp   = exp(numerator_mWp/denominator_mWp);
  double exp_mWn   = exp(numerator_mWn/denominator_mWn);
  double exp_mtop  = exp(numerator_mtop/denominator_mtop);
  double exp_mtbar = exp(numerator_mtbar/denominator_mtbar);



  double m_sigma_b_jvc  = 0.82;
  double m_sigma_bbar_jvc  = 0.84;
  double dbc = -0.28 - b_jvc;
  if (dbc>0) dbc = 0;
  double numerator_bc = -dbc*dbc;
  double denominator_bc = 2.*m_sigma_b_jvc*m_sigma_b_jvc;
  double exp_bc = exp(numerator_bc/denominator_bc);

  double dbc_veto = 0.30 - b_jvc;
  if (dbc_veto<0) dbc = 0;
  double numerator_bc_veto = -dbc_veto*dbc_veto;
  double denominator_bc_veto = 2.*m_sigma_bbar_jvc*m_sigma_bbar_jvc;
  double exp_bc_veto = exp(numerator_bc_veto/denominator_bc_veto);

  double dbbarc = 0.30 - bbar_jvc;
  if (dbc<0) dbbarc = 0;
  double numerator_bbarc = -dbbarc*dbbarc;
  double denominator_bbarc = 2.*m_sigma_bbar_jvc*m_sigma_bbar_jvc;
  double exp_bbarc = exp(numerator_bbarc/denominator_bbarc);

  double dbbarc_veto = -0.28 - bbar_jvc;
  if (dbbarc_veto>0) dbbarc = 0;
  double numerator_bbarc_veto = -dbbarc_veto*dbbarc_veto;
  double denominator_bbarc_veto = 2.*m_sigma_b_jvc*m_sigma_b_jvc;
  double exp_bbarc_veto = exp(numerator_bbarc_veto/denominator_bbarc_veto);


  double factor_b_jvc_1    = exp_bc ;
  double factor_bbar_jvc_1 = exp_bbarc ;
  double factor_b_jvc_2    = 1 - exp_bc_veto;
  double factor_bbar_jvc_2 = 1 - exp_bbarc_veto;
  double factor_b_jvc    = (factor_b_jvc_1 + factor_b_jvc_2) / 2.0;
  double factor_bbar_jvc    = (factor_bbar_jvc_1 + factor_bbar_jvc_2) / 2.0;

  double exp_all = 1;
  if(m_include_x){
    exp_all *= exp_x;
  }
  if(m_include_y){
    exp_all *= exp_y;
  }
  if(m_include_t){
    exp_all *= exp_t;
  }
  if(m_include_phi){
    exp_all *= exp_phi;
  }
  if(m_include_mWp){
    exp_all *= exp_mWp;
  }
  if(m_include_mWn){
    exp_all *= exp_mWn;
  }
  if(m_include_mtop){
    exp_all *= exp_mtop;
  }
  if(m_include_mtbar){
    exp_all *= exp_mtbar;
  }
  if(m_include_b_jvc){
    exp_all *= factor_b_jvc;
  }
  if(m_include_bbar_jvc){
    exp_all *= factor_bbar_jvc;
  }
  // if (exp_all > 0.1){
  //   std::cout << "Weight: " << exp_all << " Different factors in the weight: ";
  //   std::cout << exp_x <<  " * ";
  //   std::cout << exp_y <<" * ";
  //   std::cout << exp_phi <<" * ";
  //   std::cout << exp_mWp <<" * ";
  //   std::cout << exp_mWn <<" * ";
  //   std::cout << exp_mtop <<" * ";
  //   std::cout << exp_mtbar <<" * ";
  //   std::cout << factor_b_jvc <<" * ";
  //   std::cout << factor_bbar_jvc ;
  //   std::cout << std::endl;
  //   std::cout << "b_jvc: " << b_jvc<< " bbar_jvc: " << bbar_jvc << std::endl;
  // std::cout << "JVC stuff:" <<std::endl;
  // std::cout << "  b_jvc: " << b_jvc << ", factor (parts): " <<factor_b_jvc << " (" <<factor_b_jvc_1<< ", " << factor_b_jvc_2<< ")"<<std::endl;
  // std::cout << "  bbar_jvc: " << bbar_jvc << ", factor (parts): " <<factor_bbar_jvc << " (" <<(factor_bbar_jvc_1)<< ", " << (factor_bbar_jvc_2)<< ")"<<std::endl;
  // }
  return exp_all;

}

std::vector<TLorentzVector> NeutrinoWeighter::solveForNeutrinoEta(TLorentzVector* lepton, TLorentzVector* bJet, int index, int index_type, double mtop, double mW) {

  double nu_cosh = -99.;
  double nu_sinh = -99.;

  if (index_type > 0){
    nu_cosh = m_nu_cosh.at(index);
    nu_sinh = m_nu_sinh.at(index);
  } else {
    nu_cosh = m_nubar_cosh.at(index);
    nu_sinh = m_nubar_sinh.at(index);
  }

  //double Wmass2 = 80.4*80.4;
  double Wmass2 = mW*mW;
  // double bmass = 4.5;
  double bmass = bJet->M();
  double Elprime = lepton->E() * nu_cosh - lepton->Pz() * nu_sinh;
  double Ebprime = bJet->E()   * nu_cosh - bJet->Pz()   * nu_sinh;

  double A = (lepton->Py() * Ebprime - bJet->Py() * Elprime) / (bJet->Px() * Elprime - lepton->Px() * Ebprime);
  double B = (Elprime * (mtop * mtop - Wmass2 - bmass * bmass - 2. * lepton->Dot(*bJet)) - Ebprime * Wmass2) / (2. * (lepton->Px() * Ebprime - bJet->Px() * Elprime));

  // std::cout << Elprime<< ", " << Elprime<< ", " << mtop<< ", " << Wmass2<< ", " << bmass<< ", " << lepton->Dot(*bJet)<< ", " << lepton->Px() << ", " << bJet->Px()<<std::endl;
  double par1 = (lepton->Px() * A + lepton->Py()) / Elprime;
  double C = A * A + 1. - par1 * par1;
  double par2 = (Wmass2 / 2. + lepton->Px() * B) / Elprime;
  double D = 2. * (A * B - par2 * par1);
  double F = B * B - par2 * par2;
  double det = D * D - 4. * C * F;


  std::vector<TLorentzVector> sol;

  ///-- 0 solutions case --///
  if (det < 0.0){
    return sol;
  }

  ///-- Only one real solution case --///
  if (det == 0.) {
    double py1 = -D / (2. * C);
    double px1 = A * py1 + B;
    double pT2_1 = px1 * px1 + py1 * py1;
    double pz1 = sqrt(pT2_1) * nu_sinh;

    TLorentzVector a1(px1, py1, pz1, sqrt(pT2_1 + pz1 * pz1));

    if (!TMath::IsNaN(a1.E()) )
      sol.push_back(a1);
    return sol;
  }

  ///-- 2 solutions case --///
  if(det > 0){
    double tmp   = sqrt(det) / (2. * C);
    double py1   = -D / (2. * C) + tmp;
    double py2   = -D / (2. * C) - tmp;
    double px1   = A * py1 + B;
    double px2   = A * py2 + B;
    double pT2_1 = px1 * px1 + py1 * py1;
    double pT2_2 = px2 * px2 + py2 * py2;
    double pz1   = sqrt(pT2_1) * nu_sinh;
    double pz2   = sqrt(pT2_2) * nu_sinh;
    TLorentzVector a1(px1, py1, pz1, sqrt(pT2_1 + pz1 * pz1));
    TLorentzVector a2(px2, py2, pz2, sqrt(pT2_2 + pz2 * pz2));

    if (!TMath::IsNaN(a1.E()) && !TMath::IsNaN(a2.E())){
      sol.push_back(a1);
      sol.push_back(a2);
    }
    return sol;
  }

  ///-- Should never reach this point --///
  return sol;
}



/*double TopDileptonEventSaver::mt2(TLorentzVector jet_1, TLorentzVector jet_2, TLorentzVector lep_1, TLorentzVector lep_2, double met, int n_points){

  double mt2_result = 999999.;#

  TLorentzVector visible_1 = jet_1 + lep_1;
  TLorentzVector visible_2 = jet_2 + lep_2;

  double energy_1 = sqrt(pow(visible_1.M(), 2) + pow(visible_1.Pt(), 2));
  double energy_2 = sqrt(pow(visible_2.M(), 2) + pow(visible_2.Pt(), 2));

  for( int i = 0; i < n_points; ++i){

    double prob = (1./n_points)*i;
    double daughter_1 = prob*met;
    double daughter_2 = (1-prob)*met;

    double result = 99999999;
    double result_1 = sqrt( pow(visible_1.M(), 2) + 2*(energy_1*fabs(daughter_1) - (visible_1.Pt()*daughter_1)));
    double result_2 = sqrt( pow(visible_1.M(), 2) + 2*(energy_1*fabs(daughter_1) - (visible_1.Pt()*daughter_1)));

    if (result_1 > result_2) {
      result = result_1;
    } else {
      result = result_2;
    }

    if (result < mt2_result){
      mt2_result = result;
    }

  } /// End of probablility loop

  return mt2_result;

  }*/



