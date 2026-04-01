void selection_156()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo313","canvas_plotflow_tempo313",0,0,700,500);
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
  TH1F* S157_DPHI_0_2PI_0 = new TH1F("S157_DPHI_0_2PI_0","S157_DPHI_0_2PI_0",32,0.0,6.4);
  // Content
  S157_DPHI_0_2PI_0->SetBinContent(0,0.0); // underflow
  S157_DPHI_0_2PI_0->SetBinContent(1,0.6797045);
  S157_DPHI_0_2PI_0->SetBinContent(2,0.6797045);
  S157_DPHI_0_2PI_0->SetBinContent(3,0.7723915);
  S157_DPHI_0_2PI_0->SetBinContent(4,0.8959741);
  S157_DPHI_0_2PI_0->SetBinContent(5,1.174035);
  S157_DPHI_0_2PI_0->SetBinContent(6,1.467544);
  S157_DPHI_0_2PI_0->SetBinContent(7,1.204931);
  S157_DPHI_0_2PI_0->SetBinContent(8,1.374857);
  S157_DPHI_0_2PI_0->SetBinContent(9,1.112244);
  S157_DPHI_0_2PI_0->SetBinContent(10,1.143139);
  S157_DPHI_0_2PI_0->SetBinContent(11,1.220379);
  S157_DPHI_0_2PI_0->SetBinContent(12,0.9268698);
  S157_DPHI_0_2PI_0->SetBinContent(13,0.7414958);
  S157_DPHI_0_2PI_0->SetBinContent(14,0.9268698);
  S157_DPHI_0_2PI_0->SetBinContent(15,0.6951523);
  S157_DPHI_0_2PI_0->SetBinContent(16,0.6488089);
  S157_DPHI_0_2PI_0->SetBinContent(17,0.5870175);
  S157_DPHI_0_2PI_0->SetBinContent(18,0.8805263);
  S157_DPHI_0_2PI_0->SetBinContent(19,0.8032872);
  S157_DPHI_0_2PI_0->SetBinContent(20,0.8650785);
  S157_DPHI_0_2PI_0->SetBinContent(21,1.0659);
  S157_DPHI_0_2PI_0->SetBinContent(22,1.189483);
  S157_DPHI_0_2PI_0->SetBinContent(23,1.112244);
  S157_DPHI_0_2PI_0->SetBinContent(24,1.390305);
  S157_DPHI_0_2PI_0->SetBinContent(25,1.004109);
  S157_DPHI_0_2PI_0->SetBinContent(26,1.436648);
  S157_DPHI_0_2PI_0->SetBinContent(27,1.359409);
  S157_DPHI_0_2PI_0->SetBinContent(28,1.019557);
  S157_DPHI_0_2PI_0->SetBinContent(29,0.8341828);
  S157_DPHI_0_2PI_0->SetBinContent(30,0.726048);
  S157_DPHI_0_2PI_0->SetBinContent(31,0.726048);
  S157_DPHI_0_2PI_0->SetBinContent(32,0.2317174);
  S157_DPHI_0_2PI_0->SetBinContent(33,0.0); // overflow
  S157_DPHI_0_2PI_0->SetEntries(2000);
  // Style
  S157_DPHI_0_2PI_0->SetLineColor(9);
  S157_DPHI_0_2PI_0->SetLineStyle(1);
  S157_DPHI_0_2PI_0->SetLineWidth(1);
  S157_DPHI_0_2PI_0->SetFillColor(9);
  S157_DPHI_0_2PI_0->SetFillStyle(1001);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_314","mystack");
  stack->Add(S157_DPHI_0_2PI_0);
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
  stack->GetXaxis()->SetTitle("#Delta#Phi_{0,2#pi} [ j, j ] ");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_156.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_156.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_156.eps");

}
