#if !defined(__SUFFIXARRAYAPPLICATIONBASE_H__INCLUDED_)
#define __SUFFIXARRAYAPPLICATIONBASE_H__INCLUDED_

#include "salm_shared.h"
#include "_IDVocabulary.h"
#include "_String.h"

using namespace std;

typedef struct level1BucketElement
{
	TextLenType first;
	TextLenType last;
} S_level1BucketElement;


/**
* Base class of Suffix Array applications
* Providing functions to load the suffix array and initialize the required vocIDs
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
class C_SuffixArrayApplicationBase  
{
public:
	void loadData(const char *fileNameStem, bool noVoc, bool noOffset, bool noLevel1Bucket);
	TextLenType returnCorpusSize();

	C_SuffixArrayApplicationBase();
	virtual ~C_SuffixArrayApplicationBase();

protected:	
	TextLenType corpusSize;

	void loadVoc(const char * filename);
	void loadOffset(const char * filename);
	void loadSuffix(const char * filename);
	void loadCorpusAndInitMem(const char * filename);

	bool noVocabulary;
	bool noOffset;
	bool noLevel1Bucket;

	C_IDVocabulary * voc;
	IndexType sentIdStart;
	IndexType vocIdForSentStart;
	IndexType vocIdForSentEnd;
	IndexType vocIdForCorpusEnd;

	IndexType * corpus_list;
	unsigned char * offset_list;
	TextLenType * suffix_list;

	S_level1BucketElement * level1Buckets;

};

#endif // !defined(__SUFFIXARRAYAPPLICATIONBASE_H__INCLUDED_)
