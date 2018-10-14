#include "TtTRF/TtTRFreader.h"

#include <iostream>

#include "TDirectory.h"
#include "TFile.h"

TtTRFreader::TtTRFreader(std::string fileName,std::string parametrisation){
  TDirectory *d0 = gDirectory;
  TFile *f = new TFile(fileName.c_str());
  h0 = 0x0; h1 = 0x0; h2 = 0x0; h3 = 0x0; h4 =0x0; h12=0x0;
  if(f!=0x0){
    h0 = (TH1F*)f->Get("h_eff");
    if(parametrisation.find("pt") !=std::string::npos) h1 = (TH1F*)f->Get("h_eff_pt");
    if(parametrisation.find("eta")!=std::string::npos) h2 = (TH1F*)f->Get("h_eff_eta");
    if(parametrisation.find("dRj")!=std::string::npos) h3 = (TH1F*)f->Get("h_eff_dRj");
    if(parametrisation.find("Nj")!=std::string::npos) h4 = (TH1F*)f->Get("h_eff_Nj_extrap");
    if(parametrisation.find("2D")!=std::string::npos) {
          if(parametrisation.find("pt") !=std::string::npos) {
                delete h1;
                h1 = 0x0;
                if(parametrisation.find("eta")!=std::string::npos) {
			delete h2;
                	h2 = 0x0;
                        h12 = (TH2F*)f->Get("h_eff_pt_eta");
                }
                //else if(parametrisation.find("dRj")!=std::string::npos){
		//	delete h3;
                //	h3 = 0x0;
                //        h13 = (TH2F*)f->Get("h_eff_pt_eta");
                //}

          }
          else {
                std::cout << "ttTRF::TtTRFreader: WARNING: No 2D parametrization without pt implemented, leaving 1Dx1D." << std::endl;
          }
    }
  }
  else{
    std::cout << "ttTRF::TtTRFreader: WARNING: File " << fileName << " cannot be openened... Skipping it." << std::endl; 
  }
//     d0->cd();
//     delete f;
}

//TtTRFreader::~TtTRFreader(){}
  
float TtTRFreader::GetEff(float v1,float v2,float v3, float v4){
//   if(h0==0x0 || h1==0x0 || h2==0x0 || h3==0x0) return 0;
  if(h0==0x0) return 0;
  float btageff0 = h0->GetBinContent(1);
//   float btageff  = h1->GetBinContent(h1->FindBin(v1));
  float btageff = btageff0;
  if(h1!=0x0) btageff *= h1->GetBinContent(h1->FindBin(v1)) / btageff0;
  if(h2!=0x0) btageff *= h2->GetBinContent(h2->FindBin(v2)) / btageff0;
  if(h3!=0x0) btageff *= h3->GetBinContent(h3->FindBin(v3)) / btageff0;
  if(h4!=0x0) btageff *= h4->GetBinContent(h4->FindBin(v4)) / btageff0;
  if(h12!=0x0) btageff = h12->GetBinContent(h12->FindBin(TMath::Log(v1),TMath::Abs(v2)));
  return btageff;
}
