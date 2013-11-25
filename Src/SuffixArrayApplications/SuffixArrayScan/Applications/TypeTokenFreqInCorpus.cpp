#include "_SuffixArrayScanningBase.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

/**
* Given an indexed corpus, output the type/token information of the n-grams in the corpus.
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
int main(int argc, char * argv[]){
	//-----------------------------------------------------------------------------
	//check parameter
	//-----------------------------------------------------------------------------
	if(argc<3){
		fprintf(stderr,"\nGiven an indexed corpus, output the type token information for n-grams.\n");
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s fileNameStem maxN \n\n",argv[0]);		
		exit(0);
	}
	
	unsigned int maxN = atoi(argv[2]);
	
	C_SuffixArrayScanningBase saObj(argv[1], maxN);	
	saObj.scanSuffixArrayForTypeToken();

	return 1;
}
