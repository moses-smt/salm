#include "stdio.h"
#include "stdlib.h"
#include "_SuffixArraySearchApplicationBase.h"
#include <iostream>
#include <vector>

using namespace std;

int SHOW_DEBUG_INFO = 0;

/**
* Given a corpus indexed by its suffix array, input a sentence from STDIN and output the frequencies of its embedded n-grams in the corpus.
*
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
int main(int argc, char* argv[]){
	//-----------------------------------------------------------------------------
	//check parameter


	if(argc<2){		

		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s corpusFileNameStem < testing sentences\n\n",argv[0]);
		
		exit(0);
	}


	//-----------------------------------------------------------------------------	
		
	C_SuffixArraySearchApplicationBase SA;
		
	char tmpString[1000];	
	
	fprintf(stderr,"Loading data...\n");
	SA.loadData_forSearch(argv[1], false, true);

	fprintf(stderr,"Input Sentences:\n");

	while(!cin.eof()){
	  cin.getline(tmpString,100000,'\n');
	  if(strlen(tmpString)>0){		  
		  SA.displayNgramMatchingFreq4Sent(tmpString);	  
	  }
	}
	return 1;
}
