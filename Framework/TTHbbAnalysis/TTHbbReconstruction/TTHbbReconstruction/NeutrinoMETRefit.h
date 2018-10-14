/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutrinoMETRefit_HH
#define NeutrinoMETRefit_HH

#include <Rtypes.h>

class TLorentzVector;

/**
 * Neutrino pz reconstruction following the method described in:
 * https://twiki.cern.ch/twiki/pub/AtlasProtected/NeutrinoPz/NeutrinoReco.pdf
 * The code is adapted from the Neutrino.h and Neutrino.cc macros available on this twiki page:
 * https://twiki.cern.ch/twiki/bin/view/AtlasProtected/NeutrinoPz
 * prepared by the Wuppertal team
 **/
class NeutrinoMETRefit {

public:
  explicit NeutrinoMETRefit(double Wmass);
  NeutrinoMETRefit()=delete;
  NeutrinoMETRefit(NeutrinoMETRefit&)=delete;
  NeutrinoMETRefit(NeutrinoMETRefit&&)=delete;
  ~NeutrinoMETRefit();
  int pz_of_W(TLorentzVector lep, TLorentzVector* met, double* pz);
  
private:
  static void delta1fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
  static void delta2fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
  double metfit(double fitterPrintLevel, int ysol);
  
  static Double_t FullReco_MET_X;
  static Double_t FullReco_MET_Y;
  static Double_t FullReco_Pxe;
  static Double_t FullReco_Pye;
  static Double_t FullReco_PTe;
  
  static double mW;
};
#endif
