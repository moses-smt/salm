#if !defined(__SUFFIXARRAYSEARCHAPPLICATIONBASE_H__INCLUDED_)
#define __SUFFIXARRAYSEARCHAPPLICATIONBASE_H__INCLUDED_

#include "_SuffixArrayApplicationBase.h"
/**
* \ingroup search
* Used by locateExactPhraseInCorpus() to return the location of an matched n-gram in the corpus
* as a pair of <sentenceId, offset pos in sentence>
**/
typedef struct simplePhraseLocationElement
{
	TextLenType sentIdInCorpus;
	unsigned char posInSentInCorpus;
}S_SimplePhraseLocationElement;

/**
* \ingroup search
* Used by findPhraseInASentence() to return the location of an embedded n-gram in the corpus
* <posStartInSrcSent, posEndInSrcSent> represents the embedded n-gram in the sentence
* <sentIdInCorpus, posInSentInCorpus> represents the location in the corpus
**/
typedef struct phraseLocationElement
{
	unsigned char posStartInSrcSent;
	unsigned char posEndInSrcSent;
	TextLenType sentIdInCorpus;
	unsigned char posInSentInCorpus;
}S_phraseLocationElement;

/**
* \ingroup search
**/
typedef struct phraseLocationWithSrcSentElement
{
	int srcPosStart;
	int srcPosEnd;	
	TextLenType sentId;
	TextLenType posInSent;
	vector<C_String> sentence;
}S_phraseLocationWithSrcSentElement;

/**
* \ingroup search
**/
typedef struct sentSearchTableElement
{
	bool found;
	TextLenType startPosInSA;
	TextLenType endingPosInSA;
}S_sentSearchTableElement;


/**
* \ingroup search
* Base class for suffix array search applications
* Provides functions to search n-grams in the corpus
* Including the frequency of the n-gram and the actual location (sentenceID+offset in sentence)
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
class C_SuffixArraySearchApplicationBase : public C_SuffixArrayApplicationBase  
{
public:
	void loadData_forSearch(const char * filename, bool noVoc, bool noOffset);

	unsigned int numberOfMatcedNgram(const char * srcSent);
	unsigned int numberOfMatcedNgram(vector<IndexType> & sentInVocId);

	TextLenType freqOfExactPhraseMatch(const char * phrase);
	TextLenType freqOfExactPhraseMatch(vector<IndexType> & phrase);

	TextLenType freqOfExactPhraseMatchAndFirstOccurrence(const char * phrase, TextLenType & startPosInSA, int & sentLen);
	TextLenType freqOfExactPhraseMatchAndFirstOccurrence(vector<IndexType> & phrase, TextLenType & startPosInSA, int & sentLen);

	vector<S_SimplePhraseLocationElement> locateExactPhraseInCorpus(const char * phrase);
	vector<S_SimplePhraseLocationElement> locateExactPhraseInCorpus(vector<IndexType> & phrase);

	vector<S_phraseLocationElement> findPhrasesInASentence(const char * srcSent);
	vector<S_phraseLocationElement> findPhrasesInASentence(vector<IndexType> & srcSentAsVocIDs);

	void displayNgramMatchingFreq4Sent(const char *);
	void displayNgramMatchingFreq4Sent(vector<IndexType> & sentInVocId);

	map<int, pair<int, unsigned long> > returnNGramMatchingStatForOneSent(const char * srcSent, int & sentLen);
	map<int, pair<int, unsigned long> > returnNGramMatchingStatForOneSent(vector<IndexType> & sentInVocId, int & sentLen);

	S_sentSearchTableElement * constructNgramSearchTable4SentWithLCP(const char * sentText, int & sentLen);
	S_sentSearchTableElement * constructNgramSearchTable4SentWithLCP( vector<IndexType> & sentInVocId);

	void setParam_reportMaxOccurrenceOfOneNgram(int reportMaxOccurrenceOfOneNgram);
	void setParam_highestFreqThresholdForReport(int highestFreqThresholdForReport);
	void setParam_longestUnitToReport(int longestUnitToReport);
	void setParam_shortestUnitToReport(int shortestUnitToReport);

	TextLenType returnTotalSentNumber();

	vector<IndexType> convertStringToVocId(const char * sentText);
	vector<C_String> convertCharStringToCStringVector(const char * sentText);
	vector<IndexType> convertCStringVectorToVocIdVector(vector<C_String> & sentAsStringVector);


	C_SuffixArraySearchApplicationBase();
	virtual ~C_SuffixArraySearchApplicationBase();

protected:
	bool locateSAPositionRangeForExactPhraseMatch(vector<IndexType> & phrase, TextLenType & rangeStart, TextLenType & rangeEnd);

	bool searchPhraseGivenRangeWithLCP(IndexType nextWord, int lcp, TextLenType rangeStartPos, TextLenType rangeEndPos, TextLenType & resultStartPos, TextLenType & resultEndPos);
	char comparePhraseWithTextWithLCP(IndexType, int, TextLenType);

	void locateSendIdFromPos(TextLenType pos, TextLenType & sentId, unsigned char & offset);
	void locateSendIdFromPos(TextLenType pos, TextLenType & sentId, unsigned char & offset, unsigned char & sentLen);

	
	unsigned int twoDimensionIndexToOneDimensionTableIndex(unsigned int posInSent, unsigned int n, unsigned int sentLen);
	void oneDimensionTableIndexToTwoDimension(unsigned int index, unsigned int sentLen, unsigned int &posInSrcSent, unsigned int &n);

	int reportMaxOccurrenceOfOneNgram;
	int highestFreqThresholdForReport;
	int longestUnitToReport;
	int shortestUnitToReport;

	TextLenType totalSentNum;
};

#endif // !defined(__SUFFIXARRAYSEARCHAPPLICATIONBASE_H__INCLUDED_)
