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
// #include <time.h>

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

// #include "midasServer.h"

// user definitions

/* number of channels */

#define BeamE 2

#define N_ADC 32
#define N_TDC 32

#define ADC_N_BINS 5000
#define ADC_X_LOW 0
#define ADC_X_HIGH 5000

#define ADC1_N_BINS 5000
#define ADC1_X_HIGH 5000

#define TDC_N_BINS 60000
#define TDC_X_LOW 0
#define TDC_X_HIGH 60000

#define S1 -1
#define S2 -1
#define S3 17
#define S4 16

#define TZERO 16

#define R1 0
#define L1 2
#define R2 4
#define L2 6
#define R3 8
#define L3 10
#define R4 12
#define L4 14

#define S3_ADC 9
#define S4_ADC 10

#define CHER_QDC -1
#define CHER_TDC -1

#define USE_COUNTERS_T0 0
#define COUNTERS_BEFORE 0

////////////////////////////////////////////////////////////////////////////////////////////////
// Output buffer macros
////////////////////////////////////////////////////////////////////////////////////////////////
#define UINT32 unsigned int
#define CVT_V792_GET_DATUM_GEO(data) ((UINT32)((((UINT32)data) & CVT_V792_DATUM_GEO_MSK) >> 27)) /*!< \brief Gets the DATUM GEO address for V792*/
#define CVT_V792_GET_DATUM_UN(data) ((UINT32)((((UINT32)data) & CVT_V792_DATUM_UN_MSK) >> 13))   /*!< \brief Gets the DATUM Under threshold for V792*/
#define CVT_V792_GET_DATUM_OV(data) ((UINT32)((((UINT32)data) & CVT_V792_DATUM_OV_MSK) >> 12))   /*!< \brief Gets the DATUM Overflow for V792*/
#define CVT_V792_GET_DATUM_ADC(data) ((UINT32)(((UINT32)data) & CVT_V792_DATUM_ADC_MSK))         /*!< \brief Gets the DATUM ADC for V792*/
#define CVT_V792N_GET_DATUM_CH(data) ((UINT32)((((UINT32)data) & CVT_V792_DATUM_CH_MSK) >> 17))  /*!< \brief Gets the DATUM channel for V792N */
#define CVT_V792A_GET_DATUM_CH(data) ((UINT32)((((UINT32)data) & CVT_V792_DATUM_CH_MSK) >> 16))  /*!< \brief Gets the DATUM channel for V792A */
#define CVT_QTP_DATA_TYPE_MSK 0x07000000                                                         /*!< \brief Data type bit masks */

#define CVT_V785_DATUM_GEO_MSK 0xf8000000                                                       /*!< \brief The DATUM GEO address msk for V785 */
#define CVT_V785_DATUM_CH_MSK 0x001f0000                                                        /*!< \brief The DATUM channel msk  for V785 ( bit 0x00010000 is always zero for V785N ) */
#define CVT_V785_DATUM_UN_MSK 0x00002000                                                        /*!< \brief The DATUM Under threshold msk  for V785 */
#define CVT_V785_DATUM_OV_MSK 0x00001000                                                        /*!< \brief The DATUM Overflow msk  for V785 */
#define CVT_V785_DATUM_ADC_MSK 0x00000fff                                                       /*!< \brief The DATUM ADC msk  for V785 */
#define CVT_V785_GET_DATUM_ADC(data) ((UINT32)(((UINT32)data) & CVT_V785_DATUM_ADC_MSK))        /*!< \brief Gets the DATUM ADC for V785*/
#define CVT_V785N_GET_DATUM_CH(data) ((UINT32)((((UINT32)data) & CVT_V785_DATUM_CH_MSK) >> 17)) /*!< \brief Gets the DATUM channel for V785N */
#define CVT_V785A_GET_DATUM_CH(data) ((UINT32)((((UINT32)data) & CVT_V785_DATUM_CH_MSK) >> 16)) /*!< \brief Gets the DATUM channel for V785A */

#define CVT_QTP_HDR_CH_COUNT_MSK 0x00003f00                                                         /*!< \brief The header channel count msk */
#define CVT_QTP_GET_HDR_CH_COUNT(data) ((UINT32)((((UINT32)data) & CVT_QTP_HDR_CH_COUNT_MSK) >> 8)) /*!< \brief Gets the header channel count */
#define CVT_QTP_EOB_EVENT_COUNT_MSK 0x00ffffff                                                      /*!< \brief The EOB Event count msk */
#define CVT_QTP_GET_EOB_EVENT_COUNT(data) ((UINT32)(((UINT32)data) & CVT_QTP_EOB_EVENT_COUNT_MSK))  /*!< \brief Gets the EOB Event count */

#define CVT_QTP_HEADER 0x02000000          /*!< \brief Header data type */
#define CVT_QTP_EOB 0x04000000             /*!< \brief End of block data type */
#define CVT_QTP_DATUM 0x00000000           /*!< \brief Datum data type */
#define CVT_QTP_NOT_VALID_DATUM 0x06000000 /*!< \brief Not valid datum data type */

#define CVT_QTP_HDR_GEO_MSK 0xf8000000      /*!< \brief The header GEO address msk */
#define CVT_QTP_HDR_CRATE_MSK 0x00ff0000    /*!< \brief The header crate msk */
#define CVT_QTP_HDR_CH_COUNT_MSK 0x00003f00 /*!< \brief The header channel count msk */

#define CVT_QTP_EOB_GEO_MSK 0xf8000000               /*!< \brief The EOB GEO address msk */
#define CVT_QTP_EOB_MAX_EVENT_COUNT (0x00ffffff + 1) /*!< \brief The EOB Max Event count */
#define CVT_QTP_EOB_EVENT_COUNT_MSK 0x00ffffff       /*!< \brief The EOB Event count msk */

//
// V792 specific
#define CVT_V792_DATUM_GEO_MSK 0xf8000000 /*!< \brief The DATUM GEO address msk for V792 */
#define CVT_V792_DATUM_CH_MSK 0x001f0000  /*!< \brief The DATUM channel msk  for V792 ( bit 0x00010000 is always zero for V792N ) */
#define CVT_V792_DATUM_UN_MSK 0x00002000  /*!< \brief The DATUM Under threshold msk  for V792 */
#define CVT_V792_DATUM_OV_MSK 0x00001000  /*!< \brief The DATUM Overflow msk  for V792 */
#define CVT_V792_DATUM_ADC_MSK 0x00000fff /*!< \brief The DATUM ADC msk  for V792 */
//
#define CVT_V1190_DATA_TYPE_MSK 0xf8000000 /*!< \brief Data type bit masks */

#define CVT_V1190_GLOBAL_HEADER 0x40000000       /*!< \brief Global header data type */
#define CVT_V1190_GLOBAL_TRAILER 0x80000000      /*!< \brief Global trailer data type */
#define CVT_V1190_TDC_HEADER 0x08000000          /*!< \brief TDC header data type */
#define CVT_V1190_TDC_MEASURE 0x00000000         /*!< \brief TDC measure data type */
#define CVT_V1190_TDC_ERROR 0x20000000           /*!< \brief TDC error data type */
#define CVT_V1190_TDC_TRAILER 0x18000000         /*!< \brief TDC trailer data type */
#define CVT_V1190_GLOBAL_TRIGGER_TIME 0x88000000 /*!< \brief Global trigger tag time data type */
#define CVT_V1190_FILLER 0xc0000000              /*!< \brief Filler data type */

#define CVT_V1190_GLB_HDR_MAX_EVENT_COUNT (0x003fffff + 1) /*!< \brief Max Event count */
#define CVT_V1190_GLB_HDR_EVENT_COUNT_MSK 0x07ffffe0       /*!< \brief Global header event count msk */
#define CVT_V1190_GLB_HDR_GEO_MSK 0x0000001f               /*!< \brief Global header GEO msk */
#define CVT_V1190_GLB_TRG_TIME_TAG_MSK 0x07ffffff          /*!< \brief Global EXTENDED TRIGGER TIME TAG msk */
#define CVT_V1190_GLB_TRL_STATUS_MSK 0x07000000            /*!< \brief Global trailer STATUS msk */
#define CVT_V1190_GLB_TRL_WCOUNT_MSK 0x001fffe0            /*!< \brief Global trailer Word count  msk */
#define CVT_V1190_GLB_TRL_GEO_MSK 0x0000001f               /*!< \brief Global trailer GEO msk */

#define CVT_V1190_TDC_HDR_TDC_MSK 0x03000000       /*!< \brief TDC event header TDC msk */
#define CVT_V1190_TDC_HDR_EVENT_ID_MSK 0x00fff000  /*!< \brief TDC event header EVENT ID msk */
#define CVT_V1190_TDC_HDR_BUNCH_ID_MSK 0x00000fff  /*!< \brief TDC event header BUNCH ID msk */
#define CVT_V1190_TDC_MSR_TRAILING_MSK 0x04000000  /*!< \brief TDC measurement TRAILING msk */
#define CVT_V1190_TDC_MSR_CHANNEL_MSK 0x03f80000   /*!< \brief TDC measurement CHANNEL msk (for V1190) */
#define CVT_V1190_TDC_MSR_MEASURE_MSK 0x0007ffff   /*!< \brief TDC measurement MEASURE msk (for V1190) */
#define CVT_V1290_TDC_MSR_CHANNEL_MSK 0x03e00000   /*!< \brief TDC measurement CHANNEL msk (for V1290) */
#define CVT_V1290_TDC_MSR_MEASURE_MSK 0x001fffff   /*!< \brief TDC measurement MEASURE msk (for V1290) */
#define CVT_V1190_TDC_TRL_TDC_MSK 0x03000000       /*!< \brief TDC event trailer TDC msk */
#define CVT_V1190_TDC_TRL_EVENT_ID_MSK 0x00fff000  /*!< \brief TDC event trailer TDC msk */
#define CVT_V1190_TDC_TRL_WCOUNT_MSK 0x00000fff    /*!< \brief TDC event trailer WORD COUNT msk */
#define CVT_V1190_TDC_ERR_TDC_MSK 0x03000000       /*!< \brief TDC error TDC msk */
#define CVT_V1190_TDC_ERR_ERR_FLAGS_MSK 0x00003fff /*!< \brief TDC error ERROR FLAGS msk */

#define CVT_V1190_IS_GLOBAL_HEADER(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_GLOBAL_HEADER)             /*!< \brief Checks if data type is global header */
#define CVT_V1190_IS_GLOBAL_TRAILER(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_GLOBAL_TRAILER)           /*!< \brief Checks if data type is global trailer */
#define CVT_V1190_IS_TDC_HEADER(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_TDC_HEADER)                   /*!< \brief Checks if data type is TDC header */
#define CVT_V1190_IS_TDC_MEASURE(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_TDC_MEASURE)                 /*!< \brief Checks if data type is TDC measure */
#define CVT_V1190_IS_TDC_ERROR(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_TDC_ERROR)                     /*!< \brief Checks if data type is TDC error */
#define CVT_V1190_IS_TDC_TRAILER(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_TDC_TRAILER)                 /*!< \brief Checks if data type is TDC trailer */
#define CVT_V1190_IS_GLOBAL_TRIGGER_TIME(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_GLOBAL_TRIGGER_TIME) /*!< \brief Checks if data type is global trigger time */
#define CVT_V1190_IS_FILLER(data) ((data & CVT_V1190_DATA_TYPE_MSK) == CVT_V1190_FILLER)                           /*!< \brief Checks if data type is filler */

#define CVT_V1190_GET_GLB_HDR_EVENT_COUNT(data) ((UINT32)((((UINT32)data) & CVT_V1190_GLB_HDR_EVENT_COUNT_MSK) >> 5)) /*!< \brief Gets the global header event counter of this event */
#define CVT_V1190_GET_GLB_HDR_GEO(data) ((UINT32)(((UINT32)data) & CVT_V1190_GLB_HDR_GEO_MSK))                        /*!< \brief Gets the global header GEO address of this event */

#define CVT_V1190_GET_GLB_TRG_TIME_TAG(data) ((UINT32)(((UINT32)data) & CVT_V1190_GLB_TRG_TIME_TAG_MSK)) /*!< \brief Gets the global trigger time tag */

#define CVT_V1190_GET_GLB_TRL_STATUS(data) ((UINT32)((((UINT32)data) & CVT_V1190_GLB_TRL_STATUS_MSK) >> 24)) /*!< \brief Gets the global trailer status */
#define CVT_V1190_GET_GLB_TRL_WCOUNT(data) ((UINT32)((((UINT32)data) & CVT_V1190_GLB_TRL_WCOUNT_MSK) >> 5))  /*!< \brief Gets the global trailer word counter counter */
#define CVT_V1190_GET_GLB_TRL_GEO(data) ((UINT32)(((UINT32)data) & CVT_V1190_GLB_TRL_GEO_MSK))               /*!< \brief Gets the global trailer GEO address */

#define CVT_V1190_GET_TDC_HDR_TDC(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_HDR_TDC_MSK) >> 24))           /*!< \brief Gets the TDC header TDC number */
#define CVT_V1190_GET_TDC_HDR_EVENT_ID(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_HDR_EVENT_ID_MSK) >> 12)) /*!< \brief Gets the TDC header event id */
#define CVT_V1190_GET_TDC_HDR_BUNCH_ID(data) ((UINT32)(((UINT32)data) & CVT_V1190_TDC_HDR_BUNCH_ID_MSK))         /*!< \brief Gets the TDC header bunch id */

#define CVT_V1190_GET_TDC_MSR_TRAILING(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_MSR_TRAILING_MSK) >> 26)) /*!< \brief Gets the TDC measurement TRAILING bit */
#define CVT_V1190_GET_TDC_MSR_CHANNEL(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_MSR_CHANNEL_MSK) >> 19))   /*!< \brief Gets the TDC measurement CHANNEL number (for V1190) */
#define CVT_V1190_GET_TDC_HDR_MEASURE(data) ((UINT32)(((UINT32)data) & CVT_V1190_TDC_MSR_MEASURE_MSK))           /*!< \brief Gets the TDC measurement measure value (for V1190) */
#define CVT_V1290_GET_TDC_MSR_CHANNEL(data) ((UINT32)((((UINT32)data) & CVT_V1290_TDC_MSR_CHANNEL_MSK) >> 21))   /*!< \brief Gets the TDC measurement CHANNEL number (for V1290) */
#define CVT_V1290_GET_TDC_HDR_MEASURE(data) ((UINT32)(((UINT32)data) & CVT_V1290_TDC_MSR_MEASURE_MSK))           /*!< \brief Gets the TDC measurement measure value (for V1290) */

#define CVT_V1190_GET_TDC_TRL_TDC(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_TRL_TDC_MSK) >> 24))           /*!< \brief Gets the TDC trailer TDC */
#define CVT_V1190_GET_TDC_TRL_EVENT_ID(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_TRL_EVENT_ID_MSK) >> 12)) /*!< \brief Gets the TDC trailer event id */
#define CVT_V1190_GET_TDC_TRL_WCOUNT(data) ((UINT32)(((UINT32)data) & CVT_V1190_TDC_TRL_WCOUNT_MSK))             /*!< \brief Gets the TDC trailer word count */

#define CVT_V1190_GET_TDC_ERR_TDC(data) ((UINT32)((((UINT32)data) & CVT_V1190_TDC_ERR_TDC_MSK) >> 24))     /*!< \brief Gets the TDC error TDC */
#define CVT_V1190_GET_TDC_ERR_ERR_FLAGS(data) ((UINT32)(((UINT32)data) & CVT_V1190_TDC_ERR_ERR_FLAGS_MSK)) /*!< \brief Gets the TDC error error flags */

/*-- Parameters ----------------------------------------------------*/

extern int gRunNumber;
extern bool gIsRunning;
extern bool gIsPedestalsRun;
extern bool gIsOffline;
extern bool gIsOfflineFinished;
extern int gEventCutoff;
extern unsigned int gQDCSize;
extern char *gQDCData;
extern unsigned int gADCSize;
extern char *gADCData;
extern unsigned int gTDCSize;
extern char *gTDCData;

TH1D *hETot;
TH1D *hECent;
TH2D *hXY;

TH1D *hAdcHists[N_ADC];
TH1D *hLength = NULL;
TH1D *hRate;
TH2D *hAmpChan;
TH1D *hNhits;
TH1D *hSum;
// static TH1D *hChans;
// static TH2D *hMult;

TH1D *hAdcHists1[N_ADC];
TH1D *hADCwCherenkov[N_ADC];
TH1D *hAdcHists1_tmp[N_ADC];
TH1D *PedVsTime[N_ADC];
TH1D *hADC_PedSub[N_ADC];
TH1D *hADC_Ped[N_ADC];
TH1D *hADCEn[N_ADC];
TH1D *hLength1;
TH1D *hRate1;
TH2D *hAmpChan1;
TH1D *hNhits1;
TH1D *hSum1;

TH1D *hCherenkovTest;

TH1D *hAdcHists2[N_TDC];
TH1D *hLength2;
TH1D *hRate2;
TH2D *hAmpChan2;
TH1D *hNhits2;
TH2D *hMult2;
TH1D *hTimeDiff;

TH2D *hAdcTdc[N_TDC];
TH2D *hQdcTdc[N_TDC];

TH1D *hNoBank;

TProfile *hAmpScan;

TProfile *hAmpScan1;

TObjArray *gHistoFolderStack = NULL;

double PedMean[32] = {0.}, PedSigma[32] = {0.};
double PedMeanq[32] = {0.}, PedSigmaq[32] = {0.};
double PedMean_scan[32] = {0.}, PedSigma_scan[32] = {0.};
double PedMeanq_scan[32] = {0.}, PedSigmaq_scan[32] = {0.};

int ped_read785();
int ped_read792();
void BookHists();
void HistResetScript();

bool PedestalsRun = kTRUE;

#define N_CHAN 32

// int adc_chans[N_CHAN] = {0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27,12,13,14,15,28,29,30,31};
// int tdc_chans[N_CHAN] = {0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,12,13,14,15,27,28,29,30,31};
// int qdc_chans[N_CHAN] = {16,17,18,19,20,21,22,23,24,25,26,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,27,28,29,30,31};

int adc_chans[N_CHAN] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 12, 13, 14, 15, 28, 29, 30, 31};
int tdc_chans[N_CHAN] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 12, 13, 14, 15, 28, 29, 30, 31};
int qdc_chans[N_CHAN] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 27, 28, 29, 30, 31};

int qdc_mapping[32] = {-1};
int adc_mapping[32] = {-1};
int tdc_mapping[32] = {-1};

void menu();
void ShowHistsScriptQDCcontrol();
void ShowHistsScriptQDCchannels();
void ShowHistsScriptADCcontrol();
void ShowHistsScriptADCchannels();
void ShowHistsScriptTDCcontrol();
void ShowHistsScriptTDCchannels();
void ShowHistsScriptTDC_QDC();
void ShowHistsScriptTDC_ADC();

/*-- init routine --------------------------------------------------*/

void InitScript()
{
    int i;

    gStyle->SetOptStat(1111111);

    for (i = 0; i < N_ADC; i++)
        qdc_mapping[i] = -1;
    for (i = 0; i < N_ADC; i++)
        adc_mapping[i] = -1;
    for (i = 0; i < N_TDC; i++)
        tdc_mapping[i] = -1;

    for (i = 0; i < N_CHAN; i++)
    {
        qdc_mapping[qdc_chans[i]] = i;
        adc_mapping[adc_chans[i]] = i;
        tdc_mapping[tdc_chans[i]] = i;
    }

    //   if (gIsRunning) BookHists();

    if (!gIsPedestalsRun)
    {
        if (ped_read785() == 1)
        {
            printf("Pedestals for V785 read error!");
            return;
        }
        if (ped_read792() == 1)
        {
            printf("Pedestals for V792 read error!");
            return;
        }
    }
    else
    {
        for (i = 0; i < 32; i++)
        {
            PedMean[i] = 0.;
            PedSigma[i] = 0.;
            PedMeanq[i] = 0.;
            PedSigmaq[i] = 0.;
        }
    }

    return;
}

void BookHists()
{
    char name[256];
    int i;

    /* book CADC histos */

    char title[256];

    //   printf("Enter BookHists!\n");

    hETot = new TH1D("hETot", ";E_{beam}, GeV;Counts", 300, -1., 2.5 * BeamE);
    hECent = new TH1D("hECent", ";E_{beam}, GeV;Counts", 300, -1., 2.5 * BeamE);
    hXY = new TH2D("hXY", "", 100, -3., 3., 100, -3., 3.);

    sprintf(name, "QDC1_Bank_Length");
    sprintf(title, "QDC1_Bank_Length");
    hLength = new TH1D(name, title, 100, 0, 100);

    sprintf(name, "QDC1_Hits_Number");
    sprintf(title, "QDC1_Hits_Number");
    hNhits = new TH1D(name, title, N_CHAN + 1, 0, N_CHAN + 1);

    sprintf(name, "QDC1_Channels_Rate");
    sprintf(title, "QDC1_Channels_Rate");
    hRate = new TH1D(name, title, N_CHAN + 1, 0, N_CHAN + 1);

    sprintf(name, "QDC1_Sum");
    sprintf(title, "QDC1_Sum");
    hSum = new TH1D(name, title, ADC_N_BINS + 10, ADC_X_LOW * 5 - 50, ADC_X_HIGH * 5);

    sprintf(name, "QDC1_Amps_vs_Channels");
    sprintf(title, "QDC1_Amps_vs_Channels");
    hAmpChan = new TH2D(name, title, N_CHAN + 1, 0, N_CHAN + 1, ADC_N_BINS + 10, ADC_X_LOW - 10, ADC_X_HIGH);

    sprintf(name, "ADC1_Bank_Length");
    sprintf(title, "ADC1_Bank_Length");
    hLength1 = new TH1D(name, title, 100, 0, 100);

    sprintf(name, "ADC1_Hits_Number");
    sprintf(title, "ADC1_Hits_Number");
    hNhits1 = new TH1D(name, title, N_CHAN + 1, 0, N_CHAN + 1);

    sprintf(name, "ADC1_Channels_Rate");
    sprintf(title, "ADC1_Channels_Rate");
    hRate1 = new TH1D(name, title, N_CHAN + 1, 0, N_CHAN + 1);

    sprintf(name, "ADC1_Sum");
    sprintf(title, "ADC1_Sum");
    hSum1 = new TH1D(name, title, ADC1_N_BINS + 10, ADC_X_LOW * 5 - 50, ADC1_X_HIGH * 5);

    sprintf(name, "ADC1_Amps_vs_Channels");
    sprintf(title, "ADC1_Amps_vs_Channels");
    hAmpChan1 = new TH2D(name, title, N_CHAN + 1, 0, N_CHAN + 1, ADC1_N_BINS + 10, ADC_X_LOW - 10, ADC1_X_HIGH);

    sprintf(name, "TDC1_Bank_Length");
    sprintf(title, "TDC1_Bank_Length");
    hLength2 = new TH1D(name, title, 100, 0, 100);

    sprintf(name, "TDC1_Hits_Number");
    sprintf(title, "TDC1_Hits_Number");
    hNhits2 = new TH1D(name, title, 50, 0, 50);

    sprintf(name, "TDC1_Channels_Rate");
    sprintf(title, "TDC1_Channels_Rate");
    hRate2 = new TH1D(name, title, N_CHAN + 1, 0, N_CHAN + 1);

    sprintf(name, "TDC1_Time_vs_Channels");
    sprintf(title, "TDC1 Time vs Channels");
    hAmpChan2 = new TH2D(name, title, N_CHAN + 1, 0, N_CHAN + 1, TDC_N_BINS / 10, TDC_X_LOW, TDC_X_HIGH);

    sprintf(name, "TDC1_Mult_vs_Channels");
    sprintf(title, "TDC1 Mult vs Channels");
    hMult2 = new TH2D(name, title, N_CHAN + 1, 0, N_CHAN + 1, 10, 0, 10);

    sprintf(name, "TDC1_Time_Diff");
    sprintf(title, "TDC1 Times difference");
    hTimeDiff = new TH1D(name, title, 2000, -1000, 1000);

    for (i = 0; i < N_CHAN; i++)
    {

        sprintf(name, "QDC%02d", i + 1);
        sprintf(title, "QDC %d", i + 1);

        hAdcHists[i] = new TH1D(name, title, ADC_N_BINS + 10, ADC_X_LOW - 10, ADC_X_HIGH);
    }

    for (i = 0; i < N_CHAN; i++)
    {

        sprintf(name, "ADC%02d", i + 1);
        sprintf(title, "ADC %d", i + 1);

        hAdcHists1[i] = new TH1D(name, title, ADC1_N_BINS + 10, ADC_X_LOW - 10, ADC1_X_HIGH);
        hAdcHists1_tmp[i] = new TH1D(Form("ADC%02d_tmp", i + 1), Form("ADC tmp %d", i + 1), ADC1_N_BINS + 10, ADC_X_LOW - 10, ADC1_X_HIGH);
        PedVsTime[i] = new TH1D(Form("ADC%02d_vs_Time", i + 1), Form("ADC %d signal vs time", i + 1), 100, 0, 100);
        hADC_PedSub[i] = new TH1D(Form("ADC%02d_PedSub", i + 1), Form("ADC %d ped substracted", i + 1), ADC1_N_BINS + 10, ADC_X_LOW - 10, ADC1_X_HIGH);
        hADC_Ped[i] = new TH1D(Form("ADC%02d_Ped", i + 1), Form("ADC %d pedestals", i + 1), ADC1_N_BINS + 10, ADC_X_LOW - 10, ADC1_X_HIGH);

        hADCEn[i] = new TH1D(Form("ADC%02d_Energy", i + 1), Form("ADC %d energy;E, GeV;Counts", i + 1), ADC1_N_BINS, 0., 5.);

        hADCwCherenkov[i] = new TH1D(Form("ADC%02d_Cherenkov", i + 1), Form("ADC %d w Cherenkov", i + 1), ADC1_N_BINS + 10, ADC_X_LOW - 10, ADC1_X_HIGH);
    }

    hCherenkovTest = new TH1D("hCherenkovTest", "Cherenkov test", ADC_N_BINS + 10, ADC_X_LOW - 10, ADC_X_HIGH);

    for (i = 0; i < N_CHAN; i++)
    {

        sprintf(name, "TDC%02d", i + 1);
        sprintf(title, "TDC %d", i + 1);

        hAdcHists2[i] = new TH1D(name, title, TDC_N_BINS, TDC_X_LOW, TDC_X_HIGH);
    }

    for (i = 0; i < N_CHAN; i++)
    {
        sprintf(name, "ADC_Amps_vs_Times_%d", i + 1);
        sprintf(title, "ADC Amps vs Times_%d", i + 1);
        hAdcTdc[i] = new TH2D(name, title, ADC1_N_BINS / 10 + 1, ADC_X_LOW - 10, ADC1_X_HIGH, TDC_N_BINS / 10, TDC_X_LOW, TDC_X_HIGH);
    }

    for (i = 0; i < N_CHAN; i++)
    {
        sprintf(name, "QDC_Amps_vs_Times_%d", i + 1);
        sprintf(title, "QDC Amps vs Times_%d", i + 1);
        hQdcTdc[i] = new TH2D(name, title, ADC_N_BINS / 10 + 1, ADC_X_LOW - 10, ADC_X_HIGH, TDC_N_BINS / 10, TDC_X_LOW, TDC_X_HIGH);
    }

    hNoBank = new TH1D("noBank", "Events without some bank", 3, 0, 3);

    hAmpScan = new TProfile("hAmpScan", "HighAmp/LowAmp vs time", 1000, 0, 100000, 0, 100, "e");

    hAmpScan1 = new TProfile("hAmpScan1", "QDC High/All vs time", 1000, 0, 100000, 0, 100, "e");

    return;
}

/*-- BOR routine ---------------------------------------------------*/

void StartRunScript(bool ped)
{
    gStyle->SetOptStat(1111111);

    //   if (!hLength)
    BookHists();
    //   else
    //    HistResetScript();
    gIsPedestalsRun = ped;

    //   if (!gIsPedestalsRun)
    //   {
    if (ped_read785() == 1)
    {
        printf("begin_of_run", "Pedestals for V785 read error!");
        return;
    }
    if (ped_read792() == 1)
    {
        printf("begin_of_run", "Pedestals for V792 read error!");
        return;
    }
    //   }
    //   else
    if (gIsPedestalsRun)
    {
        for (int i = 0; i < 32; i++)
        {
            PedMean_scan[i] = PedMean[i];
            PedSigma_scan[i] = PedSigma[i];
            PedMeanq_scan[i] = PedMeanq[i];
            PedSigmaq_scan[i] = PedSigmaq[i];
            PedMean[i] = 0.;
            PedSigma[i] = 0.;
            PedMeanq[i] = 0.;
            PedSigmaq[i] = 0.;
        }
    }

    return;
}

/*-- eor routine ---------------------------------------------------*/

void EndRunScript()
{
    return;
}

// void padRefresh(TCanvas *pad);

/*-- event routine -------------------------------------------------*/

void ProcessEventScript()
{
    UINT32 evnum_qdc = 0, evnum_adc = 0, evnum_tdc = 0;
    int i, l, nob = 0, nob1 = 0, nob2 = 0;
    unsigned *pdata, *pdata1, *pdata2;
    static unsigned ecount = 0, done_evnum = 0;
    static int diff_tdc_qdc = 0;
    static int diff_tdc_adc = 0;
    static int diff_qdc_adc = 0;
    static int diff_tdc_qdc_prev = 0;
    static int diff_tdc_adc_prev = 0;
    static int diff_qdc_adc_prev = 0;
    static UINT32 evnum_adc_prev = 0;
    static UINT32 evnum_qdc_prev = 0;
    static UINT32 evnum_tdc_prev = 0;
    static double adcaq_prev[32], adca_prev[32];
    static unsigned time_prev[32] = {0}, mult_prev[32] = {0}, time0_prev = 0, ts1_prev = 0, ts2_prev = 0, ts3_prev = 0, ts4_prev = 0, tcher_prev = 0, time0tr_prev = 0;
    static bool prev_exist = false;
    static int nhits_prev = 0;

    double calib[N_CHAN] = {137, 120, 95, 138, 124, 76, 165, 147, 144.5, 86, 96.5, 104, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    UINT32 peds[N_CHAN] = {134, 144, 86, 163, 118, 159, 188, 174, 187, 130, 113, 154, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // TFile *FileCalib = new TFile("/home/petukhov/PHOSBeamTests/ADCcalib.root", "read");
    // TH1F *hCalib = (TH1F*)FileCalib->Get("hCalib");

    // for (int ich = 0; ich < hCalib->GetNbinsX(); ich++)
    //  calib[ich] = hCalib->GetBinContent(ich+1);

    // delete FileCalib;

    /* look for ADC0 bank, return if not present */
    ecount++;
    //   if (ecount > 30000) return;
    if (gQDCData == NULL)
    {
        l = 0;
        nob = 1;
        hNoBank->Fill(0);
    }
    else
    {
        l = gQDCSize;
        nob = 0;
    }
    //   printf("qp = %p qs = %d ap = %p as = %d tp = %p ts = %d\n",gQDCData, gQDCSize, gADCData, gADCSize, gTDCData, gTDCSize);
    //   printf("gIsRun %d gRunNum %d gPedes %d hLen %p\n", (int)gIsRunning, gRunNumber, (int)gIsPedestalsRun, hLength);

    pdata = (unsigned *)gQDCData;

    if (!nob)
        hLength->Fill(l);

    //   if (!l) return 1;

    //   printf("QDC before loop\n");

    /* fill ADC histos if above threshold */
    unsigned multq[32];
    for (i = 0; i < 32; i++)
        multq[i] = 0;
    double adcaq[32];
    for (i = 0; i < 32; i++)
        adcaq[i] = 1000000.;
    unsigned data = 0, nhits = 0;
    double sum = 0.;
    //   printf("**********New event*************\n");
    for (i = 0; i < l; i++)
    {
        data = *pdata++;
        switch (data & CVT_QTP_DATA_TYPE_MSK)
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
            evnum_qdc = CVT_QTP_GET_EOB_EVENT_COUNT(data);
            //			printf("adc_calib", "    EOB, event counter %d", event_count);
        }
        break;
        case CVT_QTP_DATUM:
        {
            // DATUM
            //			UINT32 geo= CVT_V792_GET_DATUM_GEO( data);
            UINT32 ch = 0;
            UINT32 adc = CVT_V792_GET_DATUM_ADC(data);
            //			UINT32 un= CVT_V792_GET_DATUM_UN( data);
            //			UINT32 ov= CVT_V792_GET_DATUM_OV( data);
            ch = CVT_V792A_GET_DATUM_CH(data);
            if (ch >= 32)
            {
                printf("    QDC channel %d > 31\n", ch);
                continue;
            }
            int det = qdc_mapping[ch];
            //        		printf("adc_calib", "Data 0x%08x chan %u amp %u det %d", data, ch, adc, det);
            if (det >= 0 && det < N_CHAN)
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

    if (!nob)
        hNhits->Fill(nhits);
    if (!nob)
        hSum->Fill(sum);
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
        hNoBank->Fill(1);
    }
    else
    {
        l = gADCSize;
        nob1 = 0;
    }
    pdata1 = (unsigned *)gADCData;

    if (!nob1)
        hLength1->Fill(l);

    double adca[32];
    for (i = 0; i < 32; i++)
        adca[i] = 1000000.;

    /* fill ADC histos if above threshold */
    for (i = 0; i < l; i++)
    {
        data = *pdata1++;
        switch (data & CVT_QTP_DATA_TYPE_MSK)
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
            evnum_adc = CVT_QTP_GET_EOB_EVENT_COUNT(data);
        }
        break;
        case CVT_QTP_DATUM:
        {
            // DATUM
            //			UINT32 geo= CVT_V785_GET_DATUM_GEO( data);
            UINT32 ch = 0;
            UINT32 adc = CVT_V785_GET_DATUM_ADC(data);
            //			UINT32 un= CVT_V785_GET_DATUM_UN( data);
            //			UINT32 ov= CVT_V785_GET_DATUM_OV( data);
            ch = CVT_V785A_GET_DATUM_CH(data);
            if (ch >= 32)
            {
                printf("    ADC channel %d > 31\n", ch);
                continue;
            }
            int det = adc_mapping[ch];
            if (det >= 0 && det < N_CHAN)
            {
                //			if (adc) hRate1->Fill(ch);
                hRate1->Fill(det);
                adca[det] = (double)adc - PedMean[ch];
                hAdcHists1[det]->Fill((double)adc - PedMean[ch]);
                hADCEn[det]->Fill((double)(adc - peds[det]) / calib[det]);
                if (adcaq[2] > 100 && adcaq[2] < 1000000)
                {
                    hCherenkovTest->Fill(adcaq[2]);
                    hADCwCherenkov[det]->Fill((double)adc - PedMean[ch]);
                }

                hAdcHists1_tmp[det]->Fill((double)adc - PedMean[ch]); // test 11.09.2023

                Int_t rate = (Int_t)(hRate1->GetBinContent(det + 1));
                if (!(rate % 2000) && rate != 0)
                { // test 11.09.2023
                    Double_t PedMean = hAdcHists1_tmp[det]->GetMaximumBin();
                    PedVsTime[det]->SetBinContent((Int_t)(rate / 2000), PedMean);
                    hAdcHists1_tmp[det]->Reset();
                }

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

    if (!nob1)
        hNhits1->Fill(nhits);
    if (!nob1)
        hSum1->Fill(sum);

    //   printf("ADC done\n");

    double Etot = 0., E[12] = {0.};
    double x = 0, y = 0, dd = 0.15;

    for (int ich = 0; ich < 12; ich++)
    {
        if (adca[ich] > 0 && adca[ich] < 1000000)
            E[ich] = (double)(adca[ich] - peds[ich]) / calib[ich] * (calib[ich] / calib[5]);
        else
            E[ich] = 0;
        Etot += E[ich];
        x += E[ich] * (ich % 4 - 1);
        y += E[ich] * (ich / 4 - 1);
    }

    hETot->Fill(Etot);

    x /= Etot;
    y /= Etot;
    if (fabs(x) < dd && fabs(y) < dd)
        hECent->Fill(Etot);
    hXY->Fill(x, y);

    unsigned time[32], mult[32], time0 = 0, time0tr = 0;
    int sg = +1;

    if (prev_exist && 0)
    {

        for (i = 0; i < N_CHAN; i++)
            time[i] = time_prev[i];
        for (i = 0; i < N_CHAN; i++)
            mult[i] = mult_prev[i];
        time0 = time0_prev;
        time0tr = time0tr_prev;

        nhits = nhits_prev;

        hNhits2->Fill(nhits);

        sg = +1;
        if (USE_COUNTERS_T0)
        {
            if (COUNTERS_BEFORE)
                sg = -1;
            else
                sg = +1;
        }

        for (i = 0; i < N_CHAN; i++)
        {

            if (time[i] == 0 || time0 == 0 || nob2)
                continue;
            hMult2->Fill(i, mult[i]);
            hAdcHists2[i]->Fill(sg * (int)time0 - sg * (int)time[i]);
            hAmpChan2->Fill(i, sg * (int)time0 - sg * (int)time[i]);

            if (i < 12)
            {
                if (!nob1)
                    if (adca[i] < 1000000)
                        hAdcTdc[i]->Fill(adca[i], (int)time0 - (int)time[i]); // slew SiPM 0-11
            }
            else if (i < 23)
            {
                if (!nob)
                    if (adcaq[i - 12] < 1000000)
                        hAdcTdc[i]->Fill(adcaq[i - 12], (int)time0 - (int)time[i]); // slew counters
            }

            // if (!nob1) if (adca[i] < 1000000) hAdcTdc[i]->Fill(adca[i], sg*(int)time0 - sg*(int)time[i]);
            // if (!nob) if (adcaq[i] < 1000000) hQdcTdc[i]->Fill(adcaq[i], sg*(int)time0 - sg*(int)time[i]);

            if (!nob1)
                if (adca[i] < 1000000 && evnum_adc != 0)
                {
                    if (adca[i] > PedMean_scan[adc_mapping[i]] + 2. * PedSigma_scan[adc_mapping[i]])
                        hAmpScan->Fill(evnum_adc, 1);
                    else
                        hAmpScan->Fill(evnum_adc, 0);
                }
        }
    }
    // end of prev_exist

    nhits = 0;

    if (gTDCData == NULL)
    {
        l = 0;
        nob2 = 1;
        hNoBank->Fill(2);
    }
    else
    {
        l = gTDCSize;
        nob2 = 0;
    }

    pdata2 = (unsigned *)gTDCData;

    //   if (!nob2) hLength2->Fill(l);

    //   printf("TDC l = %d\n", l);

    //   if (nob) return;

    for (i = 0; i < 32; i++)
        time[i] = 0;
    for (i = 0; i < 32; i++)
        mult[i] = 0;
    time0 = 0;
    time0tr = 0; /* fill ADC histos if above threshold */
                 //   if (l>20) printf(" ********* New event with l = %d\n", l);
    for (i = 0; i < l; i++)
    {
        data = *pdata2++;
        //	if (l>20) printf(" Word %d, data is 0x%08x\n", i, data);
        switch (data & CVT_V1190_DATA_TYPE_MSK)
        {
        case CVT_V1190_GLOBAL_HEADER:
        {
            // Global header
            //			UINT32 event_count= CVT_V1190_GET_GLB_HDR_EVENT_COUNT( data);
            evnum_tdc = CVT_V1190_GET_GLB_HDR_EVENT_COUNT(data);
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
            UINT32 channel = CVT_V1190_GET_TDC_MSR_CHANNEL(data);
            UINT32 measure = CVT_V1190_GET_TDC_HDR_MEASURE(data);
            // fix July 2013
            //			if (user_setting.m_V1190_type ==  CVT_V1190_TYPE_A || user_setting.m_V1190_type == CVT_V1190_TYPE_B) {
            //				channel= CVT_V1190_GET_TDC_MSR_CHANNEL( data);
            //				measure= CVT_V1190_GET_TDC_HDR_MEASURE( data);
            //			}
            //			else {
            channel = CVT_V1290_GET_TDC_MSR_CHANNEL(data);
            if (channel >= 32)
            {
                printf("    TDC channel %d > 15\n", channel);
                continue;
            }
            int det = tdc_mapping[channel];
            if (det >= 0 && det < N_CHAN)
            {
                measure = CVT_V1290_GET_TDC_HDR_MEASURE(data);
                if (time[det] != 0)
                {
                    if (measure < time[det])
                        time[det] = measure;
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
                measure = CVT_V1290_GET_TDC_HDR_MEASURE(data);
                time0 = measure;
            }

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
    }

    //   if (!nob2) hNhits2->Fill(nhits);

    sg = +1;
    time0tr = time0;
    if (USE_COUNTERS_T0)
    {
        if (COUNTERS_BEFORE)
            sg = -1;
        else
            sg = +1;
    }

    //   printf("TDC done\n");
    /*
     */
    // store previous
    prev_exist = true;
    for (i = 0; i < N_CHAN; i++)
        time_prev[i] = time[i];
    for (i = 0; i < N_CHAN; i++)
        mult_prev[i] = mult[i];
    time0_prev = time0;
    time0tr_prev = time0tr;
    nhits_prev = nhits;
    // end store previous

    if (!nob2)
        hNhits2->Fill(nhits);

    sg = +1;
    time0tr = time0;
    if (USE_COUNTERS_T0)
    {
        if (COUNTERS_BEFORE)
            sg = -1;
        else
            sg = +1;
    }

    //   printf("TDC done\n");

    for (i = 0; i < N_CHAN; i++)
    {

        if (time[i] == 0)
        {
            if (!nob1)
                if (!nob1)
                    hADC_Ped[i]->Fill(adca[i]);
        }

        if (time[i] == 0 || time0 == 0 || nob2)
            continue;
        hMult2->Fill(i, mult[i]);
        hAdcHists2[i]->Fill(sg * (int)time0 - sg * (int)time[i]);
        hAmpChan2->Fill(i, sg * (int)time0 - sg * (int)time[i]);
        if (!nob1)
            if (adca[i] < 1000000)
                hAdcTdc[i]->Fill(adca[i], sg * (int)time0 - sg * (int)time[i]);
        if (!nob)
            if (adcaq[i] < 1000000)
                hQdcTdc[i]->Fill(adcaq[i], sg * (int)time0 - sg * (int)time[i]);

        if (!nob1)
            if (adca[i] < 1000000)
            {
                hADC_PedSub[i]->Fill(adca[i]);
            }

        //    if (!nob1) if (adca[i] < 1000000 && evnum_adc != 0)
        if (!nob1)
            if (adca[i] < 1000000)
            {
                if (adca[i] > PedMean_scan[adc_mapping[i]] + 2. * PedSigma_scan[adc_mapping[i]])
                    hAmpScan->Fill(ecount, 1);
                else
                    hAmpScan->Fill(ecount, 0);
            }
        //    if (!nob1) if (adcaq[i] < 1000000 && evnum_qdc != 0)
        if (!nob1)
            if (adcaq[i] < 1000000)
            {
                if (adcaq[i] > PedMeanq_scan[qdc_mapping[i]] + 2. * PedSigmaq_scan[qdc_mapping[i]])
                    hAmpScan1->Fill(ecount, 1);
                else
                    hAmpScan1->Fill(ecount, 0);
            }
    }

    if (ecount > 0 && !done_evnum)
    {
        done_evnum = 1;
        diff_tdc_qdc = (int)evnum_qdc - (int)evnum_tdc;
        diff_tdc_adc = (int)evnum_adc - (int)evnum_tdc;
        diff_qdc_adc = (int)evnum_qdc - (int)evnum_adc;
        printf("Fix difference in event numbers: QDC = %u ADC = %u TDC = %u\n",
               evnum_qdc, evnum_adc, evnum_tdc);
    }
    if (done_evnum && nob == 0 && nob1 == 0 && nob2 == 0)
    {
        //	if (diff_tdc_adc_prev != ((int)evnum_adc - (int)evnum_tdc) && evnum_adc != 0 && evnum_tdc != 0)
        //	if (diff_tdc_qdc_prev != ((int)evnum_qdc - (int)evnum_tdc) && evnum_qdc != 0 && evnum_tdc != 0)
        //	if (diff_qdc_adc_prev != ((int)evnum_qdc - (int)evnum_adc) && evnum_qdc != 0 && evnum_adc != 0)
        if (diff_tdc_adc_prev != ((int)evnum_adc - (int)evnum_tdc))
            if (diff_tdc_qdc_prev != ((int)evnum_qdc - (int)evnum_tdc))
                if (diff_qdc_adc_prev != ((int)evnum_qdc - (int)evnum_adc))
                {
                    printf("Mismathing in event numbers: QDC = %u ADC = %u TDC = %u\n",
                           evnum_qdc, evnum_adc, evnum_tdc);
                    printf("                   previous: QDC = %u ADC = %u TDC = %u\n",
                           evnum_qdc_prev, evnum_adc_prev, evnum_tdc_prev);
                }
        if (!(ecount % 1000) && 0)
        {
            printf("Event numbers: QDC = %u ADC = %u TDC = %u\n",
                   evnum_qdc, evnum_adc, evnum_tdc);
            //        printf("                   previous: QDC = %u ADC = %u TDC = %u\n",
            //	evnum_qdc_prev, evnum_adc_prev, evnum_tdc_prev);
        }
        diff_tdc_qdc_prev = (int)evnum_qdc - (int)evnum_tdc;
        diff_tdc_adc_prev = (int)evnum_adc - (int)evnum_tdc;
        diff_qdc_adc_prev = (int)evnum_qdc - (int)evnum_adc;
    }
    if (nob == 0 && nob1 == 0 && nob2 == 0)
    {
        evnum_adc_prev = evnum_adc;
        evnum_qdc_prev = evnum_qdc;
        evnum_tdc_prev = evnum_tdc;
    }
    //   if (evnum_qdc < 100 || evnum_qdc == 0 || evnum_adc == 0 || evnum_tdc == 0)
    //   {
    //    printf("Mismathing in event numbers: QDC = %d ADC = %d TDC = %d\n",
    //	evnum_qdc, evnum_adc, evnum_tdc);
    //    printf("       qp = %p qs = %d ap = %p as = %d tp = %p ts = %d\n",gQDCData, gQDCSize, gADCData, gADCSize, gTDCData, gTDCSize);
    //   }
    if (!(ecount % 1000))
    {
        size_t n = gROOT->GetListOfCanvases()->GetEntries();
        //    if (ecount == 1000) printf("Ncanvases %d\n",n);
        for (size_t i = 0; i < n; i++)
        {
            TCanvas *can = dynamic_cast<TCanvas *>(gROOT->GetListOfCanvases()->At(i));
            //      if (ecount == 1000) printf("%s\n",can->GetName());
            if (!strcmp(can->GetName(), "cqdc_control"))
                ShowHistsScriptQDCcontrol();
            else if (!strcmp(can->GetName(), "cqdc_channels"))
                ShowHistsScriptQDCchannels();
            else if (!strcmp(can->GetName(), "cadc_control"))
                ShowHistsScriptADCcontrol();
            else if (!strcmp(can->GetName(), "cadc_channels"))
                ShowHistsScriptADCchannels();
            else if (!strcmp(can->GetName(), "ctdc_control"))
                ShowHistsScriptTDCcontrol();
            else if (!strcmp(can->GetName(), "ctdc_channels"))
                ShowHistsScriptTDCchannels();
            else if (!strcmp(can->GetName(), "ctdc_qdc"))
                ShowHistsScriptTDC_QDC();
            else if (!strcmp(can->GetName(), "ctdc_adc"))
                ShowHistsScriptTDC_ADC();
            //      padRefresh(can);
            can->Modified();
            can->Update();
        }
    }

    return;
}

void ShowHistsScriptQDCcontrol()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("cqdc_control")))
        ;
    else
    {
        c = new TCanvas("cqdc_control", "QDC control", 900, 900);
        c->Divide(2, 3);
    }
    //    printf("In ShowHistsScriptQDCcontrol!\n");
    c->cd();
    c->cd(1);
    hNhits->Draw();
    gPad->AddExec("exsel_cqdc_control", "select_hist()");
    //    gPad->AddExec("slice","DynamicExec()");
    c->cd(2);
    hRate->Draw();
    gPad->AddExec("exsel_cqdc_control", "select_hist()");
    c->cd(3);
    hAmpChan->Draw();
    gPad->AddExec("exsel_cqdc_control", "select_hist()");
    c->cd(4);
    hSum->Draw();
    gPad->AddExec("exsel_cqdc_control", "select_hist()");
}

void ShowHistsScriptADCcontrol()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("cadc_control")))
        ;
    else
    {
        c = new TCanvas("cadc_control", "ADC control", 900, 900);
        c->Divide(2, 3);
    }
    c->cd();
    c->cd(1);
    hNhits1->Draw();
    gPad->AddExec("exsel_cadc_control", "select_hist()");
    c->cd(2);
    hRate1->Draw();
    gPad->AddExec("exsel_cadc_control", "select_hist()");
    c->cd(3);
    hAmpChan1->Draw();
    gPad->AddExec("exsel_cadc_control", "select_hist()");
    c->cd(4);
    hSum1->Draw();
    gPad->AddExec("exsel_cadc_control", "select_hist()");
    c->cd(5);
    hAmpScan->Draw();
    gPad->AddExec("exsel_cadc_control", "select_hist()");
    c->cd(6);
    hAmpScan1->Draw();
    gPad->AddExec("exsel_cadc_control", "select_hist()");
}

void ShowHistsScriptTDCcontrol()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("ctdc_control")))
        ;
    else
    {
        c = new TCanvas("ctdc_control", "TDC control", 900, 900);
        c->Divide(2, 3);
    }
    c->cd();
    c->cd(1);
    hNhits2->Draw();
    gPad->AddExec("exsel_ctdc_control", "select_hist()");
    c->cd(2);
    hLength2->Draw();
    gPad->AddExec("exsel_ctdc_control", "select_hist()");
    c->cd(3);
    hRate2->Draw();
    gPad->AddExec("exsel_ctdc_control", "select_hist()");
    c->cd(4);
    hAmpChan2->Draw();
    gPad->AddExec("exsel_ctdc_control", "select_hist()");
    c->cd(5);
    hMult2->Draw();
    gPad->AddExec("exsel_ctdc_control", "select_hist()");
    c->cd(6);
    hTimeDiff->Draw();
    gPad->AddExec("exsel_ctdc_control", "select_hist()");
}

int showmap[9] = {7, 4, 1, 8, 5, 2, 9, 6, 3};

void ShowHistsScriptQDCchannels()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("cqdc_channels")))
        ;
    else
    {
        c = new TCanvas("cqdc_channels", "QDC channels", 900, 900);
        c->Divide(3, 3);
    }
    c->cd();
    for (int i = 0; i < 9; i++)
    {
        c->cd(showmap[i]);
        hAdcHists[i]->Draw();
        gPad->AddExec("exsel_cqdc_channels", "select_hist()");
        gPad->SetLogy();
    }
}
// void ShowHistsScriptADCchannels()
// {
//     if (!hLength) return;
//     TCanvas *c = 0;
//     if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cadc_channels")));
//     else
//     {
//         c = new TCanvas("cadc_channels","ADC channels",900,900);
//         c->Divide(4,3);
//     }
//     c->cd();
//     for (int i=0;i<12;i++)
//     {
// 	c->cd(i+1);
// 	hAdcHists1[i]->Draw();
// 	gPad->AddExec("exsel_cadc_channels","select_hist()");
// 	gPad->SetLogy();
//     }
// }

void ShowHistsScriptADCchannels()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("cadc_channels_15mu")))
        ;
    else
    {
        c = new TCanvas("cadc_channels_15mu", "ADC channels", 900, 900);
        c->Divide(4, 3);
    }
    c->cd();
    for (int i = 0; i < 12; i++)
    {
        c->cd(i + 1);
        c->cd(i + 1)->SetLogy();
        hAdcHists1[i]->Draw();
        gPad->AddExec("exsel_cadc_channels_15mu", "select_hist()");
    }
}

// void ShowHistsScriptTDCchannels()
// {
//     if (!hLength) return;
//     TCanvas *c = 0;
//     if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_channels")));
//     else
//     {
//         c = new TCanvas("ctdc_channels","TDC channels",900,900);
//         c->Divide(3,3);
//     }
//     c->cd();
//     for (int i=0;i<9;i++)
//     {
// 	c->cd(showmap[i]);
// 	hAdcHists2[i]->Draw();
// 	gPad->AddExec("exsel_ctdc_channels","select_hist()");
//     }
// }

void ShowHistsScriptTDCchannels()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("ctdc_channels")))
        ;
    else
    {
        c = new TCanvas("ctdc_channels", "TDC channels", 900, 900);
        c->Divide(4, 3);
    }
    c->cd();
    for (int i = 1; i <= 12; i++)
    {
        c->cd(i);
        hAdcHists2[i - 1]->Draw();
        gPad->AddExec("exsel_ctdc_channels", "select_hist()");
    }
}

// void ShowHistsScriptTDC_QDC()
// {
//     if (!hLength) return;
//     TCanvas *c = 0;
//     if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_qdc")));
//     else
//     {
//         c = new TCanvas("ctdc_qdc","TDC-QDC",900,900);
//         c->Divide(3,3);
//     }
//     c->cd();
//     for (int i=0;i<9;i++)
//     {
// 	c->cd(showmap[i]);
// 	hQdcTdc[i]->Draw();
// 	gPad->AddExec("exsel_ctdc_qdc","select_hist()");
//     }
// }

// void ShowHistsScriptTDC_ADC()
// {
//     if (!hLength) return;
//     TCanvas *c = 0;
//     if    ((c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("ctdc_adc")));
//     else
//     {
//         c = new TCanvas("ctdc_adc","TDC-ADC",900,900);
// 	c->Divide(3,3);
//     }
//     c->cd();
//     for (int i=0;i<9;i++)
//     {
// 	c->cd(showmap[i]);
// 	hAdcTdc[i]->Draw();
// 	gPad->AddExec("exsel_ctdc_adc","select_hist()");
// //	gPad->AddExec("exsel_ctdc_adc","DynamicExec()");
//     }
// }

void ShowHistsScriptTDC_QDC()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("ctdc_qdc")))
        ;
    else
    {
        c = new TCanvas("ctdc_qdc", "QDC-TDC correlations", 900, 900);
        c->Divide(4, 2);
    }
    c->cd();
    for (int i = 3; i < 11; i++)
    {
        c->cd(i - 2);
        hAdcTdc[i + 12]->Draw("colz");
        gPad->AddExec("exsel_ctdc_qdc", "select_hist()");
    }
}

void ShowHistsScriptTDC_ADC()
{
    if (!hLength)
        return;
    TCanvas *c = 0;
    if ((c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("ctdc_adc")))
        ;
    else
    {
        c = new TCanvas("ctdc_adc", "ADC-TDC correlations", 900, 900);
        c->Divide(4, 3);
    }
    c->cd();
    for (int i = 0; i < 12; i++)
    {
        c->cd(i + 1);
        hAdcTdc[i]->Draw("colz");
        gPad->AddExec("exsel_ctdc_adc", "select_hist()");
    }
}

void HistResetScript()
{
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
    hTimeDiff->Reset();
    hAmpScan->Reset();
    hAmpScan1->Reset();
    hNoBank->Reset();
    for (i = 0; i < N_CHAN; i++)
    {
        hAdcHists[i]->Reset();
    }
    for (i = 0; i < N_CHAN; i++)
    {
        hAdcHists1[i]->Reset();
    }
    for (i = 0; i < N_CHAN; i++)
    {
        hAdcHists2[i]->Reset();
        hAdcTdc[i]->Reset();
        hQdcTdc[i]->Reset();
    }
    return;
}

int ped_read785()
{
    FILE *fped;
    int n, j;
    double mean, sigma;
    char buf[201];

    if ((fped = fopen("pedestal785.dat", "r")) == NULL)
    {
        printf("ped_read785", " Can't open file pedestal785.dat!");
        return 1;
    }
    fgets(buf, 200, fped);
    for (j = 0; j < 32; j++)
    {
        PedMean[j] = 0;
        PedSigma[j] = 0;
        fgets(buf, 200, fped);
        if (sscanf(buf, "%i %lf %lf", &n, &mean, &sigma) == 3)
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

    if ((fped = fopen("pedestal792.dat", "r")) == NULL)
    {
        printf("ped_read792", " Can't open file pedestal792.dat!");
        return 1;
    }
    fgets(buf, 200, fped);
    for (j = 0; j < 32; j++)
    {
        PedMeanq[j] = 0;
        PedSigmaq[j] = 0;
        fgets(buf, 200, fped);
        if (sscanf(buf, "%i %lf %lf", &n, &mean, &sigma) == 3)
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
    sprintf(name, "Offline run %05d", gRunNumber);
    bar = new TControlBar("vertical", name, 10, 10);
    //  bar->AddButton("QDC control","ShowHistsScriptQDCcontrol()","QDC control hists");
    //  bar->AddButton("QDC channels","ShowHistsScriptQDCchannels()","QDC channels hists");
    bar->AddButton("ADC control", "ShowHistsScriptADCcontrol()", "ADC control hists");
    bar->AddButton("ADC channels", "ShowHistsScriptADCchannels()", "ADC channels hists");
    bar->AddButton("TDC control", "ShowHistsScriptTDCcontrol()", "TDC control hists");
    bar->AddButton("TDC channels", "ShowHistsScriptTDCchannels()", "TDC channels hists");
    bar->AddButton("TDC-ADC correlation", "ShowHistsScriptTDC_ADC()", "TDC-ADC correlation hists");
    bar->AddButton("TDC-QDC correlation", "ShowHistsScriptTDC_QDC()", "TDC-QDC correlation hists");
    // bar->AddButton("Reset all hists","HistResetScript()","Reset all hists");
    bar->AddButton("Quit", "exana()", "Quit ROOT");
    //  bar->AddButton("Quit","gROOT->ProcessLine(\".q\")","Quit ROOT");
    bar->SetButtonWidth(210);
    bar->Show();
    gROOT->SaveContext();
}

void exana()
{
    gIsOfflineFinished = true;
    //    gROOT->SetInterrupt(kTRUE);
}

void select_hist()
{
    // example of macro called when a mouse event occurs in a pad.
    //  Example:
    //  Root > TFile f("hsimple.root");
    //  Root > hpxpy.Draw();
    //  Root > c1.AddExec("exsel","select_hist()");
    //  When moving the mouse in the canvas, a second canvas shows the
    //  projection along X of the bin corresponding to the Y position
    //  of the mouse. The resulting histogram is fitted with a gaussian.
    //  A "dynamic" line shows the current bin position in Y.
    //  This more elaborated example can be used as a starting point
    //  to develop more powerful interactive applications exploiting CINT
    //  as a development engine.
    // Author: Rene Brun

    int event = gPad->GetEvent();
    if (event != 11)
        return;

    TObject *select = gPad->GetSelected();
    if (!select)
        return;
    if (!(select->InheritsFrom("TH1")))
        return;

    // create or set the new canvas c2
    TCanvas *csingle = 0;
    if ((csingle = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("csingle")))
        ;
    else
        csingle = new TCanvas("csingle");
    csingle->cd();

    // draw slice corresponding to mouse position
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
    else
        return;
    csingle->Update();
}
/*
void padRefresh(TCanvas *pad)
{
  if (!pad) return;
  TList *tl = pad->GetListOfPrimitives();
  if (!tl) return;
  TListIter next(tl);
  TObject *to;
  while ((to=next())) {
    if (to->InheritsFrom("TH1"))
    {
    if (!(to->InheritsFrom("TH1F")))
    {
            TH1F *h = (TH1F *)to;
            h->Draw();
        }
        else if (!(to->InheritsFrom("TH2F")))
        {
            TH2F *h = (TH2F *)to;
            h->Draw();
        }
        else if (!(to->InheritsFrom("TProfile")))
        {
            TProfile *h = (TProfile *)to;
            h->Draw();
        }
        else if (!(to->InheritsFrom("TH1D")))
        {
            TH1D *h = (TH1D *)to;
            h->Draw();
        }
        else if (!(to->InheritsFrom("TH2D")))
        {
            TH2D *h = (TH2D *)to;
            h->Draw();
        }
        else if (!(to->InheritsFrom("TProfile2D")))
        {
            TProfile2D *h = (TProfile2D *)to;
            h->Draw();
        }
    }
  }
//  pad->Modified();
//  pad->Update();
//  gSystem->ProcessEvents();
}
*/
void DynamicExec()
{
    // Example of function called when a mouse event occurs in a pad.
    // When moving the mouse in the canvas, a second canvas shows the
    // projection along X of the bin corresponding to the Y position
    // of the mouse. The resulting histogram is fitted with a gaussian.
    // A "dynamic" line shows the current bin position in Y.
    // This more elaborated example can be used as a starting point
    // to develop more powerful interactive applications exploiting CINT
    // as a development engine.
    //
    // Author:  Rene Brun

    int event = gPad->GetEvent();
    if (event != 11)
        return;

    TObject *select = gPad->GetSelected();
    if (!select)
        return;
    if (!select->InheritsFrom(TH2::Class()))
    {
        gPad->SetUniqueID(0);
        return;
    }
    TH2 *h = (TH2 *)select;
    gPad->GetCanvas()->FeedbackMode(kTRUE);

    // erase old position and draw a line at current position
    int pxold = gPad->GetUniqueID();
    //   if (pxold < 0)
    //   {
    //	gVirtualX->SetLineColor(0);
    //	gVirtualX->DrawLine(pxold,pymin,pxold,pymax);
    //	gVirtualX->SetLineColor(1);
    //	return;
    //   }
    //   gPad->SetUniqueID(-pxold);
    int px = gPad->GetEventX();
    //   int py = gPad->GetEventY();
    float uymin = gPad->GetUymin();
    float uymax = gPad->GetUymax();
    int pymin = gPad->YtoAbsPixel(uymin);
    int pymax = gPad->YtoAbsPixel(uymax);
    //   if(pxold) gVirtualX->SetLineColor(0);
    if (pxold)
        gVirtualX->DrawLine(pxold, pymin, pxold, pymax);
    //   if(pxold) gVirtualX->SetLineColor(1);
    gVirtualX->DrawLine(px, pymin, px, pymax);
    gPad->SetUniqueID(px);
    //   printf("pxold %d px %d\n",pxold,px);
    Float_t upx = gPad->AbsPixeltoX(px);
    Float_t x = gPad->PadtoX(upx);

    // create or set the new canvas c2
    TVirtualPad *padsav = gPad;
    TCanvas *c2 = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("cproj");
    if (c2)
        delete c2->GetPrimitive("Projection");
    else
        c2 = new TCanvas("cproj", "Projection Canvas", 710, 10, 700, 500);
    c2->SetGrid();
    c2->cd();

    // draw slice corresponding to mouse position
    Int_t binx = h->GetXaxis()->FindBin(x);
    TH1D *hp = h->ProjectionY("", binx - 5, binx + 5);
    hp->SetFillColor(38);
    char title[80];
    sprintf(title, "Projection of binx=%d", binx);
    hp->SetName("Projection");
    hp->SetTitle(title);
    hp->Draw();
    //  with fit
    //   hp->Fit("gaus","ql");
    //   hp->GetFunction("gaus")->SetLineColor(kRed);
    //   hp->GetFunction("gaus")->SetLineWidth(2);
    //   TPaveStats *s =
    //     (TPaveStats*)h->GetListOfFunctions()->FindObject("stats");
    //   gStyle->SetOptFit(111);
    //   gPad->SetUniqueID(px);
    c2->Update();
    padsav->cd();
    //   gPad->GetCanvas()->Update();
    return;
}
