#include "stdio.h"
#include "stdlib.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "_String.h"
#include "_SuffixArraySearchApplicationBase.h"

#include <time.h>
#include <stdio.h>
#include <map>

using namespace std;


vector<C_String> convertTextToStringVector(const char * sentText)
{  

    vector<C_String> sentAsStringVect;

    char tmpToken[MAX_TOKEN_LEN];
    memset(tmpToken,0,MAX_TOKEN_LEN);

    int pos = 0;

    int inputLen = strlen(sentText);

    for(int posInInput = 0; posInInput<inputLen; posInInput++){
        char thisChar = sentText[posInInput];

        if((thisChar==' ')||(thisChar=='\t')){  //delimiters
            if(strlen(tmpToken)>0){
                tmpToken[pos] = '\0';               
                sentAsStringVect.push_back(C_String(tmpToken));
                pos=0;
                tmpToken[pos] = '\0';
            }
        }
        else{
            tmpToken[pos] = thisChar;
            pos++;
            if(pos>=MAX_TOKEN_LEN){ //we can handle it
                fprintf(stderr,"Can't read tokens that exceed length limit %d. Quit.\n", MAX_TOKEN_LEN);
                exit(0);
            }
        }
    }

    tmpToken[pos] = '\0';
    if(strlen(tmpToken)>0){     
        sentAsStringVect.push_back(C_String(tmpToken));
    }

    return sentAsStringVect;
}

/**
* \ingroup search
*
* Given the training corpus indexed by its suffix array,
* output all the n-grams in a testing data that can be found in the training corpus
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
int main(int argc, char* argv[]){
	//-----------------------------------------------------------------------------
	//check parameter


	if(argc<2){		
		fprintf(stderr,"\nOutput the matched n-gram types a testing data set given an indexed corpus\n");
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s corpusFileNameStem < testing data\n\n",argv[0]);
		
		exit(0);
	}


	//-----------------------------------------------------------------------------	
		
	C_SuffixArraySearchApplicationBase SA;
	
	map<C_String, double> matchedNgrams;
	map<C_String, double>::iterator iterMatchedNgrams;


	int maxSentLen = 4086;


	char fileName[1000];
	char tmpString[10000];

	strcpy(fileName, argv[1]);
	
	fprintf(stderr,"Loading data...\n");
	SA.loadData_forSearch(fileName, false, true);

	cerr<<"Input sentences:\n";

	long ltime1, ltime2;

	time( &ltime1 );

	int totalSentences = 0;
	int matchedSentences = 0;
	while(!cin.eof()){
		cin.getline(tmpString,10000,'\n');

		if(strlen(tmpString)>0){
			vector<C_String> sentAsStringVector = convertTextToStringVector(tmpString);
			
			int sentLen;
			S_sentSearchTableElement * freqTable = SA.constructNgramSearchTable4SentWithLCP(tmpString, sentLen);
		  
			if(sentLen!=sentAsStringVector.size()){
				cerr<<"Something wrong, can not proceed.!\n";
				exit(-1);
			}
			

			//go over the frequency table
			for(int startPos = 0; startPos<sentLen; startPos++){
				C_String ngram;
				bool stillMatching = true;
				int n=1;
				while(stillMatching & (n<=(sentLen-startPos)) ){
				
					ngram.appending(sentAsStringVector[startPos+n-1]);

					int posInFreqTable = (n-1)*sentLen+startPos;
					if(freqTable[posInFreqTable].found){
						double frequency = freqTable[posInFreqTable].endingPosInSA - freqTable[posInFreqTable].startPosInSA + 1;

						iterMatchedNgrams = matchedNgrams.find(ngram);
						if(iterMatchedNgrams!=matchedNgrams.end()){	//exist already
							iterMatchedNgrams->second=frequency;	//frequency is not meaningful in this case, just use it because map need some values to be mapped to
						}
						else{
							matchedNgrams.insert(make_pair(ngram, frequency));
						}
					}
					else{
						stillMatching = false;
					}
					

					ngram.appending(C_String(" "));

					n++;
				}
			}

		}

		tmpString[0]=0;
		
	}


	//now output all the n-grams
	iterMatchedNgrams = matchedNgrams.begin();
	while(iterMatchedNgrams != matchedNgrams.end()){
		cout<<(iterMatchedNgrams->first).toString()<<endl;

		iterMatchedNgrams++;
	}


	time( &ltime2 );	
	cerr<<"Time spent:"<<ltime2-ltime2<<" seconds\n";

	return 1;
}
