void DrawPedestals(int RunNumber = 993){

	// run 226 -- shifted ADC pedestals in 2022 (lab tests), for comparison

	gStyle->SetLineScalePS(1);

	TFile *file = new TFile(Form("~/data/output00%d.root", RunNumber));

	TH1F *hADCHists[32];
	for (int i = 0; i < 32; i++){
		hADCHists[i] = (TH1F*)file->Get(Form("ADC%02d", i+1));
		hADCHists[i]->SetStats(0);
		hADCHists[i]->GetXaxis()->SetTitle("Channel");
		hADCHists[i]->GetXaxis()->SetRange(hADCHists[i]->GetMaximumBin()-15, hADCHists[i]->GetMaximumBin()+15);
	}

    TCanvas *c = new TCanvas("adc_channels","VME Data Hists",900,900);
    c->Divide(8,4);
    c->cd();
    for (int i=1;i<=32;i++){
		c->cd(i);
		hADCHists[i-1]->Draw();
    }

}