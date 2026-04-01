void selection_9()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo19","canvas_plotflow_tempo19",0,0,700,500);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  canvas->SetHighLightColor(2);
  canvas->SetFillColor(0);
  canvas->SetBorderMode(0);
  canvas->SetBorderSize(3);
  canvas->SetFrameBorderMode(0);
  canvas->SetFrameBorderSize(0);
  canvas->SetTickx(1);
  canvas->SetTicky(1);
  canvas->SetLeftMargin(0.14);
  canvas->SetRightMargin(0.05);
  canvas->SetBottomMargin(0.15);
  canvas->SetTopMargin(0.05);

  // Creating a new TH1F
  TH1F* S10_NAPID_0 = new TH1F("S10_NAPID_0","S10_NAPID_0",9,0.0,9);
  // Content
  S10_NAPID_0->SetBinContent(0,0.0); // underflow
  S10_NAPID_0->SetBinContent(1,8.141006);
  S10_NAPID_0->SetBinContent(2,8.141006);
  S10_NAPID_0->SetBinContent(3,7.306824);
  S10_NAPID_0->SetBinContent(4,7.306824);
  S10_NAPID_0->SetBinContent(5,30.89566);
  S10_NAPID_0->SetBinContent(6,7.816602);
  S10_NAPID_0->SetBinContent(7,7.816602);
  S10_NAPID_0->SetBinContent(8,7.631228);
  S10_NAPID_0->SetBinContent(9,7.631228);
  S10_NAPID_0->SetBinContent(10,0.0); // overflow
  S10_NAPID_0->SetEntries(0);
  // Style
  S10_NAPID_0->SetLineColor(9);
  S10_NAPID_0->SetLineStyle(1);
  S10_NAPID_0->SetLineWidth(1);
  S10_NAPID_0->SetFillColor(9);
  S10_NAPID_0->SetFillStyle(1001);
  S10_NAPID_0->SetBarWidth(0.8);
  S10_NAPID_0->SetBarOffset(0.1);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_20","mystack");
  stack->Add(S10_NAPID_0);
  stack->Draw("bar1");

  // Y axis
  stack->GetYaxis()->SetLabelSize(0.04);
  stack->GetYaxis()->SetLabelOffset(0.005);
  stack->GetYaxis()->SetTitleSize(0.06);
  stack->GetYaxis()->SetTitleFont(22);
  stack->GetYaxis()->SetTitleOffset(1);
  stack->GetYaxis()->SetTitle("N. of particles (not normalized)");

  // X axis
  stack->GetXaxis()->SetLabelSize(0.04);
  stack->GetXaxis()->SetLabelOffset(0.005);
  stack->GetXaxis()->SetTitleSize(0.06);
  stack->GetXaxis()->SetTitleFont(22);
  stack->GetXaxis()->SetTitleOffset(1);
  stack->GetXaxis()->SetTitle("|NPID| ");
  stack->GetXaxis()->SetBinLabel(1,"d~/d");
  stack->GetXaxis()->SetBinLabel(2,"u~/u");
  stack->GetXaxis()->SetBinLabel(3,"s~/s");
  stack->GetXaxis()->SetBinLabel(4,"c~/c");
  stack->GetXaxis()->SetBinLabel(5,"b~/b");
  stack->GetXaxis()->SetBinLabel(6,"e+/e-");
  stack->GetXaxis()->SetBinLabel(7,"ve~/ve");
  stack->GetXaxis()->SetBinLabel(8,"mu+/mu-");
  stack->GetXaxis()->SetBinLabel(9,"vm~/vm");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_9.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_9.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_9.eps");

}
