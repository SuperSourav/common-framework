import ROOT
import numpy as np
import root_numpy as rnp


def main():
  ROOT.gROOT.SetBatch(1)
  ROOT.gStyle.SetOptStat(0)
  Snonbinitcount  = {}
  Snonb1count  = {}
  Snonb2count  = {}
  SnonbISRcount  = {}
  for p in process.keys():
    if (njet=="all"):
      t = ROOT.TChain("nominal_Loose")
      jet_cat = ["ge10", "9", "8", "7", "6", "5"]
      for jetc in jet_cat:
        fj = ROOT.TFile("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/%s/ljets%sj/ttbar_powpyt8.root"%(p, jetc), 'READ')
        t.Add("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/%s/ljets%sj/ttbar_powpyt8.root"%(p, jetc))
    else:
      f = ROOT.TFile("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/%s/ljets%sj/ttbar_powpyt8.root"%(p, njet), 'READ')
      f.ls()
      t = f.Get("nominal_Loose")
    t.ls()
    Snonbinitcount[p] = rnp.tree2array(t, branches="nonbinitcount")
    Snonb1count[p] = rnp.tree2array(t, branches="nonb1count")
    Snonb2count[p] = rnp.tree2array(t, branches="nonb2count")
    SnonbISRcount[p]  = rnp.tree2array(t, branches="nonbISRcount")
  plotter("before buckets", "nonbinit", Snonbinitcount)
  plotter("in B1", "nonb1", Snonb1count)
  plotter("in B2", "nonb2", Snonb2count)
  plotter("in BISR", "nonbISR", SnonbISRcount)

def plotter(title_string, file_name, countdict):
  c = ROOT.TCanvas('c', 'c', 800, 600)
  leg = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg.SetFillColor(0)
  leg.SetLineColor(0)
  maxY = []
  histdict = {}
  for p in process.keys():
    h = ROOT.TH1F("non b jets %s (%s)"%(title_string, process[p]), "", 16, -0.5, 15.5)
    fill_hist(h, countdict[p], "nonb jets per event %s"%title_string, "a.u.")
    maxY.append(h.GetMaximum())
    histdict[p] = h
  counter = 0
  for p in process.keys():
    counter += 1
    print p, processcolor[p]
    histdict[p].SetLineColor(processcolor[p])
    if counter==1:
      histdict[p].SetMaximum(max(maxY)*1.05)	  
      histdict[p].Draw("hist")
    else:
      histdict[p].Draw("hist same")
    leg.AddEntry(histdict[p], process[p], 'L')
  leg.Draw()
  c.Print("%s.eps"%file_name)
  return


def fill_hist(h, arr, Xtitle, Ytitle):
  h.GetXaxis().SetTitle(Xtitle)
  h.GetYaxis().SetTitle(Ytitle)
  [h.Fill(_) for _ in arr if (_ > -900)]



if __name__ == '__main__':
  b1wt = [1, 10, 100]
  process = {}
  processcolor = {}
  for i in range(len(b1wt)):
    process["testB1wt%i"%b1wt[i]]="B1wt=%i"%b1wt[i]
    processcolor["testB1wt%i"%b1wt[i]]=i+1
  njet = "all"
  #njet = "ge10" 
  #njet = "9" 
  #njet = "8" 
  #njet = "7" 
  #njet = "6" 
  #njet = "5"
  main()
