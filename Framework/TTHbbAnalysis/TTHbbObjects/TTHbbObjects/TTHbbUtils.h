/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBBUTILS_H_
#define _TTHBBUTILS_H_

#include <string>
#include <map>
#include <vector>

namespace TTHbb{
  namespace util{

    /// Function for comparing two case insensitive strings to see if they are the same
    bool iequals(const std::string& a, const std::string& b);

    /// Function to convert a string of parameters delimited by a character into a vector of strings
    /// @param str The string of paramters
    /// @param delim The character which the parameters are separated by
    std::vector<std::string> vectoriseString(std::string str, std::string delim=",");

    /// Function to read a file and return a vector of strings corresponding to each line
    /// @param file The file to open
    std::vector<std::string> fileToVector(std::string file);

    /// Functions to create a map of DSIDs with normalisation weights from the totalEventsWeighted tree
    /// @param inputs A vector of strings, where each string is an input file
    std::map<int,float> makeNormalisationMaps(std::vector<std::string> inputs);

    /// Functions to create a map of DSIDs with normalisation weights from the cutflow histograms
    /// @param inputs A vector of strings, where each string is an input file
    std::map<int,float> makeNormalisationMapsFromCutflow(std::vector<std::string> inputs);

    /// Functions to create a map of dsids mapped to mc generator weight names and normalisation weights for them
    /// @parmas inputs A vector of strings, where each string is an input file
    std::map<int, std::map<std::string,float> > makeMCWeightNormalisationMaps(std::vector<std::string> inputs);

    ///// get short btag WP, copied from EventSaverFlatNtuple, will be removed when this function become public
    std::string shortBtagWP (const std::string WP);

    ///// get the WP value, now that name of btagging has been extended
    std::string onlyBtagWP (const std::string WP);


    /// decode region name with njets and nbjets of the form 6ji3be
    void decodeRegionName(std::string region, int& njets, int& nmaxjets, int& nbjets, int& nmaxbjets);

    /// decode region name with name and value of the form name_<val>
    void decodeRegionName(std::string region, std::string &name, int &val);

  }
}

#endif
