#include "stdio.h"
#include "stdlib.h"
#include "_SuffixArraySearchApplicationBase.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
int SHOW_DEBUG_INFO = 0;

typedef struct s_ngram_freq_info{
	C_String ngramText;
	vector<IndexType> ngram;
	unsigned int freq;
}S_Ngram_Freq_Info;

/**
* Given several corpora indexed by their suffix array,
* collect counts of n-grams in a list from all the corpora.
* This is useful when a corpus is very large,
* one can split the data into many chunks and sum up the n-gram frquencies.
*
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
int main(int argc, char* argv[]){
	//check parameters
	if(argc<2){
		cerr<<"\n-------------------------------------------";
		cerr<<"\nUsage:";
		cerr<<"\n\t"<<argv[0]<<" n-gram_list_filename < list of suffix arry used";
		cerr<<"\nNote:";
		cerr<<"\n\tn-gram_list_filename.id_voc must exist first.";
		cerr<<"\n-------------------------------------------\n\n";

		exit(0);
	}

	//load vocabulary
	char id_voc_filename[1024];
	sprintf(id_voc_filename, "%s.id_voc", argv[1]);
	C_IDVocabulary voc(id_voc_filename);

	//load the n-gram list
	vector<S_Ngram_Freq_Info> ngramList;

	ifstream NgramListFile;
	NgramListFile.open(argv[1]);
	char tmpString[4096];
	while(!NgramListFile.eof()){

		NgramListFile.getline(tmpString, 4096, '\n');

		if(strlen(tmpString)>0){
			S_Ngram_Freq_Info tmpNode;
			tmpNode.ngramText = C_String(tmpString);
			tmpNode.freq = 1;
			tmpNode.ngram.clear();

			//conver the n-gram as string to vocId
			char tmpToken[MAX_TOKEN_LEN];
			memset(tmpToken,0,MAX_TOKEN_LEN);
			int pos = 0;
			int inputLen = strlen(tmpString);

			for(int posInInput = 0; posInInput<inputLen; posInInput++){
				char thisChar = tmpString[posInInput];

				if((thisChar==' ')||(thisChar=='\t')){  //delimiters
					if(strlen(tmpToken)>0){
						tmpToken[pos] = '\0';               
						tmpNode.ngram.push_back(voc.returnId(C_String(tmpToken)));
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
				tmpNode.ngram.push_back(voc.returnId(C_String(tmpToken)));
			}

			ngramList.push_back(tmpNode);
		}
		tmpString[0]='\0';
	}
	cerr<<"Total "<<ngramList.size()<<" ngrams loaded.\n";

	//loop over all suffix array and collec the n-gram counts
	char sa_filename[1024];
	while(! cin.eof()){
		cin>>sa_filename;

		if(strlen(sa_filename)>0){
			cerr<<"Considering "<<sa_filename<<endl;

			C_SuffixArraySearchApplicationBase sa;
			sa.loadData_forSearch(sa_filename, true, true);

			for(int i=0; i<ngramList.size(); i++){
				unsigned int freq;

				freq = sa.freqOfExactPhraseMatch(ngramList[i].ngram);

				ngramList[i].freq+=freq;	
			}
		}

		sa_filename[0]=0;
	}


	for(int m=0;m<ngramList.size();m++){
		cout<<ngramList[m].freq<<"\t";
		cout<<ngramList[m].ngramText.toString()<<"\n";
	}


	return 1;
}
