void DrawContamination(){

	TFile *ContFile = new TFile("HadronContamination.root", "read");

	TH1F *hContamination1 = (TH1F*)ContFile->Get("hContamination_20240501_Warm");
	TH1F *hContamination2 = (TH1F*)ContFile->Get("hContamination_20240504_Cold");

	hContamination1->SetLineColor(kRed+2);

	TLegend *l = new TLegend();
	l->SetBorderSize(0);
	l->SetFillStyle(0);
	l->AddEntry(hContamination1, "20240501 T=+16C", "l");
	l->AddEntry(hContamination2, "20240504 T=-25C", "l");

	TCanvas *c1 = new TCanvas();
	hContamination1->Draw("hist");
	hContamination2->Draw("same hist");
	l->Draw("same");

}