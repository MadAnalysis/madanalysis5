void selection_91()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo183","canvas_plotflow_tempo183",0,0,700,500);
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
  TH1F* S92_GAMMA_0 = new TH1F("S92_GAMMA_0","S92_GAMMA_0",50,1.0,100.0);
  // Content
  S92_GAMMA_0->SetBinContent(0,0.0); // underflow
  S92_GAMMA_0->SetBinContent(1,0.0);
  S92_GAMMA_0->SetBinContent(2,0.06179132);
  S92_GAMMA_0->SetBinContent(3,0.2626131);
  S92_GAMMA_0->SetBinContent(4,0.6951523);
  S92_GAMMA_0->SetBinContent(5,0.911422);
  S92_GAMMA_0->SetBinContent(6,0.9268698);
  S92_GAMMA_0->SetBinContent(7,1.112244);
  S92_GAMMA_0->SetBinContent(8,0.9886611);
  S92_GAMMA_0->SetBinContent(9,0.9268698);
  S92_GAMMA_0->SetBinContent(10,1.050452);
  S92_GAMMA_0->SetBinContent(11,0.8341828);
  S92_GAMMA_0->SetBinContent(12,0.818735);
  S92_GAMMA_0->SetBinContent(13,0.6024654);
  S92_GAMMA_0->SetBinContent(14,0.5561219);
  S92_GAMMA_0->SetBinContent(15,0.5252262);
  S92_GAMMA_0->SetBinContent(16,0.5097784);
  S92_GAMMA_0->SetBinContent(17,0.4479871);
  S92_GAMMA_0->SetBinContent(18,0.4170914);
  S92_GAMMA_0->SetBinContent(19,0.3244044);
  S92_GAMMA_0->SetBinContent(20,0.2626131);
  S92_GAMMA_0->SetBinContent(21,0.2008218);
  S92_GAMMA_0->SetBinContent(22,0.2935088);
  S92_GAMMA_0->SetBinContent(23,0.2008218);
  S92_GAMMA_0->SetBinContent(24,0.2471653);
  S92_GAMMA_0->SetBinContent(25,0.07723915);
  S92_GAMMA_0->SetBinContent(26,0.1544783);
  S92_GAMMA_0->SetBinContent(27,0.2008218);
  S92_GAMMA_0->SetBinContent(28,0.2471653);
  S92_GAMMA_0->SetBinContent(29,0.1699261);
  S92_GAMMA_0->SetBinContent(30,0.01544783);
  S92_GAMMA_0->SetBinContent(31,0.06179132);
  S92_GAMMA_0->SetBinContent(32,0.07723915);
  S92_GAMMA_0->SetBinContent(33,0.07723915);
  S92_GAMMA_0->SetBinContent(34,0.1390305);
  S92_GAMMA_0->SetBinContent(35,0.09268698);
  S92_GAMMA_0->SetBinContent(36,0.04634349);
  S92_GAMMA_0->SetBinContent(37,0.06179132);
  S92_GAMMA_0->SetBinContent(38,0.07723915);
  S92_GAMMA_0->SetBinContent(39,0.04634349);
  S92_GAMMA_0->SetBinContent(40,0.03089566);
  S92_GAMMA_0->SetBinContent(41,0.07723915);
  S92_GAMMA_0->SetBinContent(42,0.03089566);
  S92_GAMMA_0->SetBinContent(43,0.01544783);
  S92_GAMMA_0->SetBinContent(44,0.03089566);
  S92_GAMMA_0->SetBinContent(45,0.0);
  S92_GAMMA_0->SetBinContent(46,0.04634349);
  S92_GAMMA_0->SetBinContent(47,0.04634349);
  S92_GAMMA_0->SetBinContent(48,0.03089566);
  S92_GAMMA_0->SetBinContent(49,0.06179132);
  S92_GAMMA_0->SetBinContent(50,0.0);
  S92_GAMMA_0->SetBinContent(51,0.3553001); // overflow
  S92_GAMMA_0->SetEntries(1000);
  // Style
  S92_GAMMA_0->SetLineColor(9);
  S92_GAMMA_0->SetLineStyle(1);
  S92_GAMMA_0->SetLineWidth(1);
  S92_GAMMA_0->SetFillColor(9);
  S92_GAMMA_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_184","mystack");
  stack->Add(S92_GAMMA_0);
  stack->Draw("");

  // Y axis
  stack->GetYaxis()->SetLabelSize(0.04);
  stack->GetYaxis()->SetLabelOffset(0.005);
  stack->GetYaxis()->SetTitleSize(0.06);
  stack->GetYaxis()->SetTitleFont(22);
  stack->GetYaxis()->SetTitleOffset(1);
  stack->GetYaxis()->SetTitle("N. of b (not normalized)");

  // X axis
  stack->GetXaxis()->SetLabelSize(0.04);
  stack->GetXaxis()->SetLabelOffset(0.005);
  stack->GetXaxis()->SetTitleSize(0.06);
  stack->GetXaxis()->SetTitleFont(22);
  stack->GetXaxis()->SetTitleOffset(1);
  stack->GetXaxis()->SetTitle("#gamma [ b ] ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_91.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_91.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_91.eps");

}
