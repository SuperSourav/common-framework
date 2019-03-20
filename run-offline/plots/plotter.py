import ROOT
import numpy as np
import root_numpy as rnp


def main():
  ROOT.gROOT.SetBatch(1)
  njet = "all" 
  #njet = "ge10" 
  #njet = "9" 
  #njet = "8" 
  #njet = "7" 
  #njet = "6" 
  #njet = "5"
  if (njet=="all"):
    t = ROOT.TChain("nominal_Loose")
    jet_cat = ["ge10", "9", "8", "7", "6", "5"]
    for jetc in jet_cat:
      fj = ROOT.TFile("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/test/ljets%sj/ttbar_powpyt8.root"%jetc, 'READ')
      t.Add("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/test/ljets%sj/ttbar_powpyt8.root"%jetc)
  else:
    f = ROOT.TFile("/afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/run-offline/test/ljets%sj/ttbar_powpyt8.root"%njet, 'READ')
    f.ls()
    t = f.Get("nominal_Loose")
  t.ls()
  
  #bucket type count
  twcount = rnp.tree2array(t, branches="twcount")
  tmincount = rnp.tree2array(t, branches="tmincount")
  t0count = rnp.tree2array(t, branches="t0count")

  ##
  mW0 = rnp.tree2array(t, branches="mW0") 
  mW1 = rnp.tree2array(t, branches="mW1")
  mW = np.concatenate((mW0, mW1), axis=None)
  mBucketPrim0 = rnp.tree2array(t, branches="mBucketPrim0")
  mBucketPrim1 = rnp.tree2array(t, branches="mBucketPrim1")
  mBucketPrim = np.concatenate((mBucketPrim0,mBucketPrim1), axis=None)
  mratio0  = rnp.tree2array(t, branches="mratio0")
  mratio1 = rnp.tree2array(t, branches="mratio1")
  mratio = np.concatenate((mratio0, mratio1), axis=None)
  twmass0  = rnp.tree2array(t, branches="twmass0")
  twmass1 = rnp.tree2array(t, branches="twmass1")
  twmass = np.concatenate((twmass0, twmass1), axis=None)
  twPt0  = rnp.tree2array(t, branches="twPt0")
  twPt1  = rnp.tree2array(t, branches="twPt1")
  twPt = np.concatenate((twPt0, twPt1), axis=None)
  tweta0  = rnp.tree2array(t, branches="tweta0")
  tweta1  = rnp.tree2array(t, branches="tweta1")
  tweta = np.concatenate((tweta0, tweta1), axis=None)
  tminmass0 = rnp.tree2array(t, branches="tminmass0")
  tminmass1 = rnp.tree2array(t, branches="tminmass1")
  tminmass = np.concatenate((tminmass0, tminmass1), axis=None)
  tminPt0  = rnp.tree2array(t, branches="tminPt0")
  tminPt1  = rnp.tree2array(t, branches="tminPt1")
  tminPt = np.concatenate((tminPt0, tminPt1), axis=None)
  tmineta0  = rnp.tree2array(t, branches="tmineta0")
  tmineta1  = rnp.tree2array(t, branches="tmineta1")
  tmineta = np.concatenate((tmineta0, tmineta1), axis=None)
  t0mass0  = rnp.tree2array(t, branches="t0mass0")
  t0mass1  = rnp.tree2array(t, branches="t0mass1")
  t0mass = np.concatenate((t0mass0, t0mass1), axis=None)
  t0Pt0  = rnp.tree2array(t, branches="t0Pt0")
  t0Pt1  = rnp.tree2array(t, branches="t0Pt1")
  t0Pt = np.concatenate((t0Pt0, t0Pt1), axis=None)
  t0eta0  = rnp.tree2array(t, branches="t0eta0")
  t0eta1  = rnp.tree2array(t, branches="t0eta1")
  t0eta = np.concatenate((t0eta0, t0eta1), axis=None)
  Xmass0  = rnp.tree2array(t, branches="Xmass0")
  Xmass1  = rnp.tree2array(t, branches="Xmass1")
  Xmass2 = rnp.tree2array(t, branches="Xmass2")
  Xmass3 = rnp.tree2array(t, branches="Xmass3")
  Xmass = np.concatenate((Xmass0, Xmass1, Xmass2, Xmass3), axis=None)
  XPt0  = rnp.tree2array(t, branches="XPt0")
  XPt1  = rnp.tree2array(t, branches="XPt1")
  XPt2 = rnp.tree2array(t, branches="XPt2")
  XPt3 = rnp.tree2array(t, branches="XPt3")
  XPt = np.concatenate((XPt0, XPt1, XPt2, XPt3), axis=None)
  Xeta0  = rnp.tree2array(t, branches="Xeta0")
  Xeta1  = rnp.tree2array(t, branches="Xeta1")
  Xeta2  = rnp.tree2array(t, branches="Xeta2")
  Xeta3  = rnp.tree2array(t, branches="Xeta3")
  Xeta = np.concatenate((Xeta0, Xeta1, Xeta2, Xeta3), axis=None)
  print type(mW)
  
  c = ROOT.TCanvas('c', 'c', 800, 600)
  #bucket type count
  htwcount = ROOT.TH1F("htwcount", "number of tw buckets per event", 5, -0.5, 4.5)
  fill_hist(htwcount, twcount, "twbuckets per event", "")
  htwcount.Draw()
  c.Print("htwcount_%sjetregion.eps"%njet)
  htmincount = ROOT.TH1F("htmincount", "number of tmin buckets per event", 5, -0.5, 4.5)
  fill_hist(htmincount, tmincount, "tminbuckets per event", "")
  htmincount.Draw()
  c.Print("htmincount_%sjetregion.eps"%njet)
  ht0count = ROOT.TH1F("ht0count", "number of t0 buckets per event", 5, -0.5, 4.5)
  fill_hist(ht0count, t0count, "t0buckets per event", "")
  ht0count.Draw()
  c.Print("ht0count_%sjetregion.eps"%njet)

  #mass
  htwmass = ROOT.TH1F("htwmass", "Mass of tw Buckets",150,0.0001,300);
  fill_hist(htwmass, twmass, "Mass (Gev)", "")
  htwmass.Draw()
  c.Print("htwmass_%sjetregion.eps"%njet)
  htminmass = ROOT.TH1F("htminmass", "Mass of t- Buckets",150,0.0001,300); 
  fill_hist(htminmass, tminmass, "Mass (Gev)", "")
  htminmass.Draw()
  c.Print("htminmass_%sjetregion.eps"%njet)
  ht0mass = ROOT.TH1F("ht0mass", "Mass of t0 Buckets",150,0.0001,300); 
  fill_hist(ht0mass, t0mass, "Mass (Gev)", "")
  ht0mass.Draw()
  c.Print("ht0mass_%sjetregion.eps"%njet)
  hXmass = ROOT.TH1F("hXmass", "Mass of the extra jets",110,-1,10); 
  fill_hist(hXmass, Xmass, "Mass (Gev)", "")
  hXmass.Draw()
  c.Print("hXmass_%sjetregion.eps"%njet)
  hXmass_more = ROOT.TH1F("hXmass_full", "Mass of the extra jets",300,-1,30); 
  fill_hist(hXmass_more, Xmass, "Mass (Gev)", "")
  hXmass_more.Draw()
  c.Print("hXmass_full_%sjetregion.eps"%njet)
  #pT
  htwPt = ROOT.TH1F("htwPt", "Pt of tw Buckets",250,0,1200); 
  fill_hist(htwPt, twPt, "pT (Gev)", "")
  htwPt.Draw()
  c.Print("htwPt_%sjetregion.eps"%njet)
  htminPt = ROOT.TH1F("htminPt", "Pt of t- Buckets",250,0,1200); 
  fill_hist(htminPt, tminPt, "pT (Gev)", "")
  htminPt.Draw()
  c.Print("htminPt_%sjetregion.eps"%njet)
  ht0Pt = ROOT.TH1F("ht0Pt", "Pt of t0 Buckets",250,0,1200); 
  fill_hist(ht0Pt, t0Pt, "pT (Gev)", "")
  ht0Pt.Draw()
  c.Print("ht0Pt_%sjetregion.eps"%njet)
  hXPt = ROOT.TH1F("hXPt", "Pt of the extra jets",100,0,500); 
  fill_hist(hXPt, XPt, "pT (Gev)", "")
  hXPt.Draw()
  c.Print("hXPt_%sjetregion.eps"%njet)
  #eta
  htweta = ROOT.TH1F("htweta", "#eta of tw Buckets",100,-10,10); 
  fill_hist(htweta, tweta, "eta", "")
  htweta.Draw()
  c.Print("htweta_%sjetregion.eps"%njet)
  htmineta = ROOT.TH1F("htmineta", "#eta of t- Buckets",100,-10,10); 
  fill_hist(htmineta, tmineta, "eta", "")
  htmineta.Draw()
  c.Print("htmineta_%sjetregion.eps"%njet)
  ht0eta = ROOT.TH1F("ht0eta", "#eta of t0 Buckets",100,-10,10);
  fill_hist(ht0eta, t0eta, "eta", "")
  ht0eta.Draw()
  c.Print("ht0eta_%sjetregion.eps"%njet)
  hXeta = ROOT.TH1F("hXeta", "#eta of the extra jets",100,-10,10);
  fill_hist(hXeta, Xeta, "eta", "")
  hXeta.Draw()
  c.Print("hXeta_%sjetregion.eps"%njet)
  #W candidate mass
  hmW = ROOT.TH1F("hmW", "Mass of the (possible) W candidate",150,0.0001,300); 
  fill_hist(hmW, mW, "Mass (GeV)", "")
  hmW.Draw()
  c.Print("hmW_%sjetregion.eps"%njet)
  hmBucketPrim = ROOT.TH1F("hmBucketPrimitive", "Mass of the Entire Buckets before Recalculation",150,0,300); 
  fill_hist(hmBucketPrim, mBucketPrim, "Mass (GeV)", "")
  hmBucketPrim.Draw()
  c.Print("hmBucketPrim_%sjetregion.eps"%njet)
  hmratio = ROOT.TH1F("hmratio", "Mass Ratio Difference",120,-0.1,1.1); 
  fill_hist(hmratio, mratio, "Mass ratio difference", "")
  hmratio.Draw()
  c.Print("hmratio_%sjetregion.eps"%njet)


def fill_hist(h, arr, Xtitle, Ytitle):
  h.GetXaxis().SetTitle(Xtitle)
  h.GetYaxis().SetTitle(Ytitle)
  [h.Fill(_) for _ in arr if (_ > -900)]



if __name__ == '__main__':
  main()
