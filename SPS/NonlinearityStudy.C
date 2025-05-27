
struct ADCParams{

	TH1F *hist = nullptr;
	TF1 *fit = nullptr;
	Double_t mean = -1.;
	Double_t ped = -1.;
	TGraph *point = nullptr;

	void CalibADC(){

		if (!hist) return;

		hist->GetXaxis()->SetRangeUser(0, 4096);

		TH1F *hCopy = (TH1F*)hist->Clone("hCopy");

		Int_t iPed = hCopy->GetMaximumBin();
		Double_t Edge = 0.;
		bool EdgeFound = false;

		for (int ibin = 1; ibin < hCopy->GetNbinsX()+1; ibin++){
			if (hCopy->GetBinContent(ibin) != 0)
				hCopy->SetBinContent(ibin, TMath::Log(hCopy->GetMaximum()) - TMath::Log(hCopy->GetBinContent(ibin)));
			if (ibin > iPed && !EdgeFound){
				if (hCopy->Integral(ibin-2, ibin+2) == 0){
					Edge = hCopy->GetBinCenter(ibin);
					EdgeFound = true;
				}
			}
			if (ibin < iPed){
				hCopy->SetBinContent(ibin, 0);
				hist->SetBinContent(ibin, 0);
			}
		}

		Double_t Min = hCopy->GetMean();

		iPed  = hist->GetMaximumBin();
		Int_t iMin  = hist->FindBin(Min);
		Int_t iEdge = hist->FindBin(Edge);
		// Int_t iEdge = hist->FindLastBinAbove(0,1);

		hist->GetXaxis()->SetRange(iMin, iEdge);
		Int_t iMax = hist->GetMaximumBin();

		if (iEdge-iMin > 100){
			Int_t step = (iEdge-iMin)/10;
			for (Int_t ibin = iMin; ibin < iEdge; ibin++){
				if (hist->Integral(ibin-step, ibin+step) > hist->Integral(iMax-step, iMax+step))
					iMax = ibin;
			}
		}

		if (iMax-iPed > 100){
			Int_t step = (iMax-iPed)/10;
			for (Int_t ibin = iPed; ibin < iMax; ibin++){
				if (hist->Integral(ibin-step, ibin+step) < hist->Integral(iMin-step, iMin+step))
					iMin = ibin;
			}	
		}

		hist->GetXaxis()->SetRangeUser(0, 4096);
		hist->Rebin(5);

		// if (iMax-iPed < 40){
		// 	hist_signal->SetBinContent(i+1, hist->GetBinCenter(iMax));
		// 	// point->Draw("same P");
		// 	continue;
		// }

		Int_t imin = iMax-0.2*(iEdge-iMax), imax = iEdge;
		// if (iMin > imin) imin = iMin;

		fit = new TF1("fit", "gaus", 0, iEdge);
		fit->SetParLimits(1, imin-10, imax);

		fit->SetParameters(hist->Integral(iMax-3, iMax+3), hist->GetBinCenter(iMax), iEdge-iMax);
		hist->Fit(fit, "QR0", "", imin, imax);

		fit->SetParameter(1, fit->GetParameter(1));
		hist->Fit(fit,"QR0", "", fit->GetParameter(1)-fit->GetParameter(2), fit->GetParameter(1)+2*fit->GetParameter(2));

		mean = fit->GetParameter(1);
		ped = iPed;

		Double_t x[1] = {mean};
		Double_t y[1] = {fit->GetParameter(0)};
		point = new TGraph(1, x, y);
	}

};

void NonlinearityStudy(){

	gStyle->SetPalette(kRainBow);

	const Int_t nPoints = 11;
	Int_t RunList[nPoints] = {1286,1285,1284,1283,1282,1281,1280,1279,1278,1277,1276}; // for 50V SPS scans
	Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	TString ParameterSet = "10GeV_MeanCalibMethod_0";
	Int_t iFitRange = 2, iBinWidth = 0;
	TFile *InputFile;

	ADCParams ADC_CentL[nPoints];
	Double_t Mean[nPoints], MeanErr[nPoints], MeanLin[nPoints], NonlinCorr[nPoints];

	TCanvas *c1 = new TCanvas();
	c1->SetLogy();

	for (Int_t i = 0; i < nPoints; i++){

		InputFile = new TFile(Form("$Projects/PHOSBeamTests/MIDAS/PHOSBeamTests/data/output%05d_%s.root", RunList[i], ParameterSet.Data()));

		ADC_CentL[i].hist = (TH1F*)InputFile->Get(Form("ADC%02d_Raw", 18));
		ADC_CentL[i].CalibADC();

		ADC_CentL[i].hist->SetName(Form("ADC%02d_Raw_%d", 18, i+1));
		ADC_CentL[i].fit->SetName(Form("ADC%02d_fit_%d", 18, i+1));

		if (i == 0) ADC_CentL[i].hist->Draw("hist PLC PMC");
		else ADC_CentL[i].hist->Draw("same hist PLC PMC");
		ADC_CentL[i].fit->Draw("same L");

		Mean[i] = ADC_CentL[i].mean-ADC_CentL[0].ped;
		MeanErr[i] = ADC_CentL[i].fit->GetParError(1);

		MeanLin[i] = EnergyList[i]*(Mean[0])/EnergyList[0];

		NonlinCorr[i] = Mean[i]/MeanLin[i];

	}

	cout << ADC_CentL[0].ped<< endl;

	TGraphErrors *EnNonlin = new TGraphErrors(nPoints, EnergyList, Mean, 0, MeanErr);
	EnNonlin->SetMarkerStyle(20);
	EnNonlin->SetMarkerColor(kRed);

	TGraphErrors *EnLin = new TGraphErrors(nPoints, EnergyList, MeanLin, 0, 0);
	EnLin->SetMarkerStyle(20);
	EnLin->SetMarkerColor(kBlue);

	TCanvas *c2 = new TCanvas();
	EnNonlin->Draw("AP");
	EnLin->Draw("same P");

	TGraphErrors *EnLinCorr = new TGraphErrors(nPoints, EnergyList, NonlinCorr, 0, 0);
	EnLinCorr->SetMarkerStyle(21);
	EnLinCorr->SetMarkerColor(kRed);

	TFile *file0 = new TFile("EnResSys.root", "read");

	TGraphErrors *hNonlin_LGThres = (TGraphErrors*)file0->Get(Form("BeamE2RecEPlot_%s_%dFitRange_%dBinWidth_LGThres10GeV_20240505", ParameterSet.Data(), iFitRange, iBinWidth));
	hNonlin_LGThres->SetMarkerStyle(25);
	hNonlin_LGThres->SetMarkerColor(kRed);

	TLegend *l = new TLegend();
	l->SetBorderSize(0);
	l->SetFillStyle(0);
	l->AddEntry(EnLinCorr, "SiPM nonlin", "p");
	l->AddEntry(hNonlin_LGThres, "Cluster nonlin", "p");

	TCanvas *c3 = new TCanvas();
	EnLinCorr->SetTitle(";Beam energy, GeV; E_{REC}/E_{BEAM}");
	EnLinCorr->Draw("AP");
	hNonlin_LGThres->Draw("same P");
	l->Draw("same");


	Double_t Leakage[nPoints];

	for (Int_t i = 0; i < nPoints; i++){
		Leakage[i] = TMath::Sqrt(dynamic_cast<TGraph*>(hNonlin_LGThres)->GetPointY(i)*dynamic_cast<TGraph*>(hNonlin_LGThres)->GetPointY(i) - 
								 dynamic_cast<TGraph*>(EnLinCorr)->GetPointY(i)*dynamic_cast<TGraph*>(EnLinCorr)->GetPointY(i));

	}

	TGraph *LeakagePlot = new TGraph(nPoints, EnergyList, Leakage);
	LeakagePlot->SetTitle(";Beam energy, GeV; Leakage");
	LeakagePlot->SetMarkerStyle(25);
	LeakagePlot->SetMarkerColor(kBlue);

	TCanvas *c4 = new TCanvas();
	LeakagePlot->Draw("AP");


}