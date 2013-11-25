#include "stdio.h"
#include "stdlib.h"
#include "float.h"
#include "_SuffixArraySearchApplicationBase.h"
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

int SHOW_DEBUG_INFO = 0;

///Given src sentence length, convert the index in one-dimensional table to pair<startingPosInSrcSent, n>
///startingPosInSrcSent starts at 0, n is the n-gram length
void local_oneDimensionTableIndexToTwoDimension(unsigned int index, unsigned int sentLen, unsigned int &posInSrcSent, unsigned int &n)
{
    n = index / sentLen + 1;
    posInSrcSent = index % sentLen;
}

///Given the starting position in src sentence and the length of the n-gram
///calculate the index in the table
///posInSent starts at 0, n is the actual len of n-gram, starts at 1
unsigned int local_twoDimensionIndexToOneDimensionTableIndex(unsigned int posInSent, unsigned int n, unsigned int sentLen)
{
    unsigned int indexInTable = (n-1)*sentLen + posInSent;

    return indexInTable;
}

/**
* Given a corpus indexed by its suffix array
* calcuate the non-compositionalities of the embedded n-grams in a testing sentence
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
	double bigN = 1000000;
	
	fprintf(stderr,"Loading data...\n");
	SA.loadData_forSearch(argv[1], false, true);

	fprintf(stderr,"Input Sentences:\n");

	while(!cin.eof()){
		cin.getline(tmpString,100000,'\n');
		if(strlen(tmpString)>0){
			
			SA.displayNgramMatchingFreq4Sent(tmpString);

			printf("\n");

			int sentLen;
		  
			S_sentSearchTableElement * matchingTable = SA.constructNgramSearchTable4SentWithLCP(tmpString, sentLen);
		  
			//convert this to frequency table
			double * freqTable = (double *) malloc (sizeof(double)*sentLen*sentLen);

			for(unsigned int i=0;i<(sentLen*sentLen);i++){
				//all the short n-grams should all exist and their frequency information should be in table now
				unsigned int startPos, n;
				double minNc;
				int leftNWithMinNc;

				local_oneDimensionTableIndexToTwoDimension(i, sentLen, startPos, n);

				if(matchingTable[i].found){
					double freq = matchingTable[i].endingPosInSA - matchingTable[i].startPosInSA +1; 
					freqTable[i]=freq;

					

					//consider all splitting method
					minNc = DBL_MAX;
					
					for(unsigned int leftN=1;leftN<n;leftN++){
						int index_left = local_twoDimensionIndexToOneDimensionTableIndex(startPos, leftN, sentLen);
						int index_right = local_twoDimensionIndexToOneDimensionTableIndex(startPos+leftN, n-leftN, sentLen);

						double leftFreq = freqTable[index_left];
						double rightFreq = freqTable[index_right];

						double nc = freq*bigN/(leftFreq*rightFreq);

						if(nc<minNc){
							minNc = nc;
							leftNWithMinNc = leftN;
						}

					}					
				}
				else{
					freqTable[i]=0;
					minNc = 0;
				}

				if(startPos==0){
					printf("\n%d\t",n);
				}

				if(n==1){
					printf("A\t");	//atom word, no way to break it
				}
				else{
					if(minNc>0){
						printf("%.1f[%d]\t", minNc, leftNWithMinNc);
					}
					else{
						printf("_\t");
					}
				}
			}

			printf("\n");
		

			free(matchingTable);
			free(freqTable);


	  }
	}
	return 1;
}
