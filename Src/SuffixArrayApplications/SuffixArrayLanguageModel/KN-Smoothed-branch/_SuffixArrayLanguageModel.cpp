/**
* Revision $Rev: 3665 $
* $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/

#include  "_SuffixArrayLanguageModel.h"
#include <iostream>
#include <fstream>
#include <set>

#include "math.h"

using namespace std;

C_SuffixArrayLanguageModel::C_SuffixArrayLanguageModel()
{

}

C_SuffixArrayLanguageModel::~C_SuffixArrayLanguageModel()
{
	
}


/**
* Construct the suffix array language model object
* Take the configuration filename as the parameter for the constructor
* 
* The configuration file is of the following format for each line:
* 
* Keyword<tab>value
* <p>
* Note: keywords are all case sensitive.
* <ul>
*	<li> <b>CORPUS</b> filename of the corpus for LM training. It should be the same as used in IndexSA
*	<li> <b>N</b> Highest order of n considered for n-gram LM. Default value = <i>5</i>
*	<li> <b>SMOOTHING_STRATEGY</b> Smoothing strategy.
*		<ul>
*			<li> <i>k</i> : default value. Modified Kneser-Ney Smoothing @see <a href=http://acl.ldc.upenn.edu/P/P96/P96-1041.pdf> An Empirical Study of Smoothing Techniques for Language Modeling </a> 
*			<li> <i>g</i> : Good-Turing discounting @see <a href=http://l2r.cs.uiuc.edu/~danr/Teaching/CS598-05/Papers/Gale-Sampson-smoothgoodturing.pdf> Good Turing without Tears</a>
*		</ul>
*	<li> <b>INTERPOLATION_STRATEGY</b> : Interpolation strategy
*		<ul>
*			<li> <i>e</i> : Probability of the next word predicted by histories of different orders are equally interpolated
*			<li> <i>m</i> : Use the maximum conditional probability from all different order of history as the probability for the next word
*			<li> <i>i</i> : Use deleted interpolation based on heuristics developed by IBM
*		</ul>
*	<li> <b>MAX_FREQ_DISC</b>: <br>
*						<i>default</i>=50<br>
*							If the frequency of an n-gram is lower than this value and SMOOTHING is set, discounting will be applied. <br>
*							If this value is set to 0 or negative values, smoothing/discounting will not be used. <br>
*	<li> <b>PURGE_CACHE</b>: Check entries in the cache after "PURGE_CACHE" number of sentences have been processed. Default = 100.
*	<li> <b>FRESH_TIME</b>: Entries in the cache that are not used since "current time - FRESH_TIME" will be purged from the cache. Mesured in seconds of wall clock time.
** </ul>
* @param Configuration File Name 
* @param corpusFileNameStem The training corpus filename used by IndexSA.
**/
C_SuffixArrayLanguageModel::C_SuffixArrayLanguageModel(const char * cfgFileName)
{

	fstream cfgFile;
	cfgFile.open(cfgFileName,ios::in);

	if(!cfgFile){
		fprintf(stderr,"Configuration file %s does not exist! quit!!\n", cfgFileName);
		exit(-1);
	}

	//-----------------------------------------------------------------------------
	//reading parameters
	char paraName[1024];
	char corpusFileNameStem[1024];

	corpusFileNameStem[0]='\0';

	//default values for member variables
	this->interpolationStrategy = 'e';		//default interpolation strategy: equally weighted n-gram conditional prob
	this->smoothingStrategy = 'k';			//default smoothing strategy: modified Kneser-Ney smoothing
	this->maxFreqForDiscounting = 50;		//default, freq that is lower than this value will not be applied with discounting
	this->maxN= 5;         // default value; consider up to 5 words
	
	this->numberOfSentSeenToPurgeCache = 100;	//default value, purge cache after processing 100 sentences
	this->freshTime = 50;				//entries in the cache that are older than 50 seconds are subject to purging
	this->sentenceProcessedSoFar = 0;
	this->typeOfBigrams = 0;
	
	while(!cfgFile.eof()){
		cfgFile>>paraName;

		if(strcmp(paraName,"CORPUS")==0){
			cfgFile>>corpusFileNameStem;
		}
		else if(strcmp(paraName, "SMOOTHING_STRATEGY")==0){
			cfgFile>>this->smoothingStrategy;
		}
		else if(strcmp(paraName,"N")==0){
			cfgFile>>this->maxN;
		}
		else if(strcmp(paraName,"MAX_FREQ_DISC")==0){
			cfgFile>>this->maxFreqForDiscounting;
		}
		else if(strcmp(paraName,"INTERPOLATION_STRATEGY")==0){
			cfgFile>>this->interpolationStrategy;
		}
		else if(strcmp(paraName,"FRESH_TIME")==0){
			cfgFile>>this->freshTime;
		}
		else if(strcmp(paraName, "PURGE_CACHE")==0){
			cfgFile>>this->numberOfSentSeenToPurgeCache;
		}

		paraName[0]=0;
		
	}


	if(strlen(corpusFileNameStem)==0){
		cerr<<"CORPUS not specified in the configuration file! Quit!"<<endl;
		exit(-1);
	}


	this->loadData_forSearch(corpusFileNameStem, false, true);	//call the constructor of the super class
					   //corpusName, with vocabulary, no offset, 

	
	this->nGramScanningList = (S_nGramScanningInfoElement *) malloc(sizeof(S_nGramScanningInfoElement)*this->maxN);

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

	if(this->maxFreqForDiscounting<=0){
		this->applyDiscounting = false;
	}
	else{
		if(this->maxFreqForDiscounting<3){
			cerr<<"MAX_FREQ_DISC has to be at least 3!"<<endl;
			exit(-1);
		}

		this->applyDiscounting = true;		
		this->constructDiscountingMap();	//scan the corpus and construct the count of counts table and then discounting map		
	}

}

/**
* Set strategy to interploate the conditional probabilities of next word given different order of histories
* 'e' for equal weighted interpolation of unigram, bigram, trigram... probabiblities
* 'm' for using the maximum probabilty from all histories and use this value as P(next word | history)
* 'i' for deleted interpolation with weights determined by a heuristic that favors long n-gram probability when the frequency is reliable
**/
void C_SuffixArrayLanguageModel::setParam_interpolationStrategy(char interpolationStrategy)
{
	this->interpolationStrategy = interpolationStrategy;
}

/**
* Set the value for parameter :numberOfSentSeenToPurgeCache
* LM will purge the entries in the cache that have not been used in 'freshTime'
**/
void C_SuffixArrayLanguageModel::setParam_numberOfSentSeenToPurgeCache(int numberOfSentSeenToPurgeCache)
{
	this->numberOfSentSeenToPurgeCache = numberOfSentSeenToPurgeCache;
}

/**
* Set the value for parameter: freshTime
* LM will purge the entries in the cache that have not been used in 'freshTime'
**/
void C_SuffixArrayLanguageModel::setParam_freshTime(long freshTime)
{
	this->freshTime = freshTime;
}

/**
* Similar to the function in C_SuffixArrayScanningBase
* Scan the corpus to obtain count of counts information 
* and construct the discounting using Good-Turing smoothing
* Also, estimate the Y, D1, D2, D3+ values as needed for the modified Kneser-Ney smoothing
**/
void C_SuffixArrayLanguageModel::constructDiscountingMap()
{
	unsigned int * countOfCountsTable = (unsigned int *) malloc(sizeof(unsigned int)*this->maxN*this->maxFreqForDiscounting);
	this->typeOfBigrams = 0;

	if(countOfCountsTable==NULL){
		cerr<<"Count of counts table can not be initialized. Exit\n";
		exit(0);
	}

	for(int c=0;c<this->maxN*this->maxFreqForDiscounting;c++){
		countOfCountsTable[c]=0;
	}


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

						
						
						for(j=i;j<this->maxN;j++){				
							
							
							if(this->nGramScanningList[j].vocId==0){		//a NULL word, then this n-gram and longer ones in the scan window are invalid
								validNgramUpSoFar = false;
							}

							if(validNgramUpSoFar){		//perform actions depends on actionType
								
								if(j==1){	//a new bigram type, this information is important for KN-smoothing
									this->typeOfBigrams++;
								}


								freqSoFar = this->nGramScanningList[j].freqSoFar;
								if( (freqSoFar > 0) && ( freqSoFar <= this->maxFreqForDiscounting) ){
									//increase the count for (j+1)-gram with freq freqSoFar
									countOfCountsTable[j*this->maxFreqForDiscounting+freqSoFar-1]++;
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
	bool validNgramUpSoFar = true;
	unsigned int freqSoFar;
	for(i=0;i<this->maxN;i++){
		if(this->nGramScanningList[i].vocId==0){	//invalide word
			validNgramUpSoFar = false;
		}

		if(validNgramUpSoFar){

			if(i==1){
				this->typeOfBigrams++;
			}
			
			freqSoFar = this->nGramScanningList[i].freqSoFar;
			if( (freqSoFar > 0) && ( freqSoFar <= this->maxFreqForDiscounting) ){
				//increase the count for (i+1)-gram with freq freqSoFar
				countOfCountsTable[i*this->maxFreqForDiscounting+freqSoFar-1]++;
			}			
		}
	}

	//now, use Good-Turing discounting to create frequency mapping
	//still assign N*Freq table for simplicity, even though that for each N, only maxFreq-1 freq type will be discounted
	this->discountingMap = (double *) malloc(sizeof(double) * this->maxN * this->maxFreqForDiscounting);
		
	for(i=0;i<this->maxN;i++){
		//for (i+1)-gram
			
		unsigned int * ccTableForThisN = countOfCountsTable + i*this->maxFreqForDiscounting;
		double * discountingMapForThisN = this->discountingMap + i*this->maxFreqForDiscounting;

		for(int freq=0;freq<(this->maxFreqForDiscounting-1);freq++){	//only goes to maxFreq-1, because we can not discount maxFreq
			//for all (freq+1) ngrams
			if((ccTableForThisN[freq]>0)&&(ccTableForThisN[freq+1]>0)){	//both freq exists
				discountingMapForThisN[freq] = (double)(ccTableForThisN[freq+1]*(freq+2))/(double)(ccTableForThisN[freq]);			
			}
			else{
				discountingMapForThisN[freq] = -1;
			}
		}

		discountingMapForThisN[this->maxFreqForDiscounting-1] = -1;	//won't be used, just for consistency
	}


	//estimate the Y, D1, D2 and D3+ values for each order of n.
	//these values will be used for KN-smoothing to estimate the gamma, the discounting factor
	this->Y = (double *) malloc(sizeof(double) * this->maxN);
	this->D1 = (double *) malloc(sizeof(double) * this->maxN);
	this->D2 = (double *) malloc(sizeof(double) * this->maxN);
	this->D3plus = (double *) malloc(sizeof(double) * this->maxN);

	for(i=0;i<this->maxN;i++){
		unsigned int * ccTableForThisN = countOfCountsTable + i*this->maxFreqForDiscounting;
		double n1 = ccTableForThisN[0];	//number of n-gram types that have freq equals 1
		double n2 = ccTableForThisN[1];	//number of n-gram types that have freq equals 2;
		double n3 = ccTableForThisN[2];	//number of n-gram types that have freq equals 3;
		double n4 = ccTableForThisN[3];	//number of n-gram types that have freq equals 4;

		this->Y[i] = n1/(n1+2*n2);		//for (i+1)-gram
		this->D1[i] = 1-2*Y[i]*n2/n1;
		this->D2[i] = 2-3*Y[i]*n3/n2;
		this->D3plus[i] = 3 - 4*Y[i]*n4/n3;
	}

	free(countOfCountsTable);
}

///if currently matched an n-gram at corpus position [currentMatchStart, currentMatchStart+currentMatchLen-1]
///get the freq for [currentMatchStart, currentMatchStart+currentMatchLen-1] + nextWord
///only need to get freq(w_n | history) of different history
///return in freq table, freq(history+Wn, history) for all the matched n
///freq: 1-gram Freq, corpusSize, 2-gram freq, freq of 2-gram history
///	    3-gram freq, freq of 3-gram history
///freqTable should have length of 2*n
///return the longest match with this updated n-gram
void C_SuffixArrayLanguageModel::calcNgramMatchingInfoTokenFreqOnlyExtendingCurrentMatch(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, double *freqTable, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen)
{
	vector<IndexType> nGram;	

	if(currentMatchStart!=(TextLenType) -1){	//-1 will be <unk>
		if(currentMatchLen==this->maxN){	//we consider only up to this->maxN for the extended n-gram
			currentMatchStart++;
			currentMatchLen--;
		}

		for(TextLenType pos=currentMatchStart; pos<(currentMatchStart+currentMatchLen); pos++){
			nGram.push_back(this->corpus_list[pos]);
		}
	}

	nGram.push_back(nextWord);

	int sentLen = nGram.size();
	
	//construct the n-gram search table	
	S_sentSearchTableElement * table = this->constructNgramSearchTable4SentWithLCP(nGram);

	int startPosForNgram;
	int startPosForLongestMatchingWithNextWord;
	int cellIndexForLongestMatchingWithNextWord;

	bool stillMatched = true;
	bool atLeastOneMatched = false;

	int indexForNgram;

	unsigned int totalOccurrences;
	unsigned int totalOccurrencesOfHistory;

	//for unigram
	indexForNgram = sentLen - 1;
	if(table[indexForNgram].found){
		totalOccurrences = table[indexForNgram].endingPosInSA - table[indexForNgram].startPosInSA + 1;
		if(this->smoothingStrategy=='g'){	//if use Good-Turing for discounting
			freqTable[0] = this->discountFreq_GT(1, totalOccurrences);
		}
		else{
			freqTable[0] = totalOccurrences; 
		}

		freqTable[1] = this->corpusSize;
		cellIndexForLongestMatchingWithNextWord = indexForNgram;
		startPosForLongestMatchingWithNextWord = sentLen-1;
		atLeastOneMatched = true;
	}
	else{
		stillMatched = false;
	}

	int n=2;	//considering 2-gram and longer n-gram now
	startPosForNgram = sentLen - 2;
	while((stillMatched)&&(startPosForNgram>=0)){
		
		indexForNgram = (n-1) * sentLen + startPosForNgram;
		int indexForHistory = (n-2) * sentLen +  startPosForNgram;
		
		if(table[indexForNgram].found){
						
			totalOccurrences = table[indexForNgram].endingPosInSA - table[indexForNgram].startPosInSA + 1;	
			totalOccurrencesOfHistory = table[indexForHistory].endingPosInSA - table[indexForHistory].startPosInSA + 1;

			
			if(this->applyDiscounting){
				freqTable[2*n-2] = this->discountFreq_GT(n, totalOccurrences);				
			}
			else{
				freqTable[2*n-2] = (double)totalOccurrences;
			}

			freqTable[2*n-1] = (double) totalOccurrencesOfHistory;	//do not discount the history
			
			if(n<this->maxN){	//new history is at most this->maxFreqForDiscounting-1 words long
				cellIndexForLongestMatchingWithNextWord = indexForNgram;
				startPosForLongestMatchingWithNextWord = startPosForNgram;
			}
		}
		else{
			stillMatched = false;
		}

		startPosForNgram--;
		n++;
	}

	if(atLeastOneMatched){	//at least one n-gram can be matched with 'nextWord'
		updatedMatchingStart = this->suffix_list[table[cellIndexForLongestMatchingWithNextWord].startPosInSA];
		updatedMatchingLen = (unsigned char) (sentLen - startPosForLongestMatchingWithNextWord);
	}
	else{
		updatedMatchingStart = (TextLenType) -1;
		updatedMatchingLen = 0;
	}

	free(table);

}


void C_SuffixArrayLanguageModel::calcNgramMatchingInfoTokenFreqContextTypeExtendingCurrentMatch(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, double *freqTable, S_ContextTypeInfo * contextTypeInfo, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen)
{
	vector<IndexType> nGram;	

	if(currentMatchStart!=(TextLenType) -1){	//-1 will be <unk>
		if(currentMatchLen==this->maxN){	//we consider only up to this->maxN for the extended n-gram
			currentMatchStart++;
			currentMatchLen--;
		}

		for(TextLenType pos=currentMatchStart; pos<(currentMatchStart+currentMatchLen); pos++){
			nGram.push_back(this->corpus_list[pos]);
		}
	}

	nGram.push_back(nextWord);

	int sentLen = nGram.size();
	
	//construct the n-gram search table	
	S_sentSearchTableElement * table = this->constructNgramSearchTable4SentWithLCP(nGram);

	int startPosForNgram;
	int startPosForLongestMatchingWithNextWord;
	int cellIndexForLongestMatchingWithNextWord;

	bool stillMatched = true;
	bool atLeastOneMatched = false;

	int indexForNgram;

	unsigned int totalOccurrences;
	unsigned int totalOccurrencesOfHistory;

	//for unigram
	indexForNgram = sentLen - 1;
	if(table[indexForNgram].found){
		totalOccurrences = table[indexForNgram].endingPosInSA - table[indexForNgram].startPosInSA + 1;
		
		freqTable[0] = totalOccurrences; 
		freqTable[1] = this->corpusSize;

		cellIndexForLongestMatchingWithNextWord = indexForNgram;
		startPosForLongestMatchingWithNextWord = sentLen-1;
		atLeastOneMatched = true;
	}
	else{
		stillMatched = false;
	}

	int n=2;	//considering 2-gram and longer n-gram now for token freq
	startPosForNgram = sentLen - n;
	while((stillMatched)&&(startPosForNgram>=0)){
		
		indexForNgram = (n-1) * sentLen + startPosForNgram;
		int indexForHistory = (n-2) * sentLen +  startPosForNgram;
		
		if(table[indexForNgram].found){
						
			totalOccurrences = table[indexForNgram].endingPosInSA - table[indexForNgram].startPosInSA + 1;	
			totalOccurrencesOfHistory = table[indexForHistory].endingPosInSA - table[indexForHistory].startPosInSA + 1;


			freqTable[2*n-2] = (double)totalOccurrences;			
			freqTable[2*n-1] = (double) totalOccurrencesOfHistory;	//do not discount the history
			
			if(n<this->maxN){	//new history is at most this->maxFreqForDiscounting-1 words long
				cellIndexForLongestMatchingWithNextWord = indexForNgram;
				startPosForLongestMatchingWithNextWord = startPosForNgram;
			}
		}
		else{
			stillMatched = false;
		}

		startPosForNgram--;
		n++;
	}

	if(atLeastOneMatched){	//at least one n-gram can be matched with 'nextWord'
		updatedMatchingStart = this->suffix_list[table[cellIndexForLongestMatchingWithNextWord].startPosInSA];
		updatedMatchingLen = (unsigned char) (sentLen - startPosForLongestMatchingWithNextWord);
	}
	else{
		updatedMatchingStart = (TextLenType) -1;
		updatedMatchingLen = 0;
	}


	//estimate the context type information which will be used for KN-smoothing
	for(n=2;n<=sentLen;n++){
		startPosForNgram = sentLen - n;
		TextLenType w_in2_i1_startPos_in_SA = 0;
		TextLenType w_in2_i1_endPos_in_SA = 0;

		if(n>2){
		  int indexForW_in2_i1 = (n-3) * sentLen + startPosForNgram + 1;  //the location information for w_{i-n+2}^{i-1} of length n-2
		  w_in2_i1_startPos_in_SA = table[indexForW_in2_i1].startPosInSA;
		  w_in2_i1_endPos_in_SA = table[indexForW_in2_i1].endingPosInSA;
		}

		int indexForW_in1_i1 = (n-2) * sentLen + startPosForNgram; //the location information of w_{i-n+1}^{i-1} of length n-1

		this->scanCorpusForContextTypeInfo(n, nextWord,
						   w_in2_i1_startPos_in_SA, w_in2_i1_endPos_in_SA,
					table[indexForW_in1_i1].startPosInSA, table[indexForW_in1_i1].endingPosInSA,
					contextTypeInfo[n-1]);
	}

	free(table);


}

///given observedFreq of n-gram, return discounted freq using Good-Turing smoothing
double C_SuffixArrayLanguageModel::discountFreq_GT(int n, unsigned int observedFreq)
{
	if(n>=this->maxN){	//do not discount
		return (double) observedFreq;
	}

	if(observedFreq>=(this->maxFreqForDiscounting-1)){	//no discounting for high freq
		return (double) observedFreq;
	}

	//else, check the discount map
	double discountedFreq = this->discountingMap[ (n-1) * this->maxFreqForDiscounting + observedFreq -1];

	if(discountedFreq>0){
		return discountedFreq;
	}

	//else, no discounting
	return (double) observedFreq;
}


///Start a new sentence now, clear up the sentence LM state
///Increase the count of 'sentenceProcessedSoFar'
///If LM has processed 'numberOfSentSeenToPurgeCache' sentences
///it is time to check if old entries in the cache should be cleaned
LMState C_SuffixArrayLanguageModel::beginOfSentenceState()
{
	long currentTime;
	time(&currentTime);
	
	this->resetLmStates();
	this->initialLmState();	

	this->sentenceProcessedSoFar++;

	if(this->sentenceProcessedSoFar==this->numberOfSentSeenToPurgeCache){
		//purge the cache
		this->purgeCache(currentTime-this->freshTime);
		
		this->sentenceProcessedSoFar = 0;
	}

	return 0;
}

void C_SuffixArrayLanguageModel::initialLmState()
{
	//add sentence start
	S_LMStateInfo sentStartNode;
	sentStartNode.posInCorpus = 1;	//if corpus is indexed correctly position 1 should be <s>
	sentStartNode.len = 1;

	this->allLMStates.push_back(sentStartNode);
	this->lmStateInfo2Id.insert(make_pair(sentStartNode, 0));
}

void C_SuffixArrayLanguageModel::resetLmStates()
{
	this->buffer.clear();
	this->allLMStates.clear();
	this->lmStateInfo2Id.clear();
}

/** 
* Purge entries in the cache that are not visited after "lastVisitedTime"
* @param lastVisitedTime Entries in the cache that are older than 'lastVisitedTime' parameter will be purged
**/
void C_SuffixArrayLanguageModel::purgeCache(long lastVisitedTime)
{
	//cerr<<this->cached_sa_access.size()<<" entries in cache, purged to ";
	
	map<S_CachedSA_Access_Key, S_Cached_SA_Access_Info, lt_s_cached_SA_access_key>::iterator iter1,iter2;

	iter1 = this->cached_sa_access.begin();

	while(iter1!=this->cached_sa_access.end()){
		iter2=iter1;
		iter2++;
		
		if(iter1->second.lastTimedUsed<lastVisitedTime){
			this->cached_sa_access.erase(iter1);
		}

		iter1=iter2;
	}
	//cerr<<this->cached_sa_access.size()<<" entries"<<endl;
}

/**
* Given the current history (as represented by the 'lmState'
* caculate the log prob of nextWord given this history P(nextword|history)
* and return the updated language model state with next word appended to the history
* @param lmState Current language model state
* @param nextWord The vocId of the next word (the word to be predicted)
* @param &nextState Returning the updated language model state when the next word is appended
**/
double C_SuffixArrayLanguageModel::logProb(LMState lmState, IndexType nextWord, LMState & nextState)
{

	//first check if we have already seen this before
	map< pair<LMState, IndexType>, S_BufferedLmInfo>::iterator iterBuffer;
	iterBuffer = this->buffer.find( make_pair( lmState, nextWord) );

	if(iterBuffer==this->buffer.end()){ //we haven't seen this lmState+word yet
		//search for it in the corpus
		S_LMStateInfo lmStateInfo = this->allLMStates[lmState];
		TextLenType updatedMatchingStart;
		unsigned char updatedMatchingLen;
		
		double logProb = this->logProbOfNgramFromCorpusInfo(lmStateInfo.posInCorpus, lmStateInfo.len, nextWord, updatedMatchingStart, updatedMatchingLen);
		

		S_LMStateInfo updatedLmStateInfo;
		updatedLmStateInfo.posInCorpus = updatedMatchingStart;
		updatedLmStateInfo.len = updatedMatchingLen;

		int updatedLmStateId;
		map<S_LMStateInfo, int, lt_lmStateInfo>::iterator iterLmStateInfo2Id;
		iterLmStateInfo2Id = this->lmStateInfo2Id.find(updatedLmStateInfo);
		if(iterLmStateInfo2Id==this->lmStateInfo2Id.end()){	//this updated lm state does not exist yet
			this->allLMStates.push_back(updatedLmStateInfo);
			updatedLmStateId = this->allLMStates.size()-1;
			this->lmStateInfo2Id.insert(make_pair(updatedLmStateInfo, updatedLmStateId));
		}
		else{
			updatedLmStateId = iterLmStateInfo2Id->second;
		}

		//buffer this
		S_BufferedLmInfo bufferedLmInfo;
		bufferedLmInfo.logProb = logProb;
		bufferedLmInfo.nextState = updatedLmStateId;

		this->buffer.insert(make_pair( make_pair(lmState, nextWord), bufferedLmInfo));

		//updated next state
		nextState = updatedLmStateId;
		
		return logProb;
	}

	nextState = iterBuffer->second.nextState;

	return iterBuffer->second.logProb;
}


/**
* Given the history as lmState and append a phrase as a vector of IndexType,
* calculate the LM prob and update the lm state
* @param lmState Current language model state
* @param phrase A vector of vocIds of the next phrase (the phrase to be predicted)
* @param &nextState Returning the updated language model state when the next word is appended
**/
double C_SuffixArrayLanguageModel::logProb(LMState lmState, vector<IndexType> phrase, LMState & nextState)
{
	double logProb = 0;
	for(int i=0;i<phrase.size();i++){
		logProb+=this->logProb(lmState, phrase[i], nextState);
		lmState = nextState;
	}

	return logProb;
}

/**
* At the end of a sentence, call logProbEnd() to extend the lmState with the sentence end symbol </s>
**/
double C_SuffixArrayLanguageModel::logProbEnd(LMState lmState)
{
	LMState dummyNextState;
	return this->logProb(lmState, this->vocIdForSentEnd, dummyNextState);
}

/**
* Extend the current matched n-gram with next word, calculate the prob and update the updated range
* the n-gram is represented by its position in the suffix array and the length
* @param currentMatchStart Starting position of the current matched n-gram in corpus
* @param currentMatchLen Length of the matched n-gram \
* @param nextWord Vocabulary ID of the next word (the word to be predicted)
* @param &updatedMatchingStart If the extended n-gram (the current matched n-gram extended with the 'nextword') exists in the corpus, return its starting position in the corpus
* @param &updatedMatchingLen The length of the extended n-gram
**/
double C_SuffixArrayLanguageModel::logProbOfNgramFromCorpusInfo(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen)
{
	long currentTime;
	time(&currentTime);

	double logProb;

	//first check if information is already in cache
	S_CachedSA_Access_Key accessKey;
	accessKey.currentMatchStart = currentMatchStart;
	accessKey.currentMatchLen = currentMatchLen;
	accessKey.nextWord = nextWord;

	map<S_CachedSA_Access_Key, S_Cached_SA_Access_Info, lt_s_cached_SA_access_key>::iterator iter_cached_sa_access;

	iter_cached_sa_access = this->cached_sa_access.find(accessKey);

	if(iter_cached_sa_access==this->cached_sa_access.end()){	//information not in cache yet
		double * freqTable = (double *) malloc(sizeof(double)*2*(this->maxN));
		memset(freqTable, 0, 2*this->maxN*sizeof(double));

		S_ContextTypeInfo * contextTypeInfo = (S_ContextTypeInfo *) malloc(sizeof(S_ContextTypeInfo)*this->maxN);

		switch(this->smoothingStrategy){
			case 'k':	//for Modified Kneser-Ney smoothing
				
				this->calcNgramMatchingInfoTokenFreqContextTypeExtendingCurrentMatch(currentMatchStart, currentMatchLen, nextWord, freqTable, contextTypeInfo, updatedMatchingStart, updatedMatchingLen);
				logProb = this->calcLogProb_kneserNeySmoothing(freqTable, contextTypeInfo);
				break;
			default:	//all other cases including 'g' (Good-Turing smoothing)
				this->calcNgramMatchingInfoTokenFreqOnlyExtendingCurrentMatch(currentMatchStart, currentMatchLen, nextWord, freqTable, updatedMatchingStart, updatedMatchingLen);
				logProb = this->calcLogProb(freqTable);
		}

		free(freqTable);
		free(contextTypeInfo);

		//insert the info into the cache
		S_Cached_SA_Access_Info accessInfo;
		accessInfo.updatedMatchingStart = updatedMatchingStart;
		accessInfo.updatedMatchingLen = updatedMatchingLen;
		accessInfo.logProb = logProb;
		accessInfo.lastTimedUsed = currentTime;

		this->cached_sa_access.insert(make_pair(accessKey, accessInfo));

		return logProb;
	}

	//otherwise, already exist in the cache, just update the last touched time
	updatedMatchingStart = iter_cached_sa_access->second.updatedMatchingStart;
	updatedMatchingLen = iter_cached_sa_access->second.updatedMatchingLen;
	logProb = iter_cached_sa_access->second.logProb;

	return logProb;
}

double C_SuffixArrayLanguageModel::calcLogProb(double *freq)
{
	switch(this->interpolationStrategy){
	case 'e':
		return this->calcLogProb_equalWeightedInterpolation(freq);
		break;
	case 'i':
		return this->calcLogProb_ibmHeuristicInterpolation(freq);
		break;
	case 'm':
		return this->calcLogProb_maxProbInterpolation(freq);
		break;
	default:
		cerr<<"Unknown interpolation strategy!\n";
		exit(0);
	}
}

double C_SuffixArrayLanguageModel::calcLogProb_equalWeightedInterpolation(double *freq)
{
	double prob = 0.0;

	
	if(freq[0]>0){

		int i=0;
		bool stillMatched = true;

		while(stillMatched && (i<this->maxN)){
			if(freq[2*i]>0){
				prob+=freq[2*i]/freq[2*i+1];
			}
			else{
				stillMatched = false;
			}

			i++;
		}

		return log(prob/(double)this->maxN);
	}
	else{	//unknown word
		return SALM_LOG_PROB_UNK;
	}
}

double C_SuffixArrayLanguageModel::calcLogProb_ibmHeuristicInterpolation(double *freq)
{
	double prob = 0.0;
	if(freq[0]==0){	//unknown word
		return SALM_LOG_PROB_UNK;
	}

	double remainingWeightSum = 1.0;

	//find the first non-zero match
	int i = this->maxN - 1;

	while(freq[2*i]==0){	//will stop for sure because freq[0]!=0
		i--;
	}

	for(int j=i;j>=0;j--){
		//for (j+1)-gram
		double historyFreq = freq[2*j+1];
		double logHistoryFreq = log(historyFreq);
		if(logHistoryFreq>1){
			logHistoryFreq = 1.0;	//cap it to 1
		}

		double reliability = 0.1*logHistoryFreq+0.3;	//heuristics for reliability of the history
		double adjustedWeights = remainingWeightSum *  reliability;
		
		prob+=adjustedWeights * freq[2*i]/freq[2*i+1];

		remainingWeightSum -= adjustedWeights;
	}

	return log(prob);	
}

double C_SuffixArrayLanguageModel::calcLogProb_maxProbInterpolation(double *freq)
{
	double maxProb = 0.0;
	
	if(freq[0]>0){

		int i=0;
		bool stillMatched = true;

		while(stillMatched && (i<this->maxN)){
			if(freq[2*i]>0){
				double prob=freq[2*i]/freq[2*i+1];

				if(prob>maxProb){
					maxProb = prob;
				}
			}
			else{
				stillMatched = false;
			}

			i++;
		}

		return log(maxProb);
	}
	else{	//unknown word
		return SALM_LOG_PROB_UNK;
	}
}

/**
* Follow the implementation described in page 23 of Chen & Goodman tech report (section 4.1.6 and 4.1.7)
* Use notation described in James 2000 pp3 for MODKN-COUNT
**/
double C_SuffixArrayLanguageModel::calcLogProb_kneserNeySmoothing(double *freq, S_ContextTypeInfo * contextTypeFreq)
{
	double prob = 0.0;
	int i;
	
	if(freq[0]>0){
		contextTypeFreq[i].
	}
	
	//unknown word
	return SALM_LOG_PROB_UNK;	
}


IndexType C_SuffixArrayLanguageModel::returnVocId(C_String aWord)
{
	return this->voc->returnId(aWord);
}


/**
* Scan corpus to collect important context-type information needed for KN-smoothing
* Knowing where n-gram w_(i-n+2)^(i-1) occurs, scan corpus for N_{1+}(dot w_{i-n+2}^i)
* and N_{1+}(dot w_{i-n+2}^{i-1} dot)
* Also, collect type freq of n-grams w_{i-n+1}^{i-1} that occur exactly 1, 2 and 3+ times
* to estimate the discounting factor gammar
* 
* @see Chen & Goodman 1998 page 19-20 for detailed description
*
* @param n order of n-gram
* @param w_in1 VocId of w<sub>i-n+1</sub>
* @param w_i VocId of w<sub>i</sub>, the next word to be predicted
* @param leftBoundaryOfSaRangeFor_w_in2_i1
* @param rightBoundaryOfSaRangeFor_w_in2_i1 [leftBoundaryOfSaRangeFor_w_in2_i1, rightBoundaryOfSaRangeFor_w_in2_i1] is the range of suffix array positions that correspond to the locations of phrase w<sub>i-n+2</sub><sup>i-1</sup>
* @param leftBoundaryOfSaRangeFor_w_in1
* @param rigthBoundaryOfSaRangeFor_w_i1 [leftBoundaryOfSaRangeFor_w_in1, rigthBoundaryOfSaRangeFor_w_i1] is the range of suffix array positions that correspond to the locations of phrase w<sub>i-n+1</sub><sup>i-1</sup>
* @return S_ContextTypeInfo containing the context type information
**/
void C_SuffixArrayLanguageModel::scanCorpusForContextTypeInfo(int n, IndexType w_i, TextLenType leftBoundaryOfSaRangeFor_w_in2_i1, TextLenType rightBoundaryOfSaRangeFor_w_in2_i1, TextLenType leftBoundaryOfSaRangeFor_w_in1_i1, TextLenType rigthBoundaryOfSaRangeFor_w_in1_i1, S_ContextTypeInfo & result)
{

	TextLenType i;
	TextLenType posInCorpus;
	IndexType nextWordInCorpus;
	int n1 = n-1;	//this value will be used frequently here

	//first scan the corpus for all the word types that follow w_{i-n+1}^{i-1}
	//to collect N1(w_in1^i1 dot) N2, and N3+ info needed
	result.N1_w_in1_i1_dot = 0;
	result.N2_w_in1_i1_dot = 0;
	result.N3plus_w_in1_i1_dot = 0;

	int freqOfCurrentType = -1; //freq of 'dot' with current type
	IndexType currentNextWordType = 0;
	for(i=leftBoundaryOfSaRangeFor_w_in1_i1;i<=rigthBoundaryOfSaRangeFor_w_in1_i1;i++){
		posInCorpus = this->suffix_list[i] + n1;
		//suffix_list[i] is the position of w_{i-n+1} in the corpus
		//suffix_list[i]+n-1 is hte position of the word (the dot in the equation) that follows w_{i-n+1}^{i-1}

		nextWordInCorpus = this->corpus_list[posInCorpus];
		freqOfCurrentType++;
		if(nextWordInCorpus!=currentNextWordType){

			if(freqOfCurrentType==1){
				result.N1_w_in1_i1_dot++;
			}
			else if(freqOfCurrentType==2){
				result.N2_w_in1_i1_dot++;
			}
			else{	//freq of this type is >=3
				result.N3plus_w_in1_i1_dot++;
			}

			currentNextWordType = nextWordInCorpus;
			freqOfCurrentType=0;
		}
	}

	//for the last type in the range
	freqOfCurrentType++;

	if(freqOfCurrentType==1){
		result.N1_w_in1_i1_dot++;
	}
	else if(freqOfCurrentType==2){
		result.N2_w_in1_i1_dot++;
	}
	else{	//freq of this type is >=3
		result.N3plus_w_in1_i1_dot++;
	}


	//step 2, scan the corpus for N_{1+}(dot w_{i-n+2}^{i}) and N_{1+}(dot w_{i-n+2}^{i-1} dot)
	IndexType precedingWord;
	IndexType followingWord;
	if(n==2){	//the special case
		result.N1plus_dot_w_in2_i1_dot = this->typeOfBigrams;

		//check if we have the N_1+(dot w_i) information already
		map<IndexType, unsigned int>::iterator iterTypeFreqPrecedingWord;
		iterTypeFreqPrecedingWord = this->typeFreqPrecedingWord.find(w_i);

		if(iterTypeFreqPrecedingWord==this->typeFreqPrecedingWord.end()){	//does not exist yet
			TextLenType startPosInSA = this->level1Buckets[w_i].first;
			TextLenType endPosInSA = this->level1Buckets[w_i].last;

			set<IndexType> wordTypePrecedesW_i;
			for(i=startPosInSA;i<=endPosInSA;i++){
				posInCorpus = this->suffix_list[i] - 1;
				precedingWord = this->corpus_list[posInCorpus];

				wordTypePrecedesW_i.insert(precedingWord);
			}

			result.N1plus_dot_w_in2_i = (double) wordTypePrecedesW_i.size();

			//and save this for future references
			this->typeFreqPrecedingWord.insert(make_pair(w_i, wordTypePrecedesW_i.size()));
		}
		else{	//already has the information in typeFreqPrecedingWord
			result.N1plus_dot_w_in2_i = (double) (iterTypeFreqPrecedingWord->second);
		}
	}
	else{
		set<IndexType> wordTypesPrecedesW_in2_i;
		set< pair<IndexType, IndexType> > wordTypesSurroundW_in2_i1;
		
		for(i=leftBoundaryOfSaRangeFor_w_in2_i1;i<=rightBoundaryOfSaRangeFor_w_in2_i1;i++){
			posInCorpus = this->suffix_list[i] -1;	//pos of preceding word (w_{i-n+1}) in the corpus
			precedingWord = this->corpus_list[posInCorpus];

			posInCorpus+=n1;						//pos of following word w_i in the corpus
			followingWord = this->corpus_list[posInCorpus];

			pair<IndexType, IndexType> tmpPair = make_pair(precedingWord, followingWord);

			//if w_i equals next word, add the preceding word to set 
			if(followingWord==w_i){
				wordTypesPrecedesW_in2_i.insert(precedingWord);
			}

			//add the pair to set
			wordTypesSurroundW_in2_i1.insert(tmpPair);

		}


		result.N1plus_dot_w_in2_i = wordTypesPrecedesW_in2_i.size();
		result.N1plus_dot_w_in2_i1_dot = wordTypesSurroundW_in2_i1.size();
	}

	result.valid = true;
}
