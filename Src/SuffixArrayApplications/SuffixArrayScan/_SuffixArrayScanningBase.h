#if !defined (_HEADER_SUFFIX_ARRAY_SCANNING_BASE_CLASS_)
#define _HEADER_SUFFIX_ARRAY_SCANNING_BASE_CLASS_


#include "_SuffixArrayApplicationBase.h"




/**
* \ingroup scan
* C_SuffixArrayScanningBase class provides functions to scan through an indexed corpus
* and output information such as the type/token frequency of the data
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
class C_SuffixArrayScanningBase : public C_SuffixArrayApplicationBase
{
public:	
	void setNgramOutputFreqThresh(int n, unsigned int freqThresh);
	void scanSuffixArrayForHighFreqNgramType();
	void scanSuffixArrayForCountofCounts(int maxFreqConsidered);
	void scanSuffixArrayForTypeToken();

	C_SuffixArrayScanningBase(const char * filename, unsigned int maxN);
	C_SuffixArrayScanningBase();
	~C_SuffixArrayScanningBase();

protected:
	void setParam_maxFreqConsidered(int maxFreqConsidered);
	void constructCountOfCountsTable();
	void initializeForScanning(const char * filename, unsigned int maxN);
	
	int maxN;
	int maxFreqConsidered;
	
	unsigned int * countOfCountsTable;
	
	IndexType vocIdForSentStart;
	IndexType vocIdForSentEnd;
	IndexType vocIdForCorpusEnd;

private:
	void scanSuffixArray(char actionType);	

	S_nGramScanningInfoElement * nGramScanningList;	
	
	
	unsigned int * typeFreq;
	unsigned int * tokenFreq;
};

#endif
