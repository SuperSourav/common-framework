/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/OfflineTtbarCorrections.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include <iostream>

namespace TTHbb{

  OfflineTtbarCorrections::OfflineTtbarCorrections(std::string name) : ToolBase(true),
  m_FracRW(nullptr),
  m_NormRW(nullptr),
  m_ShapeRW(nullptr),
  m_ttcRW(nullptr)
  //m_doNNLOrw(nullptr)
  {
    m_name = name;
    auto* config = TTHbb::GlobalConfiguration::get();
    m_DoFracRw = TTHbb::util::iequals((*config)(m_name + ".ttbarFracRw"),"true");
    m_DoNormRw = TTHbb::util::iequals((*config)(m_name + ".ttbbNormRw"),"true");
    m_DoShapeRw = TTHbb::util::iequals((*config)(m_name + ".ttbbShapeRw"),"true");
    m_DottccRw = TTHbb::util::iequals((*config)(m_name + ".ttccRw"),"true");
    std::cout << "m_DoFracRw being set to: " << m_DoFracRw << std::endl;
  } 
  
  OfflineTtbarCorrections::~OfflineTtbarCorrections(){
    if (m_FracRW)
      delete m_FracRW;
    if (m_NormRW)
      delete m_NormRW;
    if (m_ShapeRW)
      delete m_ShapeRW;
    if (m_ttcRW)
      delete m_ttcRW;
    //if (m_doNNLOrw)
      //delete m_NNLOReweighter;
  }
 

  void OfflineTtbarCorrections::initialise(){
    //initialise with some DSID and change in the loop as needed
    std::cout << "m_DoFracRw: " << m_DoFracRw << std::endl;
    if (m_DoFracRw){
      m_FracRW = new FractionReweighting(410501);
    }

    if (m_DoNormRw){
      m_NormRWTargets = {"410051", "410052_SherpaCSSKIN", "410053_SherpaMSTW", "410054_SherpaNNPDF", "410055_SherpaQCMMPS", "410056_SherpaX05", "410057_SherpaX2", "410058_SherpaGlosoft", "410184_SherpaMPIup", "410185_SherpaMPIdown" };
      m_NormRWNames = {"", "_SherpaCSSKIN", "_SherpaMSTW", "_SherpaNNPDF", "_SherpaQCMMPS", "_SherpaX05", "_SherpaX2", "_SherpaGlosoft", "_SherpaMPIup", "_SherpaMPIdown"};
      m_NormRW = new ttbbNormReweighting(410501, m_NormRWTargets);
    }

    if (m_DoShapeRw){
      m_shapeRWTargets = {"SherpaNominal", "SherpaCSSKIN", "SherpaMSTW", "SherpaNNPDF", "SherpaQCMMPS", "SherpaX05", "SherpaX2", "SherpaGlosoft", "SherpaMPIup", "SherpaMPIdown"};
      m_shapeRWVarNames = {"truth_top_pt", "truth_ttbar_pt", "HFClassification_q1_pt", "HFClassification_qq_dr", "HFClassification_qq_pt"};
      m_ShapeRW = new ttbbShapeReweighting(410501, m_shapeRWTargets, m_shapeRWVarNames);
    }

    if (m_DottccRw){
      m_ttcRW = new ttcReweighting(410501);
    }
     
    //m_doNNLOrw = false;
    //m_NNLOReweighter = new NNLOReweighter(0);
  }

  void OfflineTtbarCorrections::finalise(){
  }

  void OfflineTtbarCorrections::apply(TTHbb::Event* event){
    std::cout << "HELLO WORLD" << std::endl;
    m_HF_Classification=event->intVariable("HF_Classification");     
    m_HF_SimpleClassification=event->intVariable("HF_SimpleClassification");
    
    //change histogram if dsid changes
    if(event->m_info->mcChannelNumber!=sampleID){
      sampleID=event->m_info->mcChannelNumber;
      if (m_DoFracRw) m_FracRW->changeDSID(sampleID);
      if (m_DoNormRw) m_NormRW->changeDSID(sampleID);
      if (m_DoShapeRw) m_ShapeRW->changeDSID(sampleID);
      if (m_DottccRw) m_ttcRW->changeDSID(sampleID);
    }
    // fraction RW
    if (m_DoFracRw){
      event->floatVariable("weight_ttbar_FracRw") = m_FracRW->applyWeight(m_HF_SimpleClassification);
    }
    
    //norm RW
    if (m_DoNormRw){ 
       for( unsigned int iTarget = 0; iTarget < m_NormRWTargets.size(); ++iTarget){
         std::string thisVarToSave = "weight_ttbb_Norm"+m_NormRWNames.at(iTarget);
         std::vector<float> weights_norm = m_NormRW->applyWeight(m_HF_Classification);
         event->floatVariable(thisVarToSave) = weights_norm.at(iTarget);
       }
    }
    //shape RW
    if (m_DoShapeRw){
      std::vector<float> shapeRWVars;
      for( unsigned int iVar = 0; iVar < m_shapeRWVarNames.size(); ++iVar){
        shapeRWVars.push_back( event->floatVariable(m_shapeRWVarNames.at(iVar)) );
      }
      
      std::vector<float> weights_shape = m_ShapeRW->applyWeight(m_HF_Classification, shapeRWVars);
      
      for( unsigned int iTarget = 0; iTarget < m_shapeRWTargets.size(); ++iTarget){
        std::string thisVarToSave = "weight_ttbb_Shape_"+m_shapeRWTargets.at(iTarget);
        event->floatVariable(thisVarToSave) = weights_shape.at(iTarget);
      }
    }
    //ttc RW
    if (m_DottccRw){
      float truth_ttbar_pt = event->floatVariable("truth_ttbar_pt");
      float truth_top_pt = event->floatVariable("truth_top_pt");
      float HFClassification_q1_pt = event->floatVariable("HFClassification_q1_pt");
      float HFClassification_qq_dr = event->floatVariable("HFClassification_qq_dr");
      float HFClassification_qq_pt = event->floatVariable("HFClassification_qq_pt");
      
      event->floatVariable("weight_ttc_rw") = m_ttcRW->applyWeight(m_HF_Classification, truth_top_pt, truth_ttbar_pt, HFClassification_q1_pt, HFClassification_qq_dr, HFClassification_qq_pt);
   }
    
 /*   m_doNNLOrw = false;
    if((sampleID >= 410000 && sampleID <= 410004) || sampleID==410009 || sampleID==410120 || sampleID==410121) m_doNNLOrw = true;
    else if(sampleID >= 407009 && sampleID <= 407012) m_doNNLOrw = true;
    else if((sampleID >= 410500 && sampleID <= 410509) || sampleID==410159) m_doNNLOrw = true;
    else if(sampleID == 410511 && sampleID == 410512) m_doNNLOrw = true;
    else if(sampleID == 410525 || sampleID == 410528) m_doNNLOrw = true;
    else if((sampleID >= 410250 && sampleID <= 410252) || sampleID==410225 || sampleID==410274) m_doNNLOrw = true;
    else if(sampleID>=410281 && sampleID<=410283) m_doNNLOrw = true;

    unsigned int tempID = sampleID;
    if( sampleID >= 410281 && sampleID <= 410283) tempID = 410251;  //redirecting Sherpa 2.2.1 AFII to Sherpa 2.2.1 FS
    else if( sampleID == 410507 || sampleID == 410508 ) tempID = 410501;  //redirecting PP8 FS boosted b-/c-filtered to PP8 non allhad
    else if( sampleID == 410528 ) tempID = 410525;  //redirecting PowHer7 bfil to PowHer7
    else if( sampleID == 410274 ) tempID = 410225;  //redirecting PMCNloP8 bfil to aMCNloP8

    if( m_doNNLOrw ) {
	    m_NNLOReweighter->SetSampleID( tempID );
	    m_NNLOReweighter->Init();
    }*/
   
  }
}
