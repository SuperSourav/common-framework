/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/OfflineCPVariables.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

#include "MVAVariables/JetOrderingTool.h"

#include <iostream>
#include <fstream>

namespace TTHbb{

  OfflineCPVariables::OfflineCPVariables(std::string name)
    : ToolBase(true)
  {
    m_name = name;
  }

  OfflineCPVariables::~OfflineCPVariables(){
  }

  void OfflineCPVariables::initialise(){
    auto* config = TTHbb::GlobalConfiguration::get();
    recobdts = TTHbb::util::vectoriseString((*config)(m_name + ".RecoBDTs"));
    varFile = (*config)(m_name + ".Variables");
    isdilepton = TTHbb::util::iequals((*config)(m_name + ".channel"),"dilepton");
  }

  void OfflineCPVariables::apply(TTHbb::Event* event){
    for(auto recobdt :recobdts){
      /// Add the provided list of reco variables, if file is provided
      if(varFile.size() != 0){
	TLorentzVector lbH, slbH, higgs;
	/// The convention is that the top1 is the leptonic top for l+jets and the particle for dilepton
	TLorentzVector top1, btop1, Wtop1, ltop1, nutop1;
	/// The convention is that the top1 is the hadroic top for l+jets and the anti-particle for dilepton
	TLorentzVector top2, btop2, Wtop2, ltop2, nutop2, lqtop2, slqtop2;
	/// Assign the different jets to the objestcs choosen by the recoBDT
	for (unsigned int ijet=0; ijet<event->m_jets.size(); ijet++){
	  /// Decorating bjets from higgs (same for l+jets and dilepton)
	  if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==1)	    lbH     = event->m_jets[ijet]->p4();
	  else if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==2)     slbH    = event->m_jets[ijet]->p4();
	  else if(isdilepton){
	    /// Decorating bjets from tops for dilepton
	    if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==8)        btop1   = event->m_jets[ijet]->p4();
	    else if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==9)   btop2   = event->m_jets[ijet]->p4();
	  } else{
	    /// Decorating bjets from tops and jets from hadronic W for l+jets
	    if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==3)        btop1   = event->m_jets[ijet]->p4();
	    else if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==4)   btop2   = event->m_jets[ijet]->p4();
	    else if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==5)   lqtop2  = event->m_jets[ijet]->p4();
	    else if (event->m_jets[ijet]->intVariable(recobdt + "_best_Cand")==6)   slqtop2 = event->m_jets[ijet]->p4();
	  }
	}
	/// Defining higgs
	higgs = lbH + slbH;

	///Different top reconstruction solutions for dilepton and l+jets
	if(isdilepton){
	  /// Dilepton: decorate the top quarks from the neutrinoweighting solutions
	  if(event->m_leptons[0]->floatVariable("charge") > 0){
	    ltop1=event->m_leptons[0]->p4();
	    ltop2=event->m_leptons[1]->p4();
	  } else {
	    ltop1=event->m_leptons[1]->p4();
	    ltop2=event->m_leptons[0]->p4();
	  }
	  if(event->floatVariable(recobdt + "_best_m_top_pX") != -9){
	    top1.SetPxPyPzE(
		event->floatVariable(recobdt + "_best_m_top_pX"),
		event->floatVariable(recobdt + "_best_m_top_pY"),
		event->floatVariable(recobdt + "_best_m_top_pZ"),
		event->floatVariable(recobdt + "_best_m_top_E"));
	    top2.SetPxPyPzE(
		event->floatVariable(recobdt + "_best_m_tbar_pX"),
		event->floatVariable(recobdt + "_best_m_tbar_pY"),
		event->floatVariable(recobdt + "_best_m_tbar_pZ"),
		event->floatVariable(recobdt + "_best_m_tbar_E"));
	  } else {
	    top1.SetPxPyPzE(-99, -99, -99, -99);
	    top2.SetPxPyPzE(-99, -99, -99, -99);
	  }
	  Wtop1 = top1 - btop1;
	  Wtop2 = top2 - btop2;
	  nutop1 = Wtop1 - ltop1;
	  nutop2 = Wtop2 - ltop2;
	} else {
	  /// l+jets: decorate the top quarks from jets and neutrino and lepton
	  nutop1.SetPtEtaPhiM(
	      event->floatVariable(recobdt + "_best_nu_pt"),
	      event->floatVariable(recobdt + "_best_nu_eta"),
	      event->floatVariable(recobdt + "_best_nu_phi"),
	      0);
	  ltop1 = event->m_leptons[0]->p4();
	  Wtop1 = nutop1 + ltop1;
	  Wtop2 = lqtop2 + slqtop2;
	  top1 = btop1 + Wtop1;
	  top2 = btop2 + Wtop2;
	}

	/// Those lines set the ltop2 equal to lqtop2 and nutop2 equal to slqtop2
	/// Which makes it possible to use the same reco variables config for both channels
	if(isdilepton){
	  lqtop2  = ltop2;
	  slqtop2 = nutop2;
	} else{
	  ltop2   = lqtop2;
	  nutop2  = slqtop2;
	}

	std::string line;
	std::ifstream infile;

	/// Open file with list of reco variables
	infile.open(varFile);

	std::vector<std::string> p;
	std::vector<TLorentzVector> v(4);
	/// Loop through all lines in the file
	while(std::getline(infile,line)){
	  /// # will mark the beginning of a comment
	  size_t hash = line.find("#");
	  if (hash != std::string::npos){
	    line = line.substr(0, hash);
	  }
	  if(line.empty()) continue;
	  /// remove whitespace from the beginning and end of line
	  const auto strBgn = line.find_first_not_of(" \t");
	  const auto strEnd = line.find_last_not_of(" \t");
	  line = line.substr(strBgn, strEnd+1);
	  /// check line isn't empty!
	  if(line.empty()) continue;


	  /// Transform reco variable input to vector
	  p = TTHbb::util::vectoriseString(line, " ");
	  /// loop over parrticles in input vector and define set the correcct reco particles j
	  for(unsigned int i=1; i<p.size(); i++){
	    if(p[i]=="b1H")        v[i-1] = lbH;
	    else if(p[i]=="b2H")   v[i-1] = slbH;
	    else if(p[i]=="H")     v[i-1] = higgs;

	    else if(p[i]=="t1")    v[i-1] = top1;
	    else if(p[i]=="bt1")   v[i-1] = btop1;
	    else if(p[i]=="Wt1")   v[i-1] = Wtop1;
	    else if(p[i]=="lt1")   v[i-1] = ltop1;
	    else if(p[i]=="nut1")  v[i-1] = nutop1;

	    else if(p[i]=="t2")    v[i-1] = top2;
	    else if(p[i]=="bt2")   v[i-1] = btop2;
	    else if(p[i]=="Wt2")   v[i-1] = Wtop2;
	    else if(p[i]=="lt2")   v[i-1] = ltop2;
	    else if(p[i]=="nut2")  v[i-1] = nutop2;
	    else if(p[i]=="q1t2")  v[i-1] = lqtop2;
	    else if(p[i]=="q2t2")  v[i-1] = slqtop2;
	  }
	  /// Check if variable to calculate is specified
	  if(p.size()){
	    /// Initialise name of variable in ntuple
	    /// and if the variable is cos also initialise second name for sin
	    std::string var_name = "";
	    std::string var_name_sin = "";
	    float reco_value = -5;
	    if(p[0]=="1_wrt_123"){
	      reco_value   = cos_theta_1_wrt_123(v[0], v[1], v[2]);
	      var_name     = "_best_m_cos_" + p[1] + "_wrt_" + p[1][0] + p[2][0] +p[3][0];
	      var_name_sin = "_best_m_sin_" + p[1] + "_wrt_" + p[1][0] + p[2][0] +p[3][0];
	    } else if(p[0]=="3_wrt_23"){
	      reco_value   = cos_theta_3_wrt_23(v[0], v[1], v[2]);
	      var_name     = "_best_m_cos_" + p[3] + "_wrt_" + p[2] +p[3];
	      var_name_sin = "_best_m_sin_" + p[3] + "_wrt_" + p[2] +p[3];
	    } else if(p[0]=="4_wrt_3"){
	      reco_value   = cos_theta_4_wrt_3(v[0], v[1], v[2], v[3]);
	      var_name     = "_best_m_cos_" + p[4] + "_wrt_" + p[3];
	      var_name_sin = "_best_m_sin_" + p[4] + "_wrt_" + p[3];
	    } else if(p[0]=="4_wrt_3_seq"){
	      reco_value   = cos_theta_4_wrt_3seq(v[0], v[1], v[2], v[3]);
	      var_name     = "_best_m_cos_" + p[4] + "_wrt_" + p[3] + "_seq";
	      var_name_sin = "_best_m_sin_" + p[4] + "_wrt_" + p[3] + "_seq";
	    } else if(p[0]=="b4"){
	      reco_value   = b4(v[0], v[1]);
	      var_name     = "_best_m_b4_" + p[1] + "_" + p[2];
	    }
	    /// Write Variable to event
	    event->floatVariable(recobdt + var_name) = reco_value;
	    if(var_name_sin!=""){
	      if(reco_value == -5) event->floatVariable(recobdt + var_name_sin) = -5.;
	      else                 event->floatVariable(recobdt + var_name_sin) = pow((1. - reco_value * reco_value), 0.5);
	    }
	  }
	}
	infile.close();
      }
    }
  }

  float OfflineCPVariables::b4(TLorentzVector v1, TLorentzVector v2){
    // ----------------------------------------------------------------------------------------------------------------
    // Define Function to Calculate b4 variable
    // ----------------------------------------------------------------------------------------------------------------

    // Check if input is sensible
    if(v1.Px()==0 && v1.Py()==0 && v1.Pz()==0) return -5;
    if(v2.Px()==0 && v2.Py()==0 && v2.Pz()==0) return -5;
    if(v1.Px()==-99 && v1.Py()==-99 && v1.Pz()==-99) return -5;
    if(v2.Px()==-99 && v2.Py()==-99 && v2.Pz()==-99) return -5;

    float b4 = (v1.Pz()*v2.Pz())/(v1.P()*v2.P());
    // If we have a nan return default value -5
    if (isnan(b4)){
      return -5.;
    } else {
      return b4;
    }
  }

  float OfflineCPVariables::cos_theta_1_wrt_123(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3){
    // ----------------------------------------------------------------------------------------------------------------
    // Define Function to Calculate Angle of v1 in ttH centre-of-mass
    // Inputs: v1, v2, v3 TLorentzVector(s) for objects in LAB system
    // Execution:
    //   I] Evaluate Ang 1
    //      1. Create S' = (v1+v2+v3) center-of-mass system;
    //      2. Boost v1 object to S' and creat v1'
    //      3. Calculate Ang1 = the angle of v1'(in S') with respect to direction of (v1+v2+v3) system in LAB
    // ----------------------------------------------------------------------------------------------------------------

    // Check if input is sensible
    if(v1.Px()==0 && v1.Py()==0 && v1.Pz()==0) return -5;
    if(v2.Px()==0 && v2.Py()==0 && v2.Pz()==0) return -5;
    if(v3.Px()==0 && v3.Py()==0 && v3.Pz()==0) return -5;
    if(v1.Px()==-99 && v1.Py()==-99 && v1.Pz()==-99) return -5;
    if(v2.Px()==-99 && v2.Py()==-99 && v2.Pz()==-99) return -5;
    if(v3.Px()==-99 && v3.Py()==-99 && v3.Pz()==-99) return -5;

    //   I] Evaluate Ang 1
    TLorentzVector     cm123 = v1 + v2 + v3;                // centre-of-mass system
    TVector3    boost_vec123 = -(cm123).BoostVector();      // define boost vector
    TLorentzVector  v1_cm123;                               // 4-vec boosted to (1+2+3)
    //__v1______
    v1_cm123 = v1;
    v1_cm123.Boost(boost_vec123);
    // Calculate cos_theta
    float cos_theta = v1_cm123.Vect().Unit().Dot(cm123.Vect().Unit());
    // If we have a nan return default value -5
    if (isnan(cos_theta)){
      return -5.;
    } else {
      return cos_theta;
    }
  }

  float OfflineCPVariables::cos_theta_3_wrt_23(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3){
    // ----------------------------------------------------------------------------------------------------------------
    // Define Function to Calculate Angle of 2 in (2,3) centre-of-mass
    // Inputs:   v1, v2, v3 TLorentzVector(s) for objects in LAB system
    // Execution:
    //   I] Evaluate Ang 1
    //      1. Create S' = (v1+v2+v3) center-of-mass system
    //      2. Boost v2,v3 objects to S' and create v2',v3'
    //      3. Create S'' = (v2'+v3') centre-of-mass system
    //      4. Boost v3' to S'' and create v3''
    //      5. Calculate Ang1 = the angle of v3''(in S'') with respect to direction of (v2'+v3') system in S'
    // ----------------------------------------------------------------------------------------------------------------

    // Check if input is sensible
    if(v1.Px()==0 && v1.Py()==0 && v1.Pz()==0) return -5;
    if(v2.Px()==0 && v2.Py()==0 && v2.Pz()==0) return -5;
    if(v3.Px()==0 && v3.Py()==0 && v3.Pz()==0) return -5;
    if(v1.Px()==-99 && v1.Py()==-99 && v1.Pz()==-99) return -5;
    if(v2.Px()==-99 && v2.Py()==-99 && v2.Pz()==-99) return -5;
    if(v3.Px()==-99 && v3.Py()==-99 && v3.Pz()==-99) return -5;


    //   I] Evaluate Ang 1
    TLorentzVector     cm123 = v1 + v2 + v3;                // centre-of-mass system
    TVector3    boost_vec123 = -(cm123).BoostVector();      // define boost vector
    TLorentzVector  v2_cm123, v3_cm123;           // 4-vec boosted to (1+2+3)
    //__v2______
    v2_cm123 = v2;
    v2_cm123.Boost(boost_vec123);
    //__v3______
    v3_cm123 = v3;
    v3_cm123.Boost(boost_vec123);
    //
    // -----------------------------------------------------------
    TLorentzVector     cm23 = v2_cm123 + v3_cm123;          // centre-of-mass system
    TVector3    boost_vec23 = -(cm23).BoostVector();        // define boost vector
    TLorentzVector  v3_cm23;                                // 4-vec boosted to (2+3)
    //__v3______
    v3_cm23 = v3_cm123;
    v3_cm23.Boost(boost_vec23);
    // Calculate cos_theta
    float cos_theta = v3_cm23.Vect().Unit().Dot(cm23.Vect().Unit());
    // If we have a nan return default value -5
    if (isnan(cos_theta)){
      return -5.;
    } else {
      return cos_theta;
    }
  }

  float OfflineCPVariables::cos_theta_4_wrt_3(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3, TLorentzVector v4){
    // ----------------------------------------------------------------------------------------------------------------
    // Define Function to Calculate Angle of 4 in 3 centre-of-mass
    // Inputs:   v1, v2, v3, v4 TLorentzVector(s) for objects in LAB system
    // Execution:
    //   I] Evaluate four-vectors in centre of mass (v1+v2+v3)
    //      1. Create S' = (v1+v2+v3) center-of-mass system
    //      2. Boost v2,v3 objects to S' and create v2',v3'
    //  II] Evaluate Ang 2
    //      1. Create S'' = (v2'+v3') centre-of-mass system
    //      2. Boost v3' to S'' and create v3''
    //      3. Boost v4 (in LAB) directly to v3 (measured in LAB) centre-of-mass system and create v4'''
    //      4. Calculate Ang 2 = the angle between v4''' and v3''
    // ----------------------------------------------------------------------------------------------------------------

    // Check if input is sensible
    if(v1.Px()==0 && v1.Py()==0 && v1.Pz()==0) return -5;
    if(v2.Px()==0 && v2.Py()==0 && v2.Pz()==0) return -5;
    if(v3.Px()==0 && v3.Py()==0 && v3.Pz()==0) return -5;
    if(v4.Px()==0 && v4.Py()==0 && v4.Pz()==0) return -5;
    if(v1.Px()==-99 && v1.Py()==-99 && v1.Pz()==-99) return -5;
    if(v2.Px()==-99 && v2.Py()==-99 && v2.Pz()==-99) return -5;
    if(v3.Px()==-99 && v3.Py()==-99 && v3.Pz()==-99) return -5;
    if(v4.Px()==-99 && v4.Py()==-99 && v4.Pz()==-99) return -5;


    //   I] Evaluate four-vectors in centre of mass (v1+v2+v3)
    TLorentzVector     cm123 = v1 + v2 + v3;                // centre-of-mass system
    TVector3    boost_vec123 = -(cm123).BoostVector();      // define boost vector
    TLorentzVector  v2_cm123, v3_cm123;                     // 4-vec boosted to (1+2+3)
    //__v2______
    v2_cm123 = v2;
    v2_cm123.Boost(boost_vec123);
    //__v3______
    v3_cm123 = v3;
    v3_cm123.Boost(boost_vec123);

    //  II] Evaluate Ang 2
    TLorentzVector     cm23 = v2_cm123 + v3_cm123;          // centre-of-mass system
    TVector3    boost_vec23 = -(cm23).BoostVector();        // define boost vector
    TLorentzVector  v3_cm23;                                // 4-vec boosted to (2+3)
    //__v3______
    v3_cm23 = v3_cm123;
    v3_cm23.Boost(boost_vec23);
    // boost 4 (at LAB) directly to centre-of-mass of 3 (in LAB)
    TLorentzVector  v4_cm3;                                 // 4-vec (LAB) boosted to 3 (LAB)
    TVector3    boost_vec3 = -(v3).BoostVector();           // define boost vector of 3 (LAB)
    //__v4______
    v4_cm3 = v4;
    v4_cm3.Boost(boost_vec3);
    // Calculate cos_theta
    float cos_theta = v4_cm3.Vect().Unit().Dot(v3_cm23.Vect().Unit());
    // If we have a nan return default value -5
    if (isnan(cos_theta)){
      return -5.;
    } else {
      return cos_theta;
    }
  }

  float OfflineCPVariables::cos_theta_4_wrt_3seq(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3, TLorentzVector v4){
    // ----------------------------------------------------------------------------------------------------------------
    // Define Function to Calculate Angle of 4 in (2,3) centre-of-mass
    // Inputs:   v1, v2, v3, v4 TLorentzVector(s) for objects in LAB system
    // Execution:
    //   I] Evaluate four-vectors in centre of mass (v1+v2+v3)
    //      1. Create S' = (v1+v2+v3) center-of-mass system
    //      2. Boost v2,v3,v4 objects to S' and create v2',v3',v4'
    //  II] Evaluate Ang 2
    //      1. Create S'' = (v2'+v3') centre-of-mass system
    //      2. Boost v3',v4' to S'' and create v3'',v4''
    //      3. Boost v4'' to v3'' and create v4'''
    //      4. Calculate Ang 2 = the angle between v4''' and v3''
    // ----------------------------------------------------------------------------------------------------------------

    // Check if input is sensible
    if(v1.Px()==0 && v1.Py()==0 && v1.Pz()==0) return -5;
    if(v2.Px()==0 && v2.Py()==0 && v2.Pz()==0) return -5;
    if(v3.Px()==0 && v3.Py()==0 && v3.Pz()==0) return -5;
    if(v4.Px()==0 && v4.Py()==0 && v4.Pz()==0) return -5;
    if(v1.Px()==-99 && v1.Py()==-99 && v1.Pz()==-99) return -5;
    if(v2.Px()==-99 && v2.Py()==-99 && v2.Pz()==-99) return -5;
    if(v3.Px()==-99 && v3.Py()==-99 && v3.Pz()==-99) return -5;
    if(v4.Px()==-99 && v4.Py()==-99 && v4.Pz()==-99) return -5;


    //   I] Evaluate four-vectors in centre of mass (v1+v2+v3)
    TLorentzVector     cm123 = v1 + v2 + v3;                // centre-of-mass system
    TVector3    boost_vec123 = -(cm123).BoostVector();      // define boost vector
    TLorentzVector v2_cm123, v3_cm123, v4_cm123;            // 4-vec boosted to (1+2+3)
    //__v2______
    v2_cm123 = v2;
    v2_cm123.Boost(boost_vec123);
    //__v3______
    v3_cm123 = v3;
    v3_cm123.Boost(boost_vec123);
    //__v4______
    v4_cm123 = v4;
    v4_cm123.Boost(boost_vec123);

    //  II] Evaluate Ang 2
    TLorentzVector     cm23 = v2_cm123 + v3_cm123;          // centre-of-mass system
    TVector3    boost_vec23 = -(cm23).BoostVector();        // define boost vector
    TLorentzVector  v3_cm23, v4_cm23;                       // 4-vec boosted to (2+3)
    //__v3______
    v3_cm23 = v3_cm123;
    v3_cm23.Boost(boost_vec23);
    //__v4______
    v4_cm23 = v4_cm123;
    v4_cm23.Boost(boost_vec23);
    // boost 4'' to centre-of-mass of 3''
    TLorentzVector  v4_cm3;                                    // 4-vec (LAB) boosted to 3 (LAB)
    TVector3    boost_vec3 = -(v3_cm23).BoostVector();         // define boost vector of 3 (LAB)
    //__v4______
    v4_cm3 = v4_cm23;
    v4_cm3.Boost(boost_vec3);
    // Calculate cos_theta
    float cos_theta = v4_cm3.Vect().Unit().Dot(v3_cm23.Vect().Unit());
    // If we have a nan return default value -5
    if (isnan(cos_theta)){
      return -5.;
    } else {
      return cos_theta;
    }
  }

  void OfflineCPVariables::finalise(){
  }
}
