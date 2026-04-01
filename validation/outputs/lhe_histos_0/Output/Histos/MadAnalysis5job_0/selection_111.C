void selection_111()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo223","canvas_plotflow_tempo223",0,0,700,500);
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
  TH1F* S112_E_0 = new TH1F("S112_E_0","S112_E_0",50,0.0,2000.0);
  // Content
  S112_E_0->SetBinContent(0,0.0); // underflow
  S112_E_0->SetBinContent(1,4.55711);
  S112_E_0->SetBinContent(2,11.47774);
  S112_E_0->SetBinContent(3,6.086445);
  S112_E_0->SetBinContent(4,3.583897);
  S112_E_0->SetBinContent(5,1.838292);
  S112_E_0->SetBinContent(6,1.158587);
  S112_E_0->SetBinContent(7,0.6951523);
  S112_E_0->SetBinContent(8,0.5561219);
  S112_E_0->SetBinContent(9,0.2780609);
  S112_E_0->SetBinContent(10,0.1699261);
  S112_E_0->SetBinContent(11,0.185374);
  S112_E_0->SetBinContent(12,0.1390305);
  S112_E_0->SetBinContent(13,0.04634349);
  S112_E_0->SetBinContent(14,0.01544783);
  S112_E_0->SetBinContent(15,0.04634349);
  S112_E_0->SetBinContent(16,0.03089566);
  S112_E_0->SetBinContent(17,0.0);
  S112_E_0->SetBinContent(18,0.0);
  S112_E_0->SetBinContent(19,0.0);
  S112_E_0->SetBinContent(20,0.03089566);
  S112_E_0->SetBinContent(21,0.0);
  S112_E_0->SetBinContent(22,0.0);
  S112_E_0->SetBinContent(23,0.0);
  S112_E_0->SetBinContent(24,0.0);
  S112_E_0->SetBinContent(25,0.0);
  S112_E_0->SetBinContent(26,0.0);
  S112_E_0->SetBinContent(27,0.0);
  S112_E_0->SetBinContent(28,0.0);
  S112_E_0->SetBinContent(29,0.0);
  S112_E_0->SetBinContent(30,0.0);
  S112_E_0->SetBinContent(31,0.0);
  S112_E_0->SetBinContent(32,0.0);
  S112_E_0->SetBinContent(33,0.0);
  S112_E_0->SetBinContent(34,0.0);
  S112_E_0->SetBinContent(35,0.0);
  S112_E_0->SetBinContent(36,0.0);
  S112_E_0->SetBinContent(37,0.0);
  S112_E_0->SetBinContent(38,0.0);
  S112_E_0->SetBinContent(39,0.0);
  S112_E_0->SetBinContent(40,0.0);
  S112_E_0->SetBinContent(41,0.0);
  S112_E_0->SetBinContent(42,0.0);
  S112_E_0->SetBinContent(43,0.0);
  S112_E_0->SetBinContent(44,0.0);
  S112_E_0->SetBinContent(45,0.0);
  S112_E_0->SetBinContent(46,0.0);
  S112_E_0->SetBinContent(47,0.0);
  S112_E_0->SetBinContent(48,0.0);
  S112_E_0->SetBinContent(49,0.0);
  S112_E_0->SetBinContent(50,0.0);
  S112_E_0->SetBinContent(51,0.0); // overflow
  S112_E_0->SetEntries(2000);
  // Style
  S112_E_0->SetLineColor(9);
  S112_E_0->SetLineStyle(1);
  S112_E_0->SetLineWidth(1);
  S112_E_0->SetFillColor(9);
  S112_E_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_224","mystack");
  stack->Add(S112_E_0);
  stack->Draw("");

  // Y axis
  stack->GetYaxis()->SetLabelSize(0.04);
  stack->GetYaxis()->SetLabelOffset(0.005);
  stack->GetYaxis()->SetTitleSize(0.06);
  stack->GetYaxis()->SetTitleFont(22);
  stack->GetYaxis()->SetTitleOffset(1);
  stack->GetYaxis()->SetTitle("N. of j (not normalized)");

  // X axis
  stack->GetXaxis()->SetLabelSize(0.04);
  stack->GetXaxis()->SetLabelOffset(0.005);
  stack->GetXaxis()->SetTitleSize(0.06);
  stack->GetXaxis()->SetTitleFont(22);
  stack->GetXaxis()->SetTitleOffset(1);
  stack->GetXaxis()->SetTitle("E [ j ] (GeV) ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_111.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_111.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_111.eps");

}
