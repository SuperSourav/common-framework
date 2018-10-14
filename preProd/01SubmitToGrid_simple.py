#!/usr/bin/env python
import TopExamples.grid as grid
import mc16aR21_SM4tops
import mc16cR21_SM4tops
import os

config = grid.Config()
config.CMake         = True
config.code          = 'top-xaod'
config.gridUsername  = 'sberlend'
config.excludedSites = ''
# config.excludedSites = 'ANALY_CONNECT,ANALY_CONNECT_SHORT'#'ANALY_GOEGRID'
# config.noSubmit      = True
# config.memory	     = '4000'
config.maxNFilesPerJob = '10'

config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
config.destSE        = 'BNL-OSG2_LOCALGROUPDISK' 
# config.destSE        = ''

####################################################################################

# MC16a
config.suffix       = 'SM4tops_mc16a_180605_v1'
config.settingsFile = 'cuts_mc16a_SM4tops.txt'
config.extFile      = '.root,.so'
names = ['mc16a_ttbar_PhPy8', 'mc16a_4tops']
samples = grid.Samples(names)
grid.submit(config, samples)
# print grid.checkDatasetExists(names)

# MC16c
config.suffix       = 'SM4tops_mc16c_180605_v1'
config.settingsFile = 'cuts_mc16c_SM4tops.txt'
config.extFile      = '.root,.so'
names = ['mc16c_ttbar_PhPy8', 'mc16c_4tops']
samples = grid.Samples(names)
grid.submit(config, samples)
# print grid.checkDatasetExists(names)

# MC16c
config.suffix       = 'SM4tops_data_180605_v1'
config.settingsFile = 'cuts_data_SM4tops.txt'
config.extFile      = '.root,.so'
names = ['allData']
samples = grid.Samples(names)
grid.submit(config, samples)
# print grid.checkDatasetExists(names)
