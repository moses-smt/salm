/**
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
#if !defined(_SA_common_h)
#define _SA_common_h

#include "math.h"

typedef unsigned int IndexType;
typedef unsigned int TextLenType;
typedef unsigned short int SearchLenType;

//constants
const int SIZE_ONE_READ = 16384;	//when loading the data, each I/O read in SIZE_ONE_READ data points
const int MAX_TOKEN_LEN = 1024;		//length of the longest word

const int NUMBER_OF_RESERVED_WORDS_IN_VOC = 100;

/// for language modeling
const double SALM_PROB_UNK = 0.00000000023283064365386962890625; // 1/4G
const double SALM_LOG_PROB_UNK = log(SALM_PROB_UNK);
const double SALM_LOG_0 = -20;

/**
* \ingroup scan
**/
typedef struct s_nGramScanningInfoElement
{
	IndexType vocId;
	TextLenType freqThreshForOutput;
	TextLenType freqSoFar;
}S_nGramScanningInfoElement;

#endif

