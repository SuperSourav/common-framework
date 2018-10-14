/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/NeutrinoReconstructionTool.h"

#include "TTHbbReconstruction/NeutrinoMETRefit.h"

namespace TTHbb{


  NeutrinoReconstructionTool::NeutrinoReconstructionTool(NeutrinoRecoType type) : m_recType(type) {

  }

  NeutrinoReconstructionTool::~NeutrinoReconstructionTool(){
    ////
  }

  NeutrinoContainer* NeutrinoReconstructionTool::compute(float met_met, float met_phi, LeptonContainer* sleptons){

    TLorentzVector lepton = (*sleptons)[0]->p4();
    static const float Wmass = 80385.;
  
    NeutrinoContainer *neutrinos = new NeutrinoContainer();

    if (m_recType==NeutrinoRecoType::ORIGINAL) {
      float neut_pt  = met_met;
      float neut_phi = met_phi;
      float neut_px  = neut_pt*TMath::Cos(neut_phi); 
      float neut_py  = neut_pt*TMath::Sin(neut_phi);

      float Beta = Wmass*Wmass - lepton.M()*lepton.M() +2*lepton.Px()*neut_px + 2*lepton.Py()*neut_py;
      float Delta = lepton.E()*lepton.E()*(Beta*Beta + (2*lepton.Pz()*neut_pt)*(2*lepton.Pz()*neut_pt)-(2*lepton.E()*neut_pt)*(2*lepton.E()*neut_pt));


      std::vector<TLorentzVector> nusp4;
      std::vector<TTHbb::NeutrinoApprox> approx;

      if(Delta<=0){
	float neut_pz = 0.5*(lepton.Pz()*Beta)/(lepton.E()*lepton.E() - lepton.Pz()*lepton.Pz());
	TLorentzVector nup4;
	nup4.SetXYZM(neut_px, neut_py, neut_pz, 0.);
	nusp4.push_back(nup4);
	approx.push_back(TTHbb::nuReco_realonlySolution);
      }
      else{
	float neut_pz1 = 0.5*(lepton.Pz()*Beta + TMath::Sqrt(Delta))/(lepton.E()*lepton.E() - lepton.Pz()*lepton.Pz());
	float neut_pz2 = 0.5*(lepton.Pz()*Beta - TMath::Sqrt(Delta))/(lepton.E()*lepton.E() - lepton.Pz()*lepton.Pz());
	  
	TLorentzVector nup41;
	TLorentzVector nup42;
	nup41.SetXYZM(neut_px, neut_py, neut_pz1, 0);
	nup42.SetXYZM(neut_px, neut_py, neut_pz2, 0);
	nusp4.push_back(nup41);
	nusp4.push_back(nup42);
	approx.push_back(TTHbb::nuReco_positiveSolution);
	approx.push_back(TTHbb::nuReco_negativeSolution);
      }

      int id=0;

      for(unsigned int in=0; in<nusp4.size(); ++in){

	Neutrino* nu = new Neutrino();
	nu->id = id;
	nu->p4(nusp4[in]);
	nu->intVariable("nuapprox") = (int)approx[in];
	nu->floatVariable("delta")=Delta;
	neutrinos->add(nu);
	++id;
      }
    }
    else if (m_recType==NeutrinoRecoType::METREFIT) {
      NeutrinoMETRefit refit(Wmass);
    
      double* pz  = new double[2];
      TLorentzVector met; met.SetPtEtaPhiM(met_met,0.,met_phi,0.);
      unsigned int nsol = 0;
      nsol = refit.pz_of_W(lepton, &met, pz);

      TLorentzVector nup41,nup42;
      if (nsol == 0) std::cout<<"Warning: no solution for neutrino pZ in __FILE__ which is strange!!"<<std::endl;
      else if (nsol >= 1) {
	nup41.SetPxPyPzE(met.Px(),met.Py(),pz[0], sqrt(met.Px()*met.Px() + met.Py()*met.Py() + pz[0]*pz[0]));
	Neutrino* nu1 = new Neutrino();
	nu1->id = 0;
	nu1->p4(nup41);
	/// fixme, use enum
	nu1->floatVariable("nuapprox") = (nsol == 1)?2:3;
	neutrinos->add(nu1);
	if (nsol==2) {
	  nup42.SetPxPyPzE(met.Px(),met.Py(),pz[1], sqrt(met.Px()*met.Px() + met.Py()*met.Py() + pz[1]*pz[1]));
	  Neutrino* nu2 = new Neutrino();
	  nu2->id = 0;
	  nu2->p4(nup42);
	  //// fixme, use enum
	  nu2->intVariable("nuapprox") = 3;
	  neutrinos->add(nu2);
	}
	else if (nsol>2) std::cout<<"Warning: more than 2 solutions for neutrino pZ in __FILE__ which is strange!!"<<std::endl;
      }
      delete pz;
    }

    return neutrinos;

  }

}
