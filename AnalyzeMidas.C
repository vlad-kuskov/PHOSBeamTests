/********************************************************************\

  Name:         AnalyzeMidas_offline.C
  Created by:   Yuri Petukhov
  Supported by: Vladislav Kuskov

  Contents:     Analyzer module for VME ADC, QDC, TDC.

  $Id:$

\********************************************************************/

/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <time.h>

/* root includes */
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TControlBar.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TFolder.h>
#include <TObjArray.h>

extern TFolder* gManaHistosFolder;
//#include "midasServer.h"

// user definitions

/* number of channels */
#define N_ADC               24
#define N_QDC               11
#define N_TDC               23

#define ADC_N_BINS  5000
#define ADC_X_LOW      0
#define ADC_X_HIGH  5000

#define ADC1_N_BINS  5000
#define ADC1_X_HIGH  5000

#define TDC_N_BINS  10000
#define TDC_X_LOW       0
#define TDC_X_HIGH  10000

#define TZERO 16

#define USE_COUNTERS_T0 0
#define COUNTERS_BEFORE 1

////////////////////////////////////////////////////////////////////////////////////////////////
// Output buffer macros
////////////////////////////////////////////////////////////////////////////////////////////////
#define UINT32 unsigned int
#define CVT_V792_GET_DATUM_GEO(data)		((UINT32)((((UINT32)data)& CVT_V792_DATUM_GEO_MSK)>>27))		/*!< \brief Gets the DATUM GEO address for V792*/
#define CVT_V792_GET_DATUM_UN(data)		((UINT32)((((UINT32)data)& CVT_V792_DATUM_UN_MSK)>>13))			/*!< \brief Gets the DATUM Under threshold for V792*/
#define CVT_V792_GET_DATUM_OV(data)		((UINT32)((((UINT32)data)& CVT_V792_DATUM_OV_MSK)>>12))			/*!< \brief Gets the DATUM Overflow for V792*/
#define CVT_V792_GET_DATUM_ADC(data)		((UINT32)(((UINT32)data)& CVT_V792_DATUM_ADC_MSK))				/*!< \brief Gets the DATUM ADC for V792*/
#define CVT_V792N_GET_DATUM_CH(data)		((UINT32)((((UINT32)data)& CVT_V792_DATUM_CH_MSK)>>17))			/*!< \brief Gets the DATUM channel for V792N */
#define CVT_V792A_GET_DATUM_CH(data)		((UINT32)((((UINT32)data)& CVT_V792_DATUM_CH_MSK)>>16))			/*!< \brief Gets the DATUM channel for V792A */
#define CVT_QTP_DATA_TYPE_MSK			0x07000000		/*!< \brief Data type bit masks */

#define CVT_V785_DATUM_GEO_MSK				0xf8000000		/*!< \brief The DATUM GEO address msk for V785 */
#define CVT_V785_DATUM_CH_MSK				0x001f0000		/*!< \brief The DATUM channel msk  for V785 ( bit 0x00010000 is always zero for V785N ) */
#define CVT_V785_DATUM_UN_MSK				0x00002000		/*!< \brief The DATUM Under threshold msk  for V785 */
#define CVT_V785_DATUM_OV_MSK				0x00001000		/*!< \brief The DATUM Overflow msk  for V785 */
#define CVT_V785_DATUM_ADC_MSK				0x00000fff		/*!< \brief The DATUM ADC msk  for V785 */
#define CVT_V785_GET_DATUM_ADC(data)			((UINT32)(((UINT32)data)& CVT_V785_DATUM_ADC_MSK))				/*!< \brief Gets the DATUM ADC for V785*/
#define CVT_V785N_GET_DATUM_CH(data)			((UINT32)((((UINT32)data)& CVT_V785_DATUM_CH_MSK)>>17))			/*!< \brief Gets the DATUM channel for V785N */
#define CVT_V785A_GET_DATUM_CH(data)			((UINT32)((((UINT32)data)& CVT_V785_DATUM_CH_MSK)>>16))			/*!< \brief Gets the DATUM channel for V785A */

#define CVT_QTP_HDR_CH_COUNT_MSK			0x00003f00		/*!< \brief The header channel count msk */
#define CVT_QTP_GET_HDR_CH_COUNT(data)			((UINT32)((((UINT32)data)& CVT_QTP_HDR_CH_COUNT_MSK)>>8))		/*!< \brief Gets the header channel count */
#define CVT_QTP_EOB_EVENT_COUNT_MSK			0x00ffffff		/*!< \brief The EOB Event count msk */
#define CVT_QTP_GET_EOB_EVENT_COUNT(data)		((UINT32)(((UINT32)data)& CVT_QTP_EOB_EVENT_COUNT_MSK))			/*!< \brief Gets the EOB Event count */

#define CVT_QTP_HEADER				0x02000000		/*!< \brief Header data type */
#define CVT_QTP_EOB				0x04000000		/*!< \brief End of block data type */
#define CVT_QTP_DATUM				0x00000000		/*!< \brief Datum data type */
#define CVT_QTP_NOT_VALID_DATUM			0x06000000		/*!< \brief Not valid datum data type */

#define CVT_QTP_HDR_GEO_MSK			0xf8000000		/*!< \brief The header GEO address msk */
#define CVT_QTP_HDR_CRATE_MSK			0x00ff0000		/*!< \brief The header crate msk */
#define CVT_QTP_HDR_CH_COUNT_MSK		0x00003f00		/*!< \brief The header channel count msk */

#define CVT_QTP_EOB_GEO_MSK			0xf8000000		/*!< \brief The EOB GEO address msk */
#define CVT_QTP_EOB_MAX_EVENT_COUNT		(0x00ffffff+ 1)	/*!< \brief The EOB Max Event count */
#define CVT_QTP_EOB_EVENT_COUNT_MSK		0x00ffffff		/*!< \brief The EOB Event count msk */

//
// V792 specific
#define CVT_V792_DATUM_GEO_MSK			0xf8000000		/*!< \brief The DATUM GEO address msk for V792 */
#define CVT_V792_DATUM_CH_MSK			0x001f0000		/*!< \brief The DATUM channel msk  for V792 ( bit 0x00010000 is always zero for V792N ) */
#define CVT_V792_DATUM_UN_MSK			0x00002000		/*!< \brief The DATUM Under threshold msk  for V792 */
#define CVT_V792_DATUM_OV_MSK			0x00001000		/*!< \brief The DATUM Overflow msk  for V792 */
#define CVT_V792_DATUM_ADC_MSK			0x00000fff		/*!< \brief The DATUM ADC msk  for V792 */
//
#define CVT_V1190_DATA_TYPE_MSK				0xf8000000		/*!< \brief Data type bit masks */

#define CVT_V1190_GLOBAL_HEADER				0x40000000		/*!< \brief Global header data type */
#define CVT_V1190_GLOBAL_TRAILER			0x80000000		/*!< \brief Global trailer data type */
#define CVT_V1190_TDC_HEADER				0x08000000		/*!< \brief TDC header data type */
#define CVT_V1190_TDC_MEASURE				0x00000000		/*!< \brief TDC measure data type */
#define CVT_V1190_TDC_ERROR					0x20000000		/*!< \brief TDC error data type */
#define CVT_V1190_TDC_TRAILER				0x18000000		/*!< \brief TDC trailer data type */
#define CVT_V1190_GLOBAL_TRIGGER_TIME		0x88000000		/*!< \brief Global trigger tag time data type */
#define CVT_V1190_FILLER					0xc0000000		/*!< \brief Filler data type */

#define CVT_V1190_GLB_HDR_MAX_EVENT_COUNT	(0x003fffff+ 1)	/*!< \brief Max Event count */
#define CVT_V1190_GLB_HDR_EVENT_COUNT_MSK	0x07ffffe0		/*!< \brief Global header event count msk */
#define CVT_V1190_GLB_HDR_GEO_MSK			0x0000001f		/*!< \brief Global header GEO msk */
#define CVT_V1190_GLB_TRG_TIME_TAG_MSK		0x07ffffff		/*!< \brief Global EXTENDED TRIGGER TIME TAG msk */
#define CVT_V1190_GLB_TRL_STATUS_MSK		0x07000000		/*!< \brief Global trailer STATUS msk */
#define CVT_V1190_GLB_TRL_WCOUNT_MSK		0x001fffe0		/*!< \brief Global trailer Word count  msk */
#define CVT_V1190_GLB_TRL_GEO_MSK			0x0000001f		/*!< \brief Global trailer GEO msk */

#define CVT_V1190_TDC_HDR_TDC_MSK			0x03000000		/*!< \brief TDC event header TDC msk */
#define CVT_V1190_TDC_HDR_EVENT_ID_MSK		0x00fff000		/*!< \brief TDC event header EVENT ID msk */
#define CVT_V1190_TDC_HDR_BUNCH_ID_MSK		0x00000fff		/*!< \brief TDC event header BUNCH ID msk */
#define CVT_V1190_TDC_MSR_TRAILING_MSK		0x04000000		/*!< \brief TDC measurement TRAILING msk */
#define CVT_V1190_TDC_MSR_CHANNEL_MSK		0x03f80000		/*!< \brief TDC measurement CHANNEL msk (for V1190) */
#define CVT_V1190_TDC_MSR_MEASURE_MSK		0x0007ffff		/*!< \brief TDC measurement MEASURE msk (for V1190) */
#define CVT_V1290_TDC_MSR_CHANNEL_MSK		0x03e00000		/*!< \brief TDC measurement CHANNEL msk (for V1290) */
#define CVT_V1290_TDC_MSR_MEASURE_MSK		0x001fffff		/*!< \brief TDC measurement MEASURE msk (for V1290) */
#define CVT_V1190_TDC_TRL_TDC_MSK			0x03000000		/*!< \brief TDC event trailer TDC msk */
#define CVT_V1190_TDC_TRL_EVENT_ID_MSK		0x00fff000		/*!< \brief TDC event trailer TDC msk */
#define CVT_V1190_TDC_TRL_WCOUNT_MSK		0x00000fff		/*!< \brief TDC event trailer WORD COUNT msk */
#define CVT_V1190_TDC_ERR_TDC_MSK			0x03000000		/*!< \brief TDC error TDC msk */
#define CVT_V1190_TDC_ERR_ERR_FLAGS_MSK		0x00003fff		/*!< \brief TDC error ERROR FLAGS msk */

#define CVT_V1190_IS_GLOBAL_HEADER(data)		((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_GLOBAL_HEADER)			/*!< \brief Checks if data type is global header */
#define CVT_V1190_IS_GLOBAL_TRAILER(data)		((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_GLOBAL_TRAILER)		/*!< \brief Checks if data type is global trailer */
#define CVT_V1190_IS_TDC_HEADER(data)			((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_TDC_HEADER)			/*!< \brief Checks if data type is TDC header */
#define CVT_V1190_IS_TDC_MEASURE(data)			((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_TDC_MEASURE)			/*!< \brief Checks if data type is TDC measure */
#define CVT_V1190_IS_TDC_ERROR(data)			((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_TDC_ERROR)				/*!< \brief Checks if data type is TDC error */
#define CVT_V1190_IS_TDC_TRAILER(data)			((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_TDC_TRAILER)			/*!< \brief Checks if data type is TDC trailer */
#define CVT_V1190_IS_GLOBAL_TRIGGER_TIME(data)	((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_GLOBAL_TRIGGER_TIME)	/*!< \brief Checks if data type is global trigger time */
#define CVT_V1190_IS_FILLER(data)				((data& CVT_V1190_DATA_TYPE_MSK)== CVT_V1190_FILLER)				/*!< \brief Checks if data type is filler */

#define CVT_V1190_GET_GLB_HDR_EVENT_COUNT(data)	((UINT32)((((UINT32)data)& CVT_V1190_GLB_HDR_EVENT_COUNT_MSK)>>5))		/*!< \brief Gets the global header event counter of this event */
#define CVT_V1190_GET_GLB_HDR_GEO(data)			((UINT32)(((UINT32)data)& CVT_V1190_GLB_HDR_GEO_MSK))					/*!< \brief Gets the global header GEO address of this event */

#define CVT_V1190_GET_GLB_TRG_TIME_TAG(data)	((UINT32)(((UINT32)data)& CVT_V1190_GLB_TRG_TIME_TAG_MSK))				/*!< \brief Gets the global trigger time tag */

#define CVT_V1190_GET_GLB_TRL_STATUS(data)		((UINT32)((((UINT32)data)& CVT_V1190_GLB_TRL_STATUS_MSK)>>24))			/*!< \brief Gets the global trailer status */
#define CVT_V1190_GET_GLB_TRL_WCOUNT(data)		((UINT32)((((UINT32)data)& CVT_V1190_GLB_TRL_WCOUNT_MSK)>>5))			/*!< \brief Gets the global trailer word counter counter */
#define CVT_V1190_GET_GLB_TRL_GEO(data)			((UINT32)(((UINT32)data)& CVT_V1190_GLB_TRL_GEO_MSK))					/*!< \brief Gets the global trailer GEO address */

#define CVT_V1190_GET_TDC_HDR_TDC(data)			((UINT32)((((UINT32)data)& CVT_V1190_TDC_HDR_TDC_MSK)>>24))				/*!< \brief Gets the TDC header TDC number */
#define CVT_V1190_GET_TDC_HDR_EVENT_ID(data)	((UINT32)((((UINT32)data)& CVT_V1190_TDC_HDR_EVENT_ID_MSK)>>12))		/*!< \brief Gets the TDC header event id */
#define CVT_V1190_GET_TDC_HDR_BUNCH_ID(data)	((UINT32)(((UINT32)data)& CVT_V1190_TDC_HDR_BUNCH_ID_MSK))				/*!< \brief Gets the TDC header bunch id */

#define CVT_V1190_GET_TDC_MSR_TRAILING(data)	((UINT32)((((UINT32)data)& CVT_V1190_TDC_MSR_TRAILING_MSK)>>26))		/*!< \brief Gets the TDC measurement TRAILING bit */
#define CVT_V1190_GET_TDC_MSR_CHANNEL(data)		((UINT32)((((UINT32)data)& CVT_V1190_TDC_MSR_CHANNEL_MSK)>>19))			/*!< \brief Gets the TDC measurement CHANNEL number (for V1190) */
#define CVT_V1190_GET_TDC_HDR_MEASURE(data)		((UINT32)(((UINT32)data)& CVT_V1190_TDC_MSR_MEASURE_MSK))				/*!< \brief Gets the TDC measurement measure value (for V1190) */
#define CVT_V1290_GET_TDC_MSR_CHANNEL(data)		((UINT32)((((UINT32)data)& CVT_V1290_TDC_MSR_CHANNEL_MSK)>>21))			/*!< \brief Gets the TDC measurement CHANNEL number (for V1290) */
#define CVT_V1290_GET_TDC_HDR_MEASURE(data)		((UINT32)(((UINT32)data)& CVT_V1290_TDC_MSR_MEASURE_MSK))				/*!< \brief Gets the TDC measurement measure value (for V1290) */

#define CVT_V1190_GET_TDC_TRL_TDC(data)			((UINT32)((((UINT32)data)& CVT_V1190_TDC_TRL_TDC_MSK)>>24))				/*!< \brief Gets the TDC trailer TDC */
#define CVT_V1190_GET_TDC_TRL_EVENT_ID(data)	((UINT32)((((UINT32)data)& CVT_V1190_TDC_TRL_EVENT_ID_MSK)>>12))		/*!< \brief Gets the TDC trailer event id */
#define CVT_V1190_GET_TDC_TRL_WCOUNT(data)		((UINT32)(((UINT32)data)& CVT_V1190_TDC_TRL_WCOUNT_MSK))				/*!< \brief Gets the TDC trailer word count */

#define CVT_V1190_GET_TDC_ERR_TDC(data)			((UINT32)((((UINT32)data)& CVT_V1190_TDC_ERR_TDC_MSK)>>24))				/*!< \brief Gets the TDC error TDC */
#define CVT_V1190_GET_TDC_ERR_ERR_FLAGS(data)	((UINT32)(((UINT32)data)& CVT_V1190_TDC_ERR_ERR_FLAGS_MSK))				/*!< \brief Gets the TDC error error flags */


/*-- Parameters ----------------------------------------------------*/

extern int  gRunNumber;
extern bool gIsRunning;
extern bool gIsPedestalsRun;
extern bool gIsOffline;
extern int  gEventCutoff;
extern TFile  *gOutputFile;
extern unsigned int gQDCSize;
extern char *gQDCData;
extern unsigned int gADCSize;
extern char *gADCData;
extern unsigned int gTDCSize;
extern char *gTDCData;

TH1D *hAdcHists[N_QDC];
TH1D *hLength = NULL;
TH1D *hRate;
TH2D *hAmpChan;
TH1D *hNhits;
TH1D *hSum;
//static TH1D *hChans;
//static TH2D *hMult;

TH1D *hAdcHists1[N_ADC];
TH1D *hLength1;
TH1D *hRate1;
TH2D *hAmpChan1;
TH1D *hNhits1;
TH1D *hSum1;

TH1D *hAdcHists2[N_TDC];
TH1D *hLength2;
TH1D *hRate2;
TH2D *hAmpChan2;
TH1D *hNhits2;
TH2D *hMult2;

TH2D *hADC_XY;
TH2D *hCounters_XY;

TH1D *hCounterTimeDiff[4];

TH2D *hAdcTdc[N_TDC];

TObjArray *gHistoFolderStack = NULL;

double PedMean[32] = {0.}, PedSigma[32] = {0.};
double PedMeanq[32] = {0.}, PedSigmaq[32] = {0.};

int ped_read785();
int ped_read792();
void BookHists();
void HistResetScript();

bool PedestalsRun = kTRUE;
unsigned ecount = 0;

int adc_chans[N_ADC] = {0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27};
int tdc_chans[N_TDC] = {0,1,2,3,4,5,6,7,8,9,10,11,
                        16,17,18,19,20,21,22,23,24,25,26};
int qdc_chans[N_QDC] = {16,17,18,19,20,21,22,23,24,25,26};

const char* adc_title[N_ADC] = {"HG (1,1)","HG (1,2)","HG (1,3)","HG (1,4)","HG (2,1)","HG (2,2)","HG (2,3)","HG (2,4)","HG (3,1)","HG (3,2)","HG (3,3)","HG (3,4)",  
                                "LG (1,1)","LG (1,2)","LG (1,3)","LG (1,4)","LG (2,1)","LG (2,2)","LG (2,3)","LG (2,4)","LG (3,1)","LG (3,2)","LG (3,3)","LG (3,4)"};
const char* tdc_title[N_TDC] = {"HG (1,1)","HG (1,2)","HG (1,3)","HG (1,4)","HG (2,1)","HG (2,2)","HG (2,3)","HG (2,4)","HG (3,1)","HG (3,2)","HG (3,3)","HG (3,4)",
                                "Lopata TDC","Hodoscope","Cherenkov","Dw1","Up1","Dw2","Up2","Dw3","Up3","Dw4","Up4"};
const char* qdc_title[N_QDC] = {"Lopata QDC","Hodoscope","Cherenkov","Dw1","Up1","Dw2","Up2","Dw3","Up3","Dw4","Up4"};

int qdc_mapping[32] = {-1};
int adc_mapping[32] = {-1};
int tdc_mapping[32] = {-1};

void menu();

void ShowHistsScriptADCcontrol();
void ShowHistsScriptADCchannels();
void ShowHistsScriptADCchannels_10mu();
void ShowHistsScriptQDCchannels();
void ShowHistsScriptTDCchannels();
void ShowHistsScriptTDC_QDC();
void ShowHistsScriptTDC_ADC();
void ShowHistsXYhitsADC();
void ShowHistsScriptTDCcounters();

void open_subfolder(char *name)
{
//   if (1) return;

//   printf("gmana %p gHisto %p\n", gManaHistosFolder, gHistoFolderStack);
   TFolder *current = (TFolder *) gHistoFolderStack->Last();
//   printf("cur %p gmana %p gHisto %p\n", current, gManaHistosFolder, gHistoFolderStack);

   if (!current)
      current = gManaHistosFolder;
//   printf("cur0 %p gmana %p gHisto %p\n", current, gManaHistosFolder, gHistoFolderStack);

   // if the subfolder already exists, use it
   TFolder *subfolder = 0;

   TCollection *listOfSubFolders = current->GetListOfFolders();
   TIter iter(listOfSubFolders);
   while (TObject * obj = iter()) {
      if (strcmp(obj->GetName(), name) == 0 && obj->InheritsFrom("TFolder"))
         subfolder = (TFolder *) obj;
   }

   if (!subfolder) {
      subfolder = new TFolder(name, name);
//   printf("cur00 %p gmana %p gHisto %p\n", current, gManaHistosFolder, gHistoFolderStack);
      current->Add(subfolder);
//   printf("cur1 %p gmana %p gHisto %p\n", current, gManaHistosFolder, gHistoFolderStack);
   }

   gHistoFolderStack->Add(subfolder);
//   printf("cur2 %p gmana %p gHisto %p\n", current, gManaHistosFolder, gHistoFolderStack);
}

void close_subfolder()
{
//   if (1) return;

   if (gHistoFolderStack->Last())
      gHistoFolderStack->Remove(gHistoFolderStack->Last());
}


/*-- init routine --------------------------------------------------*/

void InitScript()
{
   int i;

   gStyle->SetOptStat(1111111);

   if (!gHistoFolderStack)
   {
   gHistoFolderStack = new TObjArray();
   gROOT->GetListOfBrowsables()->Add(gManaHistosFolder, "histos");
   }
   
   for (i=0; i<32; i++) qdc_mapping[i] = -1;
   for (i=0; i<32; i++) adc_mapping[i] = -1;
   for (i=0; i<32; i++) tdc_mapping[i] = -1;


// int adc_chans[N_ADC] = {0,1,2,3,4,5,6,7,8,9,10,11,  16,17,18,19,20,21,22,23,24,25,26,27};
// int tdc_chans[N_TDC] = {0,1,2,3,4,5,6,7,8,9,10,11,            16,17,18,19,20,21,22,23,24,25,26};
   for (i=0; i<N_QDC; i++)
	  qdc_mapping[qdc_chans[i]] = i;
   for (i=0; i<N_ADC; i++)
	  adc_mapping[adc_chans[i]] = i;  //adc_mapping[16] = 12
   for (i=0; i<N_TDC; i++)
	  tdc_mapping[tdc_chans[i]] = i;

   // int det = adc_mapping[ch]; det = adc_mapping[16] = 12

//   if (gIsRunning) BookHists();

   if (!gIsPedestalsRun)
   {
    if( ped_read785() == 1 )
    {
	    printf("Pedestals for V785 read error!");
	    return;
    }
    if( ped_read792() == 1 )
    {
	    printf("Pedestals for V792 read error!");
	    return;
    }
   }
   else
   {
    for (i=0; i<32; i++)
    {
	PedMean[i] = 0.;
	PedSigma[i] = 0.;
	PedMeanq[i] = 0.;
	PedSigmaq[i] = 0.;
    }
   }
//   BookHists();
   menu();
   return;
}

void BookHists()
{
   char name[256];
   int i;

   /* book CADC histos */

   char title[256];

   sprintf(name, "QDC1_Bank_Length");
   sprintf(title, "QDC1_Bank_Length");
   hLength = new TH1D(name, title, 100, 0, 100);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hLength);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hLength);
   sprintf(name, "QDC1_Hits_Number");
   sprintf(title, "QDC1_Hits_Number");
   hNhits = new TH1D(name, title, N_QDC+1, 0, N_QDC+1);
//   if (gManaHistosFolder)
//        gManaHistosFolder->Add(hNhits);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hNhits);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hNhits);
//   sprintf(name, "QDC1_Channels_Number");
//   sprintf(title, "QDC1_Channels_Number");
//   hChans = new TH1D(name, title, 50, 0, 50);
   sprintf(name, "QDC1_Channels_Rate");
   sprintf(title, "QDC1_Channels_Rate");
   hRate = new TH1D(name, title, N_QDC+1, 0, N_QDC+1);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hRate);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hRate);
   sprintf(name, "QDC1_Sum");
   sprintf(title, "QDC1_Sum");
   hSum = new TH1D(name, title, ADC_N_BINS+10, ADC_X_LOW*5-50, ADC_X_HIGH*5);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hSum);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hSum);
   sprintf(name, "QDC1_Amps_vs_Channels");
   sprintf(title, "QDC1_Amps_vs_Channels");
   hAmpChan = new TH2D(name, title, N_QDC+1, 0, N_QDC+1, ADC_N_BINS+10, ADC_X_LOW-10, ADC_X_HIGH);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAmpChan);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAmpChan);
//   sprintf(name, "QDC1_Mult_vs_Channels");
//   sprintf(title, "QDC1 Mult vs Channels");
//   hMult = new TH2D(name, title, 32, 0, 32, 10, 0, 10);

   sprintf(name, "ADC1_Bank_Length");
   sprintf(title, "ADC1_Bank_Length");
   hLength1 = new TH1D(name, title, 100, 0, 100);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hLength1);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hLength1);
   sprintf(name, "ADC1_Hits_Number");
   sprintf(title, "ADC1_Hits_Number");
   hNhits1 = new TH1D(name, title, N_ADC+1, 0, N_ADC+1);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hNhits1);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hNhits1);
   sprintf(name, "ADC1_Channels_Rate");
   sprintf(title, "ADC1_Channels_Rate");
   hRate1 = new TH1D(name, title, N_ADC+1, 0, N_ADC+1);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hRate1);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hRate1);
   sprintf(name, "ADC1_Sum");
   sprintf(title, "ADC1_Sum");
   hSum1 = new TH1D(name, title, ADC1_N_BINS+10, ADC_X_LOW*5-50, ADC1_X_HIGH*5);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hSum1);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hSum1);
   sprintf(name, "ADC1_Amps_vs_Channels");
   sprintf(title, "ADC1_Amps_vs_Channels");
   hAmpChan1 = new TH2D(name, title, N_ADC+1, 0, N_ADC+1, ADC1_N_BINS+10, ADC_X_LOW-10, ADC1_X_HIGH);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAmpChan1);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAmpChan1);

   sprintf(name, "TDC1_Bank_Length");
   sprintf(title, "TDC1_Bank_Length");
   hLength2 = new TH1D(name, title, 100, 0, 100);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hLength2);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hLength2);
   sprintf(name, "TDC1_Hits_Number");
   sprintf(title, "TDC1_Hits_Number");
   hNhits2 = new TH1D(name, title, 50, 0, 50);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hNhits2);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hNhits2);
   sprintf(name, "TDC1_Channels_Rate");
   sprintf(title, "TDC1_Channels_Rate");
   hRate2 = new TH1D(name, title, N_TDC+1, 0, N_TDC+1);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hRate2);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hRate2);
   sprintf(name, "TDC1_Time_vs_Channels");
   sprintf(title, "TDC1 Time vs Channels");
   hAmpChan2 = new TH2D(name, title, N_TDC+1, 0, N_TDC+1, TDC_N_BINS/10, TDC_X_LOW, TDC_X_HIGH);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAmpChan2);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAmpChan2);
   sprintf(name, "TDC1_Mult_vs_Channels");
   sprintf(title, "TDC1 Mult vs Channels");
   hMult2 = new TH2D(name, title, N_TDC+1, 0, N_TDC+1, 10, 0, 10);
   if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hMult2);
   else
        ((TFolder *) gHistoFolderStack->Last())->Add(hMult2);

   open_subfolder((char *)"QDC channels");
   for (i = 0; i < N_QDC; i++) {

      sprintf(name, "QDC%02d", qdc_chans[i]);
      // sprintf(title, "QDC %d", i+1);

      hAdcHists[i] = new TH1D(name, qdc_title[i], ADC_N_BINS+10, ADC_X_LOW-10, ADC_X_HIGH);
      if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAdcHists[i]);
      else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAdcHists[i]);
   }
   close_subfolder();
   open_subfolder((char *)"ADC channels");
   for (i = 0; i < N_ADC; i++) {

      sprintf(name, "ADC%02d", adc_chans[i]);
      // sprintf(title, "ADC %d", i+1);

      hAdcHists1[i] = new TH1D(name, adc_title[i], ADC1_N_BINS+10, ADC_X_LOW-10, ADC1_X_HIGH);
      hAdcHists1[i]->GetXaxis()->SetRangeUser(50,500);
      if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAdcHists1[i]);
      else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAdcHists1[i]);
   }
   close_subfolder();
   open_subfolder((char *)"TDC channels");
   for (i = 0; i < N_TDC; i++) {

      sprintf(name, "TDC%02d", tdc_chans[i]);
      // sprintf(title, "TDC %d", i+1);

      hAdcHists2[i] = new TH1D(name, tdc_title[i], TDC_N_BINS, TDC_X_LOW, TDC_X_HIGH);
      if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAdcHists2[i]);
      else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAdcHists2[i]);
   }
   close_subfolder();

   for (i = 0; i < 4; i++){

      sprintf(name, "CounterTimeDiff%02d", i+1);
      sprintf(title, "Counter Up%d-Dw%d", i+1, i+1);

      hCounterTimeDiff[i] = new TH1D(name, title, 2000, -1000, 1000);
      if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hCounterTimeDiff[i]);
      else
        ((TFolder *) gHistoFolderStack->Last())->Add(hCounterTimeDiff[i]);
   }

   open_subfolder((char *)"TDC-ADC correlation");
   for (i = 0; i < N_TDC; i++) {
      sprintf(name, "ADC_Times_vs_Amps_%d", i+1);
      // sprintf(title, "ADC Times vs Amps_%d", i+1);
      hAdcTdc[i] = new TH2D(name, tdc_title[i], ADC1_N_BINS/10+1, ADC_X_LOW-10, ADC1_X_HIGH, TDC_N_BINS/10, TDC_X_LOW, TDC_X_HIGH);
      if (!gHistoFolderStack->Last())
        gManaHistosFolder->Add(hAdcTdc[i]);
      else
        ((TFolder *) gHistoFolderStack->Last())->Add(hAdcTdc[i]);
   }
   close_subfolder();
   open_subfolder((char *)"TDC-QDC correlation");

   hADC_XY = new TH2D("ADC_XY", "SiPM map;X,cell;Y,cell", 4,0,4, 3,0,3);
   hADC_XY->SetStats(0);
   for (int j = 1; j < 4; j++) hADC_XY->GetYaxis()->SetBinLabel(j,Form("%d", 4-j));
   for (int j = 1; j < 5; j++) hADC_XY->GetXaxis()->SetBinLabel(j,Form("%d", j));

   if (!gHistoFolderStack->Last())
      gManaHistosFolder->Add(hADC_XY);
   else
      ((TFolder *) gHistoFolderStack->Last())->Add(hADC_XY);

   hCounters_XY = new TH2D("Counters_XY", "Counters map;Counter;", 4,0,4, 1,0,1);
   hCounters_XY->SetStats(0);
   for (int j = 1; j < 5; j++) hCounters_XY->GetXaxis()->SetBinLabel(j,Form("%d", j));

   if (!gHistoFolderStack->Last())
      gManaHistosFolder->Add(hCounters_XY);
   else
      ((TFolder *) gHistoFolderStack->Last())->Add(hCounters_XY);

   return;
}

/*-- BOR routine ---------------------------------------------------*/

void StartRunScript()
{
   gStyle->SetOptStat(1111111);

   if (!gHistoFolderStack)
   {
    gHistoFolderStack = new TObjArray();
    gROOT->GetListOfBrowsables()->Add(gManaHistosFolder, "histos");
   }

//   if (!hLength)
   BookHists();
//   else
//    HistResetScript();

   if (!gIsPedestalsRun)
   {
    if( ped_read785() == 1 )
    {
	    printf("begin_of_run","Pedestals for V785 read error!");
	    return;
    }
    if( ped_read792() == 1 )
    {
	    printf("begin_of_run","Pedestals for V792 read error!");
	    return;
    }
   }
   else
   {
    for (int i=0; i<32; i++)
    {
	PedMean[i] = 0.;
	PedSigma[i] = 0.;
	PedMeanq[i] = 0.;
	PedSigmaq[i] = 0.;
    }
   }

   ecount = 0;

   return;
}

/*-- eor routine ---------------------------------------------------*/

void EndRunScript()
{
   return;
}

//void padRefresh(TCanvas *pad);

/*-- event routine -------------------------------------------------*/

void ProcessEventScript()
{
   if (gOutputFile == NULL) return;
   if (!hLength) return;
   UINT32 evnum_qdc = 0, evnum_adc = 0, evnum_tdc = 0;
   int i, l, nob = 0, nob1 = 0, nob2 = 0;
   unsigned *pdata, *pdata1, *pdata2;
   static unsigned done_evnum = 0;
   static int diff_tdc_qdc = 0;
   static int diff_tdc_adc = 0;
   static int diff_qdc_adc = 0;
   static int diff_tdc_qdc_prev = 0;
   static int diff_tdc_adc_prev = 0;
   static int diff_qdc_adc_prev = 0;
   static UINT32  evnum_adc_prev = 0;
   static UINT32  evnum_qdc_prev = 0;
   static UINT32  evnum_tdc_prev = 0;
   /* look for ADC0 bank, return if not present */
   ecount++;
   if (gQDCData == NULL)
   {
    l = 0;
    nob = 1;
   }
   else
   {
    l = gQDCSize;
    nob = 0;
   }
//   printf("qp = %p qs = %d ap = %p as = %d tp = %p ts = %d\n",gQDCData, gQDCSize, gADCData, gADCSize, gTDCData, gTDCSize);
//   printf("gIsRun %d gRunNum %d gPedes %d hLen %p\n", (int)gIsRunning, gRunNumber, (int)gIsPedestalsRun, hLength);
   pdata = (unsigned *)gQDCData;

   if (!nob) hLength->Fill(l);

//   if (!l) return 1;

//   printf("QDC before loop\n");

   /* fill ADC histos if above threshold */
   unsigned multq[N_QDC];
   for (i = 0; i < N_QDC; i++) multq[i] = 0;
   double adcaq[N_QDC];
   for (i = 0; i < N_QDC; i++) adcaq[i] = 1000000.;
   unsigned data = 0, nhits = 0;
   double sum = 0.;
//   printf("**********New event*************\n");
   for (i = 0; i < l; i++)
   {
		data = *pdata++;
                switch ( data & CVT_QTP_DATA_TYPE_MSK)
                {
            	    case CVT_QTP_HEADER: 
		    {
	    	    // header
//			UINT32 geo= CVT_QTP_GET_HDR_GEO( data);
//			UINT32 crate= CVT_QTP_GET_HDR_CRATE( data);
//			UINT32 ch_count= CVT_QTP_GET_HDR_CH_COUNT( data);
//			hChans->Fill(ch_count);
//			printf("adc_calib", "    Header, count %d", ch_count);
		    }
		    break;
            	    case CVT_QTP_EOB: 
		    {
			// EOB
//			UINT32 geo= CVT_QTP_GET_EOB_GEO( data);
			evnum_qdc= CVT_QTP_GET_EOB_EVENT_COUNT( data);
//			printf("adc_calib", "    EOB, event counter %d", event_count);
	            }
		    break;
            	    case CVT_QTP_DATUM: 
		    {
			// DATUM
//			UINT32 geo= CVT_V792_GET_DATUM_GEO( data);
			UINT32 ch= 0;
			UINT32 adc = CVT_V792_GET_DATUM_ADC( data);
//			UINT32 un= CVT_V792_GET_DATUM_UN( data);
//			UINT32 ov= CVT_V792_GET_DATUM_OV( data);
			ch = CVT_V792A_GET_DATUM_CH( data);
			if (ch >= 32)
			{
			    printf("    QDC channel %d > 31\n", ch);
			    continue;
			}
			int det = qdc_mapping[ch];
//        		printf("adc_calib", "Data 0x%08x chan %u amp %u det %d", data, ch, adc, det);
			if (det >= 0 && det < N_QDC)
			{
//				if (adc) hRate->Fill(ch);
			    hRate->Fill(det);
//				printf("adc_calib","ch %d det %d",ch,det);
        		    hAdcHists[det]->Fill((double)adc - PedMeanq[ch]);
        		    hAmpChan->Fill(det, (double)adc - PedMeanq[ch]);
			    adcaq[det] = (double)adc - PedMeanq[ch];
        		    sum += (double)adc - PedMeanq[ch];
//        			printf("adc_calib", "Data 0x%08x chan %d amp %d", data, ch, adc);
			    multq[ch]++;
			    nhits++;
			}
		    }
		    break;
            	    case CVT_QTP_NOT_VALID_DATUM: 
//			printf("adc_calib", "    Not valid datum              ");
                	// NOT VALID DATUM
		    break;
		    default: 
                	// UNKNOWM data type !!?!
		    break;
                }
   }

   if (!nob) hNhits->Fill(nhits);
   if (!nob) hSum->Fill(sum);
//   printf("QDC done\n");
//   for (i=0; i<32; i++)
//   {
//    hMult->Fill(i,multq[i]);
//   }
   nhits = 0;

   sum = 0.;

   if (gADCData == NULL)
   {
    l = 0;
    nob1 = 1;
   }
   else
   {
    l = gADCSize;
    nob1 = 0;
   }
   pdata1 = (unsigned *)gADCData;

   if (!nob1) hLength1->Fill(l);

   double adca[N_ADC];
   for (i = 0; i < N_ADC; i++) adca[i] = 1000000.;


   /* fill ADC histos if above threshold */
   for (i = 0; i < l; i++)
   {
		data = *pdata1++;
                switch ( data & CVT_QTP_DATA_TYPE_MSK)
                {
            	    case CVT_QTP_HEADER: 
		    {
	    	    // header
//			UINT32 geo= CVT_QTP_GET_HDR_GEO( data);
//			UINT32 crate= CVT_QTP_GET_HDR_CRATE( data);
//			UINT32 ch_count= CVT_QTP_GET_HDR_CH_COUNT( data);
		    }
		    break;
            	    case CVT_QTP_EOB: 
		    {
			// EOB
//			UINT32 geo= CVT_QTP_GET_EOB_GEO( data);
//			UINT32 event_cout= CVT_QTP_GET_EOB_EVENT_COUNT( data);
			evnum_adc = CVT_QTP_GET_EOB_EVENT_COUNT( data);
	            }
		    break;
            	    case CVT_QTP_DATUM: 
		    {
			// DATUM
//			UINT32 geo= CVT_V785_GET_DATUM_GEO( data);
			UINT32 ch= 0;
			UINT32 adc= CVT_V785_GET_DATUM_ADC( data);
//			UINT32 un= CVT_V785_GET_DATUM_UN( data);
//			UINT32 ov= CVT_V785_GET_DATUM_OV( data);
			ch = CVT_V785A_GET_DATUM_CH( data);
			if (ch >= 32)
			{
			    printf("    ADC channel %d > 31\n", ch);
			    continue;
			}
			int det = adc_mapping[ch];
			if (det >= 0 && det < N_ADC)
			{
//			if (adc) hRate1->Fill(ch);
			hRate1->Fill(det);
			adca[det] = (double)adc - PedMean[ch];
        		hAdcHists1[det]->Fill((double)adc - PedMean[ch]);
        		hAmpChan1->Fill(det, (double)adc - PedMean[ch]);
        		sum += (double)adc - PedMean[ch];
			nhits++;
			}
		    }
		    break;
            	    case CVT_QTP_NOT_VALID_DATUM: 
                	// NOT VALID DATUM
		    break;
		    default: 
                	// UNKNOWM data type !!?!
		    break;
                }
   }

   if (!nob1) hNhits1->Fill(nhits);
   if (!nob1) hSum1->Fill(sum);

//   printf("ADC done\n");
   
   nhits = 0;

   if (gTDCData == NULL)
   {
    l = 0;
    nob2 = 1;
   }
   else
   {
    l = gTDCSize;
    nob2 = 0;
   }

   pdata2 = (unsigned *)gTDCData;

   if (!nob2) hLength2->Fill(l);

//   printf("TDC l = %d\n", l);

//   if (nob) return;

   unsigned time[N_TDC], mult[N_TDC], time0 = 0;
   // int tcounter[8] = {0};
   for (i = 0; i < N_TDC; i++) time[i] = 0;
   for (i = 0; i < N_TDC; i++) mult[i] = 0;
   /* fill ADC histos if above threshold */
//   if (l>20) printf(" ********* New event with l = %d\n", l);
   for (i = 0; i < l; i++)
   {
	data = *pdata2++;
//	if (l>20) printf(" Word %d, data is 0x%08x\n", i, data);
	switch( data & CVT_V1190_DATA_TYPE_MSK)
	{
	    case CVT_V1190_GLOBAL_HEADER:
		{
			// Global header
//			UINT32 event_count= CVT_V1190_GET_GLB_HDR_EVENT_COUNT( data);
			evnum_tdc = CVT_V1190_GET_GLB_HDR_EVENT_COUNT( data);
//			UINT32 geo= CVT_V1190_GET_GLB_HDR_GEO( data);
//			if (l>20) printf("    Global Header, event count %d\n", evnum_tdc);
		}
		break;
	    case CVT_V1190_GLOBAL_TRAILER:
		{
			// Global trailer
//			UINT32 status= CVT_V1190_GET_GLB_TRL_STATUS( data);
//			UINT32 wcount= CVT_V1190_GET_GLB_TRL_WCOUNT( data);
//			UINT32 geo= CVT_V1190_GET_GLB_TRL_GEO( data);
//			if (l>20) printf("    Global trailer, word count %d\n", wcount);
		}
		break;
	    case CVT_V1190_TDC_HEADER:
		{
			// TDC header
//			UINT32 tdc= CVT_V1190_GET_TDC_HDR_TDC( data);
//			UINT32 event_id= CVT_V1190_GET_TDC_HDR_EVENT_ID( data);
//			UINT32 bunch_id= CVT_V1190_GET_TDC_HDR_BUNCH_ID( data);
//			if (l>20) printf("    TDC Header, event id %d TDC %d\n", event_id, tdc);
		}
		break;
	    case CVT_V1190_TDC_MEASURE:
		{
			// TDC measure
//			UINT32 trailing= CVT_V1190_GET_TDC_MSR_TRAILING( data);
			UINT32 channel= CVT_V1190_GET_TDC_MSR_CHANNEL( data);
			UINT32 measure= CVT_V1190_GET_TDC_HDR_MEASURE( data);
			//fix July 2013
//			if (user_setting.m_V1190_type ==  CVT_V1190_TYPE_A || user_setting.m_V1190_type == CVT_V1190_TYPE_B) {
//				channel= CVT_V1190_GET_TDC_MSR_CHANNEL( data);
//				measure= CVT_V1190_GET_TDC_HDR_MEASURE( data);
//			}
//			else {
			channel= CVT_V1290_GET_TDC_MSR_CHANNEL( data);
         // printf("TDC channel %d\n", channel);
			if (channel >= 32)
			{
			    printf("    TDC channel %d > 15\n", channel);
			    continue;
			}
			int det = tdc_mapping[channel];
			if (det >= 0 && det < N_TDC)
			{
			    measure= CVT_V1290_GET_TDC_HDR_MEASURE( data);
			    if (time[det] != 0)
			    {
				if (measure < time[det]) time[det] = measure;
			    }
			    else
			    {
				time[det] = measure;
				hRate2->Fill(det);
				nhits++;
			    }
			    mult[det]++;
			}
			if (channel == TZERO)
			{
			    measure = CVT_V1290_GET_TDC_HDR_MEASURE( data);
			    time0 = measure;
			}

         // for (int ich = 19; ich < 27; ich++){
         //    if (channel == ich){
         //       measure = CVT_V1290_GET_TDC_HDR_MEASURE( data);
         //       tcounter[ich-19] = measure;
         //    }

         // }

//			if (l>20) printf("    Measurement, data %d, channel %d\n", measure, channel);
			
//			}
		}
		break;
	    case CVT_V1190_TDC_ERROR:
		{
//			UINT32 tdc= CVT_V1190_GET_TDC_ERR_TDC( data);
//			UINT32 err_flags= CVT_V1190_GET_TDC_ERR_ERR_FLAGS( data);
//			if (l>20) printf("    TDC Error, event id %d TDC %d\n", err_flags, tdc);
		}
		break;
	    case CVT_V1190_TDC_TRAILER:
		{
//			UINT32 tdc= CVT_V1190_GET_TDC_TRL_TDC( data);
//			UINT32 event_id= CVT_V1190_GET_TDC_TRL_EVENT_ID( data);
//			UINT32 wcount= CVT_V1190_GET_TDC_TRL_WCOUNT( data);
//			if (l>20) printf("    TDC Trailer, event id %d TDC %d Wcount %d\n", event_id, tdc, wcount);
		}
		break;
	    case CVT_V1190_GLOBAL_TRIGGER_TIME:
		{
//			UINT32 trg_time_tag= CVT_V1190_GET_GLB_TRG_TIME_TAG( data);
//			if (l>20) printf("    Trigger time, tag %d\n", trg_time_tag);
		}
		break;
	    case CVT_V1190_FILLER:
		{
//			if (l>20) printf("    Filler\n");
		}
		break;
	    default:
		{
		}
		break;
	}

   // printf("==================\n");

   }

   if (!nob2) hNhits2->Fill(nhits);

//   printf("TDC done\n");

   int sg = +1;
   unsigned time0tr = time0;
   // if (USE_COUNTERS_T0)
   // {
       if (COUNTERS_BEFORE) sg = -1;
       else		 sg = +1;
   // }

   for (i = 0; i < N_TDC; i++)
   {
       // int i = tdc_mapping[ich];
       // if (i < 0 || i > N_TDC) continue;

       if (time[i] == 0 || time0 == 0 || nob2) continue;

       if (!nob1) if (adca[i] < 1000000.) hADC_XY->Fill(i%4, i/4, 1);

       hMult2->Fill(i,mult[i]);
       hAdcHists2[i]->Fill((int)time0 - (int)time[i]);
       hAmpChan2->Fill(i, (int)time0 - (int)time[i]);

      if (i < 12) {
         if (!nob1) 
            if (adca[i] < 1000000) hAdcTdc[i]->Fill(adca[i], (int)time0 - (int)time[i]); // slew SiPM 0-11

      } else {
         if (!nob) 
            if (adcaq[i-12] < 1000000) hAdcTdc[i]->Fill(adcaq[i-12], (int)time0 - (int)time[i]); // slew counters
      }

   }

   for (int iCounter = 0; iCounter < 4; iCounter++){
      int ich = 16 + 2*iCounter;
      if (time[ich] != 0 && time[ich-1] != 0)
         hCounterTimeDiff[iCounter]->Fill((int)time[ich] - (int)time[ich-1]);
      if (adcaq[ich-12] > 350 && adcaq[ich-12] < 1000000)
         hCounters_XY->Fill(3-iCounter, 0);
   }

   if (!(ecount%1000))
   {
    size_t n = gROOT->GetListOfCanvases()->GetEntries();
//    if (ecount == 1000) printf("Ncanvases %d\n",n);
    for( size_t i = 0; i < n; i++ ) {
      TCanvas *can = dynamic_cast<TCanvas*>(gROOT->GetListOfCanvases()->At(i));
//      if (ecount == 1000) printf("%s\n",can->GetName());
      if      (!strcmp(can->GetName(), "cadc_control")) ShowHistsScriptADCcontrol();
      else if (!strcmp(can->GetName(), "cadc_channels_15mu")) ShowHistsScriptADCchannels();
      else if (!strcmp(can->GetName(), "cadc_channels_10mu")) ShowHistsScriptADCchannels_10mu();
      else if (!strcmp(can->GetName(), "cqdc_channels")) ShowHistsScriptQDCchannels();
      else if (!strcmp(can->GetName(), "ctdc_channels")) ShowHistsScriptTDCchannels();
      else if (!strcmp(can->GetName(), "ctdc_counters")) ShowHistsScriptTDCcounters();
      else if (!strcmp(can->GetName(), "ctdc_qdc")) ShowHistsScriptTDC_QDC();
      else if (!strcmp(can->GetName(), "ctdc_adc")) ShowHistsScriptTDC_ADC();
      else if (!strcmp(can->GetName(), "xy_hits")) ShowHistsXYhitsADC();
//      padRefresh(can);
      can->Modified();
      can->Update();
    }   
   }

   return;
}

void ShowHistsScriptADCcontrol(){

    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cqdc_control")));
    else
    {
   c = new TCanvas("cqdc_control","General control",900,900);
   c->Divide(3,2);
    }
//    printf("In ShowHistsScriptQDCcontrol!\n");
    c->cd();
    c->cd(1);
    hNhits->Draw();
    gPad->AddExec("exsel_cqdc_control","select_hist()");
//    gPad->AddExec("slice","DynamicExec()");
    c->cd(2);
    hRate->Draw();
    gPad->AddExec("exsel_cqdc_control","select_hist()");
    c->cd(4);
    hNhits1->Draw();
    gPad->AddExec("exsel_cqdc_control","select_hist()");
    c->cd(5);
    hRate1->Draw();
    gPad->AddExec("exsel_cqdc_control","select_hist()");
    c->cd(3);
    hAdcHists[0]->Draw();
    gPad->AddExec("exsel_cqdc_channels","select_hist()");
    c->cd(6);
    hAdcHists2[12]->Draw();
    gPad->AddExec("exsel_cqdc_channels","select_hist()");

}

int showQDC[11] = {9,10,11,5,1,6,2,7,3,8,4};

void ShowHistsScriptQDCchannels()
{
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cqdc_channels")));
    else
    {
        c = new TCanvas("cqdc_channels","QDC channels",900,900);
        c->Divide(4,2);
    }
    c->cd();
    for (int i=3;i<11;i++)
    {
	     c->cd(showQDC[i]);
        c->cd(showQDC[i])->SetLogy();
	     hAdcHists[i]->Draw();
	     gPad->AddExec("exsel_cqdc_channels","select_hist()");
    }
}

void ShowHistsScriptADCchannels()
{
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cadc_channels_15mu")));
    else
    {
        c = new TCanvas("cadc_channels_15mu","ADC channels",900,900);
        c->Divide(4,3);
    }
    c->cd();
    for (int i=0;i<12;i++)
    {
      c->cd(i+1);
   	c->cd(i+1)->SetLogy();
   	hAdcHists1[i]->Draw();
   	gPad->AddExec("exsel_cadc_channels_15mu","select_hist()");
    }
}

void ShowHistsScriptADCchannels_10mu()
{
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cadc_channels_10mu")));
    else
    {
        c = new TCanvas("cadc_channels_10mu","VME Data Hists",900,900);
        c->Divide(4,3);
    }
    c->cd();
    for (int i=0;i<12;i++)
    {
      c->cd(i+1);
      c->cd(i+1)->SetLogy();
      hAdcHists1[i+12]->Draw();
      gPad->AddExec("exsel_cadc_channels_10mu","select_hist()");
    }
}

void ShowHistsScriptTDCchannels()
{
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_channels")));
    else
    {
        c = new TCanvas("ctdc_channels","TDC channels",900,900);
        c->Divide(4,3);
    }
    c->cd();
    for (int i=1;i<=12;i++)
    {
	c->cd(i);
	hAdcHists2[i-1]->Draw();
	gPad->AddExec("exsel_ctdc_channels","select_hist()");
    }
}

void ShowHistsScriptTDCcounters(){

   if (gOutputFile == NULL) return;
   if (!hLength) return;
   TCanvas *c = 0;

   if  ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_counters")));
   else{
     c = new TCanvas("ctdc_counters","TDC counters",900,900);
     c->Divide(2,2);
   }

   c->cd();
   for (int i = 0; i < 4; i++){
      c->cd(i+1);
      hCounterTimeDiff[i]->Draw();
      gPad->AddExec("exsel_ctdc_counters","select_hist()");
   }

}

void ShowHistsScriptTDC_QDC()
{
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_qdc")));
    else
    {
        c = new TCanvas("ctdc_qdc","QDC-TDC correlations",900,900);
        c->Divide(4,2);
    }
    c->cd();
    for (int i=3;i<11;i++)
    {
   	c->cd(i-2);
   	hAdcTdc[i+12]->Draw("colz");
   	gPad->AddExec("exsel_ctdc_qdc","select_hist()");
    }
}

void ShowHistsScriptTDC_ADC()
{
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_adc")));
    else
    {
        c = new TCanvas("ctdc_adc","ADC-TDC correlations",900,900);
	     c->Divide(4,3);
    }
    c->cd();
    for (int i=0;i<12;i++)
    {
	c->cd(i+1);
	hAdcTdc[i]->Draw("colz");
	gPad->AddExec("exsel_ctdc_adc","select_hist()");
    }

}

void ShowHistsXYhitsADC(){
    if (gOutputFile == NULL) return;
    if (!hLength) return;
    TCanvas *c = 0;
    if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("xy_hits")));
    else
    {
        c = new TCanvas("xy_hits","XY distribution",900,900);
        c->Divide(2,1);
    }
    gStyle->SetGridStyle(0);
    c->cd();
    c->cd(1);
    c->cd(1)->SetGrid(1,1);
    hADC_XY->Draw("colz");
    c->cd(2);
    c->cd(2)->SetGridx(1);
    hCounters_XY->Draw("colz");
   gPad->AddExec("exsel_xy_hist","select_hist()");
}

void HistResetScript()
{
   if (gOutputFile == NULL) return;
   if (!hLength) return;
   int i;
   hLength->Reset();
   hRate->Reset();
   hNhits->Reset();
   hSum->Reset();
//   hChans->Reset();
//   hMult->Reset();
   hAmpChan->Reset();
   hLength1->Reset();
   hRate1->Reset();
   hNhits1->Reset();
   hSum1->Reset();
   hAmpChan1->Reset();
   hLength2->Reset();
   hRate2->Reset();
   hNhits2->Reset();
   hMult2->Reset();
   hAmpChan2->Reset();
   for (i = 0; i < N_ADC; i++) 
      hAdcHists1[i]->Reset();
   for (i = 0; i < N_QDC; i++)
      hAdcHists[i]->Reset();
   for (i = 0; i < N_TDC; i++){
      hAdcHists2[i]->Reset();
      hAdcTdc[i]->Reset();
   }
   return;
}

int ped_read785()
{
	FILE *fped;
	int n, j;
	double mean, sigma;
	char buf[201];
        
	if((fped=fopen("pedestal785.dat","r"))==NULL)
        {
         printf( "ped_read785", " Can't open file pedestal785.dat!");
         return 1;
        }
	fgets(buf, 200, fped) ;	 
        for(j = 0; j < 32; j++)
	{
	 PedMean[j] = 0;
	 PedSigma[j] = 0;
	 fgets(buf, 200, fped) ;
         if (sscanf(buf,"%i %lf %lf",&n,&mean,&sigma) == 3)
         {
	    PedMean[j] = mean;
	    PedSigma[j] = sigma;
         };
	}
        fclose(fped);
        return 0;
} 

int ped_read792()
{
	FILE *fped;
	int n, j;
	double mean, sigma;
	char buf[201];
        
	if((fped=fopen("pedestal792.dat","r"))==NULL)
        {
         printf( "ped_read792", " Can't open file pedestal792.dat!");
         return 1;
        }
	fgets(buf, 200, fped) ;	 
        for(j = 0; j < 32; j++)
	{
	 PedMeanq[j] = 0;
	 PedSigmaq[j] = 0;
	 fgets(buf, 200, fped) ;
         if (sscanf(buf,"%i %lf %lf",&n,&mean,&sigma) == 3)
         {
      	    PedMeanq[j] = mean;
      	    PedSigmaq[j] = sigma;
         };
	}
        fclose(fped);
        return 0;
} 

TControlBar *bar = 0;
void menu()
{
  char name[128];
//  sprintf(name, "Online run %05d", gRunNumber);
  sprintf(name, "Online");
  bar = new TControlBar("vertical", name, 10, 10);
  bar->AddButton("General control","ShowHistsScriptADCcontrol()","ADC control hists"); 
  bar->AddButton("QDC channels","ShowHistsScriptQDCchannels()","QDC channels hists"); 
  bar->AddButton("QDC vs TDC","ShowHistsScriptTDC_QDC()","QDC vs TDC"); 
  bar->AddButton("ADC channels","ShowHistsScriptADCchannels()","ADC channels"); 
  bar->AddButton("SiPM 10#mu ADC","ShowHistsScriptADCchannels_10mu()","SiPM 10#mu ADC channels");
  bar->AddButton("ADC vs TDC","ShowHistsScriptTDC_ADC()","SiPM 15#mu ADC vs TDC");
  bar->AddButton("TDC channels","ShowHistsScriptTDCchannels()","TDC channels hists");
  bar->AddButton("TDC counters", "ShowHistsScriptTDCcounters()","TDC counters"); 
  bar->AddButton("XY distribution", "ShowHistsXYhitsADC()","XY distribution"); 
  bar->AddButton("Reset all hists","HistResetScript()","Reset all hists");    
  bar->AddButton("Quit","gROOT->ProcessLine(\".q\")","Quit ROOT"); 
  bar->Show();
  gROOT->SaveContext();
}

void select_hist()
{
  //example of macro called when a mouse event occurs in a pad.
  // Example:
  // Root > TFile f("hsimple.root");
  // Root > hpxpy.Draw();
  // Root > c1.AddExec("exsel","select_hist()");
  // When moving the mouse in the canvas, a second canvas shows the
  // projection along X of the bin corresponding to the Y position
  // of the mouse. The resulting histogram is fitted with a gaussian.
  // A "dynamic" line shows the current bin position in Y.
  // This more elaborated example can be used as a starting point
  // to develop more powerful interactive applications exploiting CINT
  // as a development engine.
  //Author: Rene Brun

  int event = gPad->GetEvent();
  if (event != 11) return;

  TObject *select = gPad->GetSelected();
  if(!select) return;
  if (!(select->InheritsFrom("TH1"))) return;

  //create or set the new canvas c2
  TCanvas *csingle = 0;
  if ((csingle = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("csingle")));
  else    csingle = new TCanvas("csingle");
  csingle->cd();

  //draw slice corresponding to mouse position
  if (!(select->InheritsFrom("TH1F")))
    {
      TH1F *h = (TH1F *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH2F")))
    {
      TH2F *h = (TH2F *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TProfile")))
    {
      TProfile *h = (TProfile *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH1D")))
    {
      TH1D *h = (TH1D *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH2D")))
    {
      TH2D *h = (TH2D *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TProfile2D")))
    {
      TProfile2D *h = (TProfile2D *)select;
      h->Draw();
    }
  else return;
  csingle->Update();
}
