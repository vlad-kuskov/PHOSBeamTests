#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>

#include "TMidasFile.h"
#include "TMidasEvent.h"

#include "VMEModulesReader.h"
#include "FileReader.C"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>

int gBeamEnergy = 0;

std::vector<int> RunNumbers;

UINT32 gQDCSize = 0;
char *gQDCData = 0;
bool gQDCisRead = false;
UINT32 gADCSize = 0;
char *gADCData = 0;
bool gADCisRead = false;
UINT32 gTDCSize = 0;
char *gTDCData = 0;
bool gTDCisRead = false;

const int N_CHAN = 32;
int QDCbit[N_CHAN] = {0}, ADCbit[N_CHAN] = {0}, TDCbit[N_CHAN] = {0};

TFile *gOutputFile;
TTree *gOutputTTree;

void ProcessQDC();
void ProcessADC();
void ProcessTDC();

void BuildTTree(int nevents);

int main(int argc, char *argv[]){

	for (int i = 1; i < argc; i++){

		const char arg = argv[i][0];
		if (arg == '-') continue;

		std::vector<std::string> RunRecords = readInfoFile("RunList.txt", static_cast<std::string>(argv[i]));

		if (strncmp(argv[i], RunRecords[0].c_str(), 2) != 0){
			printf("\nERROR: run %s has not been foud in run list!!!!\n\n", argv[i]);
			// printf("WARNING: beam energy set as 0 GeV\n");
			return -1;
		}

		if (!gBeamEnergy){
			gBeamEnergy = atoi(RunRecords[1].c_str());
			printf("\nBeam energy is set %d GeV\n", gBeamEnergy);
		}

		int energy = atoi(RunRecords[1].c_str());
		if (energy != gBeamEnergy){
			printf("\nWARNING: beam energy in run %s is not the same as in the first run!!!\n", argv[i]);
			printf("WARNING: beam energ isy set %d GeV as in the first run\n", gBeamEnergy);
		}

		int RunNumber = atoi(argv[i]);
		RunNumbers.push_back(RunNumber);
	}

	const int nFiles = RunNumbers.size();
	TMidasFile InputFile;

	int nevents = 0;

	char filename[256];
	sprintf(filename, "/home/petukhov/PHOSBeamTests/data/output%05d.root", RunNumbers[0]);
	gOutputFile = new TFile(filename, "recreate");

	for (int iFile = 0; iFile < nFiles; iFile++){
		// char filename[256];
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

			BuildTTree(nevents);

			if(!(nevents%500))
				printf("%d events have been proccesed\n", nevents);
		}
	}

	if (gOutputFile){
		gOutputFile->Write();
		gOutputFile->Close();
		gOutputFile = NULL;
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

		if (ch >= 0 && ch < N_CHAN)
		    QDCbit[ch] = (int)qdc;
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

		if (ch >= 0 && ch < N_CHAN)
		    ADCbit[ch] = (int)adc;
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

	for (int i = 0; i < 32; i++) TDCbit[i] = 0;

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

		if (ch >= 0 && ch < N_CHAN)
			TDCbit[ch] = (int)tdc;

	}

}

void BuildTTree(int nevents = -1){

	if (nevents < 0)
		return;

	int nchannels = 32;

	gOutputTTree = new TTree("vme_readout","VME readout");

	gOutputTTree->Branch("events", &nevents, "nevents/I");
	gOutputTTree->Branch("nchannels", &nchannels, "nchannels/I");
    gOutputTTree->Branch("ADC", ADCbit, "ADCbit[nchannels]/I");
    gOutputTTree->Branch("TDC", TDCbit, "TDCbit[nchannels]/I");
    gOutputTTree->Branch("QDC", QDCbit, "QDCbit[nchannels]/I");

    gOutputTTree->Fill();
    // gOutputTTree->Write();


}