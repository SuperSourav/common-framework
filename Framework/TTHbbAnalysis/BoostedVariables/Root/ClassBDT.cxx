/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <iostream>
#include <string>
#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include "Riostream.h"
#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include <TList.h>
   
#include "BoostedVariables/ClassBDT.h"


ClassBDT::ClassBDT(){
  //need to update to new regions
  region = -1;
  reader_reg5_EvenOdd = new TMVA::Reader();
  reader_reg5_OddEven = new TMVA::Reader();
  score_name="score"; 
   
}

ClassBDT::~ClassBDT(){
  
}


void ClassBDT::setRegion(int Njet, int NBtag){
  // old school regions
  if (Njet==5 && NBtag>=4) region = 5;
  else if (Njet>=6 && NBtag==3) region = 7;
  else if (Njet>=6 && NBtag>=4) region = 8;
  else region =-1;
}  

void ClassBDT::setRegion(int regionFlag){
  // old school regions
  if (regionFlag==1) region = 5;
  else region =-1;

}  

void ClassBDT::loadModel(std::string pathToXml){
    // for different signal regions, here 5,7 & 8
    
    index=0; // index runs over target_var[index] global to all regions in defineVariables()

    std::cout << "loading.... " << pathToXml+"/reg5_EvenOdd_BDT.weights.xml" << std::endl;
    
    //parseVariablesXml(pathToXml+"/reg5_EvenOdd_BDT.weights.xml", varsToClass_reg5);

    std::cout << "define variables.... " << std::endl;

    std::ifstream infile(pathToXml+"/reg5_BDT.txt");

    for( std::string line; getline( infile, line ); ){
      varsToClass_reg5.push_back(line);
      std::cout << line << std::endl;
    }

    defineVariables(reader_reg5_EvenOdd, varsToClass_reg5);
    defineVariables(reader_reg5_OddEven, varsToClass_reg5);

    std::cout << "Book MVA.... " << std::endl;

    reader_reg5_EvenOdd->BookMVA( "myBDT_5_EvenOdd", pathToXml+"/reg5_EvenOdd_BDT.weights.xml");
    reader_reg5_OddEven->BookMVA( "myBDT_5_OddEven", pathToXml+"/reg5_OddEven_BDT.weights.xml");

}

void ClassBDT::setScoreName(std::string name){
  score_name=name;
}

std::string ClassBDT::getScoreName(){
  return score_name;
  
}

void ClassBDT::defineVariables(TMVA::Reader *reader, std::vector<std::string> varsToClass){
  // add variables to reader
  for(auto & var : varsToClass){
    reader->AddVariable( var, &target_var[index] );
    index++;
  }
}

void ClassBDT::add_spectator(std::string spec_name, float spec){
 // not there yet
}

void ClassBDT::parseVariablesXml(std::string pathToXml, std::vector<std::string> &varsToClass)
{
  // read xml to extract which variables were trained on
  TDOMParser *domParser = new TDOMParser();
  domParser->SetValidate(false);
  domParser->ParseFile(pathToXml.c_str());
  TXMLNode *node = domParser->GetXMLDocument()->GetRootNode();

  ParseContext(node,varsToClass);

}

void ClassBDT::ParseContext(TXMLNode *node,std::vector<std::string> &varsToClass)
{	
  // run through xml to get training variables
  for ( ; node; node = node->GetNextNode()) {
    if (node->GetNodeType() == TXMLNode::kXMLElementNode) { // Element Node
      TString name=node->GetNodeName();
      if(name=="Variable"){
	if (node->HasAttributes()) {
	  TList* attrList = node->GetAttributes();
	  TIter next(attrList);
	  TXMLAttr *attr;
	  while ((attr =(TXMLAttr*)next())) {
	    name=attr->GetName();
	    if(name=="Expression"){
	      varsToClass.push_back(attr->GetValue());
	    }
	  }
	}
      }
      
    }
    
    ParseContext(node->GetChildren(),varsToClass);
  }
}

const std::vector<std::string> ClassBDT::returnVarsToClass(){
  // tell the event loop which variables the BDT needs
  if (region==5) return varsToClass_reg5;
  else {
    std::vector<std::string> none;
    return none;
  }
}

void ClassBDT::set_all_variables(std::vector<float> varsToClass_event_values){
  // not being used, should go
  int counter=0;
  for(auto & var : varsToClass_event_values){
    target_var[counter]= var;
    //std::cout<<varsToClass_reg8[counter]<< " "<< target_var[counter]<<std::endl;
    counter++;
  }
  
}


void ClassBDT::set_weight(std::string weight){
  // maybe add later
  //reader->SetWeightExpression(weight);
}

float ClassBDT::event_score(std::vector<float> varsToClass_event_values, UInt_t eventNumber){ 
// get event values to classify and return BDT score
  //std::cout << region << std::endl;
  if (region==5 && eventNumber%2==1) return reader_reg5_EvenOdd->EvaluateMVA(varsToClass_event_values,"myBDT_5_EvenOdd");
  else if (region==5 && eventNumber%2==0) return reader_reg5_OddEven->EvaluateMVA(varsToClass_event_values,"myBDT_5_OddEven");
  
  else return -9;
}
