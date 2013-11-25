#include "stdio.h"
#include "stdlib.h"
#include "_SuffixArraySearchApplicationBase.h"
#include <iostream>
#include <vector>

using namespace std;

int SHOW_DEBUG_INFO = 0;


/**
* Application main functionL ExactNgramMatchingFreq
* Input from stdin ngrams with each line containing one n-gram
* Search the corpus for the occurrences of each n-gram and output their frequencies in the corpus
*
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
int main(int argc, char * argv[]){
	//-----------------------------------------------------------------------------
	//check parameter
	if(argc<2){		
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s fileNameStem \n",argv[0]);
		
		exit(0);
	}


	C_SuffixArraySearchApplicationBase sa;
	sa.loadData_forSearch(argv[1], false, true);	//we need vocabulary, but do not need offset information here

	cerr<<"Input N-grams:\n";
	char tmpString[1000];
	while(!cin.eof()){
	  cin.getline(tmpString,100000,'\n');
	  if(strlen(tmpString)>0){
		  TextLenType freq = 0;
		  freq = sa.freqOfExactPhraseMatch(tmpString);
		  cout<<freq<<": "<<tmpString<<endl;
	  }
	}

	return 0;
}
