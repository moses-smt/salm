#if !defined(__IDVocabulary_H__INCLUDED_)
#define __IDVocabulary_H__INCLUDED_

#include "_String.h"
#include <string>
#include <map>
#include <vector>
#include "salm_shared.h"

using namespace std;


struct ltstr
{
  bool operator()(C_String s1, C_String s2) const
  {
    return s1<s2;
  }
};

/**
* Vocabulary class
* Mapping between words and their IDs
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
class C_IDVocabulary  
{

public:
	///Return the ID of word "text", if the word does not exist, add the word into the voc and return the newly assigned ID
	IndexType getId(C_String text);

	void addingReservedWords();
	void outputToFile(char * filename);
	IndexType returnNullWordID();
	IndexType returnMaxID();
	IndexType returnId(C_String text);	

	IndexType getSize();	
	C_String getText(IndexType);	
	
	C_IDVocabulary();
	C_IDVocabulary(const char * fileName);
	virtual ~C_IDVocabulary();

private:
	void insertWord(C_String text, IndexType id);
	void loadFromFile(const char * fileName);
	IndexType maxIdInVoc;
	map<C_String, IndexType, ltstr> text2id;
	map<IndexType, C_String> id2text;	
};

#endif // !defined(__IDVocabulary_H__INCLUDED_)
