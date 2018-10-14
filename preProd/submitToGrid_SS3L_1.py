#!/usr/bin/env python
import TopExamples.grid as grid
import mc16a_SM4tops
import mc16c_SM4tops
import mc16d_SM4tops
import data_SM4tops
import os

config = grid.Config()
config.CMake         = True
config.code          = 'top-xaod'
config.gridUsername  = 'lrustige'
config.excludedSites = ''
# config.excludedSites = 'ANALY_CONNECT,ANALY_CONNECT_SHORT'#'ANALY_GOEGRID'
# config.noSubmit      = True
# config.memory	     = '4000'
config.maxNFilesPerJob = '10'

config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
# config.destSE        = 'BNL-OSG2_LOCALGROUPDISK'
config.destSE        = ''


####################################################################################

################
## Step1: Nominal
################

# Data
config.suffix       = 'SM4t-212380_data_SS3L_v1'
config.settingsFile = 'cuts_data_SM4tops_SS3L.txt'
config.extFile      = '.root,.so'
names = ['data2015', 'data2016', 'data2017']
samples = grid.Samples(names)
grid.submit(config, samples)

# MC16a
config.suffix       = 'SM4t-212380_mc16a_SS3L_v1'
config.settingsFile = 'cuts_mc16a_SM4tops_SS3L.txt'
config.extFile      = '.root,.so'
names = ['mc16a_4tops', 'mc16a_ttbar_PhPy8', 'mc16a_SingleTop_Alt',
         'mc16a_ttX', 'mc16a_Vjets_PhPy8', 'mc16a_VV']
samples = grid.Samples(names)
grid.submit(config, samples)

################
## Step2: Nominal for alternative MC samples
################

# MC16a
config.suffix       = 'SM4t-212380_mc16a_SS3L_v1'
config.settingsFile = 'cuts_mc16a_SM4tops_SS3L.txt'
config.extFile      = '.root,.so'
names = ['mc16a_ttbar_Alt', 'mc16a_ttbar_Sliced', 'mc16a_Vjets_Sherpa221']
samples = grid.Samples(names)
grid.submit(config, samples)

################
## Step3: systematic runs
################

# JER
config.suffix       = 'SM4t-212380_mc16a_SS3L_jer_v1'
config.settingsFile = 'cuts_mc16a_SM4tops_SS3L_jer.txt'
config.extFile      = '.root,.so'
names = ['mc16a_4tops', 'mc16a_ttbar_PhPy8', 'mc16a_SingleTop_Alt',
         'mc16a_ttX', 'mc16a_Vjets_PhPy8', 'mc16a_VV']
samples = grid.Samples(names)
grid.submit(config, samples)

# SYS
config.suffix       = 'SM4t-212380_mc16a_SS3L_sys_v1'
config.settingsFile = 'cuts_mc16a_SM4tops_SS3L_sys.txt'
config.extFile      = '.root,.so'
names = ['mc16a_4tops', 'mc16a_ttbar_PhPy8', 'mc16a_SingleTop_Alt',
         'mc16a_ttX', 'mc16a_Vjets_PhPy8', 'mc16a_VV']
samples = grid.Samples(names)
grid.submit(config, samples)

################
## Step4: extra runs
################

# PFlow
config.suffix       = 'SM4t-212380_mc16a_SS3L_pflow_v1'
config.settingsFile = 'cuts_mc16a_SM4tops_SS3L_pflow.txt'
config.extFile      = '.root,.so'
names = ['mc16a_4tops', 'mc16a_ttbar_PhPy8', 'mc16a_ttX']
samples = grid.Samples(names)
grid.submit(config, samples)

# Skim
config.suffix       = 'SM4t-212380_mc16a_SS3L_skim_v1'
config.settingsFile = 'cuts_mc16a_SM4tops_SS3L_skim.txt'
config.extFile      = '.root,.so'
names = ['mc16a_4tops', 'mc16a_ttbar_PhPy8', 'mc16a_ttX']
samples = grid.Samples(names)
grid.submit(config, samples)
