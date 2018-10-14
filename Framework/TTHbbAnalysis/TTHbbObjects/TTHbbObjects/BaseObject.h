/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _BASEOBJECT_H_
#define _BASEOBJECT_H_

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include "TTHbbObjects/Containers.h"

namespace TTHbb{

  template<typename K, typename T> 
  using BaseObjectMap = std::unordered_map<K, T>;

  enum class DecorationType{ Char=0, Int=1, Float=2 };

  class BaseObject{
    /**
     * @brief The base class for all objects in the EDM that enables decoration of floats and ints
     */

  public:

    BaseObject();
    BaseObject(const BaseObject&);
    virtual ~BaseObject();

    std::string name;
    int id;

    /// Access int by name 
    int& intVariable(std::string);
    /// Access float by name
    float& floatVariable(std::string);
    /// Access char by name
    char& charVariable(std::string);

    /// Access int by name, const version
    const int& intVariableConst(std::string) const;
    // Access float by name, const version
    const float& floatVariableConst(std::string) const;
    /// Access char by name, const version
    const char& charVariableConst(std::string) const;
    
    /// Safe access functions could be used, returning m_floatDynVars.at(s)
    /// For now had checker functions
    bool checkIntVariable(std::string) const;
    bool checkFloatVariable(std::string) const;
    bool checkCharVariable(std::string) const;

    inline const BaseObjectMap<std::string, int>&   variableIntList(){   return m_intDynVars;   };
    inline const BaseObjectMap<std::string, float>& variableFloatList(){ return m_floatDynVars; };
    inline const BaseObjectMap<std::string, char>& variableCharList(){   return m_charDynVars;  };

    inline void variableIntList  (const BaseObjectMap<std::string, int>& a){   m_intDynVars=a;   };
    inline void variableFloatList(const BaseObjectMap<std::string, float>& a){ m_floatDynVars=a; };
    inline void variableCharList (const BaseObjectMap<std::string, char>& a){  m_charDynVars=a;  };

    inline const BaseObjectMap<std::string, std::vector<int>* >&   variableIntVecList(){   return m_intVecDynVars;   };
    inline const BaseObjectMap<std::string, std::vector<float>* >& variableFloatVecList(){ return m_floatVecDynVars; };
    inline const BaseObjectMap<std::string, std::vector<char>* >& variableCharVecList(){   return m_charVecDynVars;  };

    static int NbrBasicObjects;

  protected:

    BaseObjectMap<std::string, int>   m_intDynVars;
    BaseObjectMap<std::string, float> m_floatDynVars;
    BaseObjectMap<std::string, char>  m_charDynVars;

    BaseObjectMap<std::string, std::vector<int>* >   m_intVecDynVars;
    BaseObjectMap<std::string, std::vector<float>* > m_floatVecDynVars;
    BaseObjectMap<std::string, std::vector<char>* >  m_charVecDynVars;

  };
  
}

#endif
