void SysEnergyResolution(Int_t iSet = 6, Int_t iFitRange = 0, Int_t iBinWidth = 0){

	const Int_t nCuts = 12;
	const Char_t *ParameterSet[nCuts] = {"1GeV_MeanCalibMethod_0", "1GeV_MeanCalibMethod_1", "1GeV_MeanCalibMethod_2",
										 "1GeV_EdgeCalibMethod_0", "1GeV_EdgeCalibMethod_1", "1GeV_EdgeCalibMethod_2",
										 "2GeV_MeanCalibMethod_0", "2GeV_MeanCalibMethod_1", "2GeV_MeanCalibMethod_2",
										 "2GeV_EdgeCalibMethod_0", "2GeV_EdgeCalibMethod_1", "2GeV_EdgeCalibMethod_2"};

	if (iFitRange > 2) iFitRange = 0;
	if (iBinWidth > 2) iBinWidth = 0;

	const Int_t nFitRanges = 3;
	Double_t FitRanges[nFitRanges] = {1.2, 0.75, 1.5};

	const Int_t nPoints = 7;
	Int_t RunList[nPoints] = {1165,1163,1150,1160,1158,1133,1121}; // for 51V scans
	Double_t EnergyList[nPoints] = {0.5,1,2,3,5,7,10};

	TH1F *hEtotL[nPoints];
	TH1F *hEtotR[nPoints];
	TF1 *fit[nPoints];

	Double_t BeamE2RecE[nPoints] = {1.};

	Double_t Sigma[nPoints] = {0.}, Mean[nPoints] = {0.};
	Double_t SigmaErr[nPoints] = {0.}, MeanErr[nPoints] = {0.};

	TFile *InputFile;
	TCanvas *c1 = new TCanvas();
	c1->Divide(3,2);

	for (Int_t i = 0; i < nPoints; i++){

		InputFile = new TFile(Form("data_20240505/output%05d_%s.root", RunList[i], ParameterSet[iSet]));

		if (i < nPoints-1 && i != 0)
			hEtotL[i] = (TH1F*)InputFile->Get("ECentL");
		else hEtotL[i] = (TH1F*)InputFile->Get("ECentR");
		hEtotL[i]->SetName(Form("ECentL_%dGeV", i+1));
		hEtotL[i]->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", EnergyList[i]));
		hEtotL[i]->SetMarkerStyle(20);
		hEtotL[i]->SetMarkerColor(kBlack);
		hEtotL[i]->SetLineColor(kBlack);
		hEtotL[i]->SetStats(0);

		if (iBinWidth == 0){
			if (i < 1)
				hEtotL[i]->Rebin(6);
			else if (i < 2)
				hEtotL[i]->Rebin(3);
			else if (i == 3)
				hEtotL[i]->Rebin(3);
			else if (i == 6)
				hEtotL[i]->Rebin(4);
			// else
			// 	hEtotL[i]->Rebin(2);
		}
		else if (iBinWidth == 1){
			if (i < 1)
				hEtotL[i]->Rebin(4);
			else if (i == 6)
				hEtotL[i]->Rebin(2);
		}
		else if (iBinWidth == 2){
			if (i < 1)
				hEtotL[i]->Rebin(10);
			else if (i < 2)
				hEtotL[i]->Rebin(4);
			else if (i == 3)
				hEtotL[i]->Rebin(4);
			else if (i == 6)
				hEtotL[i]->Rebin(6);
			// else
			// 	hEtotL[i]->Rebin(3);
		}

		if (i == nPoints-1)
			hEtotL[i]->Rebin(3);

		if (i != 0)
			hEtotL[i]->GetXaxis()->SetRangeUser(0.5*EnergyList[i], 1.5*EnergyList[i]);


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

		if (i < 2){
			fit[i]->SetRange(fit[i]->GetParameter(1)-FitRanges[iFitRange]*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
			hEtotL[i]->Fit(fit[i], "QR0", "", fit[i]->GetParameter(1)-FitRanges[iFitRange]*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
		}
		else{
			fit[i]->SetRange(fit[i]->GetParameter(1)-1.2*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
			hEtotL[i]->Fit(fit[i], "QR0", "", fit[i]->GetParameter(1)-FitRanges[iFitRange]*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
		}

		// cout << hEtotL[i]->GetBinCenter(hEtotL[i]->GetMaximumBin()) << "   " << hEtotL[i]->GetBinCenter(iEdge) << endl;


		Sigma[i] = fit[i]->GetParameter(2)/fit[i]->GetParameter(1)*100;
		SigmaErr[i] = fit[i]->GetParError(2)/fit[i]->GetParameter(1)*100;

		// Sigma[i] = fit[i]->GetParameter(2)/EnergyList[i]*100;
		// SigmaErr[i] = fit[i]->GetParError(2)/EnergyList[i]*100;

		Mean[i] = fit[i]->GetParameter(1);
		MeanErr[i] = fit[i]->GetParError(1);

		BeamE2RecE[i] = fit[i]->GetParameter(1)/EnergyList[i];

		c1->cd(i+1);
		hEtotL[i]->Draw("E0");
		fit[i]->Draw("same L");

		printf("%1.2f GeV: res. = %2.3f +/- %2.3f\n", EnergyList[i], Sigma[i], SigmaErr[i]);

	}


	TGraphErrors *EnResPlot = new TGraphErrors(nPoints, EnergyList, Sigma, 0, SigmaErr);
	EnResPlot->SetName(Form("EnResPlot_%s_%dFitRange_%dBinWidth", ParameterSet[iSet], iFitRange, iBinWidth));
	EnResPlot->SetTitle(";Beam energy, GeV; Energy resolution #sigma, %");
	EnResPlot->SetMarkerStyle(20);
	EnResPlot->SetMarkerColor(kRed);

	TF1 *EnResFit = new TF1(Form("EnResFit_%s_%dFitRange_%dBinWidth", ParameterSet[iSet], iFitRange, iBinWidth), 
								 "sqrt(([0]/x)*([0]/x) + ([1]/sqrt(x))*([1]/sqrt(x)) + [2]*[2])", EnergyList[0]-0.5, EnergyList[nPoints-1]+0.5);
	EnResFit->SetParNames("a", "b", "c");
	EnResFit->SetLineColor(kRed);
	EnResFit->SetLineWidth(2);

	EnResFit->SetParameters(2.05, 4.5, 1.1);
	// EnResFit->SetParLimits(0, 2., 2.5);
	// EnResFit->SetParLimits(1, 3.3, 5.);
	// EnResFit->SetParLimits(2, 0.8, 1.2);
	EnResPlot->Fit(EnResFit, "QR0", "", EnergyList[0], EnergyList[nPoints-1]);

	TLegend *EnResFormula = new TLegend();
	EnResFormula->SetBorderSize(0);
	EnResFormula->SetFillStyle(0);
	EnResFormula->AddEntry((TObject*)0, Form("#frac{#sigma}{E} (%%) = #frac{%4.2f #pm %4.2f}{E} #oplus #frac{%4.2f #pm %4.2f}{#sqrt{E}} #oplus (%4.2f #pm %4.2f)", 
		                                EnResFit->GetParameter(0),  EnResFit->GetParError(0),
		                                EnResFit->GetParameter(1),  EnResFit->GetParError(1),
		                                EnResFit->GetParameter(2),  EnResFit->GetParError(2)), "");

	TCanvas *cEnRes = new TCanvas();
	EnResPlot->Draw("AP");
	EnResFit->Draw("same L");
	EnResFormula->Draw("same");

	TGraphErrors *BeamE2RecEPlot = new TGraphErrors(nPoints, EnergyList, BeamE2RecE, 0, 0);
	BeamE2RecEPlot->SetName(Form("BeamE2RecEPlot_%s_%dFitRange_%dBinWidth", ParameterSet[iSet], iFitRange, iBinWidth));
	BeamE2RecEPlot->SetTitle(";Beam energy, GeV; Deposited energy, GeV");
	BeamE2RecEPlot->SetMarkerStyle(20);
	BeamE2RecEPlot->SetMarkerColor(kRed);


	TFile *OutputFile = new TFile("EnResSys.root", "update");
	EnResPlot->Write(0, TObject::kOverwrite);
	EnResFit->Write(0, TObject::kOverwrite);
	BeamE2RecEPlot->Write(0, TObject::kOverwrite);
}