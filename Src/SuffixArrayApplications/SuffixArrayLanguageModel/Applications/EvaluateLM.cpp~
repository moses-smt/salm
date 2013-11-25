#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "_SuffixArrayLanguageModel.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

/**
* A simple example of using the C_SuffixArrayLanguageModel class to calculate the LM prob of input sentences
*
* Revision $Rev: 3816 $
* Last Modified $LastChangedDate: 2007-07-06 14:36:11 -0400 (Fri, 06 Jul 2007) $
**/
int main(int argc, char * argv[]){
	if(argc<2){
		cerr<<"\nUsage:\n\t"<<argv[0]<<" configurationFileName < sentences\n";
		exit(0);
	}

	C_SuffixArrayLanguageModel salm(argv[1]);

	long ltime1, ltime2;
	time( &ltime1 );

	string aWord;
	char aLine[10240];
	while(!cin.eof()){
		cin.getline(aLine, 10240, '\n');

		if(strlen(aLine)>0){
			istringstream inputLine(aLine, istringstream::in);
			LMState lmState = salm.beginOfSentenceState();

			LMState nextState;
			double logProb = 0;

			while(! inputLine.eof()){
				inputLine>>aWord;
				if(aWord.length()>0){
					IndexType vocId = salm.returnVocId(C_String((char *) aWord.c_str()));
					logProb+=salm.logProb(lmState, vocId, nextState);
					lmState = nextState;
				}
				aWord="";
			}

			logProb+=salm.logProbEnd(lmState);
			cout<<"LogProb="<<logProb<<endl;
			
		}

		aLine[0]=0;
	}

	time( &ltime2 );	
	cerr<<"\n"<<ltime2-ltime1<<" seconds spent."<<endl;

	return 1;
}
