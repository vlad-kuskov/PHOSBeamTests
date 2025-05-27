#include "/home/petukhov/PHOSBeamTests/MidasReader/FileReader.C"

Double_t GS(Double_t *x, Double_t *par){
  Double_t m=par[1];
  Double_t s=par[2];
  Double_t dx=(x[0]-m)/s;

  return par[0]*exp(-dx*dx/2);
}


// Double_t GSs(Double_t *x, Double_t *par)
// {
// 	// Parameterizatin of signal
// 	Double_t m = par[1];
// 	Double_t s = par[2];
// 	Double_t dx = (x[0] - m) / s;
// 	if (dx > 0)
// 		return par[0] * exp(-dx * dx / 2.);
// 	else{
// 		return par[0] * (TMath::Exp(-dx * dx / 2) + TMath::Exp((x[0] - m) / par[3])/* * (1. - TMath::Exp(-dx * dx / 2))*/);
// 	}
// }

Double_t GSs(Double_t * x, Double_t * par){
  //Parameterizatin of signal
  Double_t m=par[1] ;
  Double_t s=par[2] ;
  Double_t dx=(x[0]-m)/s ;
  Double_t alpha_l = par[3];
  Double_t n_l     = par[4];


  Double_t result;
  Double_t fact1TLessMinosAlphaL = alpha_l/n_l;
  Double_t fact2TLessMinosAlphaL = (n_l/alpha_l) - alpha_l -dx;

   if (-alpha_l <= dx) {
     result = exp(-0.5*dx*dx);
   } else if (dx < -alpha_l) {
     result = exp(-0.5*alpha_l*alpha_l)*pow(fact1TLessMinosAlphaL*fact2TLessMinosAlphaL, -n_l);
   }
   return par[0]*result;
}


Double_t CBs(Double_t *x, Double_t *par)
{	
	Double_t m = par[1];
	Double_t s = par[2];
	Double_t n = par[3];
	Double_t a = par[4];
	Double_t dx = (x[0]-m)/s;
	if (dx > a)
		return par[0] * exp(-dx*dx /2.) /*+ par[5]*/;
	else
	{
		Double_t A = TMath::Power((n / TMath::Abs(a)), n) * TMath::Exp(-a*a/2.);
		Double_t B = n/TMath::Abs(a)-TMath::Abs(a);
		return par[0]*A * TMath::Power((B - dx), -n)/* + par[5]*/;
	}
}


void CalculateADCAmplitude(Int_t run = 1302, TString CalibEnergy = "10GeV", TString CalibMethod = "MeanCalibMethod"){

	// run530 -- ADC каналы подключены по изначальному маппингу, 2022
	// run544 -- на ADC поменяли местами каналы 0-15 <-> 16-31, 2022

	// 1070 -- 2 GeV/c, 1072 -- 1 GeV/c, 1076 -- 3 GeV/c, 1077 -- 4 GeV/c, 1085 -- 5 GeV/c, 
	// 1070 -- 2 GeV, 45V
	// 1163 -- 1 GeV, 51V

	// int run = 1063;

	// if (CalibEnergy.Contains("1GeV")) run = 1163;
	// if (CalibEnergy.Contains("2GeV")) run = 1150;
	// // if (CalibEnergy.Contains("10GeV")) run = 1266; //1234
	// if (CalibEnergy.Contains("10GeV")) run = 1286; //1234

	gStyle->SetLineScalePS(1);
	gStyle->SetGridStyle(0);
	TFile *File = new TFile(Form("/home/petukhov/PHOSBeamTests/data/output%05d_%s_%s_0.root", run, CalibEnergy.Data(), CalibMethod.Data()), "read");

	std::vector<std::string> RunRecords = readInfoFile("/home/petukhov/PHOSBeamTests/MidasReader/RunList.txt", Form("%d", run));
	int BeamEnergy = atoi(RunRecords[1].c_str());

	cout << "Beam energy: " << BeamEnergy << endl;

	const int nChannels = 32;
	TH1F *hADC[nChannels];
	// TH1F *hADC_Ped[nChannels];

	TFile *output = new TFile(Form("/home/petukhov/PHOSBeamTests/MidasReader/Calibration.root"), "update");
	TH1F *hADC_signal = new TH1F(Form("ADCPeak_%s_%s", CalibEnergy.Data(), CalibMethod.Data()), ";ADC channel;Signal amplitude", 32, 0, 32);
	TH1F *hADC_pedestals = new TH1F(Form("ADCPed_%s_%s", CalibEnergy.Data(), CalibMethod.Data()), ";ADC channel;Signal amplitude", 32, 0, 32);

	TH1F *hADCAmp_Copy = (TH1F*)hADC_signal->Clone(Form("ADCAmp_%dGeV", BeamEnergy));
	TH1F *hADCPed_Copy = (TH1F*)hADC_pedestals->Clone(Form("ADCPed_%dGeV", BeamEnergy));


	TH2D *hHitMap = new TH2D("hHitMap", ";X, cell;Y,cell", 4,0,4, 3,0,3);
	for (int j = 1; j < 4; j++) hHitMap->GetYaxis()->SetBinLabel(j,Form("%d", 4-j));
    for (int j = 1; j < 5; j++) hHitMap->GetXaxis()->SetBinLabel(j,Form("%d", j));
	hHitMap->SetStats(0);

	TCanvas *c1 = new TCanvas();
	c1->Divide(4,3);


	TCanvas *cADC2 = new TCanvas();
	cADC2->Divide(4,3);


	// Int_t ShowMap[9] = {1,2,3, 5,6,7, 9,10,11}; // mapping for beam tests 2023 at PS
	Int_t ShowMap[9] = {1,2,4, 5,6,7, 9,10,11}; // mapping for beam tests 2024 at SPS

	// mapping for beam tests 2024 at SPS
	Int_t ADCMap[nChannels] = {0,1,3,2,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27,12,13,14,15,28,29,30,31};


	for (int i = 0; i < nChannels; i++){

		if (i > 23) continue;
		Int_t ich = ADCMap[i];

		hADC[i] = (TH1F*)File->Get(Form("ADC%02d_Raw", i+1));
		// hADC_Ped[i] = (TH1F*)File->Get(Form("ADC%02d_Ped", i+1));
		// hADC[i]->Add(hADC_Ped[i], -1);
		hADC[i]->GetXaxis()->SetRangeUser(0, 4096);

		TH1F *hCopy = (TH1F*)hADC[i]->Clone(Form("hCopy_%d", i));
		if (i < 12)
			hCopy->Rebin(5);

		Int_t iPed = hCopy->GetMaximumBin();
		Double_t Edge = 0.;
		bool EdgeFound = false;

		for (int ibin = 1; ibin < hCopy->GetNbinsX()+1; ibin++){
			if (hCopy->GetBinContent(ibin) != 0)
				hCopy->SetBinContent(ibin, TMath::Log(hCopy->GetMaximum()) - TMath::Log(hCopy->GetBinContent(ibin)));
			if (ibin > iPed && !EdgeFound){
				if (hCopy->Integral(ibin-5, ibin+5) == 0){
					Edge = hCopy->GetBinCenter(ibin);
					EdgeFound = true;
				}
			}
			if (ibin < iPed)
				hCopy->SetBinContent(ibin, 0);
		}

		// c2->cd(i+1);
		// hCopy->DrawCopy();

		Double_t Min = hCopy->GetMean();

		iPed  = hADC[i]->GetMaximumBin();
		Int_t iMin  = hADC[i]->FindBin(Min);
		Int_t iEdge = hADC[i]->FindBin(Edge);
		// Int_t iEdge = hADC[i]->FindLastBinAbove(0,1);

		hADC[i]->GetXaxis()->SetRange(iMin, iEdge);
		Int_t iMax = hADC[i]->GetMaximumBin();

		if (iEdge-iMin > 100){
			Int_t step = (iEdge-iMin)/10;
			for (Int_t ibin = iMin; ibin < iEdge; ibin++){
				if (hADC[i]->Integral(ibin-step, ibin+step) > hADC[i]->Integral(iMax-step, iMax+step))
					iMax = ibin;
			}
		}

		if (iMax-iPed > 100){
			Int_t step = (iMax-iPed)/10;
			for (Int_t ibin = iPed; ibin < iMax; ibin++){
				if (hADC[i]->Integral(ibin-step, ibin+step) < hADC[i]->Integral(iMin-step, iMin+step))
					iMin = ibin;
			}	
		}

		hADC[i]->GetXaxis()->SetRangeUser(0, 4096);
		if (i < 12)
			hADC[i]->Rebin(5);

		// if (iMax-iPed < 40){
		// 	hADC_signal->SetBinContent(i+1, hADC[i]->GetBinCenter(iMax));
		// 	// point->Draw("same P");
		// 	continue;
		// }

		Int_t imin = iMax-0.2*(iEdge-iMax), imax = iEdge;
		// if (iMin > imin) imin = iMin;

		TF1 *fit = new TF1("fit", "gaus",0, 1.1*iEdge);
		// fit->SetParLimits(1, imin-10, imax);

		fit->SetParameters(hADC[i]->Integral(iMax-3, iMax+3), hADC[i]->GetBinCenter(iMax), iEdge-iMax);
		hADC[i]->Fit(fit, "QR0", "", imin, imax);

		fit->SetParameter(1, fit->GetParameter(1));
		hADC[i]->Fit(fit,"QR0", "", fit->GetParameter(1)-fit->GetParameter(2), fit->GetParameter(1)+2*fit->GetParameter(2));

		Double_t ADCPeak = fit->GetParameter(1);
		if (CalibMethod.Contains("MeanCalibMethod")) ADCPeak = fit->GetParameter(1);
		if (CalibMethod.Contains("EdgeCalibMethod")) ADCPeak = hADC[i]->GetBinCenter(iEdge);

		hADC_signal->SetBinContent(i+1, (ADCPeak-hADC[i]->GetBinCenter(iPed))/BeamEnergy);
		hADC_signal->SetBinError(i+1, fit->GetParameter(2));
		hADC_pedestals->SetBinContent(i+1, hADC[i]->GetBinCenter(iPed));

		hADCAmp_Copy->SetBinContent(i+1, fit->GetParameter(1));
		hADCAmp_Copy->SetBinError(i+1, fit->GetParameter(2));
		hADCPed_Copy->SetBinContent(i+1, hADC[i]->GetBinCenter(iPed));

		Double_t x[3] = {fit->GetParameter(1), fit->GetParameter(1), hADC[i]->GetBinCenter(iEdge)};
		Double_t y[3] = {fit->GetParameter(0), fit->GetParameter(0), 1};

		// Double_t x[3] = {hADC[i]->GetBinCenter(imin), fit->GetParameter(1), hADC[i]->GetBinCenter(iEdge)};
		// Double_t y[3] = {hADC[i]->GetBinContent(imin), fit->GetParameter(0), hADC[i]->GetBinContent(iEdge)};

		TGraph *point = new TGraph(3, x, y);
		point->SetMarkerStyle(20);
		point->SetMarkerSize(1);
		point->SetMarkerColor(kRed);

		if (i < 12){
			c1->cd(i+1);
			c1->cd(i+1)->SetLogy();

			hADC[i]->SetStats(0);
			hADC[i]->GetXaxis()->SetRangeUser(iPed, iEdge+20);

			hADC[i]->Draw();
			point->Draw("same P");
			fit->DrawCopy("same L");
		}
		else{
			cADC2->cd(i-11);
			cADC2->cd(i-11)->SetLogy();

			hADC[i]->SetStats(0);
			hADC[i]->GetXaxis()->SetRangeUser(iPed, iEdge+20);

			hADC[i]->Draw();
			point->Draw("same P");
			fit->DrawCopy("same L");
		}

		hHitMap->SetBinContent(i%4+1, 3-i/4, hADC[i]->Integral(fit->GetParameter(1)-2.5*fit->GetParameter(2), fit->GetParameter(1)+2.5*fit->GetParameter(2)));
		delete fit;

	}

	output->cd();
	hADC_signal->Write(0, TObject::kOverwrite);
	hADC_pedestals->Write(0, TObject::kOverwrite);
	// output->Close();

	TCanvas *c3 = new TCanvas();
	c3->cd();
	c3->SetGrid();
	hHitMap->Draw("colz");

	TFile *file_1 = new TFile("LinearityCalib.root", "update");
	file_1->cd();
	hADCAmp_Copy->Write(0, TObject::kOverwrite);
	hADCPed_Copy->Write(0, TObject::kOverwrite);
	// file_1->Close();

}

//testing new fitting procedure GSs, CBs 17/04/2024
void CalculateADCAmplitude_test(int i = 5, std::string CalibEnergy = "1GeV", std::string CalibMethod = "MeanCalibMethod"){

	int run = 1063;

	if (strncmp(CalibEnergy.c_str(), "1", 1) == 0) run = 1163;
	if (strncmp(CalibEnergy.c_str(), "2", 1) == 0) run = 1150;

	TFile *File = new TFile(Form("/home/petukhov/PHOSBeamTests/data/output%05d_%s_%s_GC0.root", run, CalibEnergy.c_str(), CalibMethod.c_str()), "read");
	TH1F *hADC = (TH1F*)File->Get(Form("ADC%02d_Raw", i+1));

	TH1F *hCopy = (TH1F*)hADC->Clone(Form("hCopy_%d", i));

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
			if (ibin < iPed)
				hCopy->SetBinContent(ibin, 0);
		}

		// c2->cd(i+1);
		// hCopy->DrawCopy();

		Double_t Min = hCopy->GetMean();

		iPed  = hADC->GetMaximumBin();
		Int_t iMin  = hADC->FindBin(Min);
		Int_t iEdge = hADC->FindBin(Edge);

		hADC->GetXaxis()->SetRange(iMin, iEdge);
		Int_t iMax = hADC->GetMaximumBin();

		// if (iEdge-iMin > 100){
		// 	Int_t step = (iEdge-iMin)/10;
		// 	for (Int_t ibin = iMin; ibin < iEdge; ibin++){
		// 		if (hADC->Integral(ibin-step, ibin+step) > hADC->Integral(iMax-step, iMax+step))
		// 			iMax = ibin;
		// 	}
		// }

		// if (iMax-iPed > 100){
		// 	Int_t step = (iMax-iPed)/10;
		// 	for (Int_t ibin = iPed; ibin < iMax; ibin++){
		// 		if (hADC->Integral(ibin-step, ibin+step) < hADC->Integral(iMin-step, iMin+step))
		// 			iMin = ibin;
		// 	}	
		// }

		hADC->GetXaxis()->SetRangeUser(0, 4096);

		// if (iMax-iPed < 40){
		// 	hADC_signal->SetBinContent(i+1, hADC[i]->GetBinCenter(iMax));
		// 	// point->Draw("same P");
		// 	continue;
		// }

		Int_t imin = iMax-0.5*(iEdge-iMax), imax = iEdge;

		Double_t x[3] = {hADC->GetBinCenter(imin), hADC->GetBinCenter(iMax), hADC->GetBinCenter(iEdge)};
		Double_t y[3] = {hADC->GetBinContent(imin), hADC->GetBinContent(iMax), 1};

		TGraph *point = new TGraph(3, x, y);
		point->SetMarkerStyle(20);
		point->SetMarkerSize(1);
		point->SetMarkerColor(kRed);

	TF1 *fit = new TF1("fit", GS, imin, imax, 3);

	TCanvas *c1 = new TCanvas();
	c1->SetLogy();
	hADC->Draw();
	fit->Draw("same L");
	point->Draw("same P");

}