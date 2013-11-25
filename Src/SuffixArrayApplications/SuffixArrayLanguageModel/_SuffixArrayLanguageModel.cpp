/**
* Revision $Rev: 3815 $
* Last Modified $LastChangedDate: 2007-07-06 14:31:12 -0400 (Fri, 06 Jul 2007) $
**/

#include  "_SuffixArrayLanguageModel.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <memory.h>
#include <cstring>

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
* Using the training data corpusFileNameStem that has been indexed by IndexSA
* Consider at most maxN-gram in language modeling
* For frequencies that are lower than maxFreqForDiscounting, use Good-Turing for discounting
* If maxFreqForDiscounting is set to be 0 or negative value, then discounting is turned off. Use MLE to estimate the probability of a word given history
* @param cfgFileName Configuration file that specifies the value of parameters for SALM
* 
* Each line in the configuration file is a Keyword Value pair. Legal keywords are:
* CORPUS : corpusFileNameStem The training corpus filename used by IndexSA. Must be specified!
* N : Highest order of n considered for n-gram LM estimation, default value = 5
* MAX_FREQ_DISC :  When Good-Turing discounting is used, n-grams which have frequencies higher than this value will not be discounted. Negative value will disable the discounting. default value = -1. 
* INTERPOLATION_STRATEGY : Set strategy to interploate the conditional probabilities of next word given different order of histories
* 	'e' default. Equal weighted interpolation of unigram, bigram, trigram... probabiblities
* 	'm' for using the maximum probabilty from all histories and use this value as P(next word | history)
* 	'i' for deleted interpolation with weights determined by a heuristic that favors long n-gram probability when the frequency is reliable
**/
C_SuffixArrayLanguageModel::C_SuffixArrayLanguageModel(const char *  cfgFileName)
{
  
	fstream cfgFile;
	cfgFile.open(cfgFileName,ios::in);

	if(!cfgFile){
		fprintf(stderr,"Configuration file does not exist! quit!!\n");
		exit(0);
	}

	//-----------------------------------------------------------------------------
	//reading parameters
	char paraName[1024];
	char corpusFileNameStem[1024];
	corpusFileNameStem[0]=0;
	this->maxFreqForDiscounting=-1;

	this->interpolationStrategy = 'e';	//default interpolation strategy: equally weighted n-gram conditional prob
	this->maxN = 5;         // default value; consider up to 5 words	
		
	while(!cfgFile.eof()){
		cfgFile>>paraName;

		if(strcmp(paraName,"CORPUS")==0){
			cfgFile>>corpusFileNameStem;
		}		
		else if(strcmp(paraName,"N")==0){
			cfgFile>>this->maxN;
		}
		else if(strcmp(paraName,"MAX_FREQ_DISC")==0){
			cfgFile>>maxFreqForDiscounting;
		}		
		else if(strcmp(paraName,"INTERPOLATION_STRATEGY")==0){
			cfgFile>>this->interpolationStrategy;
		}	

		paraName[0]=0;
		
	}
	
	//load corpus and suffix array
	if(strlen(corpusFileNameStem)==0){
		cerr<<"CORPUS need to be specified in the configuration file. This should be the corpus name used for LM.\n";
		exit(-1);
	}
	this->loadData_forSearch(corpusFileNameStem, false, true);	//call the constructor of the super class to load suffix array for corpusName, with vocabulary, no offset, 
	
	
	//if apply discounting construct the discounting map
	if(this->maxFreqForDiscounting<=0){
		this->applyDiscounting = false;
	}
	else{
		this->applyDiscounting = true;		
		this->constructDiscountingMap();	//scan the corpus and construct the count of counts table and then discounting map		
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

	this->interpolationStrategy = 'e';			//default: interpolation strategy: equally weighted n-gram conditional prob

}


/**
* Similar to the function in C_SuffixArrayScanningBase
* Scan the corpus to obtain count of counts information 
* and construct the discounting using Good-Turing smoothing
**/
void C_SuffixArrayLanguageModel::constructDiscountingMap()
{
	int i,j;
	unsigned int * countOfCountsTable = (unsigned int *) malloc(sizeof(unsigned int)*this->maxN*this->maxFreqForDiscounting);

	if(countOfCountsTable==NULL){
		cerr<<"Count of counts table can not be initialized. Exit\n";
		exit(0);
	}
	
	//initialize count of counts table
	for(int c=0;c<this->maxN*this->maxFreqForDiscounting;c++){
		countOfCountsTable[c]=0;
	}
		
	//initialize the scanning list
	S_nGramScanningInfoElement * nGramScanningList = (S_nGramScanningInfoElement *) malloc(sizeof(S_nGramScanningInfoElement)*this->maxN);
	for(i=0;i<this->maxN;i++){
		nGramScanningList[i].freqSoFar=0;
		nGramScanningList[i].vocId = 0;
		nGramScanningList[i].freqThreshForOutput = (unsigned int) -1;	//default, do not output
	}

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
						(wordInCorpus==nGramScanningList[i].vocId)){	//still match

						nGramScanningList[i].freqSoFar++;
					}
					else{	//we will have new (i+1) and longer n-grams soon, before that check if we should increase the count of counts for n because of this n-gram type
									
						bool validNgramUpSoFar = true;
						unsigned int freqSoFar;
						
						for(j=i;j<this->maxN;j++){				
							
							
							if(nGramScanningList[j].vocId==0){		//a NULL word, then this n-gram and longer ones in the scan window are invalid
								validNgramUpSoFar = false;
							}

							if(validNgramUpSoFar){		//perform actions depends on actionType
								
								freqSoFar = nGramScanningList[j].freqSoFar;
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
								nGramScanningList[j].freqSoFar = 0;
							}
							else{
								nGramScanningList[j].freqSoFar = 1;
							}

							nGramScanningList[j].vocId = wordInCorpus;							
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
		if(nGramScanningList[i].vocId==0){	//invalide word
			validNgramUpSoFar = false;
		}

		if(validNgramUpSoFar){
			
			freqSoFar = nGramScanningList[i].freqSoFar;
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
		if(this->applyDiscounting){
			freqTable[0] = this->discountFreq(1, totalOccurrences);
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
				freqTable[2*n-2] = this->discountFreq(n, totalOccurrences);				
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


//given observedFreq of n-gram, return discounted freq using Good-Turing smoothing
double C_SuffixArrayLanguageModel::discountFreq(int n, unsigned int observedFreq)
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
LMState C_SuffixArrayLanguageModel::beginOfSentenceState()
{
	
	this->resetLmStates();
	this->initialLmState();	

	return 0;
}

void C_SuffixArrayLanguageModel::initialLmState()
{
	//add sentence start
	S_LMStateInfo sentStartNode;
	sentStartNode.locationInCorpus.posInCorpus = 1;	//if corpus is indexed correctly position 1 should be <s>
	sentStartNode.locationInCorpus.len = 1;
	sentStartNode.cachedNextWordExtension.clear();

	this->allLMStates.push_back(sentStartNode);
	this->ngramLocation2LmStateId.insert(make_pair(sentStartNode.locationInCorpus, 0));
}

void C_SuffixArrayLanguageModel::resetLmStates()
{
	this->allLMStates.clear();
	this->ngramLocation2LmStateId.clear();
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
	if(lmState>=this->allLMStates.size()){
		cerr<<"Invalid LM State: "<<lmState<<endl;
		exit(-1);
	}

	//first check if we have already seen this 'nextWord' before
	map< IndexType, S_CachedLmInfo>::iterator iterNextWordExtensionCache;
	iterNextWordExtensionCache = this->allLMStates[lmState].cachedNextWordExtension.find( nextWord );

	if(iterNextWordExtensionCache==this->allLMStates[lmState].cachedNextWordExtension.end()){ //we haven't seen this lmState+word yet

		//search for it in the corpus
		S_NgramLocationInCorpus correspondingNgramLocation = this->allLMStates[lmState].locationInCorpus;
		S_NgramLocationInCorpus updatedNgramLocation;
		
		double logProb = this->logProbFromFreq(
			correspondingNgramLocation.posInCorpus, 
			correspondingNgramLocation.len, 
			nextWord, 
			updatedNgramLocation.posInCorpus,
			updatedNgramLocation.len);
		
		//caching the logprob of 'nextword' given the lmState
		int updatedLmStateId;
		map<S_NgramLocationInCorpus, int, lt_ngramLocationInCorpus>::iterator iterNgramLocation2LmStateId;
		iterNgramLocation2LmStateId = this->ngramLocation2LmStateId.find(updatedNgramLocation);
		if(iterNgramLocation2LmStateId==this->ngramLocation2LmStateId.end()){	//this updated lm state does not exist yet
			S_LMStateInfo newLmStateNode;

			newLmStateNode.locationInCorpus = updatedNgramLocation;
			newLmStateNode.cachedNextWordExtension.clear();
			
			this->allLMStates.push_back(newLmStateNode);
			updatedLmStateId = this->allLMStates.size() -1 ;
			this->ngramLocation2LmStateId.insert(make_pair(updatedNgramLocation, updatedLmStateId));
		}
		else{
			updatedLmStateId = iterNgramLocation2LmStateId->second;
		}

		//cache this
		S_CachedLmInfo cachedLmInfo;
		cachedLmInfo.logProb = logProb;
		cachedLmInfo.nextState = updatedLmStateId;

		this->allLMStates[lmState].cachedNextWordExtension.insert(make_pair(nextWord, cachedLmInfo));

		//updated next state
		nextState = updatedLmStateId;
		
		return logProb;
	}

	nextState = iterNextWordExtensionCache->second.nextState;

	return iterNextWordExtensionCache->second.logProb;
}


/**
* Given the history as lmState and append a phrase as a vector of IndexType,
* calculate the LM prob and update the lm state
* Modification suggested by Erik Peterson (eepter@cs.cmu.edu) to check the size of phrase. 
* For cases where phrase is empty, i.e. phrase.size()==0, nextState will not be updated correctly and may cause problems in the calling function.
 * @param lmState Current language model state
* @param phrase A vector of vocIds of the next phrase (the phrase to be predicted)
* @param &nextState Returning the updated language model state when the next word is appended
**/
double C_SuffixArrayLanguageModel::logProb(LMState lmState, vector<IndexType> phrase, LMState & nextState)
{
	double logProb = 0;
	
	if (phrase.size() == 0) {
		nextState = lmState;
		return logProb;
	}

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
double C_SuffixArrayLanguageModel::logProbFromFreq(TextLenType currentMatchStart, unsigned char currentMatchLen, IndexType nextWord, TextLenType &updatedMatchingStart, unsigned char &updatedMatchingLen)
{

	double logProb;

	double * freqTable = (double *) malloc(sizeof(double)*2*(this->maxN));
	memset(freqTable, 0, 2*this->maxN*sizeof(double));

	this->calcNgramMatchingInfoTokenFreqOnlyExtendingCurrentMatch(currentMatchStart, currentMatchLen, nextWord, freqTable, updatedMatchingStart, updatedMatchingLen);

	logProb = this->calcLogProb(freqTable);

	free(freqTable);

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

IndexType C_SuffixArrayLanguageModel::returnVocId(C_String aWord)
{
	return this->voc->returnId(aWord);
}
