void CalculateCalibFactor(){

	TFile *file_input = new TFile("LinearityCalib.root", "read");

	const Int_t nPoints = 3;
	const Int_t nChannels = 12;

	Double_t En[nPoints] = {0,0.8,1.6};
	TH1F *hADCAmp[nPoints];

	TGraphErrors *CalibFactorPlot[nChannels];
	TF1 *fit[nChannels];

	TH1F *hCalibFactors = new TH1F("CalibFactors", ";ADC ch.;bits/GeV", 12,0,12);

	for (Int_t i = 0; i < nPoints; i++){

		if (i == 0)
			hADCAmp[i] = (TH1F*)file_input->Get("ADCPed_1GeV");
		else{
			hADCAmp[i] = (TH1F*)file_input->Get(Form("ADCAmp_%dGeV", i));
		}

	}


	for (Int_t ich = 0; ich < nChannels; ich++){

		Double_t ADCAmp[nPoints] = {0.}, ADCErr[nPoints] = {0.}; 
		for (Int_t iEn = 0; iEn < nPoints; iEn++){
			ADCAmp[iEn] = hADCAmp[iEn]->GetBinContent(ich+1);
			if (iEn == 0) ADCErr[iEn] = 0;
			else ADCErr[iEn] = hADCAmp[iEn]->GetBinError(ich+1);
		}

		CalibFactorPlot[ich] = new TGraphErrors(nPoints, En, ADCAmp, 0, ADCErr);
		fit[ich] = new TF1(Form("fit_%dch", ich+1), "[0]*x + [1]", 0, 2);

		CalibFactorPlot[ich]->Fit(fit[ich], "QR0", "", 0, 2);

		// CalibFactors[ich] = fit[ich]->GetParameter(0);
		// CalibFactorErrors[ich] = fit[ich]->GetParError(0);

		hCalibFactors->SetBinContent(ich+1, fit[ich]->GetParameter(0));
		hCalibFactors->SetBinError(ich+1, fit[ich]->GetParError(0));
	}

	CalibFactorPlot[5]->SetTitle(";Beam energy, GeV; ADC amp.");
	CalibFactorPlot[5]->SetMarkerStyle(20);
	CalibFactorPlot[5]->SetMarkerColor(kRed);

	TCanvas *c1 = new TCanvas();
	CalibFactorPlot[5]->Draw("AP");
	fit[5]->Draw("same L");

	hCalibFactors->SetStats(0);
	hCalibFactors->SetMarkerStyle(20);
	hCalibFactors->SetMarkerColor(kRed);
	hCalibFactors->SetLineColor(kRed);

	TH1F *hCalib_1GeV = (TH1F*)hADCAmp[1]->Clone("Calib_1GeV");
	TH1F *hCalib_2GeV = (TH1F*)hADCAmp[2]->Clone("Calib_2GeV");

	hCalib_1GeV->Add(hADCAmp[0], -1);
	hCalib_2GeV->Add(hADCAmp[0], -1);

	hCalib_2GeV->Scale(0.5);

	hCalib_1GeV->SetMarkerStyle(20);
	hCalib_1GeV->SetMarkerColor(kBlue);
	hCalib_1GeV->SetLineColor(kBlue);

	hCalib_2GeV->SetMarkerStyle(20);
	hCalib_2GeV->SetMarkerColor(kBlack);
	hCalib_2GeV->SetLineColor(kBlack);

	TCanvas *c2 = new TCanvas();
	hCalibFactors->Draw("E0");
	hCalib_1GeV->Draw("same");
	hCalib_2GeV->Draw("same");


}