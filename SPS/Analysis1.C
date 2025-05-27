void Analysis1(){

	gStyle->SetPalette(kRainBow);

	const Int_t nPoints = 11;
	Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	TFile *file0 = new TFile("EnResSys.root", "read");

	TString ParameterSet = "10GeV_MeanCalibMethod_0";
	Int_t iFitRange = 2, iBinWidth = 0;

	TGraphErrors *hResCold_HG = (TGraphErrors*)file0->Get(Form("EnResPlot_%s_%dFitRange_%dBinWidth_HG", ParameterSet.Data(), iFitRange, iBinWidth));
	TGraphErrors *hResCold_LG = (TGraphErrors*)file0->Get(Form("EnResPlot_%s_%dFitRange_%dBinWidth_LG", ParameterSet.Data(), iFitRange, iBinWidth));
	TGraphErrors *hResCold_LGThres = (TGraphErrors*)file0->Get(Form("EnResPlot_%s_0FitRange_%dBinWidth_LGThres10GeV_20240505", ParameterSet.Data(), iBinWidth));
	TGraphErrors *hResWarm = (TGraphErrors*)file0->Get(Form("EnResPlot_%s_%dFitRange_%dBinWidth_LGThres10GeV_Warm", ParameterSet.Data(), iFitRange, iBinWidth));

	for (Int_t i = 3; i < nPoints; i++){
		hResCold_HG->RemovePoint(i);
	}

	TCanvas *c1 = new TCanvas();

	hResCold_HG->GetYaxis()->SetRangeUser(0.5, 2.6);
	hResCold_HG->Draw("AP PMC");
	hResCold_LG->Draw("same P PMC");
	hResCold_LGThres->Draw("same P PMC");
	// hResWarm->Draw("same P PMC");

	TLine *line0 = new TLine(80, 0.5, 80, 2.6);
	line0->SetLineColor(kBlack);
	line0->SetLineStyle(2);

	TLegend *l = new TLegend();
	l->SetBorderSize(0);
	l->SetFillStyle(0);
	l->AddEntry(hResCold_HG, "Low Energy Channel (HG) +53V", "p");
	l->AddEntry(hResCold_LG, "High Energy Channel (LG) +50V", "p");
	l->AddEntry(hResCold_LGThres, "High Energy Channel (LG) +43V", "p");
	// l->AddEntry(hResWarm, "+16C", "p");

	c1->cd();
	// line0->Draw("same");
	l->Draw("same");

	//_____________________________________

	TGraphErrors *hNonlin_HG = (TGraphErrors*)file0->Get(Form("BeamE2RecEPlot_%s_%dFitRange_%dBinWidth_HG", ParameterSet.Data(), iFitRange, iBinWidth));
	TGraphErrors *hNonlin_LG = (TGraphErrors*)file0->Get(Form("BeamE2RecEPlot_%s_%dFitRange_%dBinWidth_LG", ParameterSet.Data(), iFitRange, iBinWidth));
	TGraphErrors *hNonlin_LGThres = (TGraphErrors*)file0->Get(Form("BeamE2RecEPlot_%s_%dFitRange_%dBinWidth_LGThres10GeV", ParameterSet.Data(), iFitRange, iBinWidth));

	TCanvas *c2 = new TCanvas();

	hNonlin_LGThres->GetYaxis()->SetTitle("E_{REC}/E_{BEAM}");

	// hNonlin_HG->GetYaxis()->SetTitle("E_{REC}/E_{BEAM}");
	// hNonlin_HG->Draw("AP PMC");
	// hNonlin_LG->Draw("same P PMC");
	hNonlin_LGThres->Draw("AP PMC");

	TLegend *l1 = new TLegend();
	l1->SetBorderSize(0);
	l1->SetFillStyle(0);
	l1->AddEntry(hNonlin_HG, "Low Energy Channel (HG) +53V", "p");
	l1->AddEntry(hNonlin_LG, "High Energy Channel (LG) +50V", "p");
	l1->AddEntry(hNonlin_LGThres, "High Energy Channel (LG) +43V", "p");

	// c2->cd();
	// l1->Draw("same");

}