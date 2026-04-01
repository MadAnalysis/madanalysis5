void selection_138()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo277","canvas_plotflow_tempo277",0,0,700,500);
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
  TH1F* S139_DPHI_0_2PI_0 = new TH1F("S139_DPHI_0_2PI_0","S139_DPHI_0_2PI_0",32,0.0,6.4);
  // Content
  S139_DPHI_0_2PI_0->SetBinContent(0,0.0); // underflow
  S139_DPHI_0_2PI_0->SetBinContent(1,0.4170914);
  S139_DPHI_0_2PI_0->SetBinContent(2,0.4788827);
  S139_DPHI_0_2PI_0->SetBinContent(3,0.3244044);
  S139_DPHI_0_2PI_0->SetBinContent(4,0.4788827);
  S139_DPHI_0_2PI_0->SetBinContent(5,0.6951523);
  S139_DPHI_0_2PI_0->SetBinContent(6,0.7106002);
  S139_DPHI_0_2PI_0->SetBinContent(7,0.5715697);
  S139_DPHI_0_2PI_0->SetBinContent(8,0.8496306);
  S139_DPHI_0_2PI_0->SetBinContent(9,0.6179132);
  S139_DPHI_0_2PI_0->SetBinContent(10,0.4943306);
  S139_DPHI_0_2PI_0->SetBinContent(11,0.6179132);
  S139_DPHI_0_2PI_0->SetBinContent(12,0.4170914);
  S139_DPHI_0_2PI_0->SetBinContent(13,0.3244044);
  S139_DPHI_0_2PI_0->SetBinContent(14,0.4943306);
  S139_DPHI_0_2PI_0->SetBinContent(15,0.4325392);
  S139_DPHI_0_2PI_0->SetBinContent(16,0.3398523);
  S139_DPHI_0_2PI_0->SetBinContent(17,0.2935088);
  S139_DPHI_0_2PI_0->SetBinContent(18,0.3089566);
  S139_DPHI_0_2PI_0->SetBinContent(19,0.4325392);
  S139_DPHI_0_2PI_0->SetBinContent(20,0.4479871);
  S139_DPHI_0_2PI_0->SetBinContent(21,0.6179132);
  S139_DPHI_0_2PI_0->SetBinContent(22,0.6024654);
  S139_DPHI_0_2PI_0->SetBinContent(23,0.5097784);
  S139_DPHI_0_2PI_0->SetBinContent(24,0.5561219);
  S139_DPHI_0_2PI_0->SetBinContent(25,0.5252262);
  S139_DPHI_0_2PI_0->SetBinContent(26,0.8496306);
  S139_DPHI_0_2PI_0->SetBinContent(27,0.4788827);
  S139_DPHI_0_2PI_0->SetBinContent(28,0.4943306);
  S139_DPHI_0_2PI_0->SetBinContent(29,0.3707479);
  S139_DPHI_0_2PI_0->SetBinContent(30,0.3553001);
  S139_DPHI_0_2PI_0->SetBinContent(31,0.2317174);
  S139_DPHI_0_2PI_0->SetBinContent(32,0.1081348);
  S139_DPHI_0_2PI_0->SetBinContent(33,0.0); // overflow
  S139_DPHI_0_2PI_0->SetEntries(1000);
  // Style
  S139_DPHI_0_2PI_0->SetLineColor(9);
  S139_DPHI_0_2PI_0->SetLineStyle(1);
  S139_DPHI_0_2PI_0->SetLineWidth(1);
  S139_DPHI_0_2PI_0->SetFillColor(9);
  S139_DPHI_0_2PI_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_278","mystack");
  stack->Add(S139_DPHI_0_2PI_0);
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
  stack->GetXaxis()->SetTitle("#Delta#Phi_{0,2#pi} [ j_{1}, j_{2} ] ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_138.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_138.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_138.eps");

}
