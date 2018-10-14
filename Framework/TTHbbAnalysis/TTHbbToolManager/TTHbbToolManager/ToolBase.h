/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TOOLBASE_H_
#define _TOOLBASE_H_

#include "TTHbbObjects/Event.h"

#include <string>
#include <utility>
#include <map>
#include <functional>

namespace TTHbb{

  class ToolBase{
    /**
     * Implementation of the TTHbb::ToolBase abstract base class
     * Tools should inherit from this to be useable in the OfflineTTHbb code.
     * In an inherited class, the functions all need to be defined and
     * the m_name property should be set in the constructor.
     **/
  public:
    /// Default constructor
    ToolBase(){};

    /// Override for setting the getting property on constrution
    ToolBase(bool b) : b_allowGetting(b){};

    virtual ~ToolBase(){};

    /// Called once for every tool before any systematics or events
    virtual void initialise() = 0;

    /// Called once per event
    /// The current event is passed as an argument
    virtual void apply(TTHbb::Event*) = 0;

    /// Called once for every tool after all systematics have been run
    virtual void finalise() = 0;

    /// returns value of b_allowGetting
    bool isGettable(){ return b_allowGetting; };

    /// Name for the instance of the tool.
    /// Useful for debugging, but also differentiating between desired implementations
    std::string m_name;
  protected:

    /// Bool to be set in construction to say whether a tool is gettable from the ToolManager
    bool b_allowGetting;
  };
}
#endif
 /** To implement your tool you need to inherit from the base class
  *  I would strongly recommend to have the constructor take a string as an argument to set m_name
  *  This will help with debugging, but also means that multiple implementations of a tool can use
  *  the name to check their settings in the config file.
  *  To inherit use a structure similar to the one below
  *  You should implement every function in a separate .cxx file (including the constructor)
  *  Remember, the tool can just be a wrapper to a pre-existing tool, acting as a convertor for the EDM,
  *  but it can also be the basis of your own tool for use in OfflineTTHbb and TTHbbLeptonic
  *
  *  #include "TTHbbToolManager/ToolBase.h"
  *
  *  namespace TTHbb{
  *      class test : public ToolBase{
  *      public:
  *       test(std::string name){m_name = name;};
  *       ~test();

  *       void initialise();
  *       void apply(TTHbb::Event* m_event);
  *       void finalise();
  *     };
  * }
  **/
