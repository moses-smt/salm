// Revision $Rev: 3794 $
// Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $

#if ! defined (__HEADER_SUFFIXARRAY_LANGUAGE_MODEL_INCLUDED__)
#define __HEADER_SUFFIXARRAY_LANGUAGE_MODEL_INCLUDED__


#include "_SuffixArraySearchApplicationBase.h"
#include "salm_shared.h"

/**
* \ingroup lm
**/
typedef unsigned int LMState;


/**
* \ingroup lm
**/
typedef struct s_cachedLmInfo{
	int nextState;
	double logProb;
}S_CachedLmInfo;

/**
* \ingroup lm
**/
typedef struct s_NgramLocationInCorpus{
	TextLenType posInCorpus;
	unsigned char len;
}S_NgramLocationInCorpus;

/**
* \ingroup lm
**/
typedef struct s_lmStateInfo{
	S_NgramLocationInCorpus locationInCorpus;
	map<IndexType, S_CachedLmInfo> cachedNextWordExtension;	//cached information of this LMState extended by the next word
}S_LMStateInfo;

/**
* \ingroup lm
**/
struct lt_ngramLocationInCorpus
{
  bool operator()(S_NgramLocationInCorpus a, S_NgramLocationInCorpus b) const{
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
* C_SuffixArrayLanguageModel inherit the C_SuffixArraySearchApplicationBase class and C_SuffixArrayScanningBase
* to provide functionalities of estimating the likelihood of a sentence given an indexed training corpus
* 
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
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

	///set the interploation strategy
	void setParam_interpolationStrategy(char interpolationStrategy);


	C_SuffixArrayLanguageModel(const char * cfgFileName);
	C_SuffixArrayLanguageModel();
	~C_SuffixArrayLanguageModel();


private:

	void calcNgramMatchingInfoTokenFreqOnlyExtendingCurrentMatch(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, double *freqTable, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen);

	//Log prob calculation
	double logProbFromFreq(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen);
	double calcLogProb(double *freq);
	double calcLogProb_equalWeightedInterpolation(double *freq);
	double calcLogProb_ibmHeuristicInterpolation(double *freq);
	double calcLogProb_maxProbInterpolation(double * freq);

	char interpolationStrategy;
	int maxN;
	IndexType vocIdForSentStart;
	IndexType vocIdForSentEnd;
	IndexType vocIdForCorpusEnd;

	///Discounting
	void constructDiscountingMap();
	double *discountingMap;
	double discountFreq(int n, unsigned int observedFreq);
	bool applyDiscounting;
	int maxFreqForDiscounting;
	S_nGramScanningInfoElement * nGramScanningList;	
	

	///LM State and related functions
	void resetLmStates();
	void initialLmState();	
	
	//caching lm prob for each sentence	
	vector<S_LMStateInfo> allLMStates;
	map<S_NgramLocationInCorpus, int, lt_ngramLocationInCorpus> ngramLocation2LmStateId;



};

#endif
