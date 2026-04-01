void selection_94()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo189","canvas_plotflow_tempo189",0,0,700,500);
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
  TH1F* S95_PHI_0 = new TH1F("S95_PHI_0","S95_PHI_0",32,-3.2,3.2);
  // Content
  S95_PHI_0->SetBinContent(0,0.0); // underflow
  S95_PHI_0->SetBinContent(1,0.185374);
  S95_PHI_0->SetBinContent(2,0.726048);
  S95_PHI_0->SetBinContent(3,0.4788827);
  S95_PHI_0->SetBinContent(4,0.4943306);
  S95_PHI_0->SetBinContent(5,0.4016436);
  S95_PHI_0->SetBinContent(6,0.5561219);
  S95_PHI_0->SetBinContent(7,0.4943306);
  S95_PHI_0->SetBinContent(8,0.4479871);
  S95_PHI_0->SetBinContent(9,0.3707479);
  S95_PHI_0->SetBinContent(10,0.5097784);
  S95_PHI_0->SetBinContent(11,0.4170914);
  S95_PHI_0->SetBinContent(12,0.633361);
  S95_PHI_0->SetBinContent(13,0.4325392);
  S95_PHI_0->SetBinContent(14,0.6797045);
  S95_PHI_0->SetBinContent(15,0.4634349);
  S95_PHI_0->SetBinContent(16,0.4479871);
  S95_PHI_0->SetBinContent(17,0.3553001);
  S95_PHI_0->SetBinContent(18,0.4325392);
  S95_PHI_0->SetBinContent(19,0.5252262);
  S95_PHI_0->SetBinContent(20,0.3861957);
  S95_PHI_0->SetBinContent(21,0.5715697);
  S95_PHI_0->SetBinContent(22,0.6024654);
  S95_PHI_0->SetBinContent(23,0.4479871);
  S95_PHI_0->SetBinContent(24,0.3707479);
  S95_PHI_0->SetBinContent(25,0.5715697);
  S95_PHI_0->SetBinContent(26,0.633361);
  S95_PHI_0->SetBinContent(27,0.3553001);
  S95_PHI_0->SetBinContent(28,0.5561219);
  S95_PHI_0->SetBinContent(29,0.6179132);
  S95_PHI_0->SetBinContent(30,0.4325392);
  S95_PHI_0->SetBinContent(31,0.5870175);
  S95_PHI_0->SetBinContent(32,0.2626131);
  S95_PHI_0->SetBinContent(33,0.0); // overflow
  S95_PHI_0->SetEntries(1000);
  // Style
  S95_PHI_0->SetLineColor(9);
  S95_PHI_0->SetLineStyle(1);
  S95_PHI_0->SetLineWidth(1);
  S95_PHI_0->SetFillColor(9);
  S95_PHI_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_190","mystack");
  stack->Add(S95_PHI_0);
  stack->Draw("");

  // Y axis
  stack->GetYaxis()->SetLabelSize(0.04);
  stack->GetYaxis()->SetLabelOffset(0.005);
  stack->GetYaxis()->SetTitleSize(0.06);
  stack->GetYaxis()->SetTitleFont(22);
  stack->GetYaxis()->SetTitleOffset(1);
  stack->GetYaxis()->SetTitle("Events  (not normalized)");

  // X axis
  stack->GetXaxis()->SetLabelSize(0.04);
  stack->GetXaxis()->SetLabelOffset(0.005);
  stack->GetXaxis()->SetTitleSize(0.06);
  stack->GetXaxis()->SetTitleFont(22);
  stack->GetXaxis()->SetTitleOffset(1);
  stack->GetXaxis()->SetTitle("#phi [ b_{1} ] ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_94.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_94.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_94.eps");

}
