/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/BaseObject.h"

namespace TTHbb{
  BaseObject::BaseObject(){
    name="";
    id=-1;
    ++NbrBasicObjects;
  }

  BaseObject::BaseObject(const BaseObject& q){
    name = q.name;
    id = q.id;
    ++NbrBasicObjects;
  }

  BaseObject::~BaseObject(){
    for(auto var : m_intVecDynVars) delete var.second;
    for(auto var : m_floatVecDynVars) delete var.second;
    for(auto var : m_charVecDynVars) delete var.second;
    --NbrBasicObjects;
  }

  int& BaseObject::intVariable(std::string s){
    return m_intDynVars[s];
  }

  float& BaseObject::floatVariable(std::string s){
    return m_floatDynVars[s];
  }

  char& BaseObject::charVariable(std::string s){
    return m_charDynVars[s];
  }

  const int& BaseObject::intVariableConst(std::string s) const {
    return m_intDynVars.at(s);
  }

  const float& BaseObject::floatVariableConst(std::string s) const {
    return m_floatDynVars.at(s);
  }

  const char& BaseObject::charVariableConst(std::string s) const {
    return m_charDynVars.at(s);
  }


  bool BaseObject::checkIntVariable(std::string s) const{
    return (bool)m_intDynVars.count(s);
  }

  bool BaseObject::checkFloatVariable(std::string s) const{
    return (bool)m_floatDynVars.count(s);
  }

  bool BaseObject::checkCharVariable(std::string s) const{
    return (bool)m_charDynVars.count(s);
  }

  int BaseObject::NbrBasicObjects=0;

}
