/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TEnv.h>

#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <map>
#include <fstream>


using namespace std;

TString m_outputDir;
map<TString,TString> m_oldToNewNames;
map<TString,TString> m_newToOldNames;

void readBranchNameMap(TString filename){

  m_oldToNewNames.clear();
  m_newToOldNames.clear();

  ifstream in;
  in.open(filename.Data());

  string newb;
  string oldb;

  while(true){
    in >> oldb >> newb;
    if(!in.good()) break;

    pair<TString,TString> on(oldb.c_str(),newb.c_str());
    pair<TString,TString> no(newb.c_str(),oldb.c_str());

    m_oldToNewNames.insert(on);
    m_newToOldNames.insert(no);
  }

}

TString findOldBranchName(TString n){
  map<TString,TString>::iterator iter = m_newToOldNames.find(n);
  if(iter == m_newToOldNames.end()) return n;
  return iter->second;
}
TString findNewBranchName(TString n){
  map<TString,TString>::iterator iter = m_oldToNewNames.find(n);
  if(iter == m_oldToNewNames.end()) return n;
  return iter->second;
}

void drawDiff(TH1* newHisto, TH1* oldHisto, TH1* diff, TString branchname){

  TCanvas *cc = new TCanvas(branchname, branchname, 1200, 600);
  cc->Divide(3,1);
  // diff->SetXTitle(branchname);
  cc->cd(1);
  newHisto->SetXTitle(branchname+" (new)");
  newHisto->Draw();
  cc->cd(2);
  oldHisto->SetXTitle(branchname+" (old)");
  oldHisto->Draw();
  cc->cd(3);
  diff->SetXTitle(branchname+" (diff)");
  diff->Draw();
  cc->SaveAs(m_outputDir+"/"+branchname+".png");
  cc->SaveAs(m_outputDir+"/"+branchname+".pdf");

}

void drawDiffSame(TH1* newHisto, TH1* oldHisto, TH1* diff, TString branchname){

  TCanvas *cc = new TCanvas(branchname, branchname, 1000, 600);
  cc->Divide(2,1);
  cc->cd(1);
  newHisto->SetXTitle(branchname);
  newHisto->SetLineColor(1);
  newHisto->Draw();
  oldHisto->SetLineColor(2);
  oldHisto->Draw("sames");

  TLegend *leg = new TLegend(0.65,0.8,0.8,0.9);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->AddEntry(newHisto,"new","l");
  leg->AddEntry(oldHisto,"old","l");
  leg->Draw();

  cc->cd(2);
  diff->SetXTitle(branchname+" (diff)");
  diff->Draw();
  cc->SaveAs(m_outputDir+"/"+branchname+".png");
  cc->SaveAs(m_outputDir+"/"+branchname+".pdf");

}

void dumpSet(set<TString> tset, bool map=false){

  for(auto a : tset){
    cout << a;
    if(map)cout<< " " << findOldBranchName(a);
    cout<< endl;
  }

}

void compareTrees(TTree* oldTree, TTree* newTree, bool useSameBinning){

  cout << "############ comparing " << oldTree->GetName() <<" tree #################" << endl;

  int oldEntries = oldTree->GetEntries();
  int newEntries = newTree->GetEntries();

  if(oldEntries == newEntries){
    cout << " old and new trees have the same number of entries = " << newEntries << endl;
  }
  else{
    cout << " different number of entries new=" <<newEntries << " old=" << oldEntries << endl;
    cout << "---> all branches will be different " << endl;
  }

  set<TString> addedBranchList;
  set<TString> removedBranchList;
  set<TString> diffBranchList;
  set<TString> okBranchList;
  

  TObjArray *newBranchList = newTree->GetListOfBranches();

  for(int i=0; i<newBranchList->GetSize(); ++i){

    TBranch *newBranch = dynamic_cast<TBranch*>(newBranchList->At(i));
    TString branchName = newBranch->GetName();
    TString oldbranchName = findOldBranchName(branchName);

    if(oldTree->FindBranch(oldbranchName)){

      /// if at some points we want to add cuts
      TString cut ="";

      //// if at some point we want to compare specific branchs
      //if(!branchName.BeginsWith(""))continue;
      //if(!branchName.EndsWith(""))continue;
      //if(!branchName.EqualTo(""))continue;
     

      TString newHistoName= branchName+"_new";
      TString oldHistoNameSB= branchName+"_oldSB";
      TString oldHistoName= branchName+"_old";

      newTree->Draw(branchName+">>"+newHistoName,cut,"goff");
      void *newHistoTmp=0;
      gDirectory->GetObject(newHistoName,newHistoTmp);
      TH1 *newHisto  = static_cast<TH1*>(newHistoTmp);

      TH1* oldHistoSB = (TH1*)newHisto->Clone(oldHistoNameSB);
      oldTree->Draw(oldbranchName+">>"+oldHistoNameSB,cut,"goff");

      oldTree->Draw(oldbranchName+">>"+oldHistoName,cut,"goff");
      void *oldHistoTmp=0;
      gDirectory->GetObject(oldHistoName,oldHistoTmp);
      TH1 *oldHisto  = static_cast<TH1*>(oldHistoTmp);

      TH1* diff = (TH1*)oldHistoSB->Clone(branchName+"_difference");
      diff->Scale(-1);
      diff->Add(newHisto);


      if(useSameBinning){
      }
	
      bool OK = true;
      for(int i=0; i<diff->GetNbinsX()+2; ++i){
	if(diff->GetBinContent(i)){
	  OK=false;
	  break;
	}
      }

      if(OK ){
	okBranchList.insert(branchName);
	//drawDiffSame(newHisto, oldHistoSB, diff, branchName);
	continue;
      }

      diffBranchList.insert(branchName);

      if(useSameBinning){
	drawDiffSame(newHisto, oldHistoSB, diff, branchName);
      }
      else{
	drawDiff(newHisto, oldHisto, diff, branchName);
      }

    }
    else{
      addedBranchList.insert(branchName);
    }

  }


  TObjArray *oldBranchList = oldTree->GetListOfBranches();
  for(int i=0; i<oldBranchList->GetSize(); ++i){
    TBranch * oldBranch = dynamic_cast<TBranch*>(oldBranchList->At(i));
    TString branchName = oldBranch->GetName();
    TString newbranchName = findNewBranchName(branchName);
    if(!newTree->FindBranch(newbranchName)){
      removedBranchList.insert(branchName);
    }
  }

  cout << "List of OK branches:" << endl;
  cout << "--------------------" << endl;
  dumpSet(okBranchList, true);
  cout <<"----> " << okBranchList.size() << " branches are OK" << endl;

  cout << "List of add branches:" << endl;
  cout << "---------------------" << endl;
  dumpSet(addedBranchList);
  cout <<"----> " << addedBranchList.size() << " branches are added" << endl;

  cout << "List of removed branches:" << endl;
  cout << "-------------------------" << endl;
  dumpSet(removedBranchList);
  cout <<"----> " << removedBranchList.size() << " branches are removed" << endl;


  cout << "List of diff branches:" << endl;
  cout << "----------------------" << endl;
  dumpSet(diffBranchList, true);
  cout <<"----> " << diffBranchList.size() << " branches are diff" << endl;

  cout << "############ end comparing " << oldTree->GetName() <<" tree #################" << endl;

}

void compareTTHOutFile(TString oldFileName, 
		       TString newFileName, 
		       TString treeName,
		       TString outdir="comparetthntuples", 
		       TString varmapfile="",
		       bool drawsame=true){

  gSystem->Exec("rm -rf "+outdir);
  gSystem->Exec("mkdir -p "+outdir);

  //gEnv->SetValue("Root.ErrorIgnoreLevel=Info");
  gEnv->SetValue("Root.ErrorIgnoreLevel=Warning");

  gStyle->SetOptStat(111111);
  m_outputDir=outdir;


  TChain *ch1 = new TChain(treeName);
  TChain *ch2 = new TChain(treeName);

  if(""!=varmapfile){
    readBranchNameMap(varmapfile);
  }

  ch1->Add(newFileName);
  ch2->Add(oldFileName);

  compareTrees(ch2, ch1, drawsame);

}


#ifndef __CLING__

void usage(string ss){
  cout << "Usage: " << ss << " oldfilename newfilename [treeName] [outdir] " << endl;
  cout << "default treeName=\"nominal\" outdir=\"comparetthntuples\" " << endl;
}

int main(int argc, char **argv){

  if(argc<3){
    usage(argv[0]);
    return 1;
  }

  TString oldFileName = argv[1];
  TString newFileName = argv[2];

  TString treeName="nominal_Loose";
  TString outdir="comparetthntuples";
  TString varmapfile="";

  if(argc>3)treeName = argv[3];
  if(argc>4)outdir = argv[4];
  if(argc>5)varmapfile = argv[5];


  compareTTHOutFile(oldFileName, newFileName, treeName, outdir, varmapfile);

  return 0;

}

#endif
