/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetOrderingTool_HH
#define JetOrderingTool_HH

#include <memory>
#include <vector>
#include "TLorentzVector.h"

#include "TTHbbObjects/Event.h"

class JetOrderingTool{


public:
  
  JetOrderingTool();
  ~JetOrderingTool();



  void decorateEventBtagSort(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
			     std::string mv2Name, std::string pseudoMV2Name);

  void decorateEventPseudoBtagSort(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
				   std::string pseudoMV2Name);

  void decoratePtSortedVar(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
			   std::string varname, TTHbb::DecorationType type);

  void decorateAbsEtaSortedVar(TTHbb::BaseObject* event, std::vector<std::shared_ptr<TTHbb::Jet> >& jets, 
			       std::string varname, TTHbb::DecorationType type);


  void upSort(std::vector<std::shared_ptr<TTHbb::Jet> >& jets, std::string variable, TTHbb::DecorationType type);
  void downSort(std::vector<std::shared_ptr<TTHbb::Jet> >& jets, std::string variable, TTHbb::DecorationType type);

  void sortPseudoBtagPt(std::vector<std::shared_ptr<TTHbb::Jet> >& jets, std::string variable);
  //void sortPt(std::vector<std::shared_ptr<TTHbb::Jet> >& jets);
  void sortAbsEta(std::vector<std::shared_ptr<TTHbb::Jet> >& jets);

  template<typename T, typename A> 
    static void sortPt( std::vector<std::shared_ptr<T>,A> & vec );

  template<typename T, typename A> 
    static void sortFloatDecorUp( std::vector<std::shared_ptr<T>,A> & vec, std::string decor );

  template<typename T, typename A> 
    static void sortFloatDecorDown( std::vector<std::shared_ptr<T>,A> & vec, std::string decor );


private:


 
};


template<typename T, typename A> 
  void JetOrderingTool::sortPt( std::vector<std::shared_ptr<T>,A>& vec ){
  
  std::sort( vec.begin(), vec.end(),
	     [](std::shared_ptr<T> v1, std::shared_ptr<T> v2){
	       return v1->p4().Pt() > v2->p4().Pt();
	     }
	     );

}

template<typename T, typename A> 
  void JetOrderingTool::sortFloatDecorUp( std::vector<std::shared_ptr<T>,A>& vec, std::string decor ){
  
  std::sort( vec.begin(), vec.end(),
	     [&](std::shared_ptr<T> v1, std::shared_ptr<T> v2){
	       return v1->floatVariable(decor) > v2->floatVariable(decor);
	     }
	     );

}

template<typename T, typename A> 
  void JetOrderingTool::sortFloatDecorDown( std::vector<std::shared_ptr<T>,A>& vec, std::string decor ){
  
  std::sort( vec.begin(), vec.end(),
	     [&](std::shared_ptr<T> v1, std::shared_ptr<T> v2){
	       return v1->floatVariable(decor) < v2->floatVariable(decor);
	     }
	     );

}



#endif

//-------------------------------------//
