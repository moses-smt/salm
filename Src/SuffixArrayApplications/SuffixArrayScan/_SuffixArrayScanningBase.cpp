/**
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/

#include "_SuffixArrayScanningBase.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

C_SuffixArrayScanningBase::C_SuffixArrayScanningBase()
{
	this->countOfCountsTable = 0;	//no memory has been allocated
	this->maxFreqConsidered = 1000;	//for freq >1000, no need to discount, MLE is good enough
}

C_SuffixArrayScanningBase::C_SuffixArrayScanningBase(const char * filename, unsigned int maxN)
{
	this->countOfCountsTable = 0;  //no memory has been allocated
	this->maxFreqConsidered = 1000; //for freq >1000, no need to discount, MLE is good enough

	//load suffix array
	this->loadData(filename, false, true, true);

	this->initializeForScanning(filename, maxN);
}

void C_SuffixArrayScanningBase::setParam_maxFreqConsidered(int maxFreqConsidered)
{
	this->maxFreqConsidered = maxFreqConsidered;
}


/**
* Initialize data structure needed for scanning after the suffix array has been loaded
**/
void C_SuffixArrayScanningBase::initializeForScanning(const char * filename, unsigned int maxN)
{
	this->maxN = maxN;
	this->nGramScanningList = (S_nGramScanningInfoElement *) malloc(sizeof(S_nGramScanningInfoElement)*this->maxN);
	this->countOfCountsTable = 0;	//no memory has been allocated

	//initialize the scanning list
	for(int i=0;i<this->maxN;i++){
		this->nGramScanningList[i].freqSoFar=0;
		this->nGramScanningList[i].vocId = 0;
		this->nGramScanningList[i].freqThreshForOutput = (unsigned int) -1;	//default, do not output
	}

	//get vocID for sentEnd
	this->vocIdForSentEnd = this->voc->returnId(C_String("_END_OF_SENTENCE_"));

	if(this->vocIdForSentEnd==0){
		cerr<<"VocID for _END_OF_SENTENCE_ can not be found. Critical error.\n";
		exit(0);
	}

	this->vocIdForSentStart = this->voc->returnId(C_String("_SENTENCE_START_"));
	if(this->vocIdForSentStart==0){
		cerr<<"VocID for _SENTENCE_START_ can not be found. Critical error.\n";
		exit(0);
	}
	
	this->vocIdForCorpusEnd = this->voc->returnId(C_String("_END_OF_CORPUS_"));
	if(this->vocIdForCorpusEnd==0){
		cerr<<"VocID for _END_OF_CORPUS_ can not be found. Critical error.\n";
		exit(0);
	}
}

C_SuffixArrayScanningBase::~C_SuffixArrayScanningBase()
{
	free(this->nGramScanningList);

	if(this->countOfCountsTable!=0){
		free(this->countOfCountsTable);
	}

}

void C_SuffixArrayScanningBase::setNgramOutputFreqThresh(int n, unsigned int freqThresh)
{
	if(n>this->maxN){
		cerr<<"Illegal operation.n="<<n<<" is greater than maxN="<<this->maxN<<endl;
		exit(0);
	}

	this->nGramScanningList[n-1].freqThreshForOutput = freqThresh;
}

void C_SuffixArrayScanningBase::scanSuffixArrayForHighFreqNgramType()
{
	this->scanSuffixArray('H');

}

/// Count of counts is the number of n-gram types that occur a certain times in the corpus.
/// Count of counts is important information in LM smoothing
/// We scan the corpus for n-gram's type/token frequency and collect information for 1-gram, 2-gram,...and up to maxFreqConsidered-gram
void C_SuffixArrayScanningBase::scanSuffixArrayForCountofCounts(int maxFreqConsidered)
{
	this->maxFreqConsidered = maxFreqConsidered;
	this->constructCountOfCountsTable();
	
	//output the count of counts
	cout<<this->maxN<<"\t"<<maxFreqConsidered<<endl;
	for(int i=0;i<this->maxN;i++){
		cout<<i+1<<endl;
		
		unsigned int * ccTableForThisN = this->countOfCountsTable + i*maxFreqConsidered;
		for(int freq=0;freq<maxFreqConsidered;freq++){
			cout<<freq+1<<"\t"<<ccTableForThisN[freq]<<endl;
		}
	}
	
}

///Check from 1-gram to maxN-gram for type-token information
///the process is similar to "scanSuffixArrayForHighFreqNgramType"
void C_SuffixArrayScanningBase::scanSuffixArrayForTypeToken()
{
	this->typeFreq = (unsigned int *) malloc(sizeof(unsigned int)*maxN);
	this->tokenFreq = (unsigned int *) malloc(sizeof(unsigned int)*maxN);

	//initialize
	for(int n=0;n<maxN;n++){
		this->typeFreq[n]=0;
		this->tokenFreq[n]=0;
	}


	//scan the suffix array
	this->scanSuffixArray('T');

	//output
	cout<<"n\tType\tToken\n";
	for(int i=0;i<this->maxN;i++){
		cout<<i+1<<"\t"<<typeFreq[i]<<"\t"<<tokenFreq[i]<<endl;
	}
}

/**
* Allocate memory for count-of-counts table and scan the corpus to fill in count of counts
* memory will be freed in the destructor
**/
void C_SuffixArrayScanningBase::constructCountOfCountsTable()
{
	if(this->countOfCountsTable!=0){	//if there is already a count of counts table
		free(this->countOfCountsTable);
	}

	this->countOfCountsTable = (unsigned int *) malloc(sizeof(unsigned int)*this->maxN*this->maxFreqConsidered);

	if(this->countOfCountsTable==NULL){
		cerr<<"Count of counts table can not be initialized. Exit\n";
		exit(0);
	}

	for(int c=0;c<this->maxN*this->maxFreqConsidered;c++){
		this->countOfCountsTable[c]=0;
	}

	this->scanSuffixArray('C');


}

/**
* Scan through the indexed corpus and according to the action type, 
* perform actions accordingly when seeing a new n-gram type
**/
void C_SuffixArrayScanningBase::scanSuffixArray(char actionType)
{
	
	int i,j;
	bool stillMeaningful = true;	
	TextLenType saPos=0;

	while(stillMeaningful && ( saPos<this->corpusSize ) ){

		TextLenType posInCorpus = this->suffix_list[saPos];
		IndexType wordInCorpus = this->corpus_list[posInCorpus];

		if(wordInCorpus<this->sentIdStart){	//SA positions pointing to sentID are not interesting
			
			if((wordInCorpus!=this->vocIdForSentStart)&&(wordInCorpus!=this->vocIdForSentEnd)&&(wordInCorpus!=this->vocIdForCorpusEnd)){	//n-grams start with <s> and </s>, or <end of corpus> are not interested
			
				bool quit =false;
				i=0;

				while(!quit && (i<this->maxN)){
					wordInCorpus = this->corpus_list[posInCorpus+i];
					if(						
						(wordInCorpus<this->sentIdStart)&&
						(wordInCorpus!=this->vocIdForSentEnd)&&
						(wordInCorpus!=this->vocIdForSentStart)&&
						(wordInCorpus==this->nGramScanningList[i].vocId)){	//still match

						this->nGramScanningList[i].freqSoFar++;
					}
					else{	//we will have new (i+1) and longer n-grams soon, before that check if we should increase the count of counts for n because of this n-gram type
									
						bool validNgramUpSoFar = true;
						unsigned int freqSoFar;
						C_String tmpPhrase; //for output high freq n-grams

						//prepare the prefix of the n-grams
						if(actionType=='H'){
							//common i-gram
							for(j=0;j<=i-1;j++){
								if(this->nGramScanningList[j].vocId==0){	//one of the word in the common i-gram is a NULL word, not a valid n-gram
									validNgramUpSoFar = false;
								}
								tmpPhrase.appending(this->voc->getText(this->nGramScanningList[j].vocId));
								tmpPhrase.appending(C_String(" "));
							}	
						}


						for(j=i;j<this->maxN;j++){				
							
							
							if(this->nGramScanningList[j].vocId==0){		//a NULL word, then this n-gram and longer ones in the scan window are invalid
								validNgramUpSoFar = false;
							}

							if(validNgramUpSoFar){		//perform actions depends on actionType
								
								switch(actionType){

								case 'C':	//count of counts
									freqSoFar = this->nGramScanningList[j].freqSoFar;
									if( (freqSoFar > 0) && ( freqSoFar <= this->maxFreqConsidered) ){
										//increase the count for (j+1)-gram with freq freqSoFar
										this->countOfCountsTable[j*this->maxFreqConsidered+freqSoFar-1]++;
									}
									break;

								case 'H':	//output high-freq n-grams
									tmpPhrase.appending(this->voc->getText(this->nGramScanningList[j].vocId));
									tmpPhrase.appending(C_String(" "));

									if(this->nGramScanningList[j].freqSoFar>=this->nGramScanningList[j].freqThreshForOutput){							
										cout<<tmpPhrase.toString()<<"\t"<<this->nGramScanningList[j].freqSoFar<<endl;
									}
									break;

								case 'T':	//type-token statistics
									if(this->nGramScanningList[j].freqSoFar>0){
										typeFreq[j]++;
									}

									tokenFreq[j]+=this->nGramScanningList[j].freqSoFar;

									break;	
								default: 
									cerr<<"Unknown action!\n";
									exit(-1);
								}
							}

							//finished output, now clear the list from point of i
							if((posInCorpus+j)<this->corpusSize){
								wordInCorpus = this->corpus_list[posInCorpus+j];
							}
							else{
								wordInCorpus = 0;	//out of bound for corpus
							}

							if((wordInCorpus==0)||(wordInCorpus>=this->sentIdStart)||(wordInCorpus==this->vocIdForSentEnd)||(wordInCorpus==this->vocIdForSentStart)){
								wordInCorpus=0;	//write 0 for <sentId>, <s> and </s>
								this->nGramScanningList[j].freqSoFar = 0;
							}
							else{
								this->nGramScanningList[j].freqSoFar = 1;
							}

							this->nGramScanningList[j].vocId = wordInCorpus;							
						}

						quit=true;	//at i+1 gram, already not match, no need to check for longer
					}

					i++;
				}
			}
		}
		else{
			stillMeaningful = false;	//once vocID is getting larger/equal than sentIdStart, everything follows it are <sentId> and no actual text
		}

		saPos++;
	}

	//at the end of corpus (according to suffix order)
	C_String finalTmpString;	//for output high-freq n-gram type
	bool validNgramUpSoFar = true;
	unsigned int freqSoFar;
	for(i=0;i<this->maxN;i++){
		if(this->nGramScanningList[i].vocId==0){	//invalide word
			validNgramUpSoFar = false;
		}

		if(validNgramUpSoFar){
			switch(actionType){
			case 'C':	//for count-of-counts
				freqSoFar = this->nGramScanningList[i].freqSoFar;
				if( (freqSoFar > 0) && ( freqSoFar <= this->maxFreqConsidered) ){
					//increase the count for (i+1)-gram with freq freqSoFar
					this->countOfCountsTable[i*this->maxFreqConsidered+freqSoFar-1]++;
				}
				break;

			case 'H':	//for high-freq n-gram types
				finalTmpString.appending(this->voc->getText(this->nGramScanningList[i].vocId));
				finalTmpString.appending(C_String(" "));
				if(this->nGramScanningList[i].freqSoFar>this->nGramScanningList[i].freqThreshForOutput){			
					cout<<finalTmpString.toString()<<"\t"<<this->nGramScanningList[i].freqSoFar<<endl;
				}
				break;

			case 'T':	//for type-token statistics
				if(this->nGramScanningList[i].freqSoFar>0){
					typeFreq[i]++;
				}

				tokenFreq[i]+=this->nGramScanningList[i].freqSoFar;
				break;

			default: 
				cerr<<"Unknown action!\n";
				exit(-1);
			}
		}
	}

}
