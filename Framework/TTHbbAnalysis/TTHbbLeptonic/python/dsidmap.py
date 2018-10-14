# Small class to load the map text file and create objects to hold this
# We can then use that in our download scripts
# Author: Ian Connelly
# 07 Feb 2018

class Sample(object):
    def __init__(self):
        self.Name     = ""
        self.Comment  = ""
        self.DSIDList = set()

    def setSampleName(self, name):
        self.Name = name

    def setComment(self, comment):
        self.Comment = comment

    def addDSIDs(self, dsidlist):
        for dsid in dsidlist.split(","):
            if dsid.strip() != "":
                self.DSIDList.add(dsid.strip())

class DSIDMap(object):
    def __init__(self):
        self.Filename   = ""
        self.File       = ""
        self.SampleList = []

    def debug(self):
        print "Debugging Sample objects in DSIDMap..."
        for S in sorted(self.SampleList, key = lambda x : x.Name):
            print " Sample    :",S.Name, "( ",S.Comment," )"
            print " DSID list :",",".join(S.DSIDList)

    def loadDSIDMapFile(self, filename):
        self.Filename = filename
        print "Loading", self.Filename
        self.File     = open(filename, "r")

    def generateMap(self):
        # Read lines in the file
        for line in self.File.readlines():
            # Ignore commented lines
            if line.startswith("#"):
                continue
            # Process the line, use ":" as delimiter
            try:
                samplename, comment, dsids = line.strip().split(":")
            except:
                print "Poorly formatted line in",self.filename
                print line.strip()
                continue
            # Use the sample class
            S = Sample()
            S.setSampleName(samplename)
            S.setComment(comment)
            S.addDSIDs(dsids)
            self.SampleList.append(S)

    def getSampleFromDSID(self, dsid):
        for S in self.SampleList:
            if dsid in S.DSIDList:
                print dsid, S.Name
                return S.Name
        print "No sample defined for",dsid,"so use the DSID"
        return dsid
