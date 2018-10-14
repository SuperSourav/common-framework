#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
import TTHbbLeptonic.TTHbbProdTools
import TTHbbLeptonic.grid
import copy, sys

#####################
### configuration ###
#####################

suffix = 'TTHbb212380_v1' # add the version of the production, e.g. "v12"

### -- Add your filtering options here (name, channel) -- Used later in the script
submissionFilterList   = [("ttH_mc16a","1l"), ("Data15","1l"),
                          ("Data15", "2l"), ("ttV_mc16c","1l_sys") ]


# prepare base configuration
configBase = TTHbbLeptonic.grid.Config()
configBase.code            = 'top-xaod'
configBase.gridUsername    = 'phys-higgs' # grid username or group (for official production)
configBase.excludedSites   = '' # sometimes it's needed
configBase.noSubmit        = True # put to False to actually launch
configBase.mergeType       = 'Default' #'None', 'Default' or 'xAOD'
configBase.destSE          = '' #This is the default (anywhere,  '',  ''), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'
configBase.groupProduction = True # for official production only; if True, gridUsername must be e.g. 'phys-top'
configBase.memory          = '' # not really needed
configBase.nameShortener   = TTHbbLeptonic.TTHbbProdTools.OutNameFormat # to use your own physics part shortening function - uncomment here and in the function definition above
configBase.reuseTarBall    = True # we want to create the tarball once for all and to re-use it

# prepare configurations - one per stream per sample type (data, mc16a, etc.)
# this is where we have to take care of the cut-files
configs = {}

for stream in [ '1l', '1l_fakes', '2l', "0l", '1l_sys', '2l_sys', "0l_sys", '1l_ftrig', '1l_jer', '2l_jer', '1l_lrj' ]:
    for campaign in ['data15', 'data16', 'data17', 'mc16a', 'mc16c', 'mc16d']:
        # Exceptions
        if "sys" in stream and "data" in campaign:
            continue
        if "fakes" in stream and "mc" in campaign:
            continue
        if "ftrig" in stream and "mc" in campaign:
            continue
        configs[stream+'_'+campaign]               = copy.deepcopy(configBase)
        configs[stream+'_'+campaign].settingsFile  = 'cuts_' + stream + '_' + ("data" if "data" in campaign else campaign) + '.txt'
        configs[stream+'_'+campaign].suffix        = suffix + "-" + stream

print configs

###############
### samples ###
###############

# read samples lists
TTHbbLeptonic.TTHbbProdTools.MakeSamplesLists(filename='Data_inputs.txt')
TTHbbLeptonic.TTHbbProdTools.MakeSamplesLists(filename='MC16a_inputs.txt')
TTHbbLeptonic.TTHbbProdTools.MakeSamplesLists(filename='MC16c_inputs.txt')
TTHbbLeptonic.TTHbbProdTools.MakeSamplesLists(filename='MC16d_inputs.txt')

##################
### submission ###
##################

# prepare data submission
dataSubmission = {}# ('name', 'stream', maxNFilesPerJob, otherOptions)
for year in ['15', '16', '17',]:
    dataSubmission[year] = [
        ('Data' + year, '1l',        '10',  '--long'),
        ('Data' + year, '1l_fakes',  '10',  '--long'),
        ('Data' + year, '2l',        '10',  '--long'),
        ('Data' + year, '1l_ftrig',  '10',  '--long'),
        ('Data' + year, '1l_jer',    '10',  '--long'),
        ('Data' + year, '2l_jer',    '10',  '--long'),
        ('Data' + year, '0l',        '10',  '--long'),
        ]

# prepare mc submission
mc16Submission = {}# ('name', 'stream', maxNFilesPerJob, otherOptions)
for campaign in ['mc16a','mc16c','mc16d',]:
    mc16Submission[campaign] = [
        ('ttH'        + '_' + campaign, '1l',       '3',  '--long' ),
        ('ttbar'      + '_' + campaign, '1l',       '3',  '--long' ),
        ('single-top' + '_' + campaign, '1l',      '10',  '--long' ),
        ('ttV'        + '_' + campaign, '1l',      '10',  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '1l',      '10',  '--long' ),
        ('Wjets'      + '_' + campaign, '1l',      '10',  '--long' ),
        ('Zjets'      + '_' + campaign, '1l',      '10',  '--long' ),
        ('diboson'    + '_' + campaign, '1l',      '10',  '--long' ),
        ('Hplus'      + '_' + campaign, '1l',       '3',  '--long' ),
        ('ttH'        + '_' + campaign, '2l',       '3',  '--long' ),
        ('ttbar'      + '_' + campaign, '2l',       '3',  '--long' ),
        ('single-top' + '_' + campaign, '2l',      '10',  '--long' ),
        ('ttV'        + '_' + campaign, '2l',      '10',  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '2l',      '10',  '--long' ),
        ('Wjets'      + '_' + campaign, '2l',      '10',  '--long' ),
        ('Zjets'      + '_' + campaign, '2l',      '10',  '--long' ),
        ('diboson'    + '_' + campaign, '2l',      '10',  '--long' ),
        ('Hplus'      + '_' + campaign, '2l',       '3',  '--long' ),
        ('ttH'        + '_' + campaign, '1l_sys',  '1' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '1l_sys',  '1' ,  '--long' ),
        ('single-top' + '_' + campaign, '1l_sys',  '1' ,  '--long' ),
        ('ttV'        + '_' + campaign, '1l_sys',  '1' ,  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '1l_sys',  '1' ,  '--long' ),
        ('Wjets'      + '_' + campaign, '1l_sys',  '3' ,  '--long' ),
        ('Zjets'      + '_' + campaign, '1l_sys',  '3' ,  '--long' ),
        ('diboson'    + '_' + campaign, '1l_sys',  '3' ,  '--long' ),
        ('Hplus'      + '_' + campaign, '1l_sys',  '3' ,  '--long' ),
        ('ttH'        + '_' + campaign, '2l_sys',  '1' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '2l_sys',  '1' ,  '--long' ),
        ('single-top' + '_' + campaign, '2l_sys',  '1' ,  '--long' ),
        ('ttV'        + '_' + campaign, '2l_sys',  '1' ,  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '2l_sys',  '1' ,  '--long' ),
        ('Wjets'      + '_' + campaign, '2l_sys',  '3' ,  '--long' ),
        ('Zjets'      + '_' + campaign, '2l_sys',  '3' ,  '--long' ),
        ('diboson'    + '_' + campaign, '2l_sys',  '3' ,  '--long' ),
        ('Hplus'      + '_' + campaign, '2l_sys',  '3' ,  '--long' ),
        ('ttH'        + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('single-top' + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('ttV'        + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('Wjets'      + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('Zjets'      + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('diboson'    + '_' + campaign, '1l_lrj',  '3' ,  '--long' ),
        ('ttH'        + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('single-top' + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('ttV'        + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('Wjets'      + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('Zjets'      + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('diboson'    + '_' + campaign, '1l_jer',  '3' ,  '--long' ),
        ('ttH'        + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('single-top' + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('ttV'        + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('Wjets'      + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('Zjets'      + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('diboson'    + '_' + campaign, '2l_jer',  '3' ,  '--long' ),
        ('ttH'        + '_' + campaign, '0l',      '3' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '0l',      '3' ,  '--long' ),
        ('single-top' + '_' + campaign, '0l',      '10',  '--long' ),
        ('ttV'        + '_' + campaign, '0l',      '10',  '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '0l',      '10',  '--long' ),
        ('ttH'        + '_' + campaign, '0l_sys',  '1' ,  '--long' ),
        ('ttbar'      + '_' + campaign, '0l_sys',  '1' ,  '--long' ),
        ('single-top' + '_' + campaign, '0l_sys',  '1',   '--long' ),
        ('ttV'        + '_' + campaign, '0l_sys',  '1',   '--long' ),
        ('ttWW3t4t'   + '_' + campaign, '0l_sys',  '1',   '--long' ),
    ]

# filter the lists in one place
def filterSubmission(submissionFilter, submissionList):
    print "Applying submission filtering..."
    for key in submissionList:
        temp = []
        for sub in submissionFilter:
            temp.extend([x for x in submissionList[key] if x[0] == sub[0] and x[1] == sub[1]])
        submissionList[key] = temp
    print "Selected configurations..."
    for key in submissionList:
        for sub in submissionList[key]:
            print "->",key,":",sub
    return submissionList

### -- Add your filtering options here (name, channel)
dataSubmission = filterSubmission(submissionFilterList, dataSubmission)
mc16Submission = filterSubmission(submissionFilterList, mc16Submission)

# Provide an exit if we almost submit the wrong thing
check = raw_input("We have applied the filtering rules listed above. \nAre these what you expect to submit? (y/n)")
if check != "y":
    sys.exit(1)

checkSuffix = raw_input("We will use the following suffix for this submission: " + suffix + "\nIs this correct? (y/n)")
if checkSuffix != "y":
    sys.exit(2)

# some helper function
def SubmissionHelper(submissionList=dataSubmission,sampleType="data",justchecking=False,outDSListFile=None):
    for submission in submissionList:
        print "Checking submission for samples "+submission[0]+" for stream "+submission[1]
        # Catch cases where we do not generate a sample list for a configuration
        try:
            samples = TTHbbLeptonic.grid.Samples([submission[0]+'_'+submission[1]])
        except KeyError:
            print "No samples available/requested for :",submission[0]+'_'+submission[1],"-> Skipping."
            print ""
            continue
        except:
            raise Exception("Unexpected error has occured")

        configs[submission[1]+'_'+sampleType].maxNFilesPerJob = submission[2]
        configs[submission[1]+'_'+sampleType].otherOptions = submission[3]
        if justchecking:
            configs[submission[1]+'_'+sampleType].details()
        else:
            TTHbbLeptonic.grid.submit(configs[submission[1]+'_'+sampleType], samples)

# first check everything
print ''
print 'Checking jobs configurations......'
for Year in dataSubmission:
    print "Checking data",Year
    SubmissionHelper(dataSubmission[Year], "data"+Year, True)
for MCcampaign in ['mc16a','mc16c','mc16d']:
    print "Checking MC",MCcampaign
    SubmissionHelper(mc16Submission[MCcampaign], MCcampaign, True)

# then do the actual submission
print ''
print 'Launching jobs......'
for Year in dataSubmission:
    SubmissionHelper(dataSubmission[Year], "data"+Year, False)
for MCcampaign in ['mc16a','mc16c','mc16d']:
    SubmissionHelper(mc16Submission[MCcampaign], MCcampaign, False)
