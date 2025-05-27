void CalculateSingleEnergyResolution(Int_t RunNumber = 1266, Double_t BeamEnergy = 10., TString CalibEnergy = "10GeV"){

	gStyle->SetLineScalePS(1);

	// 1086 -- 0.5 GeV, 1072,1073 -- 1 GeV, 1070 -- 2 GeV, 1074,1075,1076 -- 3 GeV, 1078,1079 -- 4 GeV, 1083,1084,1085 -- 5 GeV
	// const Int_t nPoints = 6;
	// Int_t RunList[nPoints] = {1086,1072,1070,1074,1078,1083}; // for 45V scans
	// Double_t EnergyList[nPoints] = {0.5,1,2,3,4,5};


	TH1F *hEtotL;
	TH1F *hEtotR;
	TF1 *fit;

	Double_t BeamE2RecE = 1.;

	Double_t Sigma = 0., Mean = 0.;
	Double_t SigmaErr = 0., MeanErr = 0.;

	TFile *OutputFile = new TFile(Form("~/PHOSBeamTests/data/output%05d_%s_MeanCalibMethod_0.root", RunNumber, CalibEnergy.Data()));

	hEtotL = (TH1F*)OutputFile->Get("ECentL");
	hEtotL->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", BeamEnergy));
	hEtotL->SetMarkerStyle(20);
	hEtotL->SetMarkerColor(kBlack);
	hEtotL->SetLineColor(kBlack);
	hEtotL->SetStats(0);


	TCanvas *c1 = new TCanvas();
	hEtotL->Draw("E0");

	/*
	if (BeamEnergy < 1.)
		hEtotL->Rebin(10);
	else if (BeamEnergy < 2.)
		hEtotL->Rebin(3);
	else if (BeamEnergy == 3.)
		hEtotL->Rebin(3);
	else if (BeamEnergy == 7.)
		hEtotL->Rebin(4);
	else
		hEtotL->Rebin(2);
	*/

	if (BeamEnergy != 0.5)
		hEtotL->GetXaxis()->SetRangeUser(0.5*BeamEnergy, 1.5*BeamEnergy);


	Int_t iEdge = hEtotL->GetNbinsX()-1;

	for (Int_t ibin = hEtotL->GetMaximumBin()+1; ibin < hEtotL->GetNbinsX()+1; ibin++){
		if (hEtotL->GetBinContent(ibin) == 0){
			iEdge = ibin;
			break;
		}
	}

	if (iEdge > hEtotL->GetNbinsX() || iEdge < hEtotL->GetMaximumBin() ) 
		iEdge = hEtotL->GetMaximumBin() + (int)(hEtotL->GetNbinsX()-hEtotL->GetMaximumBin())/2;

	fit = new TF1("GS", "gaus",  0.95*hEtotL->GetBinCenter( hEtotL->GetMaximumBin() ), 
													 hEtotL->GetBinCenter(iEdge));

	hEtotL->Fit(fit, "QR0", "", 0.95*hEtotL->GetBinCenter( hEtotL->GetMaximumBin() ), 
									  hEtotL->GetBinCenter(iEdge));


	if (BeamEnergy < 2.){
		fit->SetRange(fit->GetParameter(1)-1.2*fit->GetParameter(2), fit->GetParameter(1)+3*fit->GetParameter(2));
		hEtotL->Fit(fit, "QR0", "", fit->GetParameter(1)-1.2*fit->GetParameter(2), fit->GetParameter(1)+3*fit->GetParameter(2));
	}
	else{
		fit->SetRange(fit->GetParameter(1)-1.2*fit->GetParameter(2), fit->GetParameter(1)+3*fit->GetParameter(2));
		hEtotL->Fit(fit, "QR0", "", fit->GetParameter(1)-1.2*fit->GetParameter(2), fit->GetParameter(1)+3*fit->GetParameter(2));
	}

	cout << hEtotL->GetBinCenter(hEtotL->GetMaximumBin()) << "   " << hEtotL->GetBinCenter(iEdge) << endl;


	Sigma = fit->GetParameter(2)/fit->GetParameter(1)*100;
	SigmaErr = fit->GetParError(2)/fit->GetParameter(1)*100;

	// Sigma = fit->GetParameter(2)/BeamEnergy*100;
	// SigmaErr = fit->GetParError(2)/BeamEnergy*100;

	Mean = fit->GetParameter(1);
	MeanErr = fit->GetParError(1);

	BeamE2RecE = fit->GetParameter(1)/BeamEnergy;

	hEtotR = (TH1F*)OutputFile->Get("ECentR");
	hEtotR->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", BeamEnergy));

	TLegend *l = new TLegend();
	l->SetFillStyle(0);
	l->SetBorderSize(0);
	l->AddEntry((TObject*)0, Form("Run %d", RunNumber), "");
	l->AddEntry((TObject*)0, "T = 16#circ C", "");
	l->AddEntry((TObject*)0, "SiPM: S14160-6015PS (15#mum)", "");
	l->AddEntry((TObject*)0, "U_{SiPM} = +53V", "");
	l->AddEntry((TObject*)0, Form("%1.2f GeV: res. = %2.3f +/- %2.3f", BeamEnergy, Sigma, SigmaErr), "");

	c1->cd();
	hEtotL->Draw("E0");
	fit->Draw("same L");
	l->Draw("same");

	printf("%1.2f GeV: res. = %2.3f +/- %2.3f\n", BeamEnergy, Sigma, SigmaErr);


}
