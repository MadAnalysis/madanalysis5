void selection_8()
{

  // ROOT version
  Int_t root_version = gROOT->GetVersionInt();

  // Creating a new TCanvas
  TCanvas* canvas = new TCanvas("canvas_plotflow_tempo17","canvas_plotflow_tempo17",0,0,700,500);
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
  TH1F* S9_NPID_0 = new TH1F("S9_NPID_0","S9_NPID_0",18,0.0,18);
  // Content
  S9_NPID_0->SetBinContent(0,0.0); // underflow
  S9_NPID_0->SetBinContent(1,4.078227);
  S9_NPID_0->SetBinContent(2,3.553001);
  S9_NPID_0->SetBinContent(3,3.583897);
  S9_NPID_0->SetBinContent(4,4.232705);
  S9_NPID_0->SetBinContent(5,15.44783);
  S9_NPID_0->SetBinContent(6,3.692031);
  S9_NPID_0->SetBinContent(7,3.614792);
  S9_NPID_0->SetBinContent(8,4.093675);
  S9_NPID_0->SetBinContent(9,4.047331);
  S9_NPID_0->SetBinContent(10,4.093675);
  S9_NPID_0->SetBinContent(11,4.047331);
  S9_NPID_0->SetBinContent(12,3.692031);
  S9_NPID_0->SetBinContent(13,3.614792);
  S9_NPID_0->SetBinContent(14,15.44783);
  S9_NPID_0->SetBinContent(15,3.583897);
  S9_NPID_0->SetBinContent(16,4.232705);
  S9_NPID_0->SetBinContent(17,4.078227);
  S9_NPID_0->SetBinContent(18,3.553001);
  S9_NPID_0->SetBinContent(19,0.0); // overflow
  S9_NPID_0->SetEntries(0);
  // Style
  S9_NPID_0->SetLineColor(9);
  S9_NPID_0->SetLineStyle(1);
  S9_NPID_0->SetLineWidth(1);
  S9_NPID_0->SetFillColor(9);
  S9_NPID_0->SetFillStyle(1001);
  S9_NPID_0->SetBarWidth(0.8);
  S9_NPID_0->SetBarOffset(0.1);

  // Creating a new THStack
  THStack* stack = new THStack("mystack_18","mystack");
  stack->Add(S9_NPID_0);
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
  stack->GetXaxis()->SetTitle("NPID ");
  stack->GetXaxis()->SetBinLabel(1,"vm~");
  stack->GetXaxis()->SetBinLabel(2,"mu+");
  stack->GetXaxis()->SetBinLabel(3,"ve~");
  stack->GetXaxis()->SetBinLabel(4,"e+");
  stack->GetXaxis()->SetBinLabel(5,"b~");
  stack->GetXaxis()->SetBinLabel(6,"c~");
  stack->GetXaxis()->SetBinLabel(7,"s~");
  stack->GetXaxis()->SetBinLabel(8,"u~");
  stack->GetXaxis()->SetBinLabel(9,"d~");
  stack->GetXaxis()->SetBinLabel(10,"d");
  stack->GetXaxis()->SetBinLabel(11,"u");
  stack->GetXaxis()->SetBinLabel(12,"s");
  stack->GetXaxis()->SetBinLabel(13,"c");
  stack->GetXaxis()->SetBinLabel(14,"b");
  stack->GetXaxis()->SetBinLabel(15,"e-");
  stack->GetXaxis()->SetBinLabel(16,"ve");
  stack->GetXaxis()->SetBinLabel(17,"mu-");
  stack->GetXaxis()->SetBinLabel(18,"vm");

  // Finalizing the TCanvas
  canvas->SetLogx(0);
  canvas->SetLogy(0);

  // Saving the image
  canvas->SaveAs("../../HTML/MadAnalysis5job_0/selection_8.png");
  canvas->SaveAs("../../PDF/MadAnalysis5job_0/selection_8.png");
  canvas->SaveAs("../../DVI/MadAnalysis5job_0/selection_8.eps");

}
