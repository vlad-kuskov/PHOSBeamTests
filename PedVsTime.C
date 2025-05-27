void BinShift(Int_t step = 0, TH1F* HistToShift = NULL){

	if (!HistToShift)
		return;

	TH1F *HistBuffer = (TH1F*)HistToShift->Clone(Form("%s_buffer", HistToShift->GetName()));
	HistToShift->Reset();
	Int_t ibin = 1;

	while (HistBuffer->GetBinContent(ibin) != 0){
		HistToShift->SetBinContent(ibin+step, HistBuffer->GetBinContent(ibin));
		ibin++;
	}

}

Int_t FindEdge(TH1F* Hist = NULL){

	if (!Hist)
		return 0;

	Int_t ibin = 1;
	while (Hist->GetBinContent(ibin) != 0)
		ibin++;

	return ibin;
}

void PedVsTime_SingleRun(){

	const Int_t RunNumber = 1070;
	TFile *file = new TFile(Form("~/data/offline%05d.root", RunNumber));

	const Int_t nChannels = 16;
	TH1F *hPedVsTime[nChannels];

	TFile *output = new TFile("output.root", "update");

	TCanvas *c = new TCanvas("adc_channels","VME Data Hists",900,900);

	for (int i = 0; i < nChannels; i++){

		hPedVsTime[i] = (TH1F*)file->Get(Form("ADC%02d_vs_Time", i+1));
		hPedVsTime[i]->SetName(Form("ADC%02d_%d", i+1, RunNumber));
		hPedVsTime[i]->SetStats(0);
		hPedVsTime[i]->SetTitle(Form("ADC%02d;for 2k events;pedestal mean", i+1));
		hPedVsTime[i]->SetMarkerStyle(20);
		hPedVsTime[i]->SetMarkerSize(1);
		// hPedVsTime[i]->GetXaxis()->SetRangeUser(0,64);

		if (i == 0)
			hPedVsTime[i]->Draw("LP PLC PMC");
		else
			hPedVsTime[i]->Draw("same LP PLC PMC");

		output->cd();
		hPedVsTime[i]->Write(0, TObject::kOverwrite);
	}

	gPad->BuildLegend();

}

void PedVsTime_SeveralRuns(){

	const Int_t nRuns = 6;
	// Int_t RunNumbers[nRuns] = {736, 737,738}; // 0.25 GeV/c
	Int_t RunNumbers[nRuns] = {1165,1163,1150,1160,1158,1133}; // 51V energy scan, 2023

	Int_t Boundaries[nRuns] = {0};

	const Int_t nChannels = 12;
	TH1F *hPedVsTime[nRuns][nChannels];

	TFile *output = new TFile("output.root", "recreate");

	for (Int_t iRun = 0; iRun < nRuns; iRun++){

		TFile *file = new TFile(Form("data/output%05d_1GeV_MeanCalibMethod_GC0.root", RunNumbers[iRun]));
		for(Int_t i = 0; i < nChannels; i++){
			hPedVsTime[iRun][i] = (TH1F*)file->Get(Form("PedEvolution_%02d", i+1));
			hPedVsTime[iRun][i]->SetName(Form("ADC%02d_%d", i+1, iRun));
			hPedVsTime[iRun][i]->SetStats(0);
			hPedVsTime[iRun][i]->SetTitle(Form("ADC%02d;for 2k events;pedestal mean", i+1));
			hPedVsTime[iRun][i]->SetMarkerStyle(20);
			hPedVsTime[iRun][i]->SetMarkerSize(1);

			if (iRun > 0){
				Boundaries[iRun] = FindEdge(hPedVsTime[0][i])-1;
				BinShift(FindEdge(hPedVsTime[0][i])-1, hPedVsTime[iRun][i]);
				hPedVsTime[0][i]->Add(hPedVsTime[iRun][i]);
			}
		}
	}

	TLine *BoundaryLines[nRuns];
	for (Int_t i = 1; i < nRuns; i++){
		BoundaryLines[i] = new TLine(Boundaries[i], 120, Boundaries[i], 220);
		BoundaryLines[i]->SetLineStyle(kDashed);
		BoundaryLines[i]->SetLineColor(kRed);
	}

	for (Int_t i = 1; i < nRuns; i++)
		cout << Boundaries[i] << endl;

	TCanvas *c = new TCanvas("adc_channels","VME Data Hists",900,900);
	for(Int_t i = 0; i < nChannels; i++){
		if (i == 0){
			hPedVsTime[0][i]->GetYaxis()->SetRangeUser(120,220);
			hPedVsTime[0][i]->Draw("LP PLC PMC");
		}
		else
			hPedVsTime[0][i]->Draw("same LP PLC PMC");

		output->cd();
		hPedVsTime[0][i]->Write(0, TObject::kOverwrite);
	}

	gPad->BuildLegend();

	for (Int_t i = 1; i < nRuns; i++)
		BoundaryLines[i]->Draw("same L");

}

void PedVsTime(Int_t option = 2){

	gStyle->SetOptTitle(kFALSE);
	gStyle->SetLineScalePS(1);
	gStyle->SetPalette(kRainBow);

	if (option == 1)
		PedVsTime_SingleRun();
	if (option == 2)
		PedVsTime_SeveralRuns();

}