void SysEnergyResolution(Int_t iSet = 0, Int_t iFitRange = 0, Int_t iBinWidth = 0){

	const Int_t nCuts = 12;
	const Char_t *ParameterSet[nCuts] = {"10GeV_MeanCalibMethod_0", "1GeV_MeanCalibMethod_1", "1GeV_MeanCalibMethod_2",
										 "1GeV_EdgeCalibMethod_0", "1GeV_EdgeCalibMethod_1", "1GeV_EdgeCalibMethod_2",
										 "2GeV_MeanCalibMethod_0", "2GeV_MeanCalibMethod_1", "2GeV_MeanCalibMethod_2",
										 "2GeV_EdgeCalibMethod_0", "2GeV_EdgeCalibMethod_1", "2GeV_EdgeCalibMethod_2"};

	if (iFitRange > 2) iFitRange = 0;
	if (iBinWidth > 2) iBinWidth = 0;

	const Int_t nFitRanges = 3;
	Double_t FitRanges[nFitRanges] = {1.2, 0.75, 1.5};

	// const Int_t nPoints = 10;
	// Int_t RunList[nPoints] = {1266,1267,1268,1269,1270,1271,1272,1273,1274,1275}; // for 50V SPS scans
	// Double_t EnergyList[nPoints] = {10,20,30,40,50,60,80,100,120,150};

	// Double_t ECutHigh[nPoints] = {8.6, 16.8, 24.5, 31, 37, 42, 52, 60, 70, 80};
	// Double_t ECutLow[nPoints] = {4, 8, 14, 18, 20, 24, 32, 36, 36, 50};

	// const Int_t nPoints = 9;
	// Int_t RunList[nPoints] = {1234,1235,1236,1237,1241,1242,1243,1244,1245}; // for 16C SPS scans
	// Double_t EnergyList[nPoints] = {10,15,20,40,50,60,80,120,150};

	// Double_t ECutHigh[nPoints] = {8, 12, 16, 30, 37, 43, 52, 70, 80};
	// Double_t ECutLow[nPoints] =  {4,  6,  8, 18, 23, 26, 38, 45, 50};

	// 05.05.2024 good energy resolution
	// const Int_t nPoints = 11;
	// Int_t RunList[nPoints] = {1286,1285,1284,1283,1282,1281,1280,1279,1278,1277,1276}; // for 50V SPS scans
	// Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	// 07.05.2024
	// const Int_t nPoints = 11;
	// Int_t RunList[nPoints] = {1299,1297,1296,1295,1294,1293,1292,1291,1290,1289,1288}; // for 50V SPS scans
	// Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	// 07.05.2024_2 LG=+43V HG=+50V
	// const Int_t nPoints = 11;
	// Int_t RunList[nPoints] = {1302,1303,1304,1305,1306,1307,1308,1309,1311,1312,1313}; // for 50V SPS scans
	// Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	// 11.05.2024 HG=+43V LG=+43V
	// const Int_t nPoints = 11;
	// Int_t RunList[nPoints] = {1384,1382,1381,1380,1379,1378,1377,1394,1376,1374,1373};
	// Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	//12.05.2024 HG=+43V LG=+43V no finger
	const Int_t nPoints = 11;
	Int_t RunList[nPoints] = {1387,1388,1389,1390,1391,1392,1393,1394,1395,1396,1397};
	Double_t EnergyList[nPoints] = {10,15,20,30,40,50,60,80,100,120,150};

	TH1F *hEtotL[nPoints];
	TH1F *hEtotR[nPoints];
	TF1 *fit[nPoints];

	TH1F *hContamination = new TH1F("hContamination", ";Beam energy, GeV;Hadron Contamination (?)", nPoints, 0, nPoints);
	for (Int_t i1 = 1; i1 <= hContamination->GetNbinsX(); i1++){
		hContamination->GetXaxis()->SetBinLabel(i1, Form("%0.1f", EnergyList[i1-1]));
	}

	Double_t BeamE2RecE[nPoints] = {1.};

	Double_t Sigma[nPoints] = {0.}, Mean[nPoints] = {0.};
	Double_t SigmaErr[nPoints] = {0.}, MeanErr[nPoints] = {0.};

	TFile *InputFile;
	TCanvas *c1 = new TCanvas();
	c1->Divide(4,3);

	for (Int_t i = 0; i < nPoints; i++){

		InputFile = new TFile(Form("$Projects/PHOSBeamTests/MIDAS/PHOSBeamTests/data_20240518/data/output%05d_%s.root", RunList[i], ParameterSet[iSet]));

		TString histname = "ECentL";
		// if (i < 2)
		// 	histname = "ECentL_HG";
		// else histname = "ECentL";

		hEtotL[i] = (TH1F*)InputFile->Get(histname);
		hEtotL[i]->SetName(Form("ECentL_%dGeV", i+1));
		hEtotL[i]->SetTitle(Form("Beam energy = %0.1f GeV;E_{deposited}, GeV;Counts", EnergyList[i]));
		hEtotL[i]->SetMarkerStyle(20);
		hEtotL[i]->SetMarkerColor(kBlack);
		hEtotL[i]->SetLineColor(kBlack);
		hEtotL[i]->SetStats(0);

		hEtotL[i]->Rebin(5);

		// if (iBinWidth == 0){
		// 	if (i < 1)
		// 		hEtotL[i]->Rebin(6);
		// 	else if (i < 2)
		// 		hEtotL[i]->Rebin(3);
		// 	else if (i == 3)
		// 		hEtotL[i]->Rebin(3);
		// 	else if (i == 6)
		// 		hEtotL[i]->Rebin(4);
		// 	else
		// 		hEtotL[i]->Rebin(2);
		// }
		// else if (iBinWidth == 1){
		// 	if (i < 1)
		// 		hEtotL[i]->Rebin(4);
		// 	else if (i == 6)
		// 		hEtotL[i]->Rebin(2);
		// }
		// else if (iBinWidth == 2){
		// 	if (i < 1)
		// 		hEtotL[i]->Rebin(10);
		// 	else if (i < 2)
		// 		hEtotL[i]->Rebin(4);
		// 	else if (i == 3)
		// 		hEtotL[i]->Rebin(4);
		// 	else if (i == 6)
		// 		hEtotL[i]->Rebin(6);
		// 	else
		// 		hEtotL[i]->Rebin(3);
		// }

		// if (i != 0)
		// 	hEtotL[i]->GetXaxis()->SetRangeUser(0.5*EnergyList[i], 1.5*EnergyList[i]);

		// if (EnergyList[i] > 50)
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

		// cout << hEtotL[i]->GetBinCenter(hEtotL[i]->GetMaximumBin()) << "   " << hEtotL[i]->GetBinCenter(iEdge) << endl;


		Sigma[i] = fit[i]->GetParameter(2)/fit[i]->GetParameter(1)*100;
		SigmaErr[i] = fit[i]->GetParError(2)/fit[i]->GetParameter(1)*100;

		// Sigma[i] = fit[i]->GetParameter(2)/EnergyList[i]*100;
		// SigmaErr[i] = fit[i]->GetParError(2)/EnergyList[i]*100;

		Mean[i] = fit[i]->GetParameter(1);
		MeanErr[i] = fit[i]->GetParError(1);

		BeamE2RecE[i] = fit[i]->GetParameter(1)/EnergyList[i];

		c1->cd(i+1);
		hEtotL[i]->GetXaxis()->SetRangeUser(Mean[i]-2.5*Sigma[i], Mean[i]+2.5*Sigma[i]);
		hEtotL[i]->Draw("E0");
		fit[i]->Draw("same L");
		// hDump->DrawCopy("same LF2");
		// hPeak->DrawCopy("same LF2");

		// TLegend *lf = new TLegend();
		// lf->SetBorderSize(0);
		// lf->SetFillStyle(0);
		// lf->AddEntry(hDump, "Hadrons (?)", "f");
		// lf->AddEntry(hPeak, "Electron peak", "f");

		// lf->Draw("same");

		printf("%1.2f GeV: res. = %2.3f +/- %2.3f\n", EnergyList[i], Sigma[i], SigmaErr[i]);

		TH1F *hMIP = (TH1F*)hEtotL[i]->Clone(Form("hMIP_%d", i+1));
		hMIP->GetXaxis()->SetRangeUser(0,0.5);
		TH1F *hElectron = (TH1F*)hEtotL[i]->Clone(Form("hMIP_%d", i+1));
		hElectron->GetXaxis()->SetRangeUser(0, 1.5*EnergyList[i]);
		hElectron->GetXaxis()->SetRangeUser(Mean[i]-3*Sigma[i], Mean[i]+3*Sigma[i]);

		hContamination->SetBinContent(i+1, hMIP->Integral()/(hElectron->Integral()+hMIP->Integral()));

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

	// EnResFit->SetParLimits(0, 0, 5);
	// EnResFit->SetParLimits(1, 0, 5);
	// EnResFit->FixParameter(2, 1);
	EnResPlot->Fit(EnResFit, "QR0", "", EnergyList[0], EnergyList[nPoints-1]);

	TCanvas *cEnRes = new TCanvas();
	EnResPlot->Draw("AP");
	EnResFit->Draw("same L");

	TGraphErrors *BeamE2RecEPlot = new TGraphErrors(nPoints, EnergyList, BeamE2RecE, 0, 0);
	BeamE2RecEPlot->SetName(Form("BeamE2RecEPlot_%s_%dFitRange_%dBinWidth", ParameterSet[iSet], iFitRange, iBinWidth));
	BeamE2RecEPlot->SetTitle(";Beam energy, GeV; E_{REC}/E_{BEAM}");
	BeamE2RecEPlot->SetMarkerStyle(20);
	BeamE2RecEPlot->SetMarkerColor(kRed);

	TCanvas *cBeamE2RecE = new TCanvas();
	BeamE2RecEPlot->Draw("AP");


	TFile *OutputFile = new TFile("EnResSys.root", "update");
	EnResPlot->Write(0, TObject::kOverwrite);
	EnResFit->Write(0, TObject::kOverwrite);
	BeamE2RecEPlot->Write(0, TObject::kOverwrite);

	TCanvas *cContamination = new TCanvas();
	hContamination->SetStats(0);
	hContamination->SetLineColor(kBlue);
	hContamination->SetLineWidth(2);
	hContamination->Draw("hist");

	TFile *HadronOutput = new TFile("HadronContamination.root", "update");
	hContamination->SetName("hContamination");
	hContamination->Write(0, TObject::kOverwrite);
}