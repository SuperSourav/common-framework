

#include "SM4topsCommon/ToolBox.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;

namespace SM4topsCommon {

ToolBox* ToolBox::m_instance = nullptr;

ToolBox::ToolBox() = default;

ToolBox* ToolBox::get() {
  if (!m_instance) m_instance = new ToolBox();
  return m_instance;
}

bool ToolBox::isDileptonTrigger(const string& triggerName) const {
  smatch match;
  regex_search(triggerName, match,
               regex("2(e[0-9]+|mu[0-9]+)|(e[0-9]+|mu[0-9]+).*(e[0-9]+|mu[0-9]+)"));
  return !match.empty();
}

vector<string> ToolBox::getSLTriggerNames(const vector<string>& allTrigNames) const {
  vector<string> res = {};
  for (auto& trigName : allTrigNames) {
    if (!isDileptonTrigger(trigName)) res.emplace_back(trigName);
  }
  return res;
}

vector<string> ToolBox::getDLTriggerNames(const vector<string>& allTrigNames) const {
  vector<string> res = {};
  for (auto& trigName : allTrigNames) {
    if (isDileptonTrigger(trigName)) res.emplace_back(trigName);
  }
  return res;
}

vector<string> ToolBox::getUniqueTriggerNamesAll(std::shared_ptr<top::TopConfig> conf, const string& selectorName) const {
  vector<string> res = conf->allTriggers_Loose(selectorName);
  vector<string> tight = conf->allTriggers_Tight(selectorName);
  res.insert(res.end(), tight.begin(), tight.end());

  // remove duplicates.
  std::set<std::string> s(res.begin(), res.end());
  res.assign(s.begin(), s.end());
  return res;
}

vector<string> ToolBox::getElectronTriggerNames(const vector<string>& allTrigNames) const {
  vector<string> res = {};
  smatch match;
  for (auto& trigName : allTrigNames) {
    regex_search(trigName, match, regex("e[0-9]+"));
    if (!match.empty()) res.emplace_back(trigName);
  }
  return res;
}

vector<string> ToolBox::getMuonTriggerNames(const vector<string>& allTrigNames) const {
  vector<string> res = {};
  smatch match;
  for (auto& trigName : allTrigNames) {
    regex_search(trigName, match, regex("mu[0-9]+"));
    if (!match.empty()) res.emplace_back(trigName);
  }
  return res;
}
}  // namespace SM4topsCommon
