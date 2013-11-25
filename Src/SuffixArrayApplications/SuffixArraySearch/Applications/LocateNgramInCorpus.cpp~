#include "stdio.h"
#include "stdlib.h"

#include "_SuffixArraySearchApplicationBase.h"

#include <vector>
#include <iostream>

using namespace std;

/**
* \ingroup search
*
* Locate an n-gram in the indexed corpus, return its locations as <sentId, offsetInSent> pairs
* SentID and offset are all 1-based
*
* Note:
*		The offset of the n-gram in a sentence is represented as "char" in the returned structure S_SimplePhraseLocationElement
*		To output it as a number, one needs to cast it to integer type for proper display
*
*
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
int main(int argc, char * argv[]){
	//-----------------------------------------------------------------------------
	//check parameter
	if(argc<2){		
		fprintf(stderr,"\nOutput all the locations of an n-gram in an indexed corpus\n");
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s corpusFileNameStem < list of n-grams\n\n",argv[0]);
		
		exit(-1);
	}

	//-----------------------------------------------------------------------------	

	C_SuffixArraySearchApplicationBase saObj;
	
	//load the indexed corpus with vocabulary(noVoc=false) and with offset(noOffset=false)
	saObj.loadData_forSearch(argv[1], false, false);


	cerr<<"Input N-grams:\n";
	char tmpString[10000];
	while(!cin.eof()){
	  cin.getline(tmpString,10000,'\n');
	  if(strlen(tmpString)>0){
		  vector<S_SimplePhraseLocationElement> locations;

		  locations = saObj.locateExactPhraseInCorpus(tmpString);
		  
		  if(locations.size()==0){
			  cout<<"No occurrences found.\n";
		  }
		  else{
			  for(int i=0;i<locations.size(); i++){
				  cout<<"SentId="<<locations[i].sentIdInCorpus<<" Pos="<<(int)locations[i].posInSentInCorpus<<endl;
			  }
		  }
		  cout<<endl;
	  }
	}

	return 0;
}
