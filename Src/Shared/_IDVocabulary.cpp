/**
* _IDVocabulary.cpp: implementation of the C_IDVocabulary class.
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/


#include "_IDVocabulary.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <memory.h>
#include <stdlib.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C_IDVocabulary::C_IDVocabulary()
{
	this->maxIdInVoc = 0;	
}

C_IDVocabulary::C_IDVocabulary(const char * fileName)
{
		
	this->maxIdInVoc = 0;	

	this->loadFromFile(fileName);
}

C_IDVocabulary::~C_IDVocabulary()
{

}

/// Return the vocID of word "text" if it exist in the vocabulary
/// Otherwise return 0
IndexType C_IDVocabulary::returnId(C_String text)
{
	IndexType id;
	
	map<C_String, IndexType, ltstr>::iterator iterText2Id;
	iterText2Id = this->text2id.find(text);

	if(iterText2Id==this->text2id.end()){ //this word does not exist in the voc yet, return ID for <unk>
		id = 0;
	}
	else{
		id = iterText2Id->second;
	}
	
	return id;
}

/// Return the text of the word given its vocID
/// return <UNK> if specified vocID does not exist
C_String C_IDVocabulary::getText(IndexType id)
{
	map<IndexType, C_String>::iterator iterId2Text;
	iterId2Text = this->id2text.find(id);

	if(iterId2Text==this->id2text.end()){
		return C_String("<UNK>");
	}

	return iterId2Text->second;
}

IndexType C_IDVocabulary::getSize()
{
	return this->text2id.size();
}


/// Load the vocabulary file into memory
/// The format of the vocabulary file is:
///		word vocID
//	in each line.
void C_IDVocabulary::loadFromFile(const char *fileName)
{

	ifstream existingVocFile;
	existingVocFile.open(fileName);

	if(!existingVocFile){
		cerr<<"Can not open existing vocabulary file "<<fileName<<endl;
		exit(0);
	}

	cerr<<"Loading existing vocabulary file: "<<fileName<<endl;

	char aLine[1024];
	char * aToken;
	char delimit[] = " \t\r\n";	
	IndexType vocId = 0;
	
	while(!existingVocFile.eof()){
		existingVocFile.getline(aLine, 1024, '\n');
		
		if(strlen(aLine)>0){	//a meaningful word, esp for the last line during reading file
				vector<C_String> tokensInLine;

				aToken = strtok(aLine, delimit);				
				while( aToken != NULL ) {	
					tokensInLine.push_back(C_String(aToken));
					aToken = strtok( NULL, delimit);
				}
				
				if(tokensInLine.size()!=2){
					cerr<<"Not valid format for Vocabulary: "<<aLine<<endl;
				}
				
				vocId = atoi(tokensInLine[1].toString());

				if(vocId>this->maxIdInVoc){
					this->maxIdInVoc = vocId;
				}

				this->text2id.insert(make_pair(tokensInLine[0], vocId));
				this->id2text.insert(make_pair(vocId, tokensInLine[0] ));
		
		}
		
		aLine[0]=0;
	}
	cerr<<"Total "<<this->text2id.size()<<" word types loaded\n";
	cerr<<"Max VocID="<<this->maxIdInVoc<<endl;
}

/// Return the maximum ID from all words in the vocabulary
/// Usually equals to the size of the vocabulary if the vocabulary is created from this corpus only.
/// If the vocabulary includes words from other corpora and the vocabulary only lists words in this corpus,
/// then max voc ID could be different from the vocabulary size
IndexType C_IDVocabulary::returnMaxID()
{
	return this->maxIdInVoc;
}

IndexType C_IDVocabulary::returnNullWordID()
{
	return 0;
}

/**
* Output the vocabulary to a file
**/
void C_IDVocabulary::outputToFile(char *filename)
{

	ofstream outputVocFile;
	outputVocFile.open(filename);

	if(!outputVocFile){
		cerr<<"Can not open "<<filename<<" to write vocabulary\n";
		exit(-1);
	}

	map<C_String, IndexType, ltstr>::iterator iterText2Id;

	iterText2Id = this->text2id.begin();
	while(iterText2Id!=this->text2id.end()){
		outputVocFile<<iterText2Id->first.toString()<<"\t"<<iterText2Id->second<<endl;
		iterText2Id++;
	}

	outputVocFile.close();
}

/// Reserver vocID 0-NUMBER_OF_RESERVED_WORDS_IN_VOC for special words that might be useful for applications
/// Here we reserved 5 words:
/// _SENT_ID_PLACEHOLDER_ 1
/// _END_OF_SENTENCE_ 2
/// _TOO_LONG_TOKEN_ 3
/// _SENTENCE_START_ 4
/// _END_OF_CORPUS_ 5
/// You can add other special words to the list as long as the assignment of vocID and its interpretation is consistent between application and indexing
void C_IDVocabulary::addingReservedWords()
{
	this->insertWord(C_String("_SENT_ID_PLACEHOLDER_"), 1);
	this->insertWord(C_String("_END_OF_SENTENCE_"), 2);
	this->insertWord(C_String("_TOO_LONG_TOKEN_"), 3);
	this->insertWord(C_String("_SENTENCE_START_"), 4);
	this->insertWord(C_String("_END_OF_CORPUS_"), 5);
	
	char reservedWord[20];
	for(int i=6; i<=NUMBER_OF_RESERVED_WORDS_IN_VOC; i++){
		memset(reservedWord, 0, 20);
		sprintf(reservedWord, "_RESERVED_WORDS_%d", i);
		this->insertWord(C_String(reservedWord), i);
	}
}

void C_IDVocabulary::insertWord(C_String text, IndexType id)
{
	this->text2id.insert(make_pair(text, id));
	this->id2text.insert(make_pair(id, text));

}

/**
*	Check if the word already exist in the voc, 
*	if so, return the vocID of the word,
*	otherwise assign an ID to this word and insert it into the voc
**/
IndexType C_IDVocabulary::getId(C_String text)
{
	IndexType id = this->returnId(text);
	if(id==0){
		this->maxIdInVoc++;
		this->insertWord(text, this->maxIdInVoc);
		return this->maxIdInVoc;
	}

	//else, already exist
	return id;
}
