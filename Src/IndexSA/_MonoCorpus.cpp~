/**
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/

#include "_MonoCorpus.h"
#include "malloc.h"
#include "time.h"

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

extern 	IndexType * corpus;
extern 	TextLenType actualCorpusSize;

bool operator<(const C_SuffixPointer& a, const C_SuffixPointer& b) 
{
    bool stillEqual = true;
	TextLenType currentPosOfA = a.pointer;
	TextLenType currentPosOfB = b.pointer;

	if(currentPosOfA==currentPosOfB){
		return false;
	}

	while(stillEqual){
		if(corpus[currentPosOfA]<corpus[currentPosOfB]){
			return true;
		}

		if(corpus[currentPosOfA]>corpus[currentPosOfB]){
			return false;
		}

		//then still equal at these two positions
		currentPosOfA++;
		currentPosOfB++;

		if(currentPosOfA>=actualCorpusSize){
			currentPosOfA=0;
		}

		if(currentPosOfB>=actualCorpusSize){
			currentPosOfB=0;
		}
	}

	//equal
	return false;
}


C_SuffixPointer::C_SuffixPointer()
{

}

//copy constructor
C_SuffixPointer::C_SuffixPointer(const C_SuffixPointer & obj)
{
	this->pointer = obj.pointer;
}

C_SuffixPointer::~C_SuffixPointer()
{

}


C_SuffixPointer::C_SuffixPointer(TextLenType pointer)
{
	this->pointer = pointer;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C_MonoCorpus::C_MonoCorpus()
{
	this->currentPosInCorpus = 0;
	this->maxVocIdFromCorpus = 0;
}

C_MonoCorpus::~C_MonoCorpus()
{
	free(corpus);
	free(this->suffix);
	free(this->offsetList);	
}


/**
* Initialize an IDVocabulary file
**/
void C_MonoCorpus::initializeVocabulary(char *fileNameStem)
{
	C_IDVocabulary tmpVoc;
	tmpVoc.addingReservedWords();

	char vocFileName[1024];
	sprintf(vocFileName, "%s.id_voc", fileNameStem);

	tmpVoc.outputToFile(vocFileName);
}


void C_MonoCorpus::loadCorpusAndSort(const char *fileName, const char * idVocFileName, bool vocNeedsToBeUpdated)
{
	IndexType id = 0;

	//load vocabulary
	this->voc = new C_IDVocabulary(idVocFileName);
	this->vocNeedsToBeUpdated = vocNeedsToBeUpdated;

	this->vocIdForSentIdPlaceHolder = this->voc->returnId(C_String("_SENT_ID_PLACEHOLDER_"));
	if(this->vocIdForSentIdPlaceHolder==0){
		cerr<<"ID vocabulary does not have the type _SENT_ID_PLACEHOLDER_, error!\n Add this word to the universal vocabulary and try again!\n";
		exit(-1);
	}
	if(this->vocIdForSentIdPlaceHolder>this->maxVocIdFromCorpus){
		this->maxVocIdFromCorpus = this->vocIdForSentIdPlaceHolder;
	}
	
	this->vocIdForSentStart = this->voc->returnId(C_String("_SENTENCE_START_"));
	if(this->vocIdForSentStart==0){
		cerr<<"ID vocabulary does not have the type _SENTENCE_START_, error!\n Add this word to the universal vocabulary and try again!\n";
		exit(-1);
	}
	if(this->vocIdForSentStart>this->maxVocIdFromCorpus){
		this->maxVocIdFromCorpus = this->vocIdForSentStart;
	}

	this->vocIdForSentEnd = this->voc->returnId(C_String("_END_OF_SENTENCE_"));	
	if(this->vocIdForSentEnd==0){
		cerr<<"ID vocabulary does not have the type _END_OF_SENTENCE_, error!\n Add this word to the universal vocabulary and try again!\n";
		exit(-1);
	}
	if(this->vocIdForSentEnd>this->maxVocIdFromCorpus){
		this->maxVocIdFromCorpus = this->vocIdForSentEnd;
	}

	this->vocIdForCorpusEnd = this->voc->returnId(C_String("_END_OF_CORPUS_"));
	if(this->vocIdForCorpusEnd==0){
		cerr<<"ID vocabulary does not have the type _END_OF_CORPUS_, error!\n Add this word to the universal vocabulary and try again!\n";
		exit(-1);
	}
	if(this->vocIdForCorpusEnd>this->maxVocIdFromCorpus){
		this->maxVocIdFromCorpus = this->vocIdForCorpusEnd;
	}

	ifstream textStream1;
	textStream1.open(fileName);

	if(textStream1==NULL){
		fprintf(stderr,"Text %s does not exist. Exit!\n",fileName);
		exit(-1);
	}
	
	long ltime1, ltime2;
	time( &ltime1 );
    	
	string aLine;
	unsigned int sentNumber = 1;
	unsigned int sentLen = 0;
	unsigned int corpusSize = 0;

	char * thisToken;
	char delimit[] =" \t\r\n";	

	//first, scan the corpus to estimate the size and check if each line is shorter than 256 words
	getline(textStream1, aLine);
	while(!textStream1.eof()){

		if(aLine.length()>0){
			sentLen = 0;

			thisToken = strtok((char*) aLine.c_str(), delimit );
			while( thisToken != NULL ) {			
				
				if(this->vocNeedsToBeUpdated){
					id = this->voc->getId(C_String(thisToken));
				}
				else{	//the provided vocabulary should cover all the words in this corpus
					id = this->voc->returnId(C_String(thisToken));

					if(id==0){	//word does not exist
						cerr<<"Vocabulary: "<<idVocFileName<<" does not cover all the words in the corpus!"<<endl;
						cerr<<"Word: "<<thisToken<<" does not exist in the voc!\n";
						exit(-1);
					}
				}



				sentLen++;

				if(id>this->maxVocIdFromCorpus){
					this->maxVocIdFromCorpus = id;
				}

				if(sentLen>=256){
					cerr<<"Sentence "<<sentNumber<<" has more than 256 words. Can not handle such long sentence. Please cut it short first!\n";
					exit(-1);
				}
				
				// While there are tokens in "string"
				// Get next token: 
				thisToken = strtok( NULL, delimit);
			}
			corpusSize+=sentLen;

			sentLen = 0;
			sentNumber++;
		}
		else{
			cerr<<"Warning: sentence "<<sentNumber<< " is empty. Ignore this message if this is the last sentence.\n";			
		}
		getline(textStream1, aLine);
	}
	
	sentNumber--;
	unsigned int estimatedSize = corpusSize+3*sentNumber+1000;	//with some redundancy
	cerr<<sentNumber<<" sentences and "<<corpusSize<<" words in corpus\n";
	cerr<<"Reserve "<<estimatedSize*2<<" bytes in RAM for sorting\n";
	textStream1.close();
	

	//second pass, convert the corpus into vocIDs and create suffix array
	ifstream textStream2;
	textStream2.open(fileName);

	this->allocateMem(estimatedSize);
	this->currentPosInCorpus = 0;
	sentNumber = 1;

	getline(textStream2, aLine);
	while(!textStream2.eof()){

		if(aLine.length()>0){
			sentLen = 0;

			//add sentId
			//offset at this position will store the acutal sentence length
			corpus[this->currentPosInCorpus]=this->vocIdForSentIdPlaceHolder;
			this->suffix[this->currentPosInCorpus]=C_SuffixPointer(this->currentPosInCorpus);			
			this->currentPosInCorpus++;

			//add <s>
			sentLen++;	//not real sentence length, but to keep track of offset
			corpus[this->currentPosInCorpus]=this->vocIdForSentStart;
			this->suffix[this->currentPosInCorpus]=C_SuffixPointer(this->currentPosInCorpus);
			this->offsetList[this->currentPosInCorpus] = (unsigned char) sentLen;
			this->currentPosInCorpus++;

			thisToken = strtok((char*) aLine.c_str(), delimit );
			while( thisToken != NULL ) {			
				
				id = this->voc->returnId(C_String(thisToken));
				if(id==0){
					cerr<<"Word \""<<thisToken<<"\" is not listed in the IDVocabulary.\n";
					exit(-1);
				}

				sentLen++;

				if(id>this->maxVocIdFromCorpus){
					this->maxVocIdFromCorpus = id;
				}

				corpus[this->currentPosInCorpus]=id;
				this->suffix[this->currentPosInCorpus]=C_SuffixPointer(this->currentPosInCorpus);
				this->offsetList[this->currentPosInCorpus] = (unsigned char) sentLen;
				this->currentPosInCorpus++;				

				if(sentLen>=256){
					cerr<<"Sentence "<<sentNumber<<" has more than 256 words. Can not handle such long sentence. Please cut it short first!\n";
					exit(-1);
				}
				
				// While there are tokens in "string"
				// Get next token: 
				thisToken = strtok( NULL, delimit);
			}

			//add <sentEnd>
			corpus[this->currentPosInCorpus]=this->vocIdForSentEnd;
			this->suffix[this->currentPosInCorpus]=C_SuffixPointer(this->currentPosInCorpus);
			this->offsetList[this->currentPosInCorpus] = (unsigned char) (sentLen + 1);
			this->offsetList[this->currentPosInCorpus - sentLen - 1] = (unsigned char) (sentLen-1);	//write the sentLen to sent begin correspond to <sentId>
			this->currentPosInCorpus++;

			sentLen = 0;
			sentNumber++;
		}
		else{
			cerr<<"Warning: sentence "<<sentNumber<< " is empty. Ignore this if this is the last sentence.\n";			
		}

		aLine[0]=0;
		getline(textStream2, aLine);
	}
	textStream2.close();

	//add <endOfCorpus> to the end of data
	corpus[this->currentPosInCorpus]=this->vocIdForCorpusEnd;
	this->suffix[this->currentPosInCorpus]=C_SuffixPointer(this->currentPosInCorpus);
	this->offsetList[this->currentPosInCorpus] = (unsigned char) 0;
	this->currentPosInCorpus++;
	
	actualCorpusSize = this->currentPosInCorpus;	

	time( &ltime2 );    	
	cerr<<"\nCorpus loaded in: "<<ltime2-ltime1<<" seconds."<<endl;
	cerr<<"Total "<<sentNumber-1<<" sentences loaded.\n";

	//replace the sentId place holder to actual sentId
	time( &ltime1 );   	
	cerr<<"Inserting sentence IDs into the corpus...\n";
	IndexType sentId = this->maxVocIdFromCorpus+1;
	for(TextLenType i=0;i<actualCorpusSize;i++){
		if(corpus[i]==this->vocIdForSentIdPlaceHolder){
			corpus[i]=sentId;
			sentId++;
		}
	}
	time( &ltime2 );    	
    cerr<<"\nSentence IDs inserted in: "<<ltime2-ltime1<<" seconds."<<endl;

	//sorting
	time( &ltime1 );   	
	cerr<<"Sorting the suffix...\n";
	sort(this->suffix, this->suffix+actualCorpusSize);
	time( &ltime2 );    	
    cerr<<"\nCorpus sorted in: "<<ltime2-ltime1<<" seconds."<<endl;
	cerr<<"Done."<<endl;

}

void C_MonoCorpus::allocateMem(TextLenType corpusSize)
{
	corpus = (IndexType *) malloc(sizeof(IndexType)*corpusSize);
	
	if(corpus==0){
		cerr<<"Failed to allocate memory for corpus. Quit!\n";
		exit(-1);
	}
	
	this->suffix = (C_SuffixPointer *) malloc(sizeof(C_SuffixPointer)*corpusSize);
	if(this->suffix==0){
		cerr<<"Failed to allocate memory for suffix. Quit!\n";
		exit(-1);
	}

	this->offsetList = (unsigned char *) malloc(sizeof(unsigned char)*corpusSize);
	if(this->offsetList==0){
		cerr<<"Failed to allocate memory for offset. Quit!\n";
		exit(-1);
	}

}


void C_MonoCorpus::outputCorpus(char *filename)
{
	cerr<<"Writing corpus to file: "<<filename<<endl;
	ofstream textOutStream;
	textOutStream.open(filename, ios::binary);

	//first, write down the corpus size
	textOutStream.write((char *)&actualCorpusSize, sizeof(TextLenType));

	for(TextLenType i=0; i<actualCorpusSize;i++){
		textOutStream.write((char *)&(corpus[i]), sizeof(IndexType));		
	}
	
	textOutStream.close();
	
}

void C_MonoCorpus::outputOffset(char *filename)
{
	cerr<<"Writing offset to file: "<<filename<<endl;

	ofstream offsetOutStream;
	offsetOutStream.open(filename, ios::binary);

	//first, write down the corpus size
	offsetOutStream.write((char *)&actualCorpusSize, sizeof(TextLenType));
	
	for(TextLenType i=0; i<actualCorpusSize; i++){		
		offsetOutStream.write((char *)& (this->offsetList[i]), sizeof(unsigned char));
	}
	offsetOutStream.close();
}

void C_MonoCorpus::outputSuffix(char *filename)
{
	cerr<<"Writing suffix information to file: "<<filename<<endl;
	
	ofstream saOutStream;
	saOutStream.open(filename, ios::binary);

	//first, write down the corpus size
	saOutStream.write((char *)&actualCorpusSize, sizeof(TextLenType));

	for(TextLenType i=0;i<actualCorpusSize; i++){
		saOutStream.write((char *) & (this->suffix[i].pointer), sizeof(TextLenType));		
	}

	saOutStream.close();
}

void C_MonoCorpus::output(char *filename)
{
	char outputVocFileName[1024];
	char outputCorpusFileName[1024];
	char outputOffsetFileName[1024];
	char outputSuffixFileName[1024];
	
	
	if(this->vocNeedsToBeUpdated){
		sprintf(outputVocFileName, "%s.id_voc", filename);
		this->voc->outputToFile(outputVocFileName);		
	}

	sprintf(outputCorpusFileName, "%s.sa_corpus", filename);
	sprintf(outputOffsetFileName, "%s.sa_offset", filename);
	sprintf(outputSuffixFileName, "%s.sa_suffix", filename);
	

	this->outputCorpus(outputCorpusFileName);
	this->outputOffset(outputOffsetFileName);
	this->outputSuffix(outputSuffixFileName);
}

