#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <iostream>
#include "_SuffixArraySearchApplicationBase.h"

using namespace std;


/**
* Return locations of all the embedded n-grams of a sentence in the indexed corpus
*
* Revison $Rev: 3794 $
* Last modified: $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
int main(int argc, char * argv[]){

	//-----------------------------------------------------------------------------
	//check arguments
	if(argc<2){		
		fprintf(stderr,"\n\nOutput locations of all the matched embedded n-grams of a sentence in an indexed corpus\n");
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s corpusFileNameStem [highestFreq maxRet smallestUnit longestUnit] < list of sentences\n\n",argv[0]);
		
		exit(-1);
	}
	

	int highFreq;
    int maxRet;
    int smallestUnit;
    int longestUnit;

	C_SuffixArraySearchApplicationBase saObj;

	saObj.loadData_forSearch(argv[1], false, false);

	if(argc>=6){	//if argument of highestFreq, maxRet, smallestUnits are set
		highFreq = atoi(argv[2]);
		maxRet = atoi(argv[3]);
		smallestUnit = atoi(argv[4]);
		longestUnit = atoi(argv[5]);

		saObj.setParam_highestFreqThresholdForReport(highFreq);
		saObj.setParam_reportMaxOccurrenceOfOneNgram(maxRet);
		saObj.setParam_shortestUnitToReport(smallestUnit);
        saObj.setParam_longestUnitToReport(longestUnit);
	}

	cerr<<"Input sentences:\n";

	char sentence[10000];
	
	while(!cin.eof()){
		cin.getline(sentence,10000,'\n');
		if(strlen(sentence)>0){

			vector<C_String> sentAsCStringVector = saObj.convertCharStringToCStringVector(sentence);	//for later display purpose
			
			
			vector<S_phraseLocationElement> locations;
			locations = saObj.findPhrasesInASentence(sentence);
		  
			if(locations.size()==0){
				cout<<"Nothing can be found in the corpus.\n";
			}
			else{
				for(int i=0;i<locations.size(); i++){
					cout<<"N-gram ["<<(int)locations[i].posStartInSrcSent<<", "<<(int)locations[i].posEndInSrcSent<<"]: ";
					for(int j=locations[i].posStartInSrcSent; j<=locations[i].posEndInSrcSent; j++){
						cout<<sentAsCStringVector[j-1].toString()<<" ";
					}
					cout<<" found in corpus: ";
					cout<<"SentId="<<locations[i].sentIdInCorpus<<" Pos="<<(int)locations[i].posInSentInCorpus<<endl;
			  }
		  }
		  cout<<endl;
	  }
	}



	return 0;
}
