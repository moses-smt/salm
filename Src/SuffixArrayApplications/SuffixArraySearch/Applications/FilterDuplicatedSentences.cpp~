#include "stdio.h"
#include "stdlib.h"
#include "_SuffixArraySearchApplicationBase.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;

/**
* Given a corpus indexed by its suffix array, filter out the duplicated sentences in the data
* and output the unique sentences within.
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
int main(int argc, char * argv[]){
	//-----------------------------------------------------------------------------
	//check parameter
	if(argc<2){		
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s fileNameStem < original corpus > corpus with uniq sentences\n",argv[0]);
		
		exit(0);
	}

	map< pair<TextLenType, int>, bool> duplicatedSentAlreadyOutput;
	map< pair<TextLenType, int>, bool>::iterator iterDuplicatedSentAlreadyOutput;


	C_SuffixArraySearchApplicationBase sa;	
	sa.loadData_forSearch(argv[1], false, true);

	unsigned long totalFilteredSent = 0;

	cerr<<"Filtering duplicated sentences:\n";
	char tmpString[4000];
	while(!cin.eof()){
	  cin.getline(tmpString,100000,'\n');
	  if(strlen(tmpString)>0){
		  TextLenType freq = 0;
		  TextLenType firstOccurrence;
		  int sentLen;

		  freq = sa.freqOfExactPhraseMatchAndFirstOccurrence(tmpString, firstOccurrence, sentLen);

		  if(freq>1){	//freq is at least 1, because this is the same corpus
				//then there are multiple occurrences of this sentence
				//check if we have already output it
				iterDuplicatedSentAlreadyOutput = duplicatedSentAlreadyOutput.find(make_pair(firstOccurrence, sentLen));
				
				if(iterDuplicatedSentAlreadyOutput == duplicatedSentAlreadyOutput.end()){	//we haven't output it
					cout<<tmpString<<endl;
					duplicatedSentAlreadyOutput.insert(make_pair(make_pair(firstOccurrence, sentLen), true));					
				}
				else{	
					//it has been output already, ignore it
					totalFilteredSent++;
				}
		  }
		  else{	//freq==1, no duplication
			  cout<<tmpString<<endl;
		  }
		  
	  }
	}

	cerr<<"Total "<<totalFilteredSent<<" duplicated sentences are filtered\n";

	return 1;
}
