/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/EventSaverHelper.h"

EventSaverHelper* EventSaverHelper::m_instance = 0;

EventSaverHelper::EventSaverHelper(){
}

EventSaverHelper::~EventSaverHelper(){
  m_variables.clear();
  m_jetVariables.clear();
  m_elecVariables.clear();
  m_muonVariables.clear();
  m_lJetVariables.clear();
}

EventSaverHelper* EventSaverHelper::get(){
  if(!m_instance){
    m_instance = new EventSaverHelper();
  }
  return m_instance;
}

void EventSaverHelper::addVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, bool truthTree, std::string origin){
  m_variables.insert(createSaveVar(name,type,nominalOnly,truthTree,origin));
}

void EventSaverHelper::addJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, bool truthTree, std::string origin){
  m_jetVariables.insert(createSaveVar(name,type,nominalOnly,truthTree,origin));
}

void EventSaverHelper::addElecVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, bool truthTree, std::string origin){
  m_elecVariables.insert(createSaveVar(name,type,nominalOnly,truthTree,origin));
}

void EventSaverHelper::addMuonVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, bool truthTree, std::string origin){
  m_muonVariables.insert(createSaveVar(name,type,nominalOnly,truthTree,origin));
}

void EventSaverHelper::addLJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, bool truthTree, std::string origin){
  m_lJetVariables.insert(createSaveVar(name,type,nominalOnly,truthTree,origin));
}



void EventSaverHelper::addVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin){
  m_variables.insert(createSaveVar(name,type,nominalOnly,false,origin));
}

void EventSaverHelper::addJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin){
  m_jetVariables.insert(createSaveVar(name,type,nominalOnly,false,origin));
}

void EventSaverHelper::addElecVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin){
  m_elecVariables.insert(createSaveVar(name,type,nominalOnly,false,origin));
}

void EventSaverHelper::addMuonVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin){
  m_muonVariables.insert(createSaveVar(name,type,nominalOnly,false,origin));
}


void EventSaverHelper::addLJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin){
  m_lJetVariables.insert(createSaveVar(name,type,nominalOnly,false,origin));
}


savevariable EventSaverHelper::createSaveVar(std::string name, TTHbb::DecorationType type, bool nominalOnly, bool truthTree, std::string origin){
  savevariable var;
  var.name = name;
  var.type = type;
  var.nominalOnly = nominalOnly;
  var.truthTree = truthTree;
  var.origin = origin;
  return var;
}
