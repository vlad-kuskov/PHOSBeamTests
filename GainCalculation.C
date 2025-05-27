void GainCalculation_BigSiPM(int run){

	gStyle->SetLineScalePS(1);

	TFile *File = new TFile(Form("~/data/output00%d.root", run), "read");

	const int nChannels = 12;
	int iADC[12] = {0,1,2,3,4,5,6,6,7,8,9,10};
	// int iADC[nChannels] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

	TH1F *hADC[nChannels];
	Double_t Max[nChannels] = {0};
	Double_t Min[nChannels] = {0};
	Double_t Edge[nChannels] = {0};

	TCanvas *c2 = new TCanvas();
	c2->Divide(3,4);

	for (Int_t i = 0; i < nChannels; i++){

		hADC[i] = (TH1F*)File->Get(Form("ADC%02d", i+1));
		hADC[i]->GetXaxis()->SetRangeUser(0, hADC[i]->GetNbinsX());

		TH1F *hCopy1 = (TH1F*)hADC[i]->Clone(Form("hCopy1_%d", i));
		TH1F *hCopy = (TH1F*)hADC[i]->Clone(Form("hCopy_%d", i));

		hCopy1->Rebin(10);
		Int_t iPed = hCopy1->GetMaximumBin();
		Int_t iEdge = 1;

		Int_t scale = 2;
		Int_t n = 0;

		for (int ibin = iPed; ibin < hCopy1->GetNbinsX()+1; ibin+=5){
			if (hCopy1->GetBinContent(ibin) == 0){
				iEdge = ibin;
				break;
			}
		}

		if (hCopy1->GetBinCenter(iEdge) < 200)
			scale = 1;
		else if (hCopy1->GetBinCenter(iEdge) > 200 && hCopy1->GetBinCenter(iEdge) < 400)
			scale = 2;
		else if (hCopy1->GetBinCenter(iEdge) > 400 && hCopy1->GetBinCenter(iEdge) < 600)
			scale = 3;
		else if (hCopy->GetBinCenter(iEdge) > 600 && hCopy1->GetBinCenter(iEdge) < 1000)
			scale = 10;
		else if (hCopy1->GetBinCenter(iEdge) > 1000)
			scale = 15;

		// cout << hCopy1->GetBinCenter(iEdge) << " " << scale << endl;

		hCopy->Rebin(scale);

		iPed = hCopy->GetMaximumBin();
		bool EdgeFound = false;

		// hCopy->Scale(1/hCopy->Integral());

		for (int ibin = 1; ibin < hCopy->GetNbinsX()+1; ibin++){
			if (hCopy->GetBinContent(ibin) != 0)
				hCopy->SetBinContent(ibin, TMath::Log(hCopy->GetBinContent(ibin)));
			// if (ibin < iPed)
			// 	hCopy->SetBinContent(ibin, 0);
			else if (ibin > iPed && !EdgeFound){
				Edge[i] = hCopy->GetBinCenter(ibin);
				EdgeFound = true;
			}
		}

		c2->cd(i+1);
		hCopy->DrawCopy();

		TSpectrum *c = new TSpectrum(2);
		Int_t npeaks = c->Search(hCopy, 2, "goff", 0.10);;

		// while (npeaks < 2){

		// 	hCopy->Rebin(2);

		// 	for (int ibin = 1; ibin < hCopy->GetNbinsX()+1; ibin++){
		// 		if (hCopy->GetBinContent(ibin) != 0)
		// 			hCopy->SetBinContent(ibin, TMath::Log(hCopy->GetBinContent(ibin)));
		// 		else if (ibin > iPed && !EdgeFound){
		// 			Edge[i] = hCopy->GetBinCenter(ibin);
		// 			EdgeFound = true;
		// 		}
		// 	}

		// 	npeaks = c->Search(hCopy, 2, "goff", 0.10);

		// 	n++;
		// 	if (n > 3)
		// 		break;
		// }

		Double_t *p = c->GetPositionX();

		Max[i] = p[0];
		for (int ip = 0; ip < npeaks; ip++){
			if (Max[i] < p[ip])
				Max[i] = p[ip];

			cout << i << "  " << p[ip] << endl;
		}

		delete c;

	}

	TCanvas *c1 = new TCanvas("cadc_channels_sipm_6x6","SiPM 6x6 ADC",900,900);
	c1->Divide(4,3);

	TFile *output = new TFile(Form("RunInfo.root"), "update");
	TH1F *hADC_signal = new TH1F(Form("ADC_%d", run), ";ADC channel;Signal amplitude", 32, 1, 32);

	for (int i = 1; i <= nChannels; i++){

		int y_i = (i-1)/4 + 1;
      	int x_i = i - 4*(y_i-1);

		c1->cd(i);
		c1->cd(i)->SetLogy();

		if (i == 7)
			continue;
		else{

			int iPed  = hADC[iADC[i-1]]->GetMaximumBin();
			int iMax  = hADC[iADC[i-1]]->FindBin(Max[iADC[i-1]]);
			int iEdge = hADC[iADC[i-1]]->FindBin(Edge[iADC[i-1]]);

			Double_t x[3] = {hADC[iADC[i-1]]->GetBinCenter(iMax), hADC[iADC[i-1]]->GetBinCenter(iMax), hADC[iADC[i-1]]->GetBinCenter(iEdge)};
			Double_t y[3] = {hADC[iADC[i-1]]->GetBinContent(iMax), hADC[iADC[i-1]]->GetBinContent(iMax), 1};

			TGraph *point = new TGraph(3, x, y);
			point->SetMarkerStyle(20);
			point->SetMarkerSize(1);
			point->SetMarkerColor(kRed);

			TF1 *fit = new TF1("fit", "gaus", 0, iEdge);
			fit->SetParameter(1, hADC[iADC[i-1]]->GetBinCenter(iMax));
			hADC[iADC[i-1]]->Fit(fit, "R0", "", iMax-(iEdge-iMax), iEdge);

			fit->SetParameter(1, fit->GetParameter(1));
			hADC[iADC[i-1]]->Fit(fit,"R0", "", fit->GetParameter(1)-fit->GetParameter(2), fit->GetParameter(1)+2*fit->GetParameter(2));

			hADC[iADC[i-1]]->SetTitle(Form("X:%d Y:%d Big;ADC channels;Counts", x_i, y_i));
		    hADC[iADC[i-1]]->SetStats(0);

		    // cout << fit->GetChisquare()/fit->GetNDF() << endl;

		    hADC[iADC[i-1]]->GetXaxis()->SetRangeUser(iPed-20, iEdge+20);
		    hADC[iADC[i-1]]->Draw();
		    fit->DrawCopy("same L");
		    point->Draw("same P");

		    hADC_signal->SetBinContent(iADC[i-1]+1, fit->GetParameter(1)-hADC[iADC[i-1]]->GetMaximumBin());

		    delete fit;

		}

	}

	// output->cd();
	// hADC_signal->Write(0, TObject::kOverwrite);


}

void GainCalculation_SmallSiPM(){

	gStyle->SetLineScalePS(1);

	TFile *File = new TFile("output00748.root", "read");

	int iADC[9] = {23,24,25,26,27,28,29,30,31};

	TH1F *hADC[9];
	Double_t Max[9] = {0};
	Double_t Edge[9] = {0};

	for (Int_t i = 0; i < 9; i++){

		hADC[i] = (TH1F*)File->Get(Form("ADC%02d", iADC[i]));

		TH1F *hCopy = (TH1F*)hADC[i]->Clone("hCopy");
		hCopy->Rebin(5);

		Double_t iPed = hCopy->GetMaximumBin();
		bool EdgeFound = false;

		for (int ibin = 1; ibin < hCopy->GetNbinsX()+1; ibin++){
			if (hCopy->GetBinContent(ibin) != 0)
				hCopy->SetBinContent(ibin, TMath::Log(hCopy->GetBinContent(ibin)));
			else if (ibin > iPed && !EdgeFound){
				Edge[i] = hCopy->GetBinCenter(ibin);
				EdgeFound = true;
			}
		}

		TSpectrum *c = new TSpectrum(2);
		int npeaks = c->Search(hCopy, 2, "", 0.10);
		Double_t *p = c->GetPositionX();

		Max[i] = p[1];

		delete c;


	}

	TCanvas *c1 = new TCanvas("cadc_channels_sipm_3x3","SiPM 3x3 ADC",900,900);
	c1->Divide(3,3);

	for (int i = 1; i <= 9; i++){

	  	c1->cd(i);
	  	c1->cd(i)->SetLogy();

		int iPed  = hADC[i-1]->GetMaximumBin();
		int iMax  = hADC[i-1]->FindBin(Max[i-1]);
		int iEdge = hADC[i-1]->FindBin(Edge[i-1]);

		TF1 *fit = new TF1("fit", "gaus", (iMax-(iEdge-iMax)), (iEdge));
		fit->SetParameter(1, hADC[i-1]->GetBinCenter(iMax));
		hADC[i-1]->Fit(fit, "R0", "", (iMax-(iEdge-iMax)), (iEdge));

		// fit->SetParameter(1, fit->GetParameter(1));
		hADC[i-1]->Fit(fit,"R0", "", fit->GetParameter(1)-fit->GetParameter(2), fit->GetParameter(1)+2*fit->GetParameter(2));

		int y_i = (i-1)/3 + 1;
      	int x_i = i - 3*(y_i-1);

      	hADC[i-1]->SetTitle(Form("X:%d Y:%d Small;ADC channels;Counts", x_i, y_i));
      	hADC[i-1]->GetXaxis()->SetRangeUser(0, 600);
      	hADC[i-1]->SetStats(0);

      	cout << fit->GetChisquare()/fit->GetNDF() << endl;
		
		hADC[i-1]->Draw();
		fit->Draw("same L");

  }

}


void GainCalculation(int run = 630){

	// if (option == 1)
	GainCalculation_BigSiPM(run);
	// if (option == 2)
	// 	GainCalculation_SmallSiPM();
}