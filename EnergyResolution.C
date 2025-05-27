void EnergyResolution(){

	gStyle->SetLineScalePS(1);

	const Int_t nPoints = 10;
	const Int_t nPoints_47V = 3;

	// 43V
	Double_t En[nPoints] = {0.25, 0.5, 0.75, 1., 2., 3., 4., 5., 7., 10}; // GeV
	Double_t Sigma[nPoints] = {15.7, 6.54, 5.46, 3.93, 2.64, 2.54, 2.157, 1.88, 1.83, 1.74}; // %
	Double_t Sigma_err[nPoints] = {0.88, 0.21, 0.16, 0.11, 0.056, 0.045, 0.07, 0.055, 0.1}; // %

	//47V
	Double_t En_47V[nPoints_47V] = {0.25, 0.75, 2.}; // GeV
	Double_t Sigma_47V[nPoints_47V] = {13.5, 4.88, 2.57}; // %
	Double_t Sigma_err_47V[nPoints_47V] = {0.53, 0.135, 0.076}; // %

	TF1 *EnResFit = new TF1("BeamTests_2022", "sqrt(([0]/x)*([0]/x) + ([1]/sqrt(x))*([1]/sqrt(x)) + [2]*[2])", En[0], En[nPoints-1]);
	EnResFit->SetParNames("a", "b", "c");
	EnResFit->SetLineColor(kMagenta);
	EnResFit->SetLineWidth(2);

	TGraphErrors *EnResPlot = new TGraphErrors(nPoints, En, Sigma, 0, Sigma_err);
	EnResPlot->SetTitle(";Beam energy, GeV; Energy resolution #sigma, %");
	EnResPlot->SetMarkerStyle(20);
	EnResPlot->SetMarkerColor(kRed);

	TGraphErrors *EnResPlot_47V = new TGraphErrors(nPoints_47V, En_47V, Sigma_47V, 0, Sigma_err_47V);
	EnResPlot_47V->SetTitle(";Beam energy, GeV; Energy resolution #sigma, %");
	EnResPlot_47V->SetMarkerStyle(22);
	EnResPlot_47V->SetMarkerSize(1.5);
	EnResPlot_47V->SetMarkerColor(kMagenta);

	EnResPlot->Fit(EnResFit, "R", "", En[0], En[nPoints-1]);

	TF1 *PHOS_2003 = new TF1("PHOS_2003", "sqrt(([0]/x)*([0]/x) + ([1]/sqrt(x))*([1]/sqrt(x)) + [2]*[2])", 0.6, En[nPoints-1]);
	PHOS_2003->SetParNames("a", "b", "c");
	PHOS_2003->SetParameters(1.3, 3.58, 1.12);
	PHOS_2003->SetLineColor(kBlue);
	PHOS_2003->SetLineStyle(2);

	TLegend *l = new TLegend();
	l->AddEntry(EnResPlot, "43V", "p");
	l->AddEntry(EnResPlot_47V, "47V", "p");
	l->AddEntry(EnResFit, "PHOS Beam test 2022", "l");
	l->AddEntry(PHOS_2003, "PHOS Beam test 2003", "l");

	TLegend *EnResFormula = new TLegend();
	EnResFormula->SetBorderSize(0);
	EnResFormula->SetFillStyle(0);
	EnResFormula->AddEntry((TObject*)0, Form("#frac{#sigma}{E} (%) = #frac{%4.2f #pm %4.2f}{E} #oplus #frac{%4.2f #pm %4.2f}{#sqrt{E}} #oplus (%4.2f #pm %4.2f)", 
		                                EnResFit->GetParameter(0),  EnResFit->GetParError(0),
		                                EnResFit->GetParameter(1),  EnResFit->GetParError(1),
		                                EnResFit->GetParameter(2),  EnResFit->GetParError(2)), "");

	TCanvas *c1 = new TCanvas();

	c1->SetLogx();
	c1->SetGridx();
	c1->SetGridy();

	EnResPlot->GetXaxis()->SetTitleOffset(1.25);
	EnResFit->SetNpx(1.e4);
	PHOS_2003->SetNpx(1.e4);

	EnResPlot->Draw("ap");
	EnResFit->Draw("same");
	EnResPlot_47V->Draw("same p");
	PHOS_2003->Draw("same");
	l->Draw("same");
	EnResFormula->Draw("same");

	TFile *output = new TFile("ResolutionLines.root", "update");
	EnResFit->Write(0, TObject::kOverwrite);
	PHOS_2003->Write(0, TObject::kOverwrite);

}