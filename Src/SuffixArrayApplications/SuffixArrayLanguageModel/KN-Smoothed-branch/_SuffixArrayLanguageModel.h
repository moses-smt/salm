#if ! defined (__HEADER_SUFFIXARRAY_LANGUAGE_MODEL_INCLUDED__)
#define __HEADER_SUFFIXARRAY_LANGUAGE_MODEL_INCLUDED__


#include "_SuffixArraySearchApplicationBase.h"
#include "salm_shared.h"
#include "time.h"

/**
* \ingroup lm
* Context type information needed in KN-smoothing
**/
typedef struct s_contextTypeInfo{
	double N1plus_dot_w_in2_i;		//Goodman and Chen 98, eq 23
	double N1plus_dot_w_in2_i1_dot;
	double N1_w_in1_i1_dot;			//Goodman and Chen 98, eq 19
	double N2_w_in1_i1_dot;
	double N3plus_w_in1_i1_dot;
	bool valid;
}S_ContextTypeInfo;


/**
* \ingroup lm
**/
typedef unsigned int LMState;


/**
* \ingroup lm
**/
typedef struct s_lmStateInfo{
	TextLenType posInCorpus;
	unsigned char len;
}S_LMStateInfo;

/**
* \ingroup lm
**/
typedef struct s_bufferedLmInfo{
	int nextState;
	double logProb;
}S_BufferedLmInfo;


/**
* \ingroup lm
**/
struct lt_lmStateInfo
{
  bool operator()(S_LMStateInfo a, S_LMStateInfo b) const{
		if(a.posInCorpus<b.posInCorpus){
			return true;
		}

		if(a.posInCorpus>b.posInCorpus){
			return false;
		}

		if(a.len<b.len){
			return true;
		}

		return false;	
	}
};


/**
* \ingroup lm
* structure for elements in the cache for accessing the suffix array for LM prob
**/
typedef struct s_cached_SA_access_key{
	TextLenType currentMatchStart;
	unsigned char currentMatchLen;
	IndexType nextWord;
}S_CachedSA_Access_Key;

typedef struct s_cached_SA_access_info{
	TextLenType updatedMatchingStart;
	unsigned char updatedMatchingLen;
	double logProb;
	long lastTimedUsed;
}S_Cached_SA_Access_Info;

struct lt_s_cached_SA_access_key
{
  bool operator()(S_CachedSA_Access_Key a, S_CachedSA_Access_Key b) const{
		if(a.currentMatchStart<b.currentMatchStart){
			return true;
		}

		if(a.currentMatchStart>b.currentMatchStart){
			return false;
		}

		if(a.currentMatchLen<b.currentMatchLen){
			return true;
		}

		if(a.currentMatchLen>b.currentMatchLen){
			return false;
		}

		if(a.nextWord<b.nextWord){
			return true;
		}

		return false;	
	}
};


/**
* \ingroup lm
* C_SuffixArrayLanguageModel inherit the C_SuffixArraySearchApplicationBase class and C_SuffixArrayScanningBase
* to provide functionalities of estimating the likelihood of a sentence given an indexed training corpus
* 
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
class C_SuffixArrayLanguageModel : public C_SuffixArraySearchApplicationBase
{

public:
	IndexType returnVocId(C_String aWord);	

	/// At the beginning of a sentence, return the LMState and reset the cache
	LMState beginOfSentenceState();
	
	/// Calculate the log prob of a word predicted by the history LM state
	double logProb(LMState lmState, IndexType nextWord, LMState & nextState);
	
	/// The log prob of a phrase extending the history as a LMState
	double logProb(LMState lmState, vector<IndexType> nextPhrase, LMState & nextState);	
	
	/// End of sentence
	double logProbEnd(LMState lmState);	

	/// Constructors
	C_SuffixArrayLanguageModel(const char * cfgFileName);
	C_SuffixArrayLanguageModel();
	~C_SuffixArrayLanguageModel();


private:
	void scanCorpusForContextTypeInfo(int n, IndexType w_i, TextLenType  leftBoundaryOfSaRangeFor_w_in2_i1, TextLenType rightBoundaryOfSaRangeFor_w_in2_i1, TextLenType  leftBoundaryOfSaRangeFor_w_in1_i1, TextLenType rigthBoundaryOfSaRangeFor_w_in1_i1, S_ContextTypeInfo & result);

	void calcNgramMatchingInfoTokenFreqOnlyExtendingCurrentMatch(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, double *freqTable, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen);
	void calcNgramMatchingInfoTokenFreqContextTypeExtendingCurrentMatch(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, double *freqTable, S_ContextTypeInfo * contextTypeInfo, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen);
	
	//Log prob calculation
	double logProbOfNgramFromCorpusInfo(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen);
	double calcLogProb(double *freq);
	double calcLogProb_equalWeightedInterpolation(double *freq);
	double calcLogProb_ibmHeuristicInterpolation(double *freq);
	double calcLogProb_maxProbInterpolation(double * freq);
	double calcLogProb_kneserNeySmoothing(double *freq, S_ContextTypeInfo * contextTypeFreq);

	///parameter and settings
	///set the interploation strategy
	void setParam_interpolationStrategy(char interpolationStrategy);

	///set the number of sentences processed by the LM before purging the cache
	void setParam_numberOfSentSeenToPurgeCache(int numberOfSentSeenToPurgeCache);

	///set the fresh time thresh for the cache entries
	void setParam_freshTime(long freshTime);

	char smoothingStrategy;
	char interpolationStrategy;
	int maxN;
	IndexType vocIdForSentStart;
	IndexType vocIdForSentEnd;
	IndexType vocIdForCorpusEnd;


	///Discounting
	void constructDiscountingMap();
	double discountFreq_GT(int n, unsigned int observedFreq);

	double * Y;	// following the notation of Chen&Goodman 98, Eq. 26
	double * D1;
	double * D2;
	double * D3plus;
	double typeOfBigrams;	//will be needed for KN-smoothing

	double *discountingMap;
	bool applyDiscounting;
	int maxFreqForDiscounting;
	S_nGramScanningInfoElement * nGramScanningList;	
	map<IndexType, unsigned int> typeFreqPrecedingWord;

	///LM State and related functions
	void resetLmStates();
	void initialLmState();	
	map< pair<LMState, IndexType>, S_BufferedLmInfo> buffer;
	vector<S_LMStateInfo> allLMStates;
	map<S_LMStateInfo, int, lt_lmStateInfo> lmStateInfo2Id;

	//caching information for SA access
	unsigned int sentenceProcessedSoFar;
	long freshTime;
	unsigned int numberOfSentSeenToPurgeCache;
	map<S_CachedSA_Access_Key, S_Cached_SA_Access_Info, lt_s_cached_SA_access_key> cached_sa_access;
	void purgeCache(long lastVisitedTime);

};

#endif
