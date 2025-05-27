void ShowADC_15mu(int RunNumber = 1115){

	TFile *file = new TFile(Form("~/data/offline%05d.root", RunNumber), "read");

	const int nChannels = 12;
	TH1F *hADC[nChannels];

	for (int i = 0; i < nChannels; i++){

		hADC[i] = (TH1F*)file->Get(Form("ADC%02d", i+1));
		hADC[i]->SetStats(0);
		hADC[i]->SetTitle(Form("%s;digits;counts", hADC[i]->GetTitle()));

	}

	TCanvas *c = new TCanvas("cadc_channels_15mu","ADC channels", 900,900);
	c->Divide(4,3);

    c->cd();
    for (int i=0;i<nChannels;i++){
		c->cd(i+1);
		c->cd(i+1)->SetLogy();
		hADC[i]->Draw();
    }

}

void SlewingADC(int RunNumber = 1045){


	TFile *file = new TFile(Form("~/data/output%dfull.root", RunNumber), "read");

	const int nChannels = 12;
	TH2F *hSlew[nChannels];

	for (int i = 0; i < nChannels; i++){

		hSlew[i] = (TH2F*)file->Get(Form("ADC_Times_vs_Amps_%d", i+1));
		hSlew[i]->SetStats(0);
		hSlew[i]->GetXaxis()->SetRangeUser(50,350);
		hSlew[i]->GetYaxis()->SetRangeUser(2000,3500);
		hSlew[i]->SetTitle(Form("%s;ADC channels;TDC channels", hSlew[i]->GetTitle()));

	}

	TCanvas *c = new TCanvas("slew","Slewing ADC", 900,900);
	c->Divide(4,3);

    c->cd();
    for (int i = 0; i < nChannels; i++){
		c->cd(i+1);
		hSlew[i]->Draw("colz");
    }

}

void SlewingQDC(int RunNumber = 1045){


	TFile *file = new TFile(Form("~/data/output%dfull.root", RunNumber), "read");

	const int nChannels = 12;
	TH2F *hSlew[nChannels];

	for (int i = 0; i < 8; i++){

		hSlew[i] = (TH2F*)file->Get(Form("ADC_Times_vs_Amps_%d", i+16));
		hSlew[i]->SetStats(0);
		// hSlew[i]->GetXaxis()->SetRangeUser(50,350);
		// hSlew[i]->GetYaxis()->SetRangeUser(2000,3500);
		hSlew[i]->SetTitle(Form("%s;QDC channels;TDC channels", hSlew[i]->GetTitle()));

	}

	TCanvas *c = new TCanvas("slew","Slewing QDC", 900,900);
	c->Divide(4,2);

    c->cd();
    for (int i = 0; i < 8; i++){
		c->cd(i+1);
		hSlew[i]->Draw("colz");
    }

}


void OnlineMonitoring(int RunNumber = 1115){

	gStyle->SetLineScalePS(1);

	int option = 1; // temproary 28.09.2023

	if (option == 1)
		ShowADC_15mu(RunNumber);
	if (option == 2)
		SlewingADC(RunNumber);
	if (option == 3)
		SlewingQDC(RunNumber);


}