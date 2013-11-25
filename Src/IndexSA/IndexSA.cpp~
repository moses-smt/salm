/**
* Main function to index a corpus according to its suffix array
* Revision: $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/

#include "stdio.h"
#include "stdlib.h"

#include <string>
#include <iostream>
#include <fstream>
#include "_MonoCorpus.h"
#include "salm_shared.h"

using namespace std;

IndexType * corpus;	//because the compare function needs to see this, make it global
TextLenType actualCorpusSize;

int main(int argc, char* argv[]){
	
	//-----------------------------------------------------------------------------
	//check parameter


	if(argc<2){		

		fprintf(stderr,"\nUsage:");
		fprintf(stderr,"\n%s fileNameStem [existingIDVocFileName]\n",argv[0]);				
			
		exit(0);
	}

	C_MonoCorpus corpus;

	char vocFileName[1024];
	sprintf(vocFileName, "%s.id_voc", argv[1]);
	
	if(argc==2){	//no existing vocabulary given
		cerr<<"Initialize vocabulary file: "<<vocFileName<<endl;
		corpus.initializeVocabulary(argv[1]);		
		corpus.loadCorpusAndSort(argv[1], vocFileName, true);
	}
	else{
		if(strcmp(vocFileName, argv[2])!=0){
			cerr<<"Error! ExistingIDVocFileName has to be called: "<<vocFileName<<" and cover all the words in the corpus."<<endl;
			exit(-1);
		}
		corpus.loadCorpusAndSort(argv[1], argv[2], false);
	}

	corpus.output(argv[1]);

	return 0;
}

