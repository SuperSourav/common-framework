# This script is designed to be run on the TTHAnalysis online output file
# And quickly plot all variable it contains
# It should therefore be as generic as possible, dynamically adapting
# Ian Connelly
# 16 Jan 2018

# Dependencies
import ROOT, sys
ROOT.gROOT.SetBatch(True)

# Functions
def GetDirectories(infile):
    """ Return all TDirectories """
    directories = []
    for obj in infile.GetListOfKeys():
        if type( obj.ReadObj() ) == ROOT.TDirectory:
            directories.append(obj.GetName())
    return directories

def GetAllHistograms(indir):
    """ Return all histogram paths in directory path """
    histogrampaths = []
    for obj in indir.GetListOfKeys():
        if type( obj.ReadObj() ) == ROOT.TH1D or type( obj.ReadObj() ) == ROOT.TH1F:
            histogrampaths.append( indir+"/"+obj.GetName() )
    return histogrampaths

def GetTrees(infile):
    """ Return all TTree names """
    trees = []
    for obj in infile.GetListOfKeys():
        if type(obj.ReadObj()) == ROOT.TTree:
            trees.append( obj.GetName() )
    return trees

def GenerateDrawCommandsForAllBranches(infile, treename):
    intree = infile.Get(treename)
    branches = []
    for b in intree.GetListOfBranches():
        branches.append( b.GetName() )
    return branches

def DrawHistogram(infile,histpath):
    h = infile.Get(histpath)
    histname = histpath.replace("/","_")
    c = ROOT.TCanvas("c_"+histname,"c_"+histname,1000,1000)
    c.cd()
    h.SetLineWidth(4)
    h.Draw("HIST")
    c.SaveAs(histname+".png")
    return

def DrawBranch(inputFile, treename, branch):
    tree = inputFile.Get(treename)
    histname = treename+"_"+branch
    tree.Draw(branch + ">> h_"+histname)
    h = ROOT.gDirectory.Get("h_"+histname)
    c = ROOT.TCanvas("c_"+histname,"c_"+histname,1000,1000)
    c.cd()
    h.SetLineWidth(4)
    h.Draw("HIST")
    c.SaveAs(histname+".png")
    return

def main():
    inputFileName = sys.argv[1]
    inputFile     = ROOT.TFile.Open(inputFileName)
    # Get all the objects
    directories   = GetDirectories(inputFile)
    histograms    = []
    trees         = GetTrees(inputFile)
    branches      = {}
    for directory in directories:
        histograms.append(GetAllHistograms(directory))
    for tree in trees:
        branches[tree] = GenerateDrawCommandsForAllBranches(inputFile, tree)
    # Print all
    for h in histograms:
        print h
        DrawHistogram(inputFile,h)
    for t in branches:
        for b in branches[t]:
            print t,"->",b
            DrawBranch(inputFile, t, b)


if __name__ == "__main__":
    main()
