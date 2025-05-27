void CalculateEnergyResolution(){

	gStyle->SetLineScalePS(1);

	// 1086 -- 0.5 GeV, 1072,1073 -- 1 GeV, 1070 -- 2 GeV, 1074,1075,1076 -- 3 GeV, 1078,1079 -- 4 GeV, 1083,1084,1085 -- 5 GeV
	// const Int_t nPoints = 6;
	// Int_t RunList[nPoints] = {1086,1072,1070,1074,1078,1083}; // for 45V scans
	// Double_t EnergyList[nPoints] = {0.5,1,2,3,4,5};

	const Int_t nPoints = 10;
	Int_t RunList[nPoints] = {1266,1267,1268,1269,1270,1271,1272,1273,1274,1275}; // for 50V scans
	Double_t EnergyList[nPoints] = {10,20,30,40,50,60,80,100,120,150};

	TH1F *hEtotL[nPoints];
	TH1F *hEtotR[nPoints];
	TF1 *fit[nPoints];

	Double_t BeamE2RecE[nPoints] = {1.};

	Double_t Sigma[nPoints] = {0.}, Mean[nPoints] = {0.};
	Double_t SigmaErr[nPoints] = {0.}, MeanErr[nPoints] = {0.};

	TFile *OutputFile;
	TCanvas *c1 = new TCanvas();
	c1->Divide(4,3);

	for (Int_t i = 0; i < nPoints; i++){

		OutputFile = new TFile(Form("~/PHOSBeamTests/data/output%05d_10GeV_MeanCalibMethod_0.root", RunList[i]));

		hEtotL[i] = (TH1F*)OutputFile->Get("ECentL");
		hEtotL[i]->SetName(Form("ECentL_%dGeV", i+1));
		hEtotL[i]->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", EnergyList[i]));
		hEtotL[i]->SetMarkerStyle(20);
		hEtotL[i]->SetMarkerColor(kBlack);
		hEtotL[i]->SetLineColor(kBlack);
		hEtotL[i]->SetStats(0);


		TCanvas *c1 = new TCanvas();
		hEtotL[i]->Draw("E0");



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
			fit[i]->SetRange(fit[i]->GetParameter(1)-1.2*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
			hEtotL[i]->Fit(fit[i], "QR0", "", fit[i]->GetParameter(1)-1.2*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
		}
		else{
			fit[i]->SetRange(fit[i]->GetParameter(1)-1.2*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
			hEtotL[i]->Fit(fit[i], "QR0", "", fit[i]->GetParameter(1)-1.2*fit[i]->GetParameter(2), fit[i]->GetParameter(1)+3*fit[i]->GetParameter(2));
		}

		// cout << hEtotL[i]->GetBinCenter(hEtotL[i]->GetMaximumBin()) << "   " << hEtotL[i]->GetBinCenter(iEdge) << endl;


		Sigma[i] = fit[i]->GetParameter(2)/fit[i]->GetParameter(1)*100;
		SigmaErr[i] = fit[i]->GetParError(2)/fit[i]->GetParameter(1)*100;

		// Sigma[i] = fit[i]->GetParameter(2)/EnergyList[i]*100;
		// SigmaErr[i] = fit[i]->GetParError(2)/EnergyList[i]*100;

		Mean[i] = fit[i]->GetParameter(1);
		MeanErr[i] = fit[i]->GetParError(1);

		BeamE2RecE[i] = fit[i]->GetParameter(1)/EnergyList[i];

		hEtotR[i] = (TH1F*)OutputFile->Get("ECentR");
		hEtotR[i]->SetName(Form("ECentR_%dGeV", i+1));
		hEtotR[i]->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", EnergyList[i]));

		c1->cd(i+1);
		hEtotL[i]->Draw("E0");
		fit[i]->Draw("same L");

		printf("%1.2f GeV: res. = %2.3f +/- %2.3f\n", EnergyList[i], Sigma[i], SigmaErr[i]);

	}


	TGraphErrors *EnResPlot = new TGraphErrors(nPoints, Mean, Sigma, MeanErr, SigmaErr);
	EnResPlot->SetTitle(";Beam energy, GeV; Energy resolution #sigma, %");
	EnResPlot->SetMarkerStyle(20);
	EnResPlot->SetMarkerColor(kRed);

	TF1 *EnResFit = new TF1("EnResFit", "sqrt(([0]/x)*([0]/x) + ([1]/sqrt(x))*([1]/sqrt(x)) + [2]*[2])", 1, 7);
	EnResFit->SetParNames("a", "b", "c");
	EnResFit->SetLineColor(kRed);
	EnResFit->SetLineWidth(2);

	// EnResFit->SetParameters(9,25,1.5);

	// EnResFit->SetParLimits(0, 0, 10);
	// EnResFit->SetParLimits(1, 0, 10);
	// EnResFit->SetParLimits(2, 0, 10);

	EnResPlot->Fit(EnResFit, "QR0", "", 1, 7);
	EnResPlot->Fit(EnResFit, "QR0", "", 1, 7);


	TF1 *EnResIR = new TF1("EnResIR", "sqrt(([0]/x)*([0]/x) + ([1]/sqrt(x))*([1]/sqrt(x)) + [2]*[2])", EnergyList[0]-0.5, EnergyList[nPoints-1]+0.5);
	EnResIR->SetParNames("a", "b", "c");
	EnResIR->SetParameters(2.344, 4.295, 1.18);
	EnResIR->SetLineColor(kBlack);
	EnResIR->SetLineWidth(2);

	TLegend *EnResFormula = new TLegend();
	EnResFormula->SetBorderSize(0);
	EnResFormula->SetFillStyle(0);
	EnResFormula->AddEntry((TObject*)0, Form("#frac{#sigma}{E} = #frac{%4.2f  #pm %4.2f}{E} #oplus #frac{%4.2f  #pm %4.2f}{#sqrt{E}} #oplus (%4.2f  #pm %4.2f)", 
		                                EnResFit->GetParameter(0),  EnResFit->GetParError(0),
		                                EnResFit->GetParameter(1),  EnResFit->GetParError(1),
		                                EnResFit->GetParameter(2),  EnResFit->GetParError(2)), "");

	TCanvas *c2 = new TCanvas();
	EnResPlot->Draw("ap");
	EnResFit->Draw("same L");
	EnResFormula->Draw("same");	


	TGraphErrors *Linearity = new TGraphErrors(nPoints, EnergyList, Mean, 0, MeanErr);
	Linearity->SetTitle(";Beam energy, GeV; Deposited energy, GeV");
	Linearity->SetMarkerStyle(20);
	Linearity->SetMarkerColor(kRed);

	TCanvas *c3 = new TCanvas();
	Linearity->Draw("AP");

	TH1F *hDepEnPositionCorr = new TH1F("DepEnCorrection", "", nPoints, 0, nPoints);

	for (Int_t i = 0; i < nPoints; i++){
		hDepEnPositionCorr->SetBinContent(i+1, Mean[i]/EnergyList[i]);
	}

	// TFile *CalibFile = new TFile("/home/petukhov/PHOSBeamTests/MidasReader/Calibration.root", "update");
	// CalibFile->cd();
	// hDepEnPositionCorr->Write(0, TObject::kOverwrite);

	TGraphErrors *BeamE2RecEPlot = new TGraphErrors(nPoints, EnergyList, BeamE2RecE, 0, 0);
	BeamE2RecEPlot->SetTitle(";Beam energy, GeV; Deposited energy, GeV");
	BeamE2RecEPlot->SetMarkerStyle(20);
	BeamE2RecEPlot->SetMarkerColor(kRed);

	// TF1 *BeamE2RecEFunc = new TF1("BeamE2RecEFunc", "[0]*log([1]*x)", 0,7);
	// BeamE2RecEFunc->SetParameters(0.14, 193);

	// BeamE2RecEPlot->Fit(BeamE2RecEFunc, "R0", "", 0,7);

	TF1 *f1nonlin = new TF1("f1nonlin","[2]*(1.+[0]/(1. + TMath::Power(x/[1],2)))",0,10);
    f1nonlin->SetNpx(1000);
    f1nonlin->SetParNames("a","b (GeV)","c");
    f1nonlin->SetParameters(-0.18, 0.9, 1.052);
    f1nonlin->FixParameter(0, -0.18);
    f1nonlin->FixParameter(1, 0.9);
    f1nonlin->FixParameter(2, 1.052);

    Linearity->Fit(f1nonlin, "R0", "", 0,5);

	TCanvas *c4 = new TCanvas();
	BeamE2RecEPlot->Draw("AP");
	f1nonlin->Draw("same L");


}