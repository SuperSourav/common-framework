import os
import sys
import ROOT
from PathResolver import PathResolver
import grid

## checks if an AMI tag is of a certain type
def CheckTag(tag,letter):
    if tag.find(letter,0,1) != -1 and tag[1:].isdigit():
        return True

## reduces the AMI tag string
def ReduceAMITags(AMITagsString):
    # first fill lists of tag of each type
    tags = AMITagsString.split('_')
    eTags = []
    sTags = []
    aTags = []
    rTags = []
    fTags = []
    mTags = []
    pTags = []
    for tag in tags:
        if CheckTag(tag,"e"):
            eTags.append(tag)
        elif CheckTag(tag,"s"):
            sTags.append(tag)
        elif CheckTag(tag,"a"):
            aTags.append(tag)
        elif CheckTag(tag,"r"):
            rTags.append(tag)
        elif CheckTag(tag,"f"):# for data derivations
            fTags.append(tag)
        elif CheckTag(tag,"m"):# for data derivations
            mTags.append(tag)
        elif CheckTag(tag,"p"):
            pTags.append(tag)
        else:
            print "Error: can't understand \'"+tag+"\'; are you sure it's an AMI tag?"
            sys.exit()
    
    # we keep the first tag of each type - the rest is for the merge step, which we don't really care
    reducedTags = ""
    if len(eTags)>=1:
        reducedTags += eTags[0]
    if len(sTags)>=1:
        reducedTags += sTags[0]
    if len(aTags)>=1:
        reducedTags += aTags[0]
    if len(rTags)>=1:
        reducedTags += rTags[0]
    if len(fTags)>=1:
        reducedTags += fTags[0]
    if len(mTags)>=1:
        reducedTags += mTags[0]
    if len(pTags)>=1:
        reducedTags += pTags[0]
    return reducedTags

## complicate function to reduce the physics part of the sample name
def ReducePhysicsPart(physics):
    shortPhysics = "" # the shortened physics part of the name

    replacementMap = [] # we need an ordered map for this.... so make a list of 2-elements Ntuples
    replacementMap += [("MadGraphPythiaEvtGen","MGPy6")]
    replacementMap += [("MadGraphPythia8EvtGen","MGPy8")]
    replacementMap += [("aMcAtNloHerwigppEvtGen","aMCNloHpp")]
    replacementMap += [("PowhegPy8EG","PoPy8")]
    replacementMap += [("PowhegHerwig7EvtGen_H7UE","PoH7")]
    replacementMap += [("PowhegHerwigppEvtGen","PoHpp")]
    replacementMap += [("PowhegPythiaEvtGen","PoPy6")]
    replacementMap += [("PowhegPythia8EvtGen","PoPy8")]
    replacementMap += [("PowhegPy8EvtGen","PoPy8")]
    replacementMap += [("aMcAtNloPythia8EvtGen","aMCNloP8")]
    replacementMap += [("aMcAtNloHerwigppEvtGen","aMCNloHpp")]
    replacementMap += [("_CT10nloME_AZNLOCTEQ6L1","")]
    replacementMap += [("_A14NNPDF23LO","")]
    replacementMap += [("_A14NNPDF23LO","")]
    replacementMap += [("_A14NNPDF23","")]
    replacementMap += [("_NNPDF23_NNPDF30ME","")]
    replacementMap += [("_AZNLOCTEQ6L1","")]
    replacementMap += [("_NN30NNLO","")]
    replacementMap += [("_CTEQ6L1_CT10ME","")]
    replacementMap += [("_UEEE5","")]
    replacementMap += [("_A14_CT10ME","")]
    replacementMap += [("_A14","")]
    replacementMap += [("_P2012","")]
    replacementMap += [("Sherpa_CT10","Sherpa")]
    replacementMap += [("Sherpa_NNPDF30NNLO","Sherpa")]
    replacementMap += [("_MEPS_NLO","")]
    replacementMap += [("_CVetoBVeto","CVBV")]
    replacementMap += [("_CFilterBVeto","CFBV")]
    replacementMap += [("_BFilter","BF")]
    replacementMap += [("_BVeto","BV")]
    replacementMap += [("enu","en")]
    replacementMap += [("munu","mn")]
    replacementMap += [("taunu","tn")]
    replacementMap += [("mumu","mm")]
    replacementMap += [("tautau","tt")]
    replacementMap += [("_Np","Np")]
    replacementMap += [("_Pt","Pt")]
    replacementMap += [("_Mll10to40","lowM")]
    replacementMap += [("_Mll10_40","lowM")]
    replacementMap += [("nonallhad","lep")]
    replacementMap += [("noAllHad","lep")]
    replacementMap += [("dilepton","dil")]
    replacementMap += [("DiLepton","dil")]
    replacementMap += [("semilep","ljets")]
    replacementMap += [("SingleLepton","ljets")]
    replacementMap += [("SingleLep","ljets")]
    replacementMap += [("AllHadronic","allhad")]
    replacementMap += [("lept","lep")]
    replacementMap += [("_tt_hdamp258p75","_ttbar")]
    replacementMap += [("_ttbar_hdamp517p5","_ttbar_hdUp")]
    replacementMap += [("_ttbar_hdamp258p75","_ttbar")]
    replacementMap += [("ttbar_hdamp172p5","ttbar")]
    replacementMap += [("ttbar_hdamp345_down","ttbar")]
    replacementMap += [("ttbar_hdamp172_up","ttbar")]
    replacementMap += [("_top","_t")]
    replacementMap += [("_antitop","_tbar")]
    replacementMap += [("singletop_tchan","tchan")]
    replacementMap += [("SingleTopSchan","schan")]
    replacementMap += [("Wt_inclusive","Wt")]
    replacementMap += [("Wt_DS","WtDS")]
    replacementMap += [("tZ_4fl_tchan","tZ")]
    replacementMap += [("_H125_a","_H125a")]
    replacementMap += [("_bbbb","bbbb")]
    replacementMap += [("_Hplus4FS","_Hp")]
    replacementMap += [("MadGraphHerwigppEvtGen","MGHpp")]
    replacementMap += [("aMcAtNloHppEG","aMCNloHpp")]
    replacementMap += [("_yt_minus1","")]
    replacementMap += [("_yt_plus1","")]
    replacementMap += [("_yt_plus2","")]
    replacementMap += [("tHjb125_gamgam","tHjb125gg")]
    replacementMap += [("tHjb125_bbbar","tHjb125bb")]
    replacementMap += [("tHjb125_lep","tHjb125lep")]
    replacementMap += [("tWH125_gamgam","tWH125gg")]
    replacementMap += [("tWH125_bbbar","tWH125bb")]
    replacementMap += [("tWH125_lep","tWH125lep")]
    replacementMap += [("_ttH125_gamgam","_ttH125_gg")]
    replacementMap += [("_SHv21_improved","impr")]
    replacementMap += [("_MAXHTPTV","_MAXHP")]
    replacementMap += [("_NNPDF30NNLO","")]
    replacementMap += [("_NNPDF23LO","")]
    replacementMap += [("_NNPDF30ME","")]
    replacementMap += [("_260000","")]
    replacementMap += [("_MEN30NLON23LO","")]
    replacementMap += [("_MEN30NLO_A14N23LO","")]
    replacementMap += [("Sherpa_221","She221")]
    replacementMap += [("Sherpa","She")]
    replacementMap += [("bfilter","bfil")]
    replacementMap += [("Pythia8EvtGen","P8")]

    # we start with the original name
    shortPhysics = physics
    # now loop on the replacement and make the string substitutions in order
    for replacement in replacementMap:
        shortPhysics = shortPhysics.replace(replacement[0],replacement[1])
    return shortPhysics


## fancy function to shorten the physics part of the name of a sample
## the aim is to 1) save space because the sample name size is limited on grid, and 2) keep relevant information in the sample name
def OutNameFormat(superLongInDSName):
    # split inDS name
    splitted = superLongInDSName.split('.')
    outDSName = ""
    
    sampleType = splitted[0] # the sample type, e.g. data17_13TeV
    if sampleType == "user" or sampleType == "group": # if it's a private derivation (user or group), we do something special
        for part in splitted[1:]: # we don't take the "user" or "group" since we'll re-add it anyway
            outDSName += part.replace('_','').replace('/','') # we remove the unnecessary characters which make the sample name too long
            if part != splitted[-1]:
                outDSName += "."
        return outDSName
    
    runNumber = splitted[1] # the run number (Data run) or channel ID (MC sample) or period (Data containers)
    physicsName = splitted[2] # the "physics" part of the MC sample, or "physics_Main" in case of Data
    prodStep = splitted[3] # the production step, either "deriv" for derivations, "merge" for merged primary xAOD, or "PhysCont" for containers
    derivType = splitted[4] # the type of the derivation, e.g. "DAOD_TOPQ1"
    amiTags = splitted[5].replace('/','') # the whole AMI tags
    
    outDSName += sampleType + "." # add the sample type
    
    outDSName += runNumber + "." # add the run number (Data run) or channel ID (MC sample) or period (Data containers)
    
    # deal with physics part
    if physicsName != "physics_Main": # we only need this on MC
        outDSName += ReducePhysicsPart(physicsName) + "."
    
    # deal with derivation type
    if derivType.find("DAOD_") == -1:
        print "Error: the type \'"+derivType+"\' doesn't seem to be a derivation - this is wrong, you can only run on derivations!"
        sys.exit()
    else:
        outDSName += derivType[5:] + "." # add the derivation type, without the trailing "DAOD_"
    
    # deal with AMI tags
    if physicsName == "physics_Main" and prodStep == "PhysCont": # for data containers we don't need to care much about the tags
        outDSName += amiTags.replace('_','')
    else: # otherwise we use our great reduction function
        outDSName += ReduceAMITags(amiTags)
    
    return outDSName


## produces TopExamples.gid.Sample instances using a list of samples as input
def MakeSamplesLists(filename,samplesSuffix=''):
    # try to find the file with the list of samples; if not found use PathResolver
    filePath = ""
    if os.path.exists(filename):
        print filename + " found. Good!"
        filePath = filename
    else:
        print filename + " not found. Trying with PathResolver...."
        filePath = ROOT.PathResolver.find_file(filename, "DATAPATH", ROOT.PathResolver.RecursiveSearch)
        if filePath == "":
            print "Error: PathResolver can't find file "+filename+". Exiting..."
            sys.exit()
        else:
            print "Using "+ filePath
    
    # read input file line by line
    f = open(filePath,"r")
    categories = [ "Other" ]
    streams = []
    samples = {}
    allSamples = []
    isFirstLine = True
    category = categories[0] # initialise to default "Other" category (in case of samples are listed at the beginning of the spreadsheet, without any category)
    for line in f:
        fields = line.split()
        # in the first line are the streams defined
        if isFirstLine:
            isFirstLine = False
            if fields[0] != 'Samples':
                print "Error: the first line of "+filePath+" should start with \'Samples\', and then have the list of streams. Exiting..."
                sys.exit()
            streams = fields[1:]
            # initialise samples to empty lists of samples
            for st in streams:
                samples[(category,st.rstrip())] = []
            continue
        else:
            # skip empty lines
            if fields==[]:
                continue
            # if line starts with ###, it's a new samples category
            if fields[0].find('###')==0:
                category = line[3:].lstrip().rstrip()
                if category not in categories:
                    categories.append(category)
                    # initialise samples to empty lists of samples
                    for st in streams:
                        samples[(category,st)] = []
                    continue
                continue
            # ignore lines starting with a #
            elif fields[0].find('#')==0:
                continue
            else:
                # otherwise, this is a sample name
                sample = fields[0].rstrip()
                allSamples.append(sample)
                # for each sample, add it to list for each stream, if marked as "Y"
                index = 0
                for field in fields[1:]:
                    if (field == 'Y'):
                        samples[(category,streams[index])].append(sample)
                    index += 1
    # now loop on categories
    for category in categories:
        for stream in streams:
            sampleList = samples[(category,stream)]
            if len(sampleList)!=0:
                name = category + "_" + stream
                print "Building Samples list: " + name
                grid.Add(name).datasets = sampleList
    # do a check of all samples, to see if they exist
    actuallyExists = []
    for ds in allSamples:
        if grid.checkDatasetExists(grid.getShortenedConcatenatedSample(ds)):
            actuallyExists.append(ds)
    print ''
    print 'For your information, in '+filename+' the following samples were found (or not):'
    for i, l in enumerate(allSamples):
        note = 'Not found'
        if l in actuallyExists:
            note = 'Found'
        print '   -%2d %s %s' % (i, l, note)
