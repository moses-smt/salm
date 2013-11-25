#if !defined (__HEADER_UNIVERSAL_VOC_INCLUDED__)
#define __HEADER_UNIVERSAL_VOC_INCLUDED__

#include "salm_shared.h"
#include "_IDVocabulary.h"
#include "_String.h"

#include <map>

using namespace std;

/**
* \ingroup utils
* Universal Vocabulary class provides function to update the univeral vocabulary
* with the words in a new corpus
* and output the vocabulary needed for the new corpus
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
class C_UniversalVocabulary{

public:
	void updateWithNewCorpus(const char * newCorpusFileName);

	C_UniversalVocabulary(const char * universalVocFileName);
	~C_UniversalVocabulary();

private:
	char * universalCorpusFileName;
	C_IDVocabulary * universalVoc;

	map<C_String, IndexType, ltstr> wordsUsedInTheNewCorpus;

};


#endif
