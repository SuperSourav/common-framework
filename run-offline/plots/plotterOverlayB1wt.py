import ROOT
import numpy as np
import root_numpy as rnp


def main():
  ROOT.gROOT.SetBatch(1)
  ROOT.gStyle.SetOptStat(0)
  njet = "all"
  #njet = "ge10" 
  #njet = "9" 
  #njet = "8" 
  #njet = "7" 
  #njet = "6" 
  #njet = "5"
  process = {"testB1wt1" : "B1wt1", "testB1wt2" : "B1wt2", "testB1wt10" : "B1wt10", "ValidttbarAllhadISRjetsB1free_50K" : "B1wt100"}
  processlabel = {"B1wt1" : "B1wt=1", "B1wt2" : "B1wt=2", "B1wt10" : "B1wt=10", "B1wt100" : "B1wt=100"}
  tlist = {}
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
    tlist[process[p]+"tree"] = t

  Dtwcount = {}
  Dtmincount = {}
  Dt0count = {}
  DmW0 = {}
  DmW1 = {}
  DmBucketPrim0 = {}
  DmBucketPrim1 = {}
  DmBucketPrim = {}
  Dtwmass0 = {}
  DtwPt0 = {}
  Dtwmass1 = {}
  DtwPt1 = {}
  DNaddjets = {}
  for p in processlabel.keys():
    #bucket type count
    Dtwcount[p] = rnp.tree2array(tlist["%stree"%p], branches="twcount")
    Dtmincount[p] = rnp.tree2array(tlist["%stree"%p], branches="tmincount")
    Dt0count[p] = rnp.tree2array(tlist["%stree"%p], branches="t0count")
  
    ##
    DmW0[p] = rnp.tree2array(tlist["%stree"%p], branches="mW0") 
    DmW1[p] = rnp.tree2array(tlist["%stree"%p], branches="mW1")
    DmBucketPrim0[p] = rnp.tree2array(tlist["%stree"%p], branches="mBucketPrim0")
    DmBucketPrim1[p] = rnp.tree2array(tlist["%stree"%p], branches="mBucketPrim1")
    DmBucketPrim[p] = np.concatenate((DmBucketPrim0[p],DmBucketPrim1[p]), axis=None)
    Dtwmass0[p] = rnp.tree2array(tlist["%stree"%p], branches="twmass0")
    DtwPt0[p] = rnp.tree2array(tlist["%stree"%p], branches="twPt0")
    Dtwmass1[p] = rnp.tree2array(tlist["%stree"%p], branches="twmass1")
    DtwPt1[p] = rnp.tree2array(tlist["%stree"%p], branches="twPt1")
    DNaddjets[p] = rnp.tree2array(tlist["%stree"%p], branches="Naddjets")
##  
  c0 = ROOT.TCanvas('c', 'c', 800, 600)
  leg0 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg0.SetFillColor(0)
  leg0.SetLineColor(0)
  hNaddjets = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    print "%s hNaddjets"%processlabel[pl]
    #bucket type count
    hNaddjets[pl] = (ROOT.TH1F("%s hNaddjets"%processlabel[pl], "", 21, -0.5, 20.5))
    fill_hist(hNaddjets[pl], DNaddjets[pl], "additional jets per event", "a.u.")
    hNaddjets[pl].Scale(1./(hNaddjets[pl].Integral()))
    hNaddjets[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(hNaddjets[pl].GetMaximum())

  hNaddjets["B1wt1"].SetMaximum(max(hmax)*1.1)
  hNaddjets["B1wt1"].Draw("hist")
  leg0.AddEntry(hNaddjets["B1wt1"], processlabel["B1wt1"], "L")

  for pl in processlabel.keys():
    if pl != "B1wt1":
      hNaddjets[pl].Draw("hist same")
      leg0.AddEntry(hNaddjets[pl], processlabel[pl], "L")

  leg0.Draw()
  c0.Print("OverlayhNaddjets_%sjetregion.eps"%njet)
##
  c1 = ROOT.TCanvas('c', 'c', 800, 600)
  leg1 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg1.SetFillColor(0)
  leg1.SetLineColor(0)
  htwcount = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    htwcount[pl] = (ROOT.TH1F("%s htwcount"%processlabel[pl], "", 4, -0.5, 3.5))
    fill_hist(htwcount[pl], Dtwcount[pl], "tw buckets per event", "a.u.")
    htwcount[pl].Scale(1./(htwcount[pl].Integral()))
    htwcount[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(htwcount[pl].GetMaximum())

  htwcount["B1wt1"].SetMaximum(max(hmax)*1.1)
  htwcount["B1wt1"].Draw("hist")
  leg1.AddEntry(htwcount["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      htwcount[pl].Draw("hist same")
      leg1.AddEntry(htwcount[pl], processlabel[pl], "L")

  leg1.Draw()
  c1.Print("Overlayhtwcount_%sjetregion.eps"%njet)
##
  c2 = ROOT.TCanvas('c', 'c', 800, 600)
  leg2 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg2.SetFillColor(0)
  leg2.SetLineColor(0)
  hmW0 = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    hmW0[pl] = (ROOT.TH1F("%s hmW0"%processlabel[pl], "", 150,0,300))
    fill_hist(hmW0[pl], DmW0[pl], "Mass of the (possible) W candidate in B1", "a.u.")
    hmW0[pl].Scale(1./(hmW0[pl].Integral()))
    hmW0[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(hmW0[pl].GetMaximum())

  hmW0["B1wt1"].SetMaximum(max(hmax)*1.1)
  hmW0["B1wt1"].Draw("hist")
  leg2.AddEntry(hmW0["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      hmW0[pl].Draw("hist same")
      leg2.AddEntry(hmW0[pl], processlabel[pl], "L")

  leg2.Draw()
  c2.Print("OverlayhmW0_%sjetregion.eps"%njet)
##
  c3 = ROOT.TCanvas('c', 'c', 800, 600)
  leg3 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg3.SetFillColor(0)
  leg3.SetLineColor(0)
  hmW1 = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    hmW1[pl] = (ROOT.TH1F("%s hmW1"%processlabel[pl], "", 150,0,300))
    fill_hist(hmW1[pl], DmW1[pl], "Mass of the (possible) W candidate in B2", "a.u.")
    hmW1[pl].Scale(1./(hmW1[pl].Integral()))
    hmW1[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(hmW1[pl].GetMaximum())

  hmW1["B1wt1"].SetMaximum(max(hmax)*1.1)
  hmW1["B1wt1"].Draw("hist")
  leg3.AddEntry(hmW1["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      hmW1[pl].Draw("hist same")
      leg3.AddEntry(hmW1[pl], processlabel[pl], "L")

  leg3.Draw()
  c3.Print("OverlayhmW1_%sjetregion.eps"%njet)
##
  c4 = ROOT.TCanvas('c', 'c', 800, 600)
  leg4 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg4.SetFillColor(0)
  leg4.SetLineColor(0)
  hmBucketPrim0 = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    hmBucketPrim0[pl] = (ROOT.TH1F("%s hmBucketPrim0"%processlabel[pl], "Mass of B1", 150,0,300))
    fill_hist(hmBucketPrim0[pl], DmBucketPrim0[pl], "Mass (GeV)", "a.u.")
    hmBucketPrim0[pl].Scale(1./(hmBucketPrim0[pl].Integral()))
    hmBucketPrim0[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(hmBucketPrim0[pl].GetMaximum())

  hmBucketPrim0["B1wt1"].SetMaximum(max(hmax)*1.1)
  hmBucketPrim0["B1wt1"].Draw("hist")
  leg4.AddEntry(hmBucketPrim0["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      hmBucketPrim0[pl].Draw("hist same")
      leg4.AddEntry(hmBucketPrim0[pl], processlabel[pl], "L")

  leg4.Draw()
  c4.Print("OverlayhmBucketPrim0_%sjetregion.eps"%njet)
##
  c5 = ROOT.TCanvas('c', 'c', 800, 600)
  leg5 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg5.SetFillColor(0)
  leg5.SetLineColor(0)
  hmBucketPrim1 = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    hmBucketPrim1[pl] = (ROOT.TH1F("%s hmBucketPrim1"%processlabel[pl], "Mass of B2", 150,0,300))
    fill_hist(hmBucketPrim1[pl], DmBucketPrim1[pl], "Mass (GeV)", "a.u.")
    hmBucketPrim1[pl].Scale(1./(hmBucketPrim1[pl].Integral()))
    hmBucketPrim1[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(hmBucketPrim1[pl].GetMaximum())

  hmBucketPrim1["B1wt1"].SetMaximum(max(hmax)*1.1)
  hmBucketPrim1["B1wt1"].Draw("hist")
  leg5.AddEntry(hmBucketPrim1["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      hmBucketPrim1[pl].Draw("hist same")
      leg5.AddEntry(hmBucketPrim1[pl], processlabel[pl], "L")

  leg5.Draw()
  c5.Print("OverlayhmBucketPrim1_%sjetregion.eps"%njet)
##
  c6 = ROOT.TCanvas('c', 'c', 800, 600)
  leg6 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg6.SetFillColor(0)
  leg6.SetLineColor(0)
  htwmass0 = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    htwmass0[pl] = (ROOT.TH1F("%s htwmass0"%processlabel[pl], "Mass of B1 tw",150,0.0001,300))
    fill_hist(htwmass0[pl], Dtwmass0[pl], "Mass (GeV)", "a.u.")
    twmass0Xaxis = htwmass0[pl].GetXaxis()
    twmass0minbin = twmass0Xaxis.FindBin(150)
    twmass0maxbin = twmass0Xaxis.FindBin(201)
    twmass0Integral = htwmass0[pl].Integral(twmass0minbin, twmass0maxbin)
    htwmass0[pl].Scale(1./(twmass0Integral))
    htwmass0[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(htwmass0[pl].GetMaximum())

  htwmass0["B1wt1"].SetMaximum(max(hmax)*1.1)
  htwmass0["B1wt1"].Draw("hist")
  leg6.AddEntry(htwmass0["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      htwmass0[pl].Draw("hist same")
      leg6.AddEntry(htwmass0[pl], processlabel[pl], "L")

  leg6.Draw()
  c6.Print("Overlayhtwmass0_%sjetregion.eps"%njet)
##
  c7 = ROOT.TCanvas('c', 'c', 800, 600)
  leg7 = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
  leg7.SetFillColor(0)
  leg7.SetLineColor(0)
  htwmass1 = {}
  colorcounter = 1
  hmax = []
  for pl in processlabel.keys():
    #bucket type count
    htwmass1[pl] = (ROOT.TH1F("%s htwmass1"%processlabel[pl], "Mass of B2 tw",150,0.0001,300))
    fill_hist(htwmass1[pl], Dtwmass1[pl], "Mass (GeV)", "a.u.")
    twmass1Xaxis = htwmass1[pl].GetXaxis()
    twmass1minbin = twmass1Xaxis.FindBin(150)
    twmass1maxbin = twmass1Xaxis.FindBin(201)
    twmass1Integral = htwmass1[pl].Integral(twmass1minbin, twmass1maxbin)
    htwmass1[pl].Scale(1./(twmass1Integral))
    htwmass1[pl].SetLineColor(colorcounter)
    colorcounter += 1
    hmax.append(htwmass1[pl].GetMaximum())

  htwmass1["B1wt1"].SetMaximum(max(hmax)*1.1)
  htwmass1["B1wt1"].Draw("hist")
  leg7.AddEntry(htwmass1["B1wt1"], processlabel["B1wt1"], "L")
  for pl in processlabel.keys():
    if pl != "B1wt1":
      htwmass1[pl].Draw("hist same")
      leg7.AddEntry(htwmass1[pl], processlabel[pl], "L")

  leg7.Draw()
  c7.Print("Overlayhtwmass1_%sjetregion.eps"%njet)



##
  for pl in processlabel.keys():
    #bucket type count
    #hmBucketPrim0[pl] = (ROOT.TH1F("%s hmBucketPrim0"%processlabel[pl], "Mass of B1", 150,0,300))
    #fill_hist(hmBucketPrim0[pl], DmBucketPrim0[pl], "Mass (GeV)", "a.u.")
    #hmBucketPrim0[pl].Scale(1./(hmBucketPrim0[pl].Integral()))
    hmBucketPrim0[pl].SetLineColor(ROOT.kRed)
    #hmBucketPrim1[pl] = (ROOT.TH1F("%s hmBucketPrim1"%processlabel[pl], "Mass of B2", 150,0,300))
    #fill_hist(hmBucketPrim1[pl], DmBucketPrim1[pl], "Mass (GeV)", "a.u.")
    #hmBucketPrim1[pl].Scale(1./(hmBucketPrim1[pl].Integral()))
    hmBucketPrim1[pl].SetLineColor(ROOT.kBlack)
    cnew = ROOT.TCanvas('c', 'c', 800, 600)
    legnew = ROOT.TLegend( 0.65, 0.75, 0.88, 0.88)
    legnew.SetFillColor(0)
    legnew.SetLineColor(0)
    hmBucketPrim0[pl].SetMaximum(max(hmBucketPrim0[pl].GetMaximum(),hmBucketPrim1[pl].GetMaximum())*1.1)
    hmBucketPrim0[pl].SetTitle("Mass of B1 (%s)"%processlabel[pl])
    hmBucketPrim0[pl].Draw("hist")
    legnew.AddEntry(hmBucketPrim0[pl], "B1", "L")
    hmBucketPrim1[pl].Draw("hist same")
    legnew.AddEntry(hmBucketPrim1[pl], "B2", "L")
    legnew.Draw()
    cnew.Print("OverlayhmBucketPrim_%s_%sjetregion.eps"%(pl, njet))
##




def fill_hist(h, arr, Xtitle, Ytitle):
  h.GetXaxis().SetTitle(Xtitle)
  h.GetYaxis().SetTitle(Ytitle)
  [h.Fill(_) for _ in arr if (_ > -900)]



if __name__ == '__main__':
  main()
