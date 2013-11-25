/**
* Revision $Rev: 3815 $
* Last Modified $LastChangedDate: 2007-07-06 14:31:12 -0400 (Fri, 06 Jul 2007) $
**/

#include "_SuffixArrayApplicationBase.h"

#include "malloc.h"
#include "time.h"

#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C_SuffixArrayApplicationBase::C_SuffixArrayApplicationBase()
{
	this->level1Buckets = NULL;
	this->noVocabulary = false;	//by default, still load the vocabulary
	this->noOffset = false;	//by default, load offset
	this->noLevel1Bucket = false;	//by default, construct level1 bucket
}

C_SuffixArrayApplicationBase::~C_SuffixArrayApplicationBase()
{
	if(this->level1Buckets!=NULL){
		free(this->level1Buckets);
	}

	//not necessary too
	free(this->corpus_list);
	free(this->suffix_list);

	if(! this->noOffset){
		free(this->offset_list);
	}

	if(! this->noVocabulary){
		delete(this->voc);
	}
}

/**
* Load the indexed corpus, suffix array, vocabulary, offset into memory for follow up applications
* It is optional to load vocabulary, offset depends on the argument.
* In the case when the testing data shares the same vocabulary as the training data and only vocIDs are used to represent the sentence/n-grams
* then vocabulary which maps between vocId and the word text can be skipped to save some memory.
*
* If the suffix array object does not need to locate the sentence id of an occurred n-gram, then offset information is not needed.
*
* Be very careful here, the suffix array class does not check if offset has been loaded in the search function to make it efficient
* you need to know what the suffix array class will be used (whether offset is needed) and load it properly
* @param fileNameStem The filename of the corpus. This should be the same filename used in IndexSA
* @param noVoc If set to be 'true', vocabulary will not be loaded
* @param noOffset If set to be 'true', the offset information will not be loaded. <sentId, offsetInSent> information for an n-gram's occurrences can not be calculated.
* @param noLevel1Bucket Level1Bucket is used to speed up the search at the cost of additional memory. For applications which do not need to locate n-grams in the corpus (such as the corpus scanning application), then there is no need to create Level1Bucket
**/
void C_SuffixArrayApplicationBase::loadData(const char *fileNameStem, bool noVoc, bool noOffset, bool noLevel1Bucket)
{
	long ltime1, ltime2;

	this->noVocabulary = noVoc;
	this->noOffset = noOffset;
	this->noLevel1Bucket = noLevel1Bucket;
	
	
	char tmpString[1000];

	//the order of loading the data is important, do not change
	if(! this->noVocabulary){
		time( &ltime1 );
		cerr<<"Loading Vocabulary...\n";
		sprintf(tmpString,"%s.id_voc",fileNameStem);
		this->loadVoc(tmpString);
		time( &ltime2);
		cerr<<"Vocabulary loaded in "<<ltime2-ltime1<<" seconds.\n";
	}
	
	time( &ltime1 );
	cerr<<"Loading corpus...\n";
	sprintf(tmpString,"%s.sa_corpus",fileNameStem);	
	this->loadCorpusAndInitMem(tmpString);
	time( &ltime2);
	cerr<<"Corpus loaded in "<<ltime2-ltime1<<" seconds.\n";
	
	time( &ltime1 );
	cerr<<"Loading suffix...\n";
	sprintf(tmpString,"%s.sa_suffix",fileNameStem);
	this->loadSuffix(tmpString);
	time( &ltime2);
	cerr<<"Suffix loaded in "<<ltime2-ltime1<<" seconds.\n";

	if(! this->noOffset){
		time( &ltime1 );
		cerr<<"Loading offset...\n";
		sprintf(tmpString,"%s.sa_offset",fileNameStem);
		this->loadOffset(tmpString);
		time( &ltime2);
		cerr<<"Offset loaded in "<<ltime2-ltime1<<" seconds.\n";
	}
}

void C_SuffixArrayApplicationBase::loadVoc(const char *filename)
{
	this->voc =  new C_IDVocabulary(filename);
}

void C_SuffixArrayApplicationBase::loadCorpusAndInitMem(const char *filename)
{
	unsigned int dwRead = 0;
	FILE *  CorpusInputFile = fopen(filename, "rb");

	if(!CorpusInputFile){
		cerr<<"Corpus file: "<<filename<<" does not exist or can not be opened!\n";
		exit(0);
	}
	
	//first, read the size of the corpus
	dwRead = fread( &(this->corpusSize), sizeof(TextLenType), 1, CorpusInputFile);
	
	//allocate memory for all data structure
	this->corpus_list = (IndexType *) malloc(sizeof(IndexType)*this->corpusSize);
	if(! this->corpus_list){
		cerr<<"Can not allocate memory to load the corpus!\n";
		exit(0);
	}

	this->suffix_list = (TextLenType *) malloc(sizeof(TextLenType)*this->corpusSize);
	if(! this->suffix_list){
		cerr<<"Can not allocate memory to load the suffix!\n";
		exit(0);
	}

	if(! this->noOffset){
		this->offset_list = (unsigned char *) malloc(sizeof(unsigned char)*this->corpusSize);
		if(! this->offset_list){
			cerr<<"Can not allocate memory to load the offset!\n";
			exit(0);
		}
	}

	//read the corpus file
	unsigned int totalRead = 0;
	unsigned int remaining = this->corpusSize;
	unsigned int oneBatchReadSize;
	char * currentPosInCorpusList = (char *) this->corpus_list;
	while(! feof(CorpusInputFile) && (totalRead<this->corpusSize)){
		oneBatchReadSize = SIZE_ONE_READ;
		if(remaining<SIZE_ONE_READ){
			oneBatchReadSize = remaining;
		}

		dwRead = fread( currentPosInCorpusList, sizeof(IndexType), oneBatchReadSize, CorpusInputFile);
				
		totalRead+=dwRead;
		remaining-=dwRead;

		currentPosInCorpusList+=sizeof(IndexType)*dwRead;
	}
	if(totalRead!=this->corpusSize){
		cerr<<"Expecting "<<this->corpusSize<<" words from the corpus, read-in "<<totalRead<<endl;
		exit(0);
	}
	fclose(CorpusInputFile);

	this->sentIdStart = this->corpus_list[0];
	this->vocIdForSentStart = this->corpus_list[1];
	this->vocIdForCorpusEnd = this->corpus_list[this->corpusSize-1];
	this->vocIdForSentEnd = this->corpus_list[this->corpusSize-2];

	if(! this->noLevel1Bucket){
		//in this corpus, we will have at most sentIdStart-1 word types
		//the index in the array correspond to the vocId, 0 is for <unk> and the last one is for <sentIdStart-1> which is the largest vocId observed in the data
		this->level1Buckets = (S_level1BucketElement *) malloc(sizeof(S_level1BucketElement)* this->sentIdStart);	
		
		//initialize the level1 buckets
		for(IndexType i=0;i<this->sentIdStart;i++){
			this->level1Buckets[i].first = (TextLenType) -1;
			this->level1Buckets[i].last = 0;
		}
	}
}

void C_SuffixArrayApplicationBase::loadSuffix(const char *filename)
{
	unsigned int dwRead = 0;
	FILE *  SuffixInputFile = fopen(filename, "rb");
	if(!SuffixInputFile){
		cerr<<"Suffix file: "<<filename<<" does not exist!"<<endl;
		exit(0);
	}

	//first, read in the size of the suffix array
	TextLenType suffixArraySize;
	dwRead = fread( &suffixArraySize, sizeof(TextLenType), 1, SuffixInputFile);
	
	if(suffixArraySize!=this->corpusSize){
		cerr<<"Something wrong, the suffix array size is different from the corpus size.\n";
		cerr<<"Corpus has "<<this->corpusSize<<" words and suffix array reported: "<<suffixArraySize<<endl;
		exit(0);
	}

	//read all the suffix into memory
	unsigned int totalRead = 0;
	unsigned int remaining = suffixArraySize;
	unsigned int oneBatchReadSize;
	char * currentPosInSuffixList = (char *) this->suffix_list;
	while(! feof(SuffixInputFile) && (totalRead<suffixArraySize)){
		oneBatchReadSize = SIZE_ONE_READ;
		if(remaining<SIZE_ONE_READ){
			oneBatchReadSize = remaining;
		}

		dwRead = fread( currentPosInSuffixList, sizeof(TextLenType), oneBatchReadSize, SuffixInputFile);
		
		totalRead+=dwRead;
		remaining -= dwRead;

		currentPosInSuffixList+=sizeof(TextLenType)*dwRead;
	}	
	if(totalRead!=suffixArraySize){
		cerr<<"Expecting "<<suffixArraySize<<" words from the suffix list, read-in "<<totalRead<<endl;
		exit(0);
	}

	fclose(SuffixInputFile);

	if(! this->noLevel1Bucket){
		//build level-1 bucket
		cerr<<"Initialize level-1 buckets...\n";
		IndexType currentVocId = 0;
		IndexType vocId;
		TextLenType pos;
		TextLenType lastSaIndex = 0;
		
		for(TextLenType i=0; i<suffixArraySize; i++){
			pos = this->suffix_list[i];
			
			//for level1 bucket
			vocId = this->corpus_list[pos];

			if(vocId<this->sentIdStart){	//is a meaningful word type
				if(vocId!=currentVocId){
					this->level1Buckets[currentVocId].last = lastSaIndex;	//for first word which is <unk> this does not matter
					this->level1Buckets[vocId].first = i;
					
					currentVocId=vocId;				
				}

				lastSaIndex = i;
			}	
		}

		//for the last word type
		this->level1Buckets[currentVocId].last = lastSaIndex;
	}
	else{
		this->level1Buckets = NULL;
	}
}

void C_SuffixArrayApplicationBase::loadOffset(const char *filename)
{
	unsigned int dwRead = 0;
	FILE *  OffsetInputFile = fopen(filename, "rb");
	
	if(!OffsetInputFile){
		cerr<<"Offset file: "<<filename<<" does not exist!"<<endl;
		exit(0);
	}
		
	//first, read the size of the corpus	
	TextLenType offsetListLen;
	dwRead = fread( &offsetListLen, sizeof(TextLenType), 1, OffsetInputFile);	
	if(offsetListLen!=this->corpusSize){
		cerr<<"Text length is inconsistent with the length of the offset.\n";
		exit(0);
	}

	//read all the suffix into memory
	unsigned int totalRead = 0;
	unsigned int remaining = offsetListLen;
	unsigned int oneBatchReadSize;
	char * currentOffsetListPos = (char *) this->offset_list;
	while(! feof(OffsetInputFile) && (totalRead < offsetListLen)){
		oneBatchReadSize = SIZE_ONE_READ;

		if(remaining<SIZE_ONE_READ){
			oneBatchReadSize = remaining;
		}

		dwRead = fread( currentOffsetListPos, sizeof(unsigned char), oneBatchReadSize, OffsetInputFile);
		
		totalRead+=dwRead;
		remaining-=dwRead;

		currentOffsetListPos+=sizeof(unsigned char)*dwRead;

	}
	if(totalRead!=offsetListLen){
		cerr<<"Expecting "<<offsetListLen<<" words from the offset list, read-in "<<totalRead<<endl;
		exit(0);
	}
	fclose(OffsetInputFile);
	
}

TextLenType C_SuffixArrayApplicationBase::returnCorpusSize()
{
    return this->corpusSize;
}
