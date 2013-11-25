#include "stdio.h"
#include "stdlib.h"
#include "_UniversalVocabulary.h"

#include <iostream>

using namespace std;

/**
* \ingroup utils
* Update the universal vocabulary with words in corpus
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
int main(int argc, char * argv[]){
	if(argc<3){
		cerr<<"\nUsage:";
		cerr<<"\n\t"<<argv[0]<<" universal_voc corpusFileName\n\n";
		exit(0);
	}

	C_UniversalVocabulary universalVoc(argv[1]);

	universalVoc.updateWithNewCorpus(argv[2]);

	return 1;
}
