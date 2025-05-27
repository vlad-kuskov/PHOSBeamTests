void DrawCombinedResolution(){

	TFile *file0 = new TFile("EnResSys_PS.root", "read");
	TFile *file1 = new TFile("EnResSys.root", "read");

	TGraphErrors *EnResPS = (TGraphErrors*)file0->Get("EnResPlot_1GeV_MeanCalibMethod_0_2FitRange_0BinWidth");
	TGraphErrors *EnResSPS = (TGraphErrors*)file1->Get("EnResPlot_10GeV_MeanCalibMethod_0_0FitRange_0BinWidth_LGThres10GeV_20240505");

	TH1F *hDraw = new TH1F("hDraw", ";Beam energy, GeV; Energy resolution #sigma, %", 20000,0,200);
	hDraw->SetStats(0);

	EnResPS->SetMarkerColor(kBlue);

	TCanvas *c1 = new TCanvas();
	hDraw->GetYaxis()->SetRangeUser(1,8);
	hDraw->GetXaxis()->SetRangeUser(0.01,160);
	c1->SetLogx();
	hDraw->Draw();
	EnResSPS->Draw("same P");
	EnResPS->Draw("same P");

	TLegend *l = new TLegend();
	l->SetBorderSize(0);
	l->SetFillStyle(0);
	l->AddEntry(EnResPS, "PS results", "lp");
	l->AddEntry((TObject*)0, "HV_{HG} = +51V, T = -25#circ C", "");
	l->AddEntry((TObject*)0, "T = -25#circ C", "");
	l->AddEntry(EnResSPS, "SPS results", "lp");
	// l->AddEntry((TObject*)0, "HV_{HG} = +50V, HV_{LG} = +53V", "");
	l->AddEntry((TObject*)0, "HV_{LG} = +43V", "");
	l->AddEntry((TObject*)0, "T = -25#circ C", "");

	c1->cd();
	l->Draw("same");

}