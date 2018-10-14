# -*- coding: utf-8 -*-
import os, glob
from optparse import OptionParser, OptionGroup

def createJO(JOname, isData, isTRF, isSystematics, isAFII, doTopParticleLevel, alternativeBCalib, alternativeCCalib, alternativeLCalib):
    jo = open(JOname, 'w')

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# SELECTION CONFIGURATION                                                      #\n")
    jo.write("#                                                                              #\n")
    jo.write("# This is a TTHbbLeptonic configuration for the AnalysisTop utility top-xaod.  #\n")
    jo.write("#                                                                              #\n")
    jo.write("# - TTHbbLeptonic: http://cern.ch/go/m989                                      #\n")
    jo.write("# - top-xaod:      http://cern.ch/go/D8bk                                      #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# PACKAGE LIBRARIES                                                            #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("LibraryNames libTopEventSelectionTools libTopEventReconstructionTools libBoostedJetTaggers libTTHbbLeptonic libJetSelectorTools libJetSubStructureUtils libJetSubStructureMomentTools\n")
    jo.write("\n")

    #--- http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/GoodRunsLists/
    GRL2015 = "data15_13TeV/20160720/physics_25ns_20.7.xml"
    GRL2016 = "data16_13TeV/20170215/physics_25ns_20.7.xml"
    if not isData:
        jo.write("# no GRL needed on MC\n")
    else:
        jo.write("################################################################################\n")
        jo.write("#                                                                              #\n")
        jo.write("# GOOD RUN LISTS                                                               #\n")
        jo.write("#                                                                              #\n")
        jo.write("# http://atlasdqm.web.cern.ch/atlasdqm/grlgen/All_Good/                        #\n")
        jo.write("#                                                                              #\n")
        jo.write("################################################################################\n")
        jo.write("\n")
        jo.write("GRLDir GoodRunsLists\n")
        jo.write("GRLFile %s %s\n" % (GRL2015, GRL2016))
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# OBJECT COLLECTIONS                                                           #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("ElectronCollectionName Electrons\n")
    jo.write("\n")

    jo.write("MuonCollectionName Muons\n")
    jo.write("\n")

    jo.write("#TauCollectionName TauJets\n")
    jo.write("TauCollectionName None\n")
    jo.write("\n")

    jo.write("PhotonCollectionName None\n")
    jo.write("\n")

    jo.write("METCollectionName MET_Reference_AntiKt4EMTopo\n")
    jo.write("LooseMETCollectionName MET_Reference_AntiKt4EMTopo\n")
    jo.write("\n")

    jo.write("JetCollectionName AntiKt4EMTopoJets\n")
    jo.write("\n")

    jo.write("#LargeJetCollectionName AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets\n")
    jo.write("LargeJetCollectionName None\n")
    jo.write("LargeJetSubstructure None\n")
    jo.write("\n")

    jo.write("#TrackJetCollectionName AntiKt4PV0TrackJets\n")
    jo.write("TrackJetCollectionName None\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# TRUTH COLLECTIONS AND SETTINGS                                               #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("TruthCollectionName TruthParticles\n")
    jo.write("\n")

    jo.write("TruthJetCollectionName AntiKt4TruthWZJets\n") # Then do the OR offline!
    jo.write("TopPartonHistory False\n")
    jo.write("TruthBlockInfo False\n")
    jo.write("PDFInfo False\n")
    if doTopParticleLevel:
        jo.write("TopParticleLevel True\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# OBJECT LOADER AND OBJECT SELECTIONS                                          #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("ObjectSelectionName TTHbbLeptonicObjectLoader\n")
    jo.write("\n")
    jo.write("BTaggingWP FixedCutBEff_60 FixedCutBEff_70 FixedCutBEff_77 FixedCutBEff_85\n")
    if alternativeBCalib!="":
        jo.write("BTaggingCalibrationB %s\n" % (alternativeBCalib))
    if alternativeCCalib!="":
        jo.write("BTaggingCalibrationC %s\n" % (alternativeCCalib))
    if alternativeLCalib!="":
        jo.write("BTaggingCalibrationLight %s\n" % (alternativeLCalib))
    jo.write("\n")

    jo.write("ElectronID TightLH\n")
    jo.write("ElectronIDLoose MediumLH\n")
    jo.write("ElectronIsolation Gradient\n")
    jo.write("ElectronIsolationLoose None\n")
    jo.write("\n")

    jo.write("MuonQuality Medium\n")
    jo.write("MuonQualityLoose Medium\n")
    jo.write("MuonIsolation Gradient\n")
    jo.write("MuonIsolationLoose None\n")
    jo.write("\n")

    jo.write("ElectronPt 10000\n")
    jo.write("MuonPt 10000\n")
    jo.write("JetPt 20000\n")
    jo.write("LargeRJetPt 150000\n")
    jo.write("TrackJetPt 7000\n")
    jo.write("TauPt 20000\n")
    jo.write("\n")

    jo.write("MuonEta 2.5\n")
    jo.write("JetEta 2.5\n")
    jo.write("LaregRJetEta 2.0\n")
    jo.write("TrackJetEta 2.5\n")
    jo.write("\n")

    jo.write("TauJetIDWPLoose Medium\n")
    jo.write("\n")

    jo.write("DoTight False\n")
    jo.write("DoLoose Both\n")
    jo.write("ApplyTightSFsInLooseTree True\n")
    jo.write("OverlapRemovalLeptonDef Tight\n")
    jo.write("applyTTVACut True\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# EXPERIMENTAL SYSTEMATICS                                                     #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("JetUncertainties_NPModel 21NP\n")
    jo.write("JetUncertainties_BunchSpacing 25ns\n")
    jo.write("JetJERSmearingModel Simple\n")
    jo.write("EgammaSystematicModel 1NP_v1\n")
    jo.write("LargeRJESUncertaintyConfig TopTagging\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# OUTPUT                                                                       #\n")
    jo.write("#                                                                              #\n")
    jo.write("# The standard AnalysisTop event savers are as follows:                        #\n")
    jo.write("#                                                                              #\n")
    jo.write("# - top::EventSaverxAOD                                                        #\n")
    jo.write("# - top::EventSaverFlatNtuple                                                  #\n")
    jo.write("#                                                                              #\n")
    jo.write("# This configuration uses event savers specialised for ttH.                    #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("OutputFilename output.root\n")
    jo.write("OutputFormat TTHbbLeptonicEventSaver\n")
    jo.write("OutputEvents SelectedEvents\n")
    jo.write("\n")

    jo.write("OutputFileSetAutoFlushZero True\n")
    jo.write("\n")

    jo.write("PerfStats Yes\n")
    jo.write("CalculateSF True\n")
    jo.write("\n")

    jo.write("#NEvents 200\n")
    jo.write("\n")

    if isSystematics:
        jo.write("Systematics All\n")
    else:
        jo.write("Systematics Nominal\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# PILEUP CONFIG                                                                #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("PRWConfigFiles TopCorrections/PRW.410000.mc15c.r7725_r7676.root\n")
    jo.write("\n")

    ilumicalc2015 = "GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root"
    ilumicalc2016 = "GoodRunsLists/data16_13TeV/20170215/physics_25ns_20.7.lumicalc.OflLumi-13TeV-008.root"
    jo.write("PRWLumiCalcFiles %s %s\n" % (ilumicalc2015, ilumicalc2016))
    jo.write("\n")

    if isAFII:
        jo.write("IsAFII True\n")
    else:
        jo.write("IsAFII False\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# GLOBAL TTH CONFIGURATION                                                     #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    jo.write("DynamicKeys ttH_btagWP,ttH_btagAlgo\n")
    jo.write("\n")

    jo.write("#B-tagging calo jets\n")
    jo.write("ttH_btagAlgo MV2c10\n")
    jo.write("ttH_btagWP FixedCutBEff_77\n")
    jo.write("\n")

    jo.write("MCGeneratorWeights Nominal\n")
    jo.write("\n")

    jo.write("################################################################################\n")
    jo.write("#                                                                              #\n")
    jo.write("# SELECTIONS                                                                   #\n")
    jo.write("#                                                                              #\n")
    jo.write("################################################################################\n")
    jo.write("\n")

    #--- Basic selection with mandatori cuts for reco level
    jo.write("########################\n")
    jo.write("### basic selection with mandatory cuts for reco level\n")
    jo.write("########################\n")
    jo.write("\n")

    jo.write("SUB BASIC\n")
    jo.write("INITIAL\n")
    jo.write("GRL\n")
    jo.write("GOODCALO\n")
    jo.write("PRIVTX\n")
    jo.write("RECO_LEVEL\n")
    jo.write("\n")
    jo.write("\n")

    #--- definition of the data periods
    jo.write("########################\n")
    jo.write("### definition of the data periods\n")
    jo.write("########################\n")
    jo.write("\n")

    jo.write("SUB period_2015\n")
    jo.write("RUN_NUMBER >= 276262\n")
    jo.write("RUN_NUMBER <= 284484\n")
    jo.write("\n")

    jo.write("SUB period_2016\n")
    jo.write("RUN_NUMBER >= 297730\n")
    jo.write("\n")
    jo.write("\n")

    #--- lepton trigger and offline cuts for reco-level selections
    jo.write("########################\n")
    jo.write("### lepton trigger and offline cuts for reco-level selections\n")
    jo.write("########################\n")
    jo.write("\n")

    jo.write("SUB EL_2015\n")
    jo.write(". BASIC\n")
    jo.write(". period_2015\n")
    jo.write("TRIGDEC HLT_e24_lhmedium_L1EM20VH HLT_e60_lhmedium HLT_e120_lhloose\n")
    jo.write("EL_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB EL_2016\n")
    jo.write(". BASIC\n")
    jo.write(". period_2016\n")
    jo.write("TRIGDEC HLT_e26_lhtight_nod0_ivarloose HLT_e60_lhmedium_nod0 HLT_e140_lhloose_nod0\n")
    jo.write("EL_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB MU_2015\n")
    jo.write(". BASIC\n")
    jo.write(". period_2015\n")
    jo.write("TRIGDEC HLT_mu20_iloose_L1MU15 HLT_mu50\n")
    jo.write("MU_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB MU_2016\n")
    jo.write(". BASIC\n")
    jo.write(". period_2016\n")
    jo.write("TRIGDEC HLT_mu26_ivarmedium HLT_mu50\n")
    jo.write("MU_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB EL_2015_Loose\n")
    jo.write(". BASIC\n")
    jo.write(". period_2015\n")
    jo.write("TRIGDEC HLT_e24_lhmedium_L1EM20VH HLT_e60_lhmedium HLT_e120_lhloose\n")
    jo.write("EL_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB EL_2016_Loose\n")
    jo.write(". BASIC\n")
    jo.write(". period_2016\n")
    jo.write("TRIGDEC HLT_e26_lhtight_nod0_ivarloose HLT_e60_lhmedium_nod0 HLT_e140_lhloose_nod0 HLT_e24_lhmedium_nod0_L1EM18VH\n")
    jo.write("EL_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB MU_2015_Loose\n")
    jo.write(". BASIC\n")
    jo.write(". period_2015\n")
    jo.write("TRIGDEC HLT_mu20_iloose_L1MU15 HLT_mu50\n")
    jo.write("MU_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB MU_2016_Loose\n")
    jo.write(". BASIC\n")
    jo.write(". period_2016\n")
    jo.write("TRIGDEC HLT_mu26_ivarmedium HLT_mu50 HLT_mu24\n")
    jo.write("MU_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB EM_2015\n")
    jo.write(". BASIC\n")
    jo.write(". period_2015\n")
    jo.write("TRIGDEC HLT_e24_lhmedium_L1EM20VH HLT_e60_lhmedium HLT_e120_lhloose HLT_mu20_iloose_L1MU15 HLT_mu50\n")
    jo.write("EL_N_OR_MU_N 27000 >= 1\n")
    jo.write("\n")

    jo.write("SUB EM_2016\n")
    jo.write(". BASIC\n")
    jo.write(". period_2016\n")
    jo.write("TRIGDEC HLT_e26_lhtight_nod0_ivarloose HLT_e60_lhmedium_nod0 HLT_e140_lhloose_nod0 HLT_mu26_ivarmedium HLT_mu50\n")
    jo.write("EL_N_OR_MU_N 27000 >= 1\n")
    jo.write("\n")
    jo.write("\n")

    #--- e+jets and mu+jets selection definitions
    jo.write("SUB ejets_basic\n")
    jo.write("EL_N 10000 >= 1\n")
    jo.write("EL_N 10000 == 1\n")
    jo.write("MU_N 10000 == 0\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("EL_N 27000 == 1\n")
    jo.write("EL_N_TIGHT 27000 == 1\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    jo.write("JET_N 20000 >= 3\n")
    if not isData and not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")

    jo.write("SUB ejets_basic_Loose\n")
    jo.write("EL_N 10000 >= 1\n")
    jo.write("EL_N 10000 == 1\n")
    jo.write("MU_N 10000 == 0\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("EL_N 27000 == 1\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    jo.write("JET_N 20000 >= 3\n")
    if not isData and not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")

    jo.write("SUB mujets_basic\n")
    jo.write("MU_N 10000 >= 1\n")
    jo.write("MU_N 10000 == 1\n")
    jo.write("EL_N 10000 == 0\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("MU_N 27000 == 1\n")
    jo.write("MU_N_TIGHT 27000 == 1\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    jo.write("JET_N 20000 >= 3\n")
    if not isData and not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")

    jo.write("SUB mujets_basic_Loose\n")
    jo.write("MU_N 10000 >= 1\n")
    jo.write("MU_N 10000 == 1\n")
    jo.write("EL_N 10000 == 0\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("MU_N 27000 == 1\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    jo.write("JET_N 20000 >= 3\n")
    if not isData and not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")
    jo.write("\n")

    jo.write("SUB ee_basic\n")
    jo.write("EL_N 10000 >= 2\n")
    jo.write("EL_N 10000 == 2\n")
    jo.write("MU_N 10000 == 0\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("EL_N 27000 >= 1\n")
    jo.write("EL_N 10000 >= 2\n")
    jo.write("EL_N 10000 == 2\n")
    jo.write("EL_N_TIGHT 15000 == 2\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    jo.write("MLL > 15000\n")
    if not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")

    jo.write("SUB mumu_basic\n")
    jo.write("MU_N 10000 >= 2\n")
    jo.write("MU_N 10000 == 2\n")
    jo.write("EL_N 10000 == 0\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("MU_N 27000 >= 1\n")
    jo.write("MU_N_TIGHT 10000 == 2\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    jo.write("MLL > 15000\n")
    if not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")

    jo.write("SUB emu_basic\n")
    jo.write("EL_N 10000 >= 1\n")
    jo.write("MU_N 10000 >= 1\n")
    jo.write("EL_N 10000 == 1\n")
    jo.write("MU_N 10000 == 1\n")
    jo.write("# at this point this selection is orthogonal to the others\n")
    jo.write("EL_N_OR_MU_N 27000 >= 1\n")
    jo.write("EL_N_TIGHT 10000 == 1\n")
    jo.write("MU_N_TIGHT 10000 == 1\n")
    jo.write("TRIGMATCH\n")
    jo.write("JETCLEAN LooseBad\n")
    jo.write("JET_N 20000 >= 1\n")
    jo.write("JET_N 20000 >= 2\n")
    if not isTRF:
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 1\n")
        jo.write("JET_N_BTAG FixedCutBEff_85 >= 2\n")
    jo.write("\n")
    jo.write("\n")


    if not isData:
        jo.write("########################\n")
        jo.write("### TRUTH all selection\n")
        jo.write("########################\n")
        jo.write("\n")
        jo.write("SELECTION truth_all\n")
        jo.write("PARTICLE_LEVEL\n")
        jo.write("TTHBBCREATEEVENT\n")
        jo.write("TTHBBTRUTHEDM\n")
        jo.write("SAVE\n")
        jo.write("\n")
        jo.write("\n")

    jo.write("########################\n")
    jo.write("### e+jets selections\n")
    jo.write("########################\n")
    jo.write("\n")

    #--- Save events ejets_2015
    jo.write("SELECTION ejets_2015\n")
    jo.write(". EL_2015\n")
    jo.write(". ejets_basic\n")
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n")
    jo.write("\n")

    if isData:
        jo.write("SELECTION ejets_2015_Loose\n")
        jo.write(". EL_2015_Loose\n")
        jo.write(". ejets_basic_Loose\n")
        jo.write("TTHBBCREATEEVENT\n")
        jo.write("TTHBBEDM\n")
        jo.write("H4BANALYSISVARS\n")
        jo.write("TRIGGERPRESCALES\n")
        jo.write("SAVE\n")
    jo.write("\n")

    #--- Save events ejets_2016
    jo.write("SELECTION ejets_2016\n")
    jo.write(". EL_2016\n")
    jo.write(". ejets_basic\n")
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n")
    jo.write("\n") 

    if isData:
        jo.write("SELECTION ejets_2016_Loose\n")
        jo.write(". EL_2016_Loose\n")
        jo.write(". ejets_basic_Loose\n")
        jo.write("TTHBBCREATEEVENT\n")
        jo.write("TTHBBEDM\n")
        jo.write("H4BANALYSISVARS\n")
        jo.write("TRIGGERPRESCALES\n")
        jo.write("SAVE\n")
        jo.write("\n")
    jo.write("\n")


    jo.write("########################\n")
    jo.write("### mu+jets selections\n")
    jo.write("########################\n")
    jo.write("\n")

    #--- Save events mujets_2015
    jo.write("SELECTION mujets_2015\n")
    jo.write(". MU_2015\n")
    jo.write(". mujets_basic\n")
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n")
    jo.write("\n") 

    if isData:
        jo.write("SELECTION mujets_2015_Loose\n")
        jo.write(". MU_2015_Loose\n")
        jo.write(". mujets_basic_Loose\n")
        jo.write("TTHBBCREATEEVENT\n")
        jo.write("TTHBBEDM\n")
        jo.write("H4BANALYSISVARS\n")
        jo.write("TRIGGERPRESCALES\n")
        jo.write("SAVE\n")
        jo.write("\n")
    jo.write("\n")

    #--- Save events mujets_2016
    jo.write("SELECTION mujets_2016\n")
    jo.write(". MU_2016\n")
    jo.write(". mujets_basic\n")
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n")

    if isData:
        jo.write("SELECTION mujets_2016_Loose\n")
        jo.write(". MU_2016_Loose\n")
        jo.write(". mujets_basic_Loose\n")
        jo.write("TTHBBCREATEEVENT\n")
        jo.write("TTHBBEDM\n")
        jo.write("H4BANALYSISVARS\n")
        jo.write("TRIGGERPRESCALES\n")
        jo.write("SAVE\n")
        jo.write("\n") 
    jo.write("\n")


    jo.write("########################\n")
    jo.write("### ee selections\n")
    jo.write("########################\n")

    #--- Save events ee_2015
    jo.write("SELECTION ee_2015\n")
    jo.write(". EL_2015\n")
    jo.write(". ee_basic\n")
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n")
    jo.write("\n")

    #--- Save events ee_2016
    jo.write("SELECTION ee_2016\n")
    jo.write(". EL_2016\n")
    jo.write(". ee_basic\n")
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n")
    jo.write("\n")
    jo.write("\n")

    #--- Save events mumu_2015
    jo.write("SELECTION mumu_2015\n")
    jo.write(". MU_2015\n") 
    jo.write(". mumu_basic\n") 
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n") 
    jo.write("\n") 

    #--- Save events mumu_2016
    jo.write("SELECTION mumu_2016\n") 
    jo.write(". MU_2016\n") 
    jo.write(". mumu_basic\n") 
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n") 
    jo.write("\n") 
    jo.write("\n")

    #--- Save events emu_2015
    jo.write("SELECTION emu_2015\n") 
    jo.write(". EM_2015\n") 
    jo.write(". emu_basic\n") 
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n") 
    jo.write("\n") 

    #--- Save events emu_2016
    jo.write("SELECTION emu_2016\n") 
    jo.write(". EM_2016\n") 
    jo.write(". emu_basic\n") 
    jo.write("TTHBBCREATEEVENT\n")
    if not isData:
        jo.write("TTHBBTRUTHEDM\n")
    jo.write("TTBARREWEIGHTING\n")
    jo.write("TTHBBEDM\n")
    if not isData:
        jo.write("JETTRUTHMATCHING SMALLJETS 0.3\n")
    if isTRF:
        jo.write("TRFCALC trf_WP:70 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
        jo.write("TRFCALC trf_WP:77 trf_cdi:2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root trf_calibName:default trf_nbjetMax:4 trf_ignoreSF:NO trf_ignoreMCMC:NO ttH_btagAlgo:MV2c10\n")
    jo.write("H4BANALYSISVARS\n")
    jo.write("SAVE\n") 
    jo.write("\n") 
    jo.write("\n")

    jo.close()



if __name__=="__main__":

    usage = "usage: %prog [options]"
    parser = OptionParser(usage=usage)

    parser.add_option("--JOname", action="store", dest="JOname", default="", help="JO name")
    parser.add_option("--isData", action="store_true", dest="isData", default=False, help="True if run on data (default: False)")
    parser.add_option("--isTRF", action="store_true", dest="isTRF", default=False, help="True if saveTRF variables (default: False)")
    parser.add_option("--isSystematics", action="store_true", dest="isSystematics", default=False, help="True if run on systematics (default: False)")
    parser.add_option("--isAFII", action="store_true", dest="isAFII", default=False, help="True if AFII (default: False)")
    parser.add_option("--doTopParticleLevel", action="store_true", dest="doTopParticleLevel", default=False, help="True if create particleLevelTree (default: False)")
    parser.add_option("--alternativeBCalib", action="store", dest="alternativeBCalib", default="", help="Name of alternative b-tag calibration, i.e. ttbar (default: \"\")")
    parser.add_option("--alternativeCCalib", action="store", dest="alternativeCCalib", default="Wc", help="Name of alternative c-mistag calibration (default: \"\")")
    parser.add_option("--alternativeLCalib", action="store", dest="alternativeLCalib", default="", help="Name of alternative light-mistag calibration (default: \"\")")

    (options, args)        = parser.parse_args()
    JOname                 = options.JOname
    isData                 = options.isData
    isTRF                  = options.isTRF
    isSystematics          = options.isSystematics
    isAFII                 = options.isAFII
    doTopParticleLevel     = options.doTopParticleLevel
    alternativeBCalib      = options.alternativeBCalib
    alternativeCCalib      = options.alternativeCCalib
    alternativeLCalib      = options.alternativeLCalib

    createJO(JOname=JOname, isData=isData, isTRF=isTRF, isSystematics=isSystematics, isAFII=isAFII, doTopParticleLevel=doTopParticleLevel, alternativeBCalib=alternativeBCalib, alternativeCCalib=alternativeCCalib, alternativeLCalib=alternativeLCalib)
