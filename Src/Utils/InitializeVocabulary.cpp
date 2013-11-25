#include "stdio.h"
#include "stdlib.h"
#include "_IDVocabulary.h"

#include <iostream>

using namespace std;

/**
* \ingroup utils
* Intialize an empty vocabulary with reserved words
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
int main(int argc, char * argv[]){
	if(argc<2){
		cerr<<"\nUsage:";
		cerr<<"\n\t"<<argv[0]<<" vocabularyFileName\n\n";
		exit(0);
	}
	
	C_IDVocabulary voc;

	voc.addingReservedWords();
	voc.outputToFile(argv[1]);

	return 0;

}
