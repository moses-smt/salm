#include "_UniversalVocabulary.h"
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>

using namespace std;

C_UniversalVocabulary::C_UniversalVocabulary(const char * universalVocFileName)
{
	int fileNameSize=strlen(universalVocFileName);
	fileNameSize++;

	this->universalCorpusFileName = (char *) malloc(sizeof(char)*fileNameSize);
	sprintf(this->universalCorpusFileName,"%s\0", universalVocFileName);

	this->universalVoc = new C_IDVocabulary(universalVocFileName);

}

C_UniversalVocabulary::~C_UniversalVocabulary()
{
	free(this->universalCorpusFileName);
	delete(this->universalVoc);
}


/**
* Update the universal vocabulary with words in a new corpus
* Output the updated universal vocabulary
* Output the vocabulary needed for the new corpus too
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
void C_UniversalVocabulary::updateWithNewCorpus(const char * newCorpusFileName)
{

	ifstream textStream;
	textStream.open(newCorpusFileName);

	if(!textStream){
		fprintf(stderr,"Corpus file %s does not exist. Exit!\n",newCorpusFileName);
		exit(-1);
	}


	//add reserved words from universal voc
	for(IndexType vocId=1; vocId<=NUMBER_OF_RESERVED_WORDS_IN_VOC; vocId++){
		C_String reservedWordText = this->universalVoc->getText(vocId);
		this->wordsUsedInTheNewCorpus.insert(make_pair(reservedWordText, vocId));
	}

	string aLine;
	unsigned int sentNumber = 1;
	unsigned int corpusSize = 0;

	char * thisToken;
	char delimit[] =" \t\r\n";
	map<C_String, IndexType, ltstr>::iterator iterWordsUsedInTheNewCorpus;
	

	getline(textStream, aLine);
	while(!textStream.eof()){

		if(aLine.length()>0){

			thisToken = strtok((char*) aLine.c_str(), delimit );
			while( thisToken != NULL ) {			
				
				C_String thisWord(thisToken);

				//check if this word has already been seen
				iterWordsUsedInTheNewCorpus = this->wordsUsedInTheNewCorpus.find(thisWord);

				if(iterWordsUsedInTheNewCorpus == this->wordsUsedInTheNewCorpus.end()){
					//new type
					IndexType vocId = this->universalVoc->getId(thisWord);
					this->wordsUsedInTheNewCorpus.insert(make_pair(thisWord, vocId));
				}

				
				// While there are tokens in "string"
				// Get next token: 
				thisToken = strtok( NULL, delimit);
			}

		}
		
		getline(textStream, aLine);
	}


	//now output the updated universal vocabulary
	this->universalVoc->outputToFile(this->universalCorpusFileName);

	//output the vocabulary needed for the new corpus
	char vocabularyForNewCorpusFileName[1024];
	sprintf(vocabularyForNewCorpusFileName, "%s.id_voc", newCorpusFileName);

	ofstream outputVocFile;
	outputVocFile.open(vocabularyForNewCorpusFileName);

	if(!outputVocFile){
		cerr<<"Can not open "<<vocabularyForNewCorpusFileName<<" to write vocabulary\n";
		exit(-1);
	}

	iterWordsUsedInTheNewCorpus = this->wordsUsedInTheNewCorpus.begin();
	while(iterWordsUsedInTheNewCorpus!=this->wordsUsedInTheNewCorpus.end()){
		outputVocFile<<iterWordsUsedInTheNewCorpus->first.toString()<<"\t"<<iterWordsUsedInTheNewCorpus->second<<endl;
		iterWordsUsedInTheNewCorpus++;
	}

	outputVocFile.close();
}
