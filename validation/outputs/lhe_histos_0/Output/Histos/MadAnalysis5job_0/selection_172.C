void selection_172()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo345","canvas_plotflow_tempo345",0,0,700,500);
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
  TH1F* S173_DELTAR_0 = new TH1F("S173_DELTAR_0","S173_DELTAR_0",50,0.0,10.0);
  // Content
  S173_DELTAR_0->SetBinContent(0,0.0); // underflow
  S173_DELTAR_0->SetBinContent(1,0.0);
  S173_DELTAR_0->SetBinContent(2,0.0);
  S173_DELTAR_0->SetBinContent(3,0.185374);
  S173_DELTAR_0->SetBinContent(4,0.3707479);
  S173_DELTAR_0->SetBinContent(5,0.6024654);
  S173_DELTAR_0->SetBinContent(6,0.8496306);
  S173_DELTAR_0->SetBinContent(7,0.4943306);
  S173_DELTAR_0->SetBinContent(8,0.9732133);
  S173_DELTAR_0->SetBinContent(9,1.158587);
  S173_DELTAR_0->SetBinContent(10,1.050452);
  S173_DELTAR_0->SetBinContent(11,1.266722);
  S173_DELTAR_0->SetBinContent(12,1.251274);
  S173_DELTAR_0->SetBinContent(13,1.467544);
  S173_DELTAR_0->SetBinContent(14,1.235826);
  S173_DELTAR_0->SetBinContent(15,1.544783);
  S173_DELTAR_0->SetBinContent(16,1.436648);
  S173_DELTAR_0->SetBinContent(17,0.726048);
  S173_DELTAR_0->SetBinContent(18,0.3861957);
  S173_DELTAR_0->SetBinContent(19,0.2317174);
  S173_DELTAR_0->SetBinContent(20,0.09268698);
  S173_DELTAR_0->SetBinContent(21,0.03089566);
  S173_DELTAR_0->SetBinContent(22,0.03089566);
  S173_DELTAR_0->SetBinContent(23,0.04634349);
  S173_DELTAR_0->SetBinContent(24,0.01544783);
  S173_DELTAR_0->SetBinContent(25,0.0);
  S173_DELTAR_0->SetBinContent(26,0.0);
  S173_DELTAR_0->SetBinContent(27,0.0);
  S173_DELTAR_0->SetBinContent(28,0.0);
  S173_DELTAR_0->SetBinContent(29,0.0);
  S173_DELTAR_0->SetBinContent(30,0.0);
  S173_DELTAR_0->SetBinContent(31,0.0);
  S173_DELTAR_0->SetBinContent(32,0.0);
  S173_DELTAR_0->SetBinContent(33,0.0);
  S173_DELTAR_0->SetBinContent(34,0.0);
  S173_DELTAR_0->SetBinContent(35,0.0);
  S173_DELTAR_0->SetBinContent(36,0.0);
  S173_DELTAR_0->SetBinContent(37,0.0);
  S173_DELTAR_0->SetBinContent(38,0.0);
  S173_DELTAR_0->SetBinContent(39,0.0);
  S173_DELTAR_0->SetBinContent(40,0.0);
  S173_DELTAR_0->SetBinContent(41,0.0);
  S173_DELTAR_0->SetBinContent(42,0.0);
  S173_DELTAR_0->SetBinContent(43,0.0);
  S173_DELTAR_0->SetBinContent(44,0.0);
  S173_DELTAR_0->SetBinContent(45,0.0);
  S173_DELTAR_0->SetBinContent(46,0.0);
  S173_DELTAR_0->SetBinContent(47,0.0);
  S173_DELTAR_0->SetBinContent(48,0.0);
  S173_DELTAR_0->SetBinContent(49,0.0);
  S173_DELTAR_0->SetBinContent(50,0.0);
  S173_DELTAR_0->SetBinContent(51,0.0); // overflow
  S173_DELTAR_0->SetEntries(1000);
  // Style
  S173_DELTAR_0->SetLineColor(9);
  S173_DELTAR_0->SetLineStyle(1);
  S173_DELTAR_0->SetLineWidth(1);
  S173_DELTAR_0->SetFillColor(9);
  S173_DELTAR_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_346","mystack");
  stack->Add(S173_DELTAR_0);
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
  stack->GetXaxis()->SetTitle("#DeltaR [ b_{1}, j_{1} ] ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_172.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_172.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_172.eps");

}
