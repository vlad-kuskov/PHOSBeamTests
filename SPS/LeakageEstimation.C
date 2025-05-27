void LeakageEstimation(Int_t iSet = 0, Int_t iFitRange = 0, Int_t iBinWidth = 0){

	const Int_t nCuts = 12;
	const Char_t *ParameterSet[nCuts] = {"10GeV_MeanCalibMethod_0", "1GeV_MeanCalibMethod_1", "1GeV_MeanCalibMethod_2",
										 "1GeV_EdgeCalibMethod_0", "1GeV_EdgeCalibMethod_1", "1GeV_EdgeCalibMethod_2",
										 "2GeV_MeanCalibMethod_0", "2GeV_MeanCalibMethod_1", "2GeV_MeanCalibMethod_2",
										 "2GeV_EdgeCalibMethod_0", "2GeV_EdgeCalibMethod_1", "2GeV_EdgeCalibMethod_2"};

	const Int_t nFitRanges = 3;
	Double_t FitRanges[nFitRanges] = {1.2, 0.75, 1.5};
	
	const Int_t nPoints = 3;

	Int_t RunList[nPoints] = {1315,1316,1317};
	Int_t RunListRef[nPoints] = {1309,1311,1313};
	Double_t EnergyList[nPoints] = {80,100,150};

	TFile *InputFile;
	TFile *InputFileRef;
	TCanvas *c1 = new TCanvas();
	c1->Divide(2,2);
	TCanvas *c2 = new TCanvas();
	c2->Divide(2,2);

	TH1F *hEtotL[nPoints];
	TF1 *fit[nPoints];

	TH1F *hLeakage[nPoints];
	TH1F *hRatio[nPoints];

	Double_t MaxLeakEn[nPoints];
	Double_t ElectronPeak[nPoints];

	for(Int_t i = 0; i < nPoints; i++){

		// if (i != 1) continue;

		InputFile = new TFile(Form("$Projects/PHOSBeamTests/MIDAS/PHOSBeamTests/data/output%05d_%s.root", RunList[i], ParameterSet[iSet]));
		InputFileRef = new TFile(Form("$Projects/PHOSBeamTests/MIDAS/PHOSBeamTests/data/output%05d_%s.root", RunListRef[i], ParameterSet[iSet]));

		TH1F *hADC = (TH1F*)InputFile->Get("ADC01_Raw"); hADC->SetName("hADC");
		Double_t nEvents = hADC->GetEntries();

		TH1F *hADCRef = (TH1F*)InputFileRef->Get("ADC01_Raw");
		Double_t nEventsRef = hADCRef->GetEntries();

		hEtotL[i] = (TH1F*)InputFileRef->Get("ECentL_LG");
		hLeakage[i] = (TH1F*)InputFile->Get("ECentL_LG");
		hEtotL[i]->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", EnergyList[i]));

		hEtotL[i]->GetXaxis()->SetRangeUser(0.2*EnergyList[i], 1.2*EnergyList[i]);

		hEtotL[i]->SetMarkerStyle(20);
		hEtotL[i]->SetMarkerColor(kBlack);
		hEtotL[i]->SetLineColor(kBlack);
		hEtotL[i]->SetStats(0);

		hEtotL[i]->GetXaxis()->SetRangeUser(0.2*EnergyList[i], 1.2*EnergyList[i]);

		Int_t iEdge = hEtotL[i]->GetNbinsX()-1;

		for (Int_t ibin = hEtotL[i]->GetMaximumBin()+1; ibin < hEtotL[i]->GetNbinsX()+1; ibin++){
			if (hEtotL[i]->GetBinContent(ibin) == 0){
				iEdge = ibin;
				break;
			}
		}

		if (iEdge > hEtotL[i]->GetNbinsX() || iEdge < hEtotL[i]->GetMaximumBin() ) 
			iEdge = hEtotL[i]->GetMaximumBin() + (int)(hEtotL[i]->GetNbinsX()-hEtotL[i]->GetMaximumBin())/2;

		fit[i] = new TF1(Form("GS_%dGeV", i+1), "gaus",  0.9*hEtotL[i]->GetBinCenter( hEtotL[i]->GetMaximumBin() ), 
														 hEtotL[i]->GetBinCenter(iEdge));

		hEtotL[i]->Fit(fit[i], "QR0", "", 0.9*hEtotL[i]->GetBinCenter( hEtotL[i]->GetMaximumBin() ), 
										  hEtotL[i]->GetBinCenter(iEdge));

		fit[i]->SetRange(fit[i]->GetParameter(1)-FitRanges[iFitRange]*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
		hEtotL[i]->Fit(fit[i], "QR0", "", fit[i]->GetParameter(1)-FitRanges[iFitRange]*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));

		ElectronPeak[i] = fit[i]->GetParameter(1);

		hEtotL[i]->Rebin(20);
		hEtotL[i]->Scale(1/nEventsRef);

		hLeakage[i]->SetMarkerStyle(20);
		hLeakage[i]->SetMarkerColor(kRed+2);
		hLeakage[i]->SetLineColor(kRed+2);
		hLeakage[i]->SetStats(0);

		hLeakage[i]->Rebin(20);
		hLeakage[i]->Scale(1/nEvents);

		hRatio[i] = (TH1F*)hLeakage[i]->Clone(Form("hRatio_%d", i+1));
		hRatio[i]->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Leakage rate", EnergyList[i]));
		hRatio[i]->SetStats(0);
		hRatio[i]->Sumw2();
		hRatio[i]->Divide(hEtotL[i]);
		// hRatio[i]->Add(hEtotL[i], -1.);

		// TF1 *fit1 = new TF1("fit1", "pol1", 0., 0.8*EnergyList[i]);
		// hRatio[i]->Fit(fit1, "QR0", "", 0.2*EnergyList[i], 0.8*EnergyList[i]);
		// hRatio[i]->Divide(fit1);

		// Int_t N = 0;
		for (Int_t ibin = 1; ibin <= hRatio[i]->GetNbinsX(); ibin++){
			if ((hRatio[i]->GetBinContent(ibin) - hRatio[i]->GetBinError(ibin)) < 1.){
				MaxLeakEn[i] = hRatio[i]->GetBinCenter(ibin);
				break;
			}
		}

		TLegend *l = new TLegend();
		l->SetBorderSize(0);
		l->SetFillStyle(0);
		l->AddEntry(hLeakage[i], "EM Shower after Absorber", "lp");
		l->AddEntry((TObject*)0, Form("Run with Absorber: %d", RunListRef[i]), "");
		l->AddEntry(hEtotL[i], "Full EM Shower", "lp");
		l->AddEntry((TObject*)0, Form("Reference Run: %d", RunList[i]), "");

		c1->cd(i+1);
		c1->cd(i+1)->SetLogy();

		hEtotL[i]->Draw("E1");
		hLeakage[i]->Draw("same E1");
		l->Draw("same");

		c2->cd(i+1);
		c2->cd(i+1)->SetLogy();

		hRatio[i]->Draw("E1");

	}

	for (Int_t i = 0; i < nPoints; i++){
		printf("%1.2f GeV: Electron Peak = %2.3f, Max Leakage En = %2.3f\n", EnergyList[i], ElectronPeak[i], MaxLeakEn[i]);
		printf("%1.2f GeV: leakage = %2.3f\n", EnergyList[i], MaxLeakEn[i]/ElectronPeak[i]);
	}

}