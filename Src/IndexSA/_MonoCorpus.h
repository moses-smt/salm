#if !defined(__MonoCorpus__H__INCLUDED_)
#define __MonoCorpus__H__INCLUDED_

#include "_IDVocabulary.h"
#include "salm_shared.h"

/**
* \ingroup index
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
* Defines the wrapper class for the comparing function
**/
class C_SuffixPointer
{
public:
	C_SuffixPointer(const C_SuffixPointer &);
	C_SuffixPointer();
	~C_SuffixPointer();
	C_SuffixPointer(TextLenType pointer);
	TextLenType pointer;
};

/**
* \ingroup index
* Monolingual corpus class for loading the corpus from file, sort it according to the suffix array order
* and convert it to the binary format for suffix array applications
**/
class C_MonoCorpus  
{
public:
	void initializeVocabulary(char * fileNameStem);
	void output(char * filename);
	void loadCorpusAndSort(const char * fileName, const char * idVocFileName, bool vocNeedsToBeUpdated);
	
	C_MonoCorpus();
	virtual ~C_MonoCorpus();

private:
	IndexType maxVocIdFromCorpus;
	void outputSuffix(char * filename);	
	void outputOffset(char * filename);
	void outputCorpus(char * filename);

	IndexType vocIdForSentIdPlaceHolder;
	IndexType vocIdForSentStart;
	IndexType vocIdForSentEnd;
	IndexType vocIdForCorpusEnd;

	TextLenType currentPosInCorpus;
	void allocateMem(TextLenType corpusSize);

	C_SuffixPointer * suffix;
	unsigned char * offsetList;
	C_IDVocabulary * voc;

	bool vocNeedsToBeUpdated;

};

#endif // !defined(__MonoCorpus__H__INCLUDED_)
