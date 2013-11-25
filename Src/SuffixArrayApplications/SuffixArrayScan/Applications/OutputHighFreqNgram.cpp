#include "_SuffixArrayScanningBase.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

/**
* Output n-gram types that have frequencies equal or higher than specified
*
*
*	CfgFile Format:
*       n1<tab>freq thresh for output n1-gram
*       n2<tab>freq thresh for output n2-gram
*       ... ... ...
*        n1<tab>freq thresh for output n1-gram
*
* Revision $Rev: 3665 $
* Last Modified $LastChangedDate: 2007-06-16 15:40:59 -0400 (Sat, 16 Jun 2007) $
**/
int main(int argc, char * argv[]){
	//-----------------------------------------------------------------------------
	//check parameter
	//-----------------------------------------------------------------------------
	if(argc<3){
		fprintf(stderr,"\nUsage:\n");
		fprintf(stderr,"\n%s fileNameStem cfgFile\n\n",argv[0]);
	
		fprintf(stderr,"\n\tCfgFile Format:");
		fprintf(stderr,"\n\t\tn1<tab>freq thresh for output n1-gram");
		fprintf(stderr,"\n\t\tn2<tab>freq thresh for output n2-gram");
		fprintf(stderr,"\n\t\t... ... ...");
		fprintf(stderr,"\n\t\tn1<tab>freq thresh for output n1-gram\n");

		
		exit(0);
	}
	
	//processing the threshold file
	map<int, unsigned int> threshMap;
	map<int, unsigned int>::iterator iterThreshMap;
	fstream threshFile;
	threshFile.open(argv[2]);
	int n;
	int maxN = 0;
	unsigned int thresh;	
	while(! threshFile.eof()){
		threshFile>>n>>thresh;
		if(n>maxN){
			maxN=n;
		}
		iterThreshMap = threshMap.find(n);
		if(iterThreshMap==threshMap.end()){
			threshMap.insert(make_pair(n,thresh));	//a little over-kill here, should have a well defined cfg file
		}
	}

	C_SuffixArrayScanningBase saObj(argv[1], maxN);
	iterThreshMap = threshMap.begin();
	while(iterThreshMap!=threshMap.end()){
		saObj.setNgramOutputFreqThresh(iterThreshMap->first, iterThreshMap->second);
		iterThreshMap++;
	}

	saObj.scanSuffixArrayForHighFreqNgramType();

	return 1;
}
