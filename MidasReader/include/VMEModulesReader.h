

#define N_ADC               12
#define N_TDC               23
#define N_QDC               11
#define N_CHAN              32

#define ADC_N_BINS  5000
#define ADC_X_LOW      0
#define ADC_X_HIGH  5000

#define ADC1_N_BINS  5000
#define ADC1_X_HIGH  5000

#define TDC_N_BINS  60000
#define TDC_X_LOW       0
#define TDC_X_HIGH  60000

#define TZERO 16

#define USE_COUNTERS_T0 0
#define COUNTERS_BEFORE 0

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