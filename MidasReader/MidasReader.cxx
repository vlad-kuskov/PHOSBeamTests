#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>

#include "TMidasFile.h"
#include "TMidasEvent.h"

#include "VMEModulesReader.h"
#include "FileReader.C"

#include <TROOT.h>
#include <TMath.h>
#include <TSystem.h>
#include <TFile.h>
#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>

double gBeamEnergy = 0;

bool gFirstRunFound = false;

std::vector<int> RunNumbers;
bool gIsCalibrationRun = false;

UINT32 gQDCSize = 0;
char *gQDCData = 0;
bool gQDCisRead = false;
UINT32 gADCSize = 0;
char *gADCData = 0;
bool gADCisRead = false;
UINT32 gTDCSize = 0;
char *gTDCData = 0;
bool gTDCisRead = false;

double PedQDC[N_CHAN] = {0.}, PedADC[N_CHAN] = {0.};
double ADCPeakMean[N_CHAN] = {0.}, ADCPeakWidth[N_CHAN] = {0.};
double CalibFactorADC = 1.;

double QDC[N_CHAN] = {-1.}, ADC[N_CHAN] = {-1.}, TDC[N_CHAN] = {0.};

int adc_chans[N_CHAN] = {0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27,12,13,14,15,28,29,30,31};
int tdc_chans[N_CHAN] = {0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27,12,13,14,15,28,29,30,31};
int qdc_chans[N_CHAN] = {16,17,18,19,20,21,22,23,24,25,26,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,27,28,29,30,31};

int qdc_mapping[N_CHAN] = {-1};
int adc_mapping[N_CHAN] = {-1};
int tdc_mapping[N_CHAN] = {-1};

TFile *OutputFile;
std::string CalibEnergy = "1GeV";
std::string CalibMethod = "MeanCalibMethod";
std::string GravityCenterSet = "GC0";

int GCcutIndex = 0;

TH1F *hEtot;

TH1F *hEtotL;
TH1F *hEClustL;
TH1F *hECentL;

TH1F *hEtotR;
TH1F *hECentR;

TH1F *hSCurveX;
TH1F *hSCurveY;
TH2F *hXY;

TH1F *hRateQDC;
TH1F *hRateADC;

TH1F *hQDCAmp[N_CHAN];

TH1F *hADCRaw[N_CHAN];
TH1F *hADCRaw_tmp[N_CHAN];
TH1F *hPedEvolution[N_CHAN];
TH1F *hADCEn[N_CHAN];
TH1F *hADCEn_MaxAmp5[N_CHAN];


TH1F *hADCMax;
TH2F *hHitMap;

TH1F *hRateTDC;
TH1F *hTDC[N_CHAN];

void InitHistograms();

void ProcessQDC();
void ProcessADC();
void ProcessTDC();

void AnalyzeData();

double RecEnergyCorrection(int runnumber, double En);

int main(int argc, char *argv[]){

	for (int i = 1; i < argc; i++){

		if (strncmp(argv[i],"-calib", 2) == 0)
			gIsCalibrationRun = true;

		if (static_cast<std::string>(argv[i]).find("GeV") != string::npos) CalibEnergy = static_cast<std::string>(argv[i]).erase(0,1);
		if (static_cast<std::string>(argv[i]).find("CalibMethod") != string::npos) CalibMethod = static_cast<std::string>(argv[i]).erase(0,1);
		if (static_cast<std::string>(argv[i]).find("GC") != string::npos){
			GravityCenterSet = static_cast<std::string>(argv[i]).erase(0,1);
			GCcutIndex = atoi(GravityCenterSet.erase(0,2).c_str());
			if (GCcutIndex > 2) GCcutIndex = 0;
		}

		const char arg = argv[i][0];
		if (arg == '-') continue;

		std::vector<std::string> RunRecords = readInfoFile("RunList.txt", static_cast<std::string>(argv[i]));

		if (strncmp(argv[i], RunRecords[0].c_str(), 2) != 0){
			printf("\nERROR: run %s has not been foud in run list!!!!\n\n", argv[i]);
			// printf("WARNING: beam energy set as 0 GeV\n");
			return -1;
		}

		if (!gBeamEnergy){
			gBeamEnergy = std::stod(RunRecords[1].c_str());
			printf("\nBeam energy is set %0.1f GeV\n", gBeamEnergy);
		}

		int energy = atoi(RunRecords[1].c_str());
		if (energy != gBeamEnergy){
			printf("\nWARNING: beam energy in run %s is not the same as in the first run!!!\n", argv[i]);
			printf("WARNING: beam energ isy set %0.1f GeV as in the first run\n", gBeamEnergy);
		}

		int RunNumber = atoi(argv[i]);
		RunNumbers.push_back(RunNumber);
	}

	// printf("\n%s_%s_%s\n", CalibEnergy.c_str(), CalibMethod.c_str(), GravityCenterSet.c_str());
	printf("\nGravity center set is %d\n", GCcutIndex);
	InitHistograms();

	const int nFiles = RunNumbers.size();
	TMidasFile InputFile;

	int nevents = 0;

	for (int iFile = 0; iFile < nFiles; iFile++){
		char filename[256];
		sprintf(filename, "/home/petukhov/data/run%05d.mid", RunNumbers[iFile]);
		InputFile.Open(filename);
		printf("\nProcessing file run%05d.mid\n", RunNumbers[iFile]);

		bool tryOpen = InputFile.Open(filename);

		if (!tryOpen){
			printf("Cannot open file %s\n", filename);
			return -1;
		}

		TMidasEvent event;

		while(InputFile.Read(&event)){
			nevents++;

			char *pdata;
			int dummy;

			gQDCSize = event.LocateBank(&dummy, "QDC1", (void**)&pdata);
			gQDCData = (char*)pdata;
			gADCSize = event.LocateBank(&dummy, "ADC1", (void**)&pdata);
			gADCData = (char*)pdata;
			gTDCSize = event.LocateBank(&dummy, "TDC1", (void**)&pdata);
			gTDCData = (char*)pdata;

			ProcessQDC();
			ProcessADC();
			ProcessTDC();

			AnalyzeData();
		}
	}

	if (OutputFile){
		OutputFile->Write();
		OutputFile->Close();
		OutputFile = NULL;
    }

	printf("\n%d events in total\n\n", nevents);

	return 0;
}

void ProcessQDC(){

	UINT32 *pdata;

	if (gQDCData == NULL){
		return;
		gQDCisRead = false;
	}
	else gQDCisRead = true;
   
   pdata = (UINT32*)gQDCData;
   UINT32 data = 0;

   for (UINT32 i = 0; i < gQDCSize; i++){

	   	data = *pdata++;

   		if ((data & CVT_QTP_DATA_TYPE_MSK) != CVT_QTP_DATUM) continue;

		UINT32 ch = 0;
		UINT32 qdc = CVT_V792_GET_DATUM_ADC(data);

		ch = CVT_V792A_GET_DATUM_CH(data);
		if (ch >= 32){
		    printf("    QDC channel %d > 31\n", ch);
		    continue;
		}

		int det = qdc_mapping[ch];
		if (det >= 0 && det < N_CHAN){
		    hRateQDC->Fill(det);
		    QDC[det] = (double)qdc - PedQDC[det];
    		hQDCAmp[det]->Fill(QDC[det]);
		}
   }

}

void ProcessADC(){

	UINT32 *pdata;
	UINT32 data = 0;

	if (gADCData == NULL){
		return;
		gADCisRead = false;
	}
	else gADCisRead = true;
   
   pdata = (UINT32*)gADCData;

   for (UINT32 i = 0; i < gADCSize; i++){

   		data = *pdata++;

   		if ((data & CVT_QTP_DATA_TYPE_MSK) != CVT_QTP_DATUM) continue;

		UINT32 ch = 0;
		UINT32 adc = CVT_V785_GET_DATUM_ADC(data);

		ch = CVT_V785A_GET_DATUM_CH(data);
		if (ch >= 32){
		    printf("    ADC channel %d > 31\n", ch);
		    continue;
		}

		int det = adc_mapping[ch];
		if (det >= 0 && det < N_CHAN){
		    hRateADC->Fill(det);
		    ADC[det] = ((double)adc - PedADC[det])/ADCPeakMean[det];
    		hADCEn[det]->Fill(ADC[det]);
    		hADCRaw[det]->Fill((double)adc);
    		hADCRaw_tmp[det]->Fill((double)adc);
    		int Nev = hRateADC->GetBinContent(det);
    		if (!(Nev%2000) && Nev != 0){
    			Double_t Ped = hADCRaw_tmp[det]->GetMaximumBin();
    			hADCRaw_tmp[det]->Reset();
    			hPedEvolution[det]->SetBinContent((Int_t)(Nev/2000), Ped);
    		}
		}
   }

}

void ProcessTDC(){

	UINT32 *pdata;
	UINT32 data = 0;

	if (gTDCData == NULL){
		return;
		gTDCisRead = false;
	}
	else gTDCisRead = true;
   
	pdata = (UINT32*)gTDCData;

	for (int i = 0; i < 32; i++) TDC[i] = 0;
	UINT32 time0 = 0;


	for (UINT32 i = 0; i < gTDCSize; i++){

   		data = *pdata++;

   		if ((data & CVT_V1190_DATA_TYPE_MSK) != CVT_V1190_TDC_MEASURE) continue;

		UINT32 ch = 0;
		UINT32 tdc = CVT_V1290_GET_TDC_HDR_MEASURE(data);

		ch = CVT_V1290_GET_TDC_MSR_CHANNEL(data);
		if (ch >= 32){
		    printf("    TDC channel %d > 31\n", ch);
		    continue;
		}

		if (ch == TZERO)
			time0 = tdc;

		int det = tdc_mapping[ch];
		if (det >= 0 && det < N_CHAN){

			// cout << tdc << endl;

			if (tdc != 0){
				TDC[det] = tdc;
				hRateTDC->Fill(det);
			}

		}
	}

	for (int i = 0; i < N_CHAN; i++){
		if (TDC[i] == 0 || time0 == 0 || !gTDCisRead) continue;

		hTDC[i]->Fill((int)time0 - (int)TDC[i]);
	}

}

void AnalyzeData(){

	double ADCMaxAmp = 0.;
	int ADCMaxCh = -1;

	for (int i = 0; i < 12; i++){
		if (ADC[i] >= ADCMaxAmp && ADC[i]!=0){
			ADCMaxAmp = ADC[i];
			ADCMaxCh = i;
		}
	}

	hADCMax->Fill(ADCMaxCh); // this histogram should be filled by energy deposition in each channel, not by ADC bit (uncalibrated)
	hHitMap->Fill(ADCMaxCh%4, ADCMaxCh/4);

	int Lindex[9] = {0,1,2, 4,5,6, 8,9,10};
	int Rindex[9] = {1,2,3, 5,6,7, 9,10,11};

	double EtotL = 0., EtotR = 0., Etot = 0.;
	double EclustL = 0.;
	double x = 0, y = 0;

	// if (QDC[2] < 500) return; // signal in Cherenkov --> to cut off MIP

	for (int i = 0; i < 12; i++){
		hADCEn_MaxAmp5[i]->Fill(ADC[i]);
		if (ADC[i] >= 0)
			Etot += ADC[i];
	}

	hEtot->Fill(Etot);

	double LeakFactor = 0.8;
	if (strncmp(CalibMethod.c_str(), "Edge", 4) == 0) LeakFactor = 1.0;
	if (strncmp(CalibMethod.c_str(), "Mean", 4) == 0) LeakFactor = 0.8;

	x = 0; y = 0;

	double dR1[3] = {0.01, 0.001, 0.03}; // 0 -- default, 1 -- low, 2 -- high
	double dR2[3] = {0.03, 0.01, 0.05};
	double dR3[3] = {0.05, 0.03, 0.1};

	double dR = dR3[GCcutIndex];
	if (gBeamEnergy < 1) dR = dR1[GCcutIndex];
	else if (gBeamEnergy < 3) dR = dR2[GCcutIndex];
	else dR = dR3[GCcutIndex];

	if (ADCMaxCh == 5){
		for (int i = 0; i < 9; i++){

			if (ADC[Lindex[i]] >= 0){

				double Ei = LeakFactor*ADC[Lindex[i]]/RecEnergyCorrection(RunNumbers[0], gBeamEnergy);

				bool AcceptCell = false;
				if (Lindex[i] == ADCMaxCh) AcceptCell = true;
				else AcceptCell = (ADC[Lindex[i]] > 0.08 && ADC[Lindex[i]] < 0.2*ADC[ADCMaxCh]); // 80 MeV noise

				if (Ei > 0){
					EtotL += Ei;
					x += Ei*(i%3 - 1);
					y += Ei*(i/3 - 1);
				}

				// Cluster energy calculation
				double Ecell = 0;
				if (AcceptCell){
					Ecell = ADC[Lindex[i]];
					EclustL += Ecell;
				}

			}
		}
	}


	if (EclustL > 0)
		hEClustL->Fill(EclustL);

	if (EtotL > 0){
		hEtotL->Fill(EtotL);
		if (EtotL > 0 /*0.5*gBeamEnergy*/){
			x /= EtotL; y /= EtotL;
			if (sqrt(x*x+y*y) < 0.15)
				hECentL->Fill(EtotL);

			if (x != 0 && y != 0){
				hSCurveX->Fill(x);
				hSCurveY->Fill(y);
				hXY->Fill(x,y);
			}
		}
	}

	if (ADCMaxCh == 6){

		x = 0; y = 0;

		for (int i = 0; i < 9; i++){

			if (ADC[Rindex[i]] >= 0){

				double Ei = LeakFactor*ADC[Rindex[i]]/RecEnergyCorrection(RunNumbers[0], gBeamEnergy);
				if (Ei > 0){
					EtotR += Ei;
					x += Ei*(i%3 - 1);
					y += Ei*(i/3 - 1);
				}
			}
		}

		if (EtotR != 0){
			hEtotR->Fill(EtotR);
			if (EtotR > 0 /*0.5*gBeamEnergy*/){
				x /= EtotR; y /= EtotR;
				if (sqrt(x*x+y*y) < 0.15)
					hECentR->Fill(EtotR);
			}
		}
	}

}

void InitHistograms(){

	for (int i = 0; i < N_CHAN; i++){
		qdc_mapping[qdc_chans[i]] = i;
		adc_mapping[adc_chans[i]] = i;
		tdc_mapping[tdc_chans[i]] = i;

		ADCPeakMean[i] = 0.;
		ADCPeakWidth[i] = 0.;
	}

	if (!gIsCalibrationRun){

		TFile *CalibFile = new TFile("Calibration.root", "read");
		TH1F *hADCPed = (TH1F*)CalibFile->Get(Form("ADCPed_%s_%s", CalibEnergy.c_str(), CalibMethod.c_str()));
		TH1F *hADCPeak = (TH1F*)CalibFile->Get(Form("ADCPeak_%s_%s", CalibEnergy.c_str(), CalibMethod.c_str()));

		for (int i = 0; i < N_ADC; i++){
			PedADC[i] = hADCPed->GetBinContent(i+1);

			ADCPeakMean[i] = hADCPeak->GetBinContent(i+1);
			ADCPeakWidth[i] = hADCPeak->GetBinError(i+1);

		}

		// CalibFactorADC = hDepEnCorr->GetBinContent(gBeamEnergy+1);

		// delete CalibFile;
	}

	char filename[256];
	sprintf(filename, "/home/petukhov/PHOSBeamTests/data/output%05d_%s_%s_%s.root", RunNumbers[0], CalibEnergy.c_str(), CalibMethod.c_str(), GravityCenterSet.c_str());
	OutputFile = new TFile(filename, "recreate");

	char name[256];

	hEtot = new TH1F("Etot", "Total energy in all setup;E, GeV;Counts", 300, 0, 1.5*gBeamEnergy);

	hEtotL = new TH1F("EtotL", "Total energy in left cluster;E, GeV;Counts",  300, 0, 1.5*gBeamEnergy);
	hECentL = new TH1F("ECentL", "Total energy in left cluster;E, GeV;Counts",  300, 0, 1.5*gBeamEnergy);
	hEClustL = new TH1F("EClustL", "Total energy in left cluster;E, GeV;Counts",  300, 0, 1.5*gBeamEnergy);

	hEtotR = new TH1F("EtotR", "Total energy in right cluster;E, GeV;Counts",  300, 0, 1.5*gBeamEnergy);
	hECentR = new TH1F("ECentR", "Total energy in right cluster;E, GeV;Counts",  300, 0, 1.5*gBeamEnergy);

	hSCurveX = new TH1F("SCurveX", ";X_{REC};Counts", 1000,-1,+1);
	hSCurveY = new TH1F("SCurveY", ";Y_{REC};Counts", 1000,-1,+1);
	hXY = new TH2F("XY", ";X_{REC};Y_{REC}", 100,-1,+1, 1000,-1,+1);

	hRateQDC = new TH1F("RateQDC", "QDC rate per channel;QDC ch.;Counts", N_CHAN+1, 0, N_CHAN+1);
	hRateADC = new TH1F("RateADC", "ADC rate per channel;ADC ch.;Counts", N_CHAN+1, 0, N_CHAN+1);
	hRateTDC = new TH1F("RateTDC", "TDC rate per channel;TDC ch.;Counts", N_CHAN+1, 0, N_CHAN+1);

	for (int i = 0; i < N_CHAN; i++)
		hPedEvolution[i] = new TH1F(Form("PedEvolution_%02d", i+1), "Pedestals per 2k events;each point is 2k events;Ped, ADC bit", 300,0,300);

	for (int i = 0; i < N_CHAN; i++){
		sprintf(name, "QDC%02d", i+1);
		hQDCAmp[i] = new TH1F(name, Form("QDC%02d;QDC bit;Counts", qdc_chans[i]), ADC1_N_BINS+10, ADC_X_LOW-10, ADC1_X_HIGH);
	}

	for (int i = 0; i < N_CHAN; i++)
		hADCRaw[i] = new TH1F(Form("ADC%02d_Raw", i+1), Form("ADC%02d;ADC bit;Counts", i+1), ADC1_N_BINS+10, ADC_X_LOW-10, ADC1_X_HIGH);
	for (int i = 0; i < N_CHAN; i++)
		hADCRaw_tmp[i] = new TH1F(Form("ADC%02d_tmp", i+1), Form("ADC%02d;ADC bit;Counts", i+1), ADC1_N_BINS+10, ADC_X_LOW-10, ADC1_X_HIGH);

	for (int i = 0; i < N_CHAN; i++)
		hADCEn[i] = new TH1F(Form("ADC%02d_En", i+1), Form("ADC%02d;E, GeV;Counts", i+1), 100*(gBeamEnergy+1), 0, gBeamEnergy+1);
	for (int i = 0; i < N_CHAN; i++)
		hADCEn_MaxAmp5[i] = new TH1F(Form("ADC%02d_En_MaxAmp5", i+1), Form("ADC%02d;E, GeV;Counts", i+1), ADC1_N_BINS+10, ADC_X_LOW-10, ADC1_X_HIGH);

	hADCMax = new TH1F("ADCMax", "Maximum ADC amplitude event by event;ADC ch.;Counts", 12, 0, 12);
	hHitMap = new TH2F("HitMap", "Hit map by ADC signals;X, cell;Y, cell", 4,0,4, 3,0,3);

	for (int i = 0; i < N_CHAN; i++)
		hTDC[i] = new TH1F(Form("TDC%02d", i+1), Form("TDC%02d;TDC bit;Counts", i+1), TDC_N_BINS, TDC_X_LOW, TDC_X_HIGH);


}

double RecEnergyCorrection(int runnumber = 1070, double En = 1){
	double p0 = 0.111463, p1 = 1923.96;
	// if (runnumber < 1100)
	// 	p0 = 0.111463, p1 = 1923.96;

	double EnCorrFactor = 1;
	if (!gIsCalibrationRun)
		EnCorrFactor = p0*TMath::Log(p1*En);

	return EnCorrFactor;
}