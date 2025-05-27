void ContaminationStudy(){

	gStyle->SetPalette(kRainBow);

	const Int_t nPoints = 11;
	Int_t RunList[nPoints] = {1286,1285,1284,1283,1282,1281,1280,1279,1278,1277,1276}; // for 50V SPS scans
	Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	TString ParameterSet = "10GeV_MeanCalibMethod_0";

	TH1F *hMIPCentL[nPoints];
	TCanvas *c1 = new TCanvas();
	TLegend *l = new TLegend();
	l->SetBorderSize(0);
	l->SetFillStyle(0);

	for (Int_t i = 0; i < nPoints; i++){
		TFile *InputFile = new TFile(Form("~/PHOSBeamTests/data/output%05d_%s.root", RunList[i], ParameterSet.Data()));

		hMIPCentL[i] = (TH1F*)InputFile->Get("hMIPCentL");
		hMIPCentL[i]->SetName(Form("EMIPCentL_%d", i+1));

		hMIPCentL[i]->SetStats(0);
		hMIPCentL[i]->SetMarkerStyle(20);
		// hMIPCentL[i]->Scale(1/hMIPCentL[i]->Integral());

		if (i == 0) hMIPCentL[i]->Draw("E1 PLC PMC");
		else hMIPCentL[i]->Draw("same E1 PLC PMC");

		l->AddEntry(hMIPCentL[i], Form("Beam energy = %0.1f GeV", EnergyList[i]), "lp");

	}

	c1->cd();
	l->Draw("same");

}