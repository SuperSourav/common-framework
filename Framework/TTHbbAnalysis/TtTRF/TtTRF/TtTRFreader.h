#ifndef TTTRFREADER_H
#define TTTRFREADER_H

#include <string>
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

class TtTRFreader {
public:
  TtTRFreader(std::string fileName,std::string parametrisation="pt/eta/dRj");
  ~TtTRFreader(){};
  
  float GetEff(float v1,float v2,float v3,float v4);
  
private:
  TH1F *h0; // tot
  TH1F *h1;
  TH1F *h2;
  TH1F *h3;
  TH1F *h4;
  TH2F *h12;
}; 

#endif
