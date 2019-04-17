import ROOT
import numpy as np
import root_numpy as rnp


def main():
  ROOT.gROOT.SetBatch(1)
  ROOT.gStyle.SetOptStat(0)
  njet = "all"
  #process = {"sig1lnotTwB1wt1000tminfixed_5K": "sig", "bkg1lnotTwB1wt1000tminfixed_5K": "bkg"}
  #process = {"sig1lnotTwB1wt100_50K": "sig", "bkg1lnotTwB1wt100_50K": "bkg"}
  process = {"sig1lnotTwB1wt1000_50K": "sig", "bkg1lnotTwB1wt1000_50K": "bkg"}
  #process = "sig1lnotTwB1wt1000_5K"
  #process = "bkg1lnotTwB1wt1000_5K"
  #process = "sig1lnotTwB1wt1000_1K"
  #process = "bkg1lnotTwB1wt1000_1K"
  #process = "sig1lnotTwB1wt100_1K"
  #process = "bkg1lnotTwB1wt100_1K"
  #process = "sig"
  #process = "bkg"
  #process = "sigOld"
  #process = "bkgOld"
  #njet = "ge10" 
  #njet = "9" 
  #njet = "8" 
  #njet = "7" 
  #njet = "6" 
  #njet = "5"
  tlist = {}
  for p in process.keys():
    if (njet=="all"):
      t = ROOT.TChain("nominal_Loose")
      jet_cat = ["ge10", "9", "8", "7", "6", "5"]
      for jetc in jet_cat:
        fj = ROOT.TFile("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/test_%s/ljets%sj/ttbar_powpyt8.root"%(p, jetc), 'READ')
        t.Add("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/test_%s/ljets%sj/ttbar_powpyt8.root"%(p, jetc))
    else:
      f = ROOT.TFile("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/test_%s/ljets%sj/ttbar_powpyt8.root"%(p, njet), 'READ')
      f.ls()
      t = f.Get("nominal_Loose")
    t.ls()
    tlist[process[p]+"tree"] = t


   
  #bucket type count
  Stwcount = rnp.tree2array(tlist["sigtree"], branches="twcount")
  Stmincount = rnp.tree2array(tlist["sigtree"], branches="tmincount")
  St0count = rnp.tree2array(tlist["sigtree"], branches="t0count")

  ##
  SmW0 = rnp.tree2array(tlist["sigtree"], branches="mW0") 
  SmW1 = rnp.tree2array(tlist["sigtree"], branches="mW1")
  SmBucketPrim0 = rnp.tree2array(tlist["sigtree"], branches="mBucketPrim0")
  SmBucketPrim1 = rnp.tree2array(tlist["sigtree"], branches="mBucketPrim1")
  SmBucketPrim = np.concatenate((SmBucketPrim0,SmBucketPrim1), axis=None)
  Stwmass0  = rnp.tree2array(tlist["sigtree"], branches="twmass0")
  StwPt0  = rnp.tree2array(tlist["sigtree"], branches="twPt0")
  SNaddjets  = rnp.tree2array(tlist["sigtree"], branches="Naddjets")

  #bucket type count
  Btwcount = rnp.tree2array(tlist["bkgtree"], branches="twcount")
  Btmincount = rnp.tree2array(tlist["bkgtree"], branches="tmincount")
  Bt0count = rnp.tree2array(tlist["bkgtree"], branches="t0count")

  ##
  BmW0 = rnp.tree2array(tlist["bkgtree"], branches="mW0") 
  BmW1 = rnp.tree2array(tlist["bkgtree"], branches="mW1")
  BmBucketPrim0 = rnp.tree2array(tlist["bkgtree"], branches="mBucketPrim0")
  BmBucketPrim1 = rnp.tree2array(tlist["bkgtree"], branches="mBucketPrim1")
  BmBucketPrim = np.concatenate((BmBucketPrim0,BmBucketPrim1), axis=None)
  Btwmass0  = rnp.tree2array(tlist["bkgtree"], branches="twmass0")
  BtwPt0  = rnp.tree2array(tlist["bkgtree"], branches="twPt0")
  BNaddjets  = rnp.tree2array(tlist["bkgtree"], branches="Naddjets")


  
  c0 = ROOT.TCanvas('c', 'c', 800, 600)
  leg0 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg0.SetFillColor(0)
  leg0.SetLineColor(0)
  #bucket type count
  ShNaddjets = ROOT.TH1F("signal hNaddjets", "", 21, -0.5, 20.5)
  fill_hist(ShNaddjets, SNaddjets, "additional jets per event", "a.u.")
  ShNaddjets.Scale(1./(ShNaddjets.Integral()))
  ShNaddjets.SetLineColor(ROOT.kRed)
  BhNaddjets = ROOT.TH1F("bkg hNaddjets", "", 21, -0.5, 20.5)
  fill_hist(BhNaddjets, BNaddjets, "additional jets per event", "a.u.")
  BhNaddjets.Scale(1./(BhNaddjets.Integral()))
  BhNaddjets.SetLineColor(ROOT.kBlack)
  ShNaddjets.SetMaximum(max(ShNaddjets.GetMaximum(), BhNaddjets.GetMaximum())*1.1)
  ShNaddjets.Draw("hist")
  leg0.AddEntry(ShNaddjets, 'signal', "L")
  BhNaddjets.Draw("hist same")
  leg0.AddEntry(BhNaddjets, 'ttbar+jets (bkg)', "L")
  leg0.Draw()
  c0.Print("OverlayhNaddjets_%sjetregion.eps"%njet)

  c1 = ROOT.TCanvas('c', 'c', 800, 600)
  leg1 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg1.SetFillColor(0)
  leg1.SetLineColor(0)
  #bucket type count
  Shtwcount = ROOT.TH1F("signal htwcount", "", 4, -0.5, 3.5)
  fill_hist(Shtwcount, Stwcount, "tw buckets per event", "a.u.")
  Shtwcount.Scale(1./(Shtwcount.Integral()))
  Shtwcount.SetLineColor(ROOT.kRed)
  Bhtwcount = ROOT.TH1F("bkg htwcount", "", 4, -0.5, 3.5)
  fill_hist(Bhtwcount, Btwcount, "tw buckets per event", "a.u.")
  Bhtwcount.Scale(1./(Bhtwcount.Integral()))
  Bhtwcount.SetLineColor(ROOT.kBlack)
  Shtwcount.SetMaximum(max(Shtwcount.GetMaximum(), Bhtwcount.GetMaximum())*1.1)
  Shtwcount.Draw("hist")
  leg1.AddEntry(Shtwcount, 'signal', "L")
  Bhtwcount.Draw("hist same")
  leg1.AddEntry(Bhtwcount, 'ttbar+jets (bkg)', "L")
  leg1.Draw()
  c1.Print("Overlayhtwcount_%sjetregion.eps"%njet)

  c2 = ROOT.TCanvas('c', 'c', 800, 600)
  leg2 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg2.SetFillColor(0)
  leg2.SetLineColor(0)
  #bucket type count
  ShmW0 = ROOT.TH1F("signal hmW0", "Mass of the (possible) W candidate in B1",150,0.0001,300)
  fill_hist(ShmW0, SmW0, "Mass (GeV)", "")
  ShmW0.Scale(1./(ShmW0.Integral()))
  ShmW0.SetLineColor(ROOT.kRed)
  BhmW0 = ROOT.TH1F("bkg hmW0", "Mass of the (possible) W candidate in B1",150,0.0001,300)
  fill_hist(BhmW0, BmW0, "Mass (GeV)", "")
  BhmW0.Scale(1./(BhmW0.Integral()))
  BhmW0.SetLineColor(ROOT.kBlack)
  ShmW0.SetMaximum(max(ShmW0.GetMaximum(), BhmW0.GetMaximum())*1.1)
  ShmW0.Draw("hist")
  leg2.AddEntry(ShmW0, 'signal', "L")
  BhmW0.Draw("hist same")
  leg2.AddEntry(BhmW0, 'ttbar+jets (bkg)', "L")
  leg2.Draw()
  c2.Print("OverlayhmW0_%sjetregion.eps"%njet)

  c3 = ROOT.TCanvas('c', 'c', 800, 600)
  leg3 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg3.SetFillColor(0)
  leg3.SetLineColor(0)
  #bucket type count
  ShmBucketPrim0 = ROOT.TH1F("signal hmBucketPrim0", "Mass of B1",150,0,300);
  fill_hist(ShmBucketPrim0, SmBucketPrim0, "Mass (GeV)", "")
  ShmBucketPrim0.Scale(1./(ShmBucketPrim0.Integral()))
  ShmBucketPrim0.SetLineColor(ROOT.kRed)
  BhmBucketPrim0 = ROOT.TH1F("bkg hmBucketPrim0", "Mass of B1",150,0,300);
  fill_hist(BhmBucketPrim0, BmBucketPrim0, "Mass (GeV)", "")
  BhmBucketPrim0.Scale(1./(BhmBucketPrim0.Integral()))
  BhmBucketPrim0.SetLineColor(ROOT.kBlack)
  ShmBucketPrim0.SetMaximum(max(ShmBucketPrim0.GetMaximum(), BhmBucketPrim0.GetMaximum())*1.1)
  ShmBucketPrim0.Draw("hist")
  leg3.AddEntry(ShmBucketPrim0, 'signal', "L")
  BhmBucketPrim0.Draw("hist same")
  leg3.AddEntry(BhmBucketPrim0, 'ttbar+jets (bkg)', "L")
  leg3.Draw()
  c3.Print("OverlayhmBucketPrim0_%sjetregion.eps"%njet)

  c4 = ROOT.TCanvas('c', 'c', 800, 600)
  leg4 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg4.SetFillColor(0)
  leg4.SetLineColor(0)
  #bucket type count
  ShmBucketPrim1 = ROOT.TH1F("signal hmBucketPrim1", "Mass of B2",150,0,300);
  fill_hist(ShmBucketPrim1, SmBucketPrim1, "Mass (GeV)", "")
  ShmBucketPrim1.Scale(1./(ShmBucketPrim1.Integral()))
  ShmBucketPrim1.SetLineColor(ROOT.kRed)
  BhmBucketPrim1 = ROOT.TH1F("bkg hmBucketPrim1", "Mass of B2",150,0,300);
  fill_hist(BhmBucketPrim1, BmBucketPrim1, "Mass (GeV)", "")
  BhmBucketPrim1.Scale(1./(BhmBucketPrim1.Integral()))
  BhmBucketPrim1.SetLineColor(ROOT.kBlack)
  ShmBucketPrim1.SetMaximum(max(ShmBucketPrim1.GetMaximum(), BhmBucketPrim1.GetMaximum())*1.1)
  ShmBucketPrim1.Draw("hist")
  leg4.AddEntry(ShmBucketPrim1, 'signal', "L")
  BhmBucketPrim1.Draw("hist same")
  leg4.AddEntry(BhmBucketPrim1, 'ttbar+jets (bkg)', "L")
  leg4.Draw()
  c4.Print("OverlayhmBucketPrim1_%sjetregion.eps"%njet)

#  htmincount = ROOT.TH1F("htmincount", "number of tmin buckets per event", 5, -0.5, 4.5)
#  fill_hist(htmincount, tmincount, "tminbuckets per event", "")
#  htmincount.Draw()
#  c.Print("htmincount_%sjetregion.eps"%njet)
#  ht0count = ROOT.TH1F("ht0count", "number of t0 buckets per event", 5, -0.5, 4.5)
#  fill_hist(ht0count, t0count, "t0buckets per event", "")
#  ht0count.Draw()
#  c.Print("ht0count_%sjetregion.eps"%njet)
#
#  #mass
#  htwmass = ROOT.TH1F("htwmass", "Mass of tw Buckets",150,0.0001,300);
#  fill_hist(htwmass, twmass, "Mass (Gev)", "")
#  htwmass.Draw()
#  c.Print("htwmass_%sjetregion.eps"%njet)
#  htminmass = ROOT.TH1F("htminmass", "Mass of t- Buckets",150,0.0001,300); 
#  fill_hist(htminmass, tminmass, "Mass (Gev)", "")
#  htminmass.Draw()
#  c.Print("htminmass_%sjetregion.eps"%njet)
#  ht0mass = ROOT.TH1F("ht0mass", "Mass of t0 Buckets",150,0.0001,300); 
#  fill_hist(ht0mass, t0mass, "Mass (Gev)", "")
#  ht0mass.Draw()
#  c.Print("ht0mass_%sjetregion.eps"%njet)
#  hXmass = ROOT.TH1F("hXmass", "Mass of the extra jets",110,-1,10); 
#  fill_hist(hXmass, Xmass, "Mass (Gev)", "")
#  hXmass.Draw()
#  c.Print("hXmass_%sjetregion.eps"%njet)
#  hXmass_more = ROOT.TH1F("hXmass_full", "Mass of the extra jets",300,-1,30); 
#  fill_hist(hXmass_more, Xmass, "Mass (Gev)", "")
#  hXmass_more.Draw()
#  c.Print("hXmass_full_%sjetregion.eps"%njet)
#  #pT
#  htwPt = ROOT.TH1F("htwPt", "Pt of tw Buckets",250,0,1200); 
#  fill_hist(htwPt, twPt, "pT (Gev)", "")
#  htwPt.Draw()
#  c.Print("htwPt_%sjetregion.eps"%njet)
#  htminPt = ROOT.TH1F("htminPt", "Pt of t- Buckets",250,0,1200); 
#  fill_hist(htminPt, tminPt, "pT (Gev)", "")
#  htminPt.Draw()
#  c.Print("htminPt_%sjetregion.eps"%njet)
#  ht0Pt = ROOT.TH1F("ht0Pt", "Pt of t0 Buckets",250,0,1200); 
#  fill_hist(ht0Pt, t0Pt, "pT (Gev)", "")
#  ht0Pt.Draw()
#  c.Print("ht0Pt_%sjetregion.eps"%njet)
#  hXPt = ROOT.TH1F("hXPt", "Pt of the extra jets",100,0,500); 
#  fill_hist(hXPt, XPt, "pT (Gev)", "")
#  hXPt.Draw()
#  c.Print("hXPt_%sjetregion.eps"%njet)
#  #eta
#  htweta = ROOT.TH1F("htweta", "#eta of tw Buckets",100,-10,10); 
#  fill_hist(htweta, tweta, "eta", "")
#  htweta.Draw()
#  c.Print("htweta_%sjetregion.eps"%njet)
#  htmineta = ROOT.TH1F("htmineta", "#eta of t- Buckets",100,-10,10); 
#  fill_hist(htmineta, tmineta, "eta", "")
#  htmineta.Draw()
#  c.Print("htmineta_%sjetregion.eps"%njet)
#  ht0eta = ROOT.TH1F("ht0eta", "#eta of t0 Buckets",100,-10,10);
#  fill_hist(ht0eta, t0eta, "eta", "")
#  ht0eta.Draw()
#  c.Print("ht0eta_%sjetregion.eps"%njet)
#  hXeta = ROOT.TH1F("hXeta", "#eta of the extra jets",100,-10,10);
#  fill_hist(hXeta, Xeta, "eta", "")
#  hXeta.Draw()
#  c.Print("hXeta_%sjetregion.eps"%njet)
#  #W candidate mass
#  hmW = ROOT.TH1F("hmW", "Mass of the (possible) W candidate",150,0.0001,300); 
#  fill_hist(hmW, mW, "Mass (GeV)", "")
#  hmW.Draw()
#  c.Print("hmW_%sjetregion.eps"%njet)
#  hmBucketPrim = ROOT.TH1F("hmBucketPrimitive", "Mass of the Entire Buckets before Recalculation",150,0,300); 
#  fill_hist(hmBucketPrim, mBucketPrim, "Mass (GeV)", "")
#  hmBucketPrim.Draw()
#  c.Print("hmBucketPrim_%sjetregion.eps"%njet)
#  hmratio = ROOT.TH1F("hmratio", "Mass Ratio Difference",120,-0.1,1.1); 
#  fill_hist(hmratio, mratio, "Mass ratio difference", "")
#  hmratio.Draw()
#  c.Print("hmratio_%sjetregion.eps"%njet)


def fill_hist(h, arr, Xtitle, Ytitle):
  h.GetXaxis().SetTitle(Xtitle)
  h.GetYaxis().SetTitle(Ytitle)
  [h.Fill(_) for _ in arr if (_ > -900)]



if __name__ == '__main__':
  main()
