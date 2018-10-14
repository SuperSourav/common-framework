/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EVENTSAVERHELPER_H_
#define _EVENTSAVERHELPER_H_

#include "TTHbbObjects/BaseObject.h"

#include <string>
#include <set>

struct savevariable{
  std::string name;
  TTHbb::DecorationType type;
  bool nominalOnly;
  bool truthTree;
  std::string origin;
};

inline bool operator<(const savevariable& lhs, const savevariable& rhs)
{
  return lhs.name < rhs.name;
}

class EventSaverHelper{
 public:
  static EventSaverHelper* get();
  
  virtual ~EventSaverHelper();

  void addVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly=false, bool truthTree=false, std::string origin="");
  void addJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly=false, bool truthTree=false, std::string origin="");
  void addElecVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly=false, bool truthTree=false, std::string origin="");
  void addMuonVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly=false, bool truthTree=false, std::string origin="");
  void addLJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly=false, bool truthTree=false, std::string origin="");

  void addVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin);
  void addJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin);
  void addElecVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin);
  void addMuonVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin);
  void addLJetVariableToBeSaved(std::string name, TTHbb::DecorationType type, bool nominalOnly, std::string origin);

  const std::set<savevariable> setOfVariables() const{ return m_variables; };
  const std::set<savevariable> setOfJetVariables() const{ return m_jetVariables; };
  const std::set<savevariable> setOfElecVariables() const{ return m_elecVariables; };
  const std::set<savevariable> setOfMuonVariables() const{ return m_muonVariables; };
  const std::set<savevariable> setOfLJetVariables() const{ return m_lJetVariables; };

 private:
  EventSaverHelper();

  savevariable createSaveVar(std::string name, TTHbb::DecorationType type, bool nominalOnly=false, bool truthTree=false, std::string origin="");

  static EventSaverHelper* m_instance;

  std::set<savevariable> m_variables;
  std::set<savevariable> m_jetVariables;
  std::set<savevariable> m_elecVariables;
  std::set<savevariable> m_muonVariables;
  std::set<savevariable> m_lJetVariables;

};


#endif
