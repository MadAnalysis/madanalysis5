void selection_155()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo311","canvas_plotflow_tempo311",0,0,700,500);
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
  TH1F* S156_DPHI_0_PI_0 = new TH1F("S156_DPHI_0_PI_0","S156_DPHI_0_PI_0",32,0.0,3.2);
  // Content
  S156_DPHI_0_PI_0->SetBinContent(0,0.0); // underflow
  S156_DPHI_0_PI_0->SetBinContent(1,0.5870175);
  S156_DPHI_0_PI_0->SetBinContent(2,0.7723915);
  S156_DPHI_0_PI_0->SetBinContent(3,0.7106002);
  S156_DPHI_0_PI_0->SetBinContent(4,0.6488089);
  S156_DPHI_0_PI_0->SetBinContent(5,0.8032872);
  S156_DPHI_0_PI_0->SetBinContent(6,0.7414958);
  S156_DPHI_0_PI_0->SetBinContent(7,0.8650785);
  S156_DPHI_0_PI_0->SetBinContent(8,0.9268698);
  S156_DPHI_0_PI_0->SetBinContent(9,1.204931);
  S156_DPHI_0_PI_0->SetBinContent(10,1.143139);
  S156_DPHI_0_PI_0->SetBinContent(11,1.575679);
  S156_DPHI_0_PI_0->SetBinContent(12,1.359409);
  S156_DPHI_0_PI_0->SetBinContent(13,1.575679);
  S156_DPHI_0_PI_0->SetBinContent(14,0.8341828);
  S156_DPHI_0_PI_0->SetBinContent(15,1.204931);
  S156_DPHI_0_PI_0->SetBinContent(16,1.544783);
  S156_DPHI_0_PI_0->SetBinContent(17,1.143139);
  S156_DPHI_0_PI_0->SetBinContent(18,1.081348);
  S156_DPHI_0_PI_0->SetBinContent(19,1.112244);
  S156_DPHI_0_PI_0->SetBinContent(20,1.174035);
  S156_DPHI_0_PI_0->SetBinContent(21,1.297618);
  S156_DPHI_0_PI_0->SetBinContent(22,1.143139);
  S156_DPHI_0_PI_0->SetBinContent(23,0.9886611);
  S156_DPHI_0_PI_0->SetBinContent(24,0.8650785);
  S156_DPHI_0_PI_0->SetBinContent(25,0.7106002);
  S156_DPHI_0_PI_0->SetBinContent(26,0.7723915);
  S156_DPHI_0_PI_0->SetBinContent(27,0.9577655);
  S156_DPHI_0_PI_0->SetBinContent(28,0.8959741);
  S156_DPHI_0_PI_0->SetBinContent(29,0.6797045);
  S156_DPHI_0_PI_0->SetBinContent(30,0.7106002);
  S156_DPHI_0_PI_0->SetBinContent(31,0.4634349);
  S156_DPHI_0_PI_0->SetBinContent(32,0.4016436);
  S156_DPHI_0_PI_0->SetBinContent(33,0.0); // overflow
  S156_DPHI_0_PI_0->SetEntries(2000);
  // Style
  S156_DPHI_0_PI_0->SetLineColor(9);
  S156_DPHI_0_PI_0->SetLineStyle(1);
  S156_DPHI_0_PI_0->SetLineWidth(1);
  S156_DPHI_0_PI_0->SetFillColor(9);
  S156_DPHI_0_PI_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_312","mystack");
  stack->Add(S156_DPHI_0_PI_0);
  stack->Draw("");

  // Y axis
  stack->GetYaxis()->SetLabelSize(0.04);
  stack->GetYaxis()->SetLabelOffset(0.005);
  stack->GetYaxis()->SetTitleSize(0.06);
  stack->GetYaxis()->SetTitleFont(22);
  stack->GetYaxis()->SetTitleOffset(1);
  stack->GetYaxis()->SetTitle("N. of (j, j) pairs (not normalized)");

  // X axis
  stack->GetXaxis()->SetLabelSize(0.04);
  stack->GetXaxis()->SetLabelOffset(0.005);
  stack->GetXaxis()->SetTitleSize(0.06);
  stack->GetXaxis()->SetTitleFont(22);
  stack->GetXaxis()->SetTitleOffset(1);
  stack->GetXaxis()->SetTitle("#Delta#Phi_{0,#pi} [ j, j ] ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_155.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_155.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_155.eps");

}
