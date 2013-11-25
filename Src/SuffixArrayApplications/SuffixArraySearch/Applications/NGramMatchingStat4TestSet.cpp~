#include "stdio.h"
#include "stdlib.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "_SuffixArraySearchApplicationBase.h"


#include <time.h>
#include <stdio.h>
#include <map>

using namespace std;

/**
*	Given the indexed training corpus, analyze the token/type matching ratio of the n-grams in the testing data.
*
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/	
int main(int argc, char* argv[]){
	//-----------------------------------------------------------------------------
	//check parameter


	if(argc<2){		
		fprintf(stderr,"\nOutput the n-gram matching statistics of a testing data given an indexed corpus\n");
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s corpusFileNameStem < testing data\n\n",argv[0]);
		
		exit(0);
	}


	//-----------------------------------------------------------------------------	
		
	C_SuffixArraySearchApplicationBase SA;
	
	map<int, pair<int, unsigned long> > results4OneSent;
	map<int, pair<int, unsigned long> >::iterator iterResult;

	vector<int> nGramTokenCountsInTest;
	vector<int> nGramInTestMatched;
	vector<double> nGramFreqInTrainMatched;

	int maxSentLen = 4086;
	nGramTokenCountsInTest.reserve(maxSentLen);
	nGramInTestMatched.reserve(maxSentLen);
	nGramFreqInTrainMatched.reserve(maxSentLen);

	//initialize
	for(int i=0;i<maxSentLen;i++){
		nGramTokenCountsInTest.push_back(0);
		nGramInTestMatched.push_back(0);
		nGramFreqInTrainMatched.push_back(0);
	}

	char fileName[1000];
	char tmpString[10000];

	strcpy(fileName, argv[1]);
	
	fprintf(stderr,"Loading data...\n");
	SA.loadData_forSearch(fileName, false, true);

	fprintf(stderr,"Input sentences:\n");

	long ltime1, ltime2;

	time( &ltime1 );

	int totalSentences = 0;
	int matchedSentences = 0;
	while(!cin.eof()){
		int sentLen;
		cin.getline(tmpString,10000,'\n');

		if(strlen(tmpString)>0){
			
			totalSentences++;
		  
			results4OneSent.clear();
			results4OneSent = SA.returnNGramMatchingStatForOneSent(tmpString, sentLen);

			if(sentLen>maxSentLen){
				cerr<<"Sentence too long, we can not handle it! Exit.\n";
				exit(0);
			}

			for(int j=1;j<=sentLen;j++){	//j-gram
				nGramTokenCountsInTest[j]+=(sentLen-j+1);	//number of j-grams in the sentence;
			}

			iterResult=results4OneSent.begin();
			while(iterResult!=results4OneSent.end()){
				
				nGramInTestMatched[iterResult->first]+=iterResult->second.first;
				nGramFreqInTrainMatched[iterResult->first]+=iterResult->second.second;
				
				if(iterResult->first==sentLen){	//a complete match
					matchedSentences++;
				}

				iterResult++;
			}
		}

		tmpString[0]=0;
		
	}

	int n = 1;
	while(nGramInTestMatched[n]!=0){		
		int matched = nGramInTestMatched[n];
		int totalInTest = nGramTokenCountsInTest[n];
		cout<<"N="<<n<<":\t"<<matched<<" / "<<totalInTest<<"\t";
		printf("%.1f\t", double(matched)/double(totalInTest)*100.0);
		cout<<"OccInTrain= "<<nGramFreqInTrainMatched[n]<<endl;
	  
	  n++;
	}

	cout<<"\nOut of "<<totalSentences<<" input sentences, "<<matchedSentences<<" can be found in the training data.\n";;
	time( &ltime2 );	
	cout<<"Time cost:"<<ltime2-ltime2<<" seconds\n";

	return 1;
}
