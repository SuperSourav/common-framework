/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/HplusBDT.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "PathResolver/PathResolver.h"

#include <iostream>

using namespace std;

HplusBDT::HplusBDT(std::string params, std::shared_ptr<top::TopConfig> config) :
    m_config(config), m_isTRF(false)
{
    auto* esh = EventSaverHelper::get();
    auto input_params = TTHbb::util::vectoriseString(params," ");
    for(auto it=input_params.begin(); it!=input_params.end(); it++)
    {
        if (!it->compare("Semilep"))
            m_Selection = "Semilep";
        else if (!it->compare("TRF"))
            m_isTRF = true;
    }

    std::string path = PathResolverFindCalibDirectory("TTHbbLeptonic/BDTweights_hplus/classification/TMVAClassification");

    m_masspoints.push_back("200");
    m_masspoints.push_back("225");
    m_masspoints.push_back("250");
    m_masspoints.push_back("275");
    m_masspoints.push_back("300");
    m_masspoints.push_back("350");
    m_masspoints.push_back("400");
    m_masspoints.push_back("500");
    m_masspoints.push_back("600");
    m_masspoints.push_back("700");
    m_masspoints.push_back("800");
    m_masspoints.push_back("900");
    m_masspoints.push_back("1000");
    m_masspoints.push_back("1200");
    m_masspoints.push_back("1400");
    m_masspoints.push_back("1600");
    m_masspoints.push_back("1800");
    m_masspoints.push_back("2000");



    m_mv2WP=70.0;
    m_mv2Algo="MV2c10";
    if (m_Selection == "Semilep")
    {
        m_regions.push_back("Semilep_v1_HF_SR5354_BDT");
        m_regions.push_back("Semilep_v1_HF_SR63_BDT");
        m_regions.push_back("Semilep_v1_HF_SR64_BDT");
        m_regions.push_back("Semilep_v1_INC_SR5354_BDT");
        m_regions.push_back("Semilep_v1_INC_SR63_BDT");
        m_regions.push_back("Semilep_v1_INC_SR64_BDT");

        m_regions.push_back("Semilep_v2_HF_SR5354_BDT");
        m_regions.push_back("Semilep_v2_HF_SR63_BDT");
        m_regions.push_back("Semilep_v2_HF_SR64_BDT");
        m_regions.push_back("Semilep_v2_INC_SR5354_BDT");
        m_regions.push_back("Semilep_v2_INC_SR63_BDT");
        m_regions.push_back("Semilep_v2_INC_SR64_BDT");

        m_regions.push_back("Semilep_v3_HF_SR5354_BDT");
        m_regions.push_back("Semilep_v3_HF_SR63_BDT");
        m_regions.push_back("Semilep_v3_HF_SR64_BDT");
        m_regions.push_back("Semilep_v3_INC_SR5354_BDT");
        m_regions.push_back("Semilep_v3_INC_SR63_BDT");
        m_regions.push_back("Semilep_v3_INC_SR64_BDT");


        m_BDT[std::make_pair(5,3)] = {};
        m_BDT[std::make_pair(6,3)] = {};
        m_BDT[std::make_pair(6,4)] = {};
    }
    else
    {
        std::cout << "WARNING: Unrecognized hplus option: " << m_Selection << std::endl;
    }

    //Create Semilep output variables
    for (auto mass : m_masspoints)
    {
        if (atoi(mass.data()) <= 300)
        {
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_HF_BDT" + mass, TTHbb::DecorationType::Float);
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_HF_BDT" + mass, TTHbb::DecorationType::Float);
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v3_HF_BDT" + mass, TTHbb::DecorationType::Float);
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_INC_BDT" + mass, TTHbb::DecorationType::Float);
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_INC_BDT" + mass, TTHbb::DecorationType::Float);
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v3_INC_BDT" + mass, TTHbb::DecorationType::Float);

            if (m_isTRF)
            {

                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_HF_BDT" + mass + "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_HF_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v3_HF_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v3_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);

                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_HF_BDT" + mass + "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_HF_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v3_HF_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v3_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
            }
        }
        else
        {
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_INC_BDT" + mass, TTHbb::DecorationType::Float);
            esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v2_INC_BDT" + mass, TTHbb::DecorationType::Float);
            if (m_isTRF)
            {
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_3ex", TTHbb::DecorationType::Float);
                esh->addVariableToBeSaved("HpBDT_" + m_Selection + "_v1_INC_BDT" + mass+ "_" + std::to_string((int)m_mv2WP) + "_4in", TTHbb::DecorationType::Float);
            }
        }
    }

    std::cout << "INFO: classifBDThplus configured with btag algo " << m_mv2Algo << " and btag WP " << m_mv2WP << std::endl;

    for (auto version : m_regions)
    {
        for (auto mass : m_masspoints)
        {
            //For semilep HF BDT we're using only low mass 200-500GeV
            if ( version.find("Semilep")!=std::string::npos && version.find("_HF_")!=std::string::npos && atoi(mass.data()) >= 600)
                continue;

            std::string mva = version+mass;

            //Split to SR
            std::size_t indSR = mva.find("SR");
            if ( indSR != std::string::npos )
            {
                unsigned int nJets = atoi(mva.substr(indSR+2,1).c_str());
                unsigned int nBTags = atoi(mva.substr(indSR+3,1).c_str());
                m_BDT[std::make_pair(nJets,nBTags)].push_back(mva);
            }

            std::vector<std::string> variables = grabVariableList(path + "/TMVA_" + mva +"_0.weights.xml");
            m_tmvaReaders[mva] = new TMVA::Reader("silent");
            for (auto var : variables)
            {
                m_tmvaReaders[mva]->AddVariable(var, &m_variableMap[var]); // Use [] operator to insert if the key doesn't exist.
                m_variableSet.insert(var);
            }

            //Cross Training loop
            for (int i = 0 ; i < 2 ; i ++ )
            {
                std::string crossMVA = mva+ "_" + std::to_string(i);
                m_tmvaReaders[mva]->BookMVA(crossMVA, path + "/TMVA_" + crossMVA + ".weights.xml");
            }

        }
    }

    //TRF
    if ( m_isTRF == true)
    {
        std::vector<std::string> trf_vars = {"Mbb_MindR", "dRbb_avg","dRbb_MaxM", "Mbb_MaxM", "dRbb_MaxPt", "Mbj_MaxPt", "dRlepbb_MindR", "Muu_MindR", "Mbj_MindR", "pTuu_MindR", "Mbb_MaxPt", "Aplan_bjets"};

        m_TRFTypes = {"_"+std::to_string((int)m_mv2WP)+"_3ex","_" + std::to_string((int)m_mv2WP) + "_4in"};

        for (auto version : m_regions)
        {
            for (auto mass : m_masspoints)
            {
                //For semilep HF BDT we're using only low mass 200-500GeV
                if ( version.find("Semilep")!=std::string::npos && version.find("_HF_")!=std::string::npos && atoi(mass.data()) >= 600)
                    continue;

                std::string mva = version+mass;
                std::string mvaFull = mva;
                std::string trf_type = "";

                //Split to SR
                std::size_t indSR = mva.find("SR");
                if ( indSR != std::string::npos )
                {
                    int nJets = atoi(mva.substr(indSR+2,1).c_str());
                    int nBTags = atoi(mva.substr(indSR+3,1).c_str());
                    if (nBTags == 3)
                        trf_type = "_" + std::to_string((int)m_mv2WP) +"_3ex";
                    else if (nBTags == 4)
                        trf_type = "_" + std::to_string((int)m_mv2WP) +"_4in";


                    mvaFull+=trf_type;
                    m_TRFBDT[nJets].push_back(mvaFull);
                }

                std::vector<std::string> variables = grabVariableList(path + "/TMVA_" + mva +"_0.weights.xml");
                m_tmvaReaders[mvaFull] = new TMVA::Reader("silent");

                for (auto var : variables)
                {
                    std::string suffix = "";
                    if ( std::find(trf_vars.begin(),trf_vars.end(),var) != trf_vars.end() )
                    {
                        suffix=trf_type;
                    }
                    //For TRF we book the variable by it's nominal name but using the var+output for our variable map
                    m_tmvaReaders[mvaFull]->AddVariable(var, &m_variableMap[var+suffix]); // Use [] operator to insert if the key doesn't exist.
                    m_variableSet.insert(var+suffix);
                }

                //Cross Training loop
                for (int i = 0 ; i < 2 ; i ++ )
                {
                    std::string crossMVA = mva+ "_" + std::to_string(i);
                    m_tmvaReaders[mvaFull]->BookMVA(crossMVA, path + "/TMVA_" + crossMVA + ".weights.xml");
                }
            }
        }
    }
}

HplusBDT::~HplusBDT()
{
    //delete readers
    std::map<std::string, TMVA::Reader*>::iterator itmap = m_tmvaReaders.begin();
    for( ; itmap != m_tmvaReaders.end(); ++itmap ) delete itmap->second;
    m_tmvaReaders.clear();
}

std::string HplusBDT::name() const{
    return "HplusBDT";
}

bool HplusBDT::apply(const top::Event & event) const
{
    std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
    m_njets = tthevt->intVariable("nJets");
    m_nbjets = tthevt->intVariable("nBTags_70");

    for (auto var : m_variableSet)
    {
        bool isAvail = false;
        if (var=="jet_pt[0]")
        {
            m_variableMap.at(var) = tthevt->m_jets[0]->Pt();
            isAvail = true;
        }
        else if (var=="tagWeightBin[0]")
        {
            m_variableMap.at(var) = tthevt->m_jets[0]->intVariable("tagWeightBin");
            isAvail = true;
        }
        else if (var=="tagWeightBin[1]")
        {
            m_variableMap.at(var) = tthevt->m_jets[1]->intVariable("tagWeightBin");
            isAvail = true;
        }
        else if (var=="tagWeightBin[2]")
        {
            m_variableMap.at(var) = tthevt->m_jets[2]->intVariable("tagWeightBin");
            isAvail = true;
        }
        else if (var=="tagWeightBin[3]")
        {
            m_variableMap.at(var) = tthevt->m_jets[3]->intVariable("tagWeightBin");
            isAvail = true;
        }
        else
        {
            isAvail = (tthevt->checkFloatVariable(var) || tthevt->checkIntVariable(var));
            if (true == isAvail)
            {
                if (var.find("hplus_pcbt")!=std::string::npos || var.find("tagWeightBin")!=std::string::npos) m_variableMap.at(var) = tthevt->intVariable(var);
                if (var.find("hplus_pcbt")==std::string::npos && var.find("tagWeightBin")==std::string::npos) m_variableMap.at(var) = tthevt->floatVariable(var);
            }
        }
        if (!isAvail && var.find("_" + std::to_string((int)m_mv2WP) + "_") != std::string::npos )
        {//TRF variable in region where it's not calculated, switch back to regular one
            for ( uint i = 0 ; i < m_TRFTypes.size(); i++ )
            {
                if ( var.find(m_TRFTypes.at(i)) != std::string::npos )
                {
                    std::string raw_var = var;
                    raw_var.replace(var.find(m_TRFTypes.at(i)),m_TRFTypes.at(i).length(),"");
                    if ((isAvail = tthevt->checkFloatVariable(raw_var) || tthevt->checkIntVariable(raw_var)))
                    {
                        if (raw_var.find("hplus_pcbt")!=std::string::npos) m_variableMap.at(raw_var) = tthevt->intVariable(raw_var);
                        if (raw_var.find("hplus_pcbt")==std::string::npos) m_variableMap.at(raw_var) = tthevt->floatVariable(raw_var);
                    }
                    break;
                }
            }
        }

        if (!isAvail)
        {
            std::cerr << "ERROR: Variable " << var << " (needed for Hplus BDT) was not found" << std::endl;
            exit(1);

        }
    }
    std::vector<std::string> running_BDT;
    if (m_Selection == "Semilep")
    {
        if ( m_njets == 5 && m_nbjets >= 3 )
            running_BDT = m_BDT[std::make_pair(5,3)];
        else if ( m_njets >= 6 && m_nbjets == 3 )
            running_BDT = m_BDT[std::make_pair(6,3)];
        else if ( m_njets >= 6  && m_nbjets >= 4 )
            running_BDT = m_BDT[std::make_pair(6,4)];
        else
            running_BDT = m_BDT[std::make_pair(6,4)];
    }

    if (m_isTRF == true )
    {
        std::vector<std::string> trf_BDT {};
        if ( m_njets == 5  )
        {
            trf_BDT = m_TRFBDT[5];
        }
        else
        {
            trf_BDT = m_TRFBDT[6];
        }
        running_BDT.insert(running_BDT.end(), trf_BDT.begin(), trf_BDT.end());
    }

    for (auto mva_name : running_BDT)
    {
        float output = -9;
        std::map<std::string, TMVA::Reader*>::iterator imap = m_tmvaReaders.find(mva_name);
        if(imap == m_tmvaReaders.end())
        {
            std::cout<<"WARNING: No TMVAReader defined for hplus "<< mva_name <<std::endl;
        }
        else
        {
            //Patch for handling the TRF BDT output
            std::string raw_mva_name = mva_name;
            if ( mva_name.find("_" + std::to_string((int)m_mv2WP) + "_") != std::string::npos)
                raw_mva_name.replace(mva_name.find("_" + std::to_string((int)m_mv2WP) + "_"),7,"");


            output = imap->second->EvaluateMVA(raw_mva_name+"_"+std::to_string(tthevt->m_info->eventNumber%2));
        }

        //Patch for combining the semilep 4 SR into single branch
        if ( mva_name.find("SR5354_") != std::string::npos ) mva_name.replace( mva_name.find("SR"),7,"");
        if ( mva_name.find("SR") != std::string::npos ) mva_name.replace( mva_name.find("SR"),5,"");
        tthevt->floatVariable("HpBDT_"+mva_name) = output;
    }
    return true;
}


/**
 * @brief Grabs the list of variables of an MVA from the weight.xml file.
 *
 * Useful when the MVAs have different variables and in different order.
 *
 * */
std::vector<std::string> HplusBDT::grabVariableList(std::string filename)
{
    std::vector<std::string> variables;
    int nVars = -1;
    int nReadVars  = 0;
    bool foundVars = false;

    std::ifstream weightFile(filename);
    if (weightFile.is_open())
    {
        std::string line;
        TString label, index, variable;

        while (std::getline(weightFile, line))
        {
            std::istringstream ss(line);
            if (!foundVars)
            {
                ss >> label >> index;
                if (label.EqualTo("<Variables"))
                {
                    foundVars = true;
                    index.ReplaceAll("NVar=","");
                    index.ReplaceAll("\"","");
                    index.ReplaceAll(">","");
                    nVars = index.Atoi();
                }
            }
            else if (nReadVars < nVars)
            {
                ss >> label >> index >> variable;
                variable.ReplaceAll("Expression=","");
                variable.ReplaceAll("\"","");
                variables.push_back(variable.Data());
                nReadVars++;
            }

        }
    }
    else
    {
        std::cerr << "ERROR: Unable to open " << filename << std::endl;
        exit(1);
    }

    if (nVars < 0)
    {
        std::cerr << "ERROR: No variables found in " << filename << std::endl;
        exit(1);
    }
    if (nVars != nReadVars)
    {
        std::cout << "ERROR: Expected " << nVars << " variables in " << filename << std::endl;
        std::cout << "Found " << nReadVars << " variables." << std::endl;
        exit(1);
    }

    return variables;
}
