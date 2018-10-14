/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>

#include "MVAVariables/JetOrderingTool.h"

JetOrderingTool::JetOrderingTool() 
{

}

JetOrderingTool::~JetOrderingTool(){
}


void JetOrderingTool::decorateEventBtagSort(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
					   std::string mv2Name, std::string pseudoMV2Name){


  downSort(jets, mv2Name, TTHbb::DecorationType::Float);
  int order=1;
  for(auto jet : jets){
    jet->intVariable(mv2Name+"_order") = order;
    if(order<7){
      std::ostringstream ss;
      ss << "jet_"<< mv2Name<<"_order_" << order << "_" <<mv2Name;
      std::ostringstream ss1;
      ss1 << "jet_"<< mv2Name<<"_order_" << order << "_" <<pseudoMV2Name;
      event->floatVariable(ss.str()) = jet->floatVariable(mv2Name); /// keep it only for cross check now
      event->intVariable(ss1.str()) = jet->intVariable(pseudoMV2Name);
    }
    ++order;
  }

}


void JetOrderingTool::decorateEventPseudoBtagSort(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
						  std::string pseudoMV2Name){

  sortPseudoBtagPt(jets, pseudoMV2Name);
  int order=1;
  for(auto jet : jets){
    jet->intVariable(pseudoMV2Name+"_order") = order;
    if(order<7){
      std::ostringstream ss;
      ss << "jet_"<< pseudoMV2Name<<"_order_" << order << "_" <<pseudoMV2Name;
      event->intVariable(ss.str()) = jet->intVariable(pseudoMV2Name);
    }
    ++order;
  }

}

void JetOrderingTool::decoratePtSortedVar(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
					  std::string varname, TTHbb::DecorationType type){

  sortPt(jets);
  int order=1;
  for(auto jet : jets){
    jet->intVariable("pt_order") = order;

    if(order<7){
      std::ostringstream ss;
      ss << "jet_pt_order_" << order << "_" <<varname;
      if(TTHbb::DecorationType::Char == type){
	event->charVariable(ss.str()) = jet->charVariable(varname);
      }
      if(TTHbb::DecorationType::Int == type){
	event->intVariable(ss.str()) = jet->intVariable(varname);
      }
      if(TTHbb::DecorationType::Float == type){
	event->floatVariable(ss.str()) = jet->floatVariable(varname);
      }
    }
    ++order;
  }

}


void JetOrderingTool::decorateAbsEtaSortedVar(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
					      std::string varname, TTHbb::DecorationType type){

  sortAbsEta(jets);
  int order=1;
  for(auto jet : jets){
    jet->intVariable("eta_order") = order;

    if(order<7){
      std::ostringstream ss;
      ss << "jet_eta_order_" << order << "_" <<varname;
      if(TTHbb::DecorationType::Char == type){
	event->charVariable(ss.str()) = jet->charVariable(varname);
      }
      if(TTHbb::DecorationType::Int == type){
	event->intVariable(ss.str()) = jet->intVariable(varname);
      }
      if(TTHbb::DecorationType::Float == type){
	event->floatVariable(ss.str()) = jet->floatVariable(varname);
      }
    }
    ++order;
  }

}



void JetOrderingTool::upSort(std::vector<std::shared_ptr<TTHbb::Jet> >& jets, std::string variable, TTHbb::DecorationType type){

  std::sort(jets.begin(), jets.end(),
	    [&](std::shared_ptr<TTHbb::Jet> jet1, std::shared_ptr<TTHbb::Jet> jet2){
	      switch(type){
	      case TTHbb::DecorationType::Char :
		return jet1->charVariable(variable) < jet2->charVariable(variable);
	      case TTHbb::DecorationType::Int :
		return jet1->intVariable(variable) < jet2->intVariable(variable);
	      case TTHbb::DecorationType::Float :
		return jet1->floatVariable(variable) < jet2->floatVariable(variable);
	      default :
		std::cout << "WARNING JetOrderingTool::upSort variable type " << (int)type << " unkown " << std::endl;
		return true;
	      }
	    }
	    );

}

void JetOrderingTool::downSort(std::vector<std::shared_ptr<TTHbb::Jet> >& jets, std::string variable, TTHbb::DecorationType type){

  std::sort(jets.begin(), jets.end(),
	    [&](std::shared_ptr<TTHbb::Jet> jet1, std::shared_ptr<TTHbb::Jet> jet2){
	      switch(type){
	      case TTHbb::DecorationType::Char :
		return jet1->charVariable(variable) > jet2->charVariable(variable);
	      case TTHbb::DecorationType::Int :
		return jet1->intVariable(variable) > jet2->intVariable(variable);
	      case TTHbb::DecorationType::Float :
		return jet1->floatVariable(variable) > jet2->floatVariable(variable);
	      default :
		std::cout << "WARNING JetOrderingTool::downSort variable type " << (int)type << " unkown " << std::endl;
		return true;
	      }
	    }
	    );


}

void JetOrderingTool::sortPseudoBtagPt(std::vector<std::shared_ptr<TTHbb::Jet> >& jets, std::string variable){


  std::sort(jets.begin(), jets.end(),
	    [&](std::shared_ptr<TTHbb::Jet> jet1, std::shared_ptr<TTHbb::Jet> jet2){
	      int a = jet1->intVariable(variable);
	      int b = jet2->intVariable(variable);
	      if( a == b ) return jet1->p4().Pt() > jet2->p4().Pt();
	      return a>b;
	   
	    }
	    );
 
}


/*void JetOrderingTool::sortPt(std::vector<std::shared_ptr<TTHbb::Jet> >& jets){

  std::sort(jets.begin(), jets.end(),
	    [](std::shared_ptr<TTHbb::Jet> jet1, std::shared_ptr<TTHbb::Jet> jet2){
	      return jet1->p4().Pt() > jet2->p4().Pt();
	    }
	    );
 
	    }*/

void JetOrderingTool::sortAbsEta(std::vector<std::shared_ptr<TTHbb::Jet> >& jets){

  std::sort(jets.begin(), jets.end(),
	    [](std::shared_ptr<TTHbb::Jet> jet1, std::shared_ptr<TTHbb::Jet> jet2){
	      return fabs(jet1->p4().Eta()) < fabs(jet2->p4().Eta());
	    }
	    );
 
}

