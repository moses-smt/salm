# compilor program
CC     = g++ 

# directories
#	source code directories
PROGRAM_SRC_ROOT_DIR = ../../Src
SA_SHARED_SRC_DIR = ${PROGRAM_SRC_ROOT_DIR}/Shared
SA_INDEX_SRC_DIR = ${PROGRAM_SRC_ROOT_DIR}/IndexSA
SA_APPLICATION_SRC_DIR = ${PROGRAM_SRC_ROOT_DIR}/SuffixArrayApplications
SA_SEARCH_SRC_DIR = ${SA_APPLICATION_SRC_DIR}/SuffixArraySearch
SA_SEARCH_APP_SRC_DIR = ${SA_SEARCH_SRC_DIR}/Applications
SA_LM_SRC_DIR = ${SA_APPLICATION_SRC_DIR}/SuffixArrayLanguageModel
SA_LM_APP_SRC_DIR = ${SA_LM_SRC_DIR}/Applications
SA_SCAN_SRC_DIR = ${SA_APPLICATION_SRC_DIR}/SuffixArrayScan
SA_SCAN_APP_SRC_DIR = ${SA_SCAN_SRC_DIR}/Applications
SA_UTIL_SRC_DIR = ${PROGRAM_SRC_ROOT_DIR}/Utils

#	directories for object files
OBJ_DIR = Objs
SA_SHARED_OBJ_DIR = ${OBJ_DIR}/Shared
SA_INDEX_OBJ_DIR = ${OBJ_DIR}/Index
SA_SEARCH_OBJ_DIR = ${OBJ_DIR}/Search
SA_SCAN_OBJ_DIR = ${OBJ_DIR}/Scan
SA_LM_OBJ_DIR = ${OBJ_DIR}/LM
SA_UTIL_OBJ_DIR = ${OBJ_DIR}/Utils

#	directories for final binary executables
BIN_DIR = ../../Bin/Linux
SA_INDEX_BIN_DIR = ${BIN_DIR}/Index
SA_SEARCH_BIN_DIR = ${BIN_DIR}/Search
SA_SCAN_BIN_DIR = ${BIN_DIR}/Scan
SA_LM_BIN_DIR = ${BIN_DIR}/LM
SA_UTIL_BIN_DIR = ${BIN_DIR}/Utils

# compilor flags
CFLAGS_O32 = -c -O -I${PROGRAM_SRC_ROOT_DIR}/Shared -I${SA_APPLICATION_SRC_DIR} -I${SA_SEARCH_SRC_DIR} -I${SA_SCAN_SRC_DIR} -I${SA_UTIL_SRC_DIR} -I${SA_LM_SRC_DIR}
CFLAGS_G32 = -c -g -I${PROGRAM_SRC_ROOT_DIR}/Shared -I${SA_APPLICATION_SRC_DIR} -I${SA_SEARCH_SRC_DIR} -I${SA_SCAN_SRC_DIR} -I${SA_UTIL_SRC_DIR} -I${SA_LM_SRC_DIR}
CFLAGS_O64 = -c -O -m64 -I${PROGRAM_SRC_ROOT_DIR}/Shared -I${SA_APPLICATION_SRC_DIR} -I${SA_SEARCH_SRC_DIR} -I${SA_SCAN_SRC_DIR}  -I${SA_UTIL_SRC_DIR} -I${SA_LM_SRC_DIR}

# shared objects
SHARED_OBJ_o32 = \
	${SA_SHARED_OBJ_DIR}/_IDVocabulary.o32	\
	${SA_SHARED_OBJ_DIR}/_String.o32	\

SHARED_OBJ_g32 = \
	${SA_SHARED_OBJ_DIR}/_IDVocabulary.g32	\
	${SA_SHARED_OBJ_DIR}/_String.g32	\

SHARED_OBJ_o64 = \
	${SA_SHARED_OBJ_DIR}/_IDVocabulary.o64	\
	${SA_SHARED_OBJ_DIR}/_String.o64	\

#
# Target to create all 32-bit optimized binaries
allO32 : CREATE_DIR	\
	${SA_INDEX_BIN_DIR}/IndexSA.O32	\
	${SA_SEARCH_BIN_DIR}/FilterDuplicatedSentences.O32	\
	${SA_SEARCH_BIN_DIR}/FrequencyOfNgrams.O32	\
	${SA_SEARCH_BIN_DIR}/CollectNgramFreqCount.O32	\
	${SA_SEARCH_BIN_DIR}/NGramMatchingStat4TestSet.O32	\
	${SA_SEARCH_BIN_DIR}/NgramMatchingFreq4Sent.O32	\
	${SA_SEARCH_BIN_DIR}/NgramTypeInTestSetMatchedInCorpus.O32	\
	${SA_SEARCH_BIN_DIR}/NgramMatchingFreqAndNonCompositionality4Sent.O32	\
	${SA_SEARCH_BIN_DIR}/LocateNgramInCorpus.O32	\
	${SA_SEARCH_BIN_DIR}/LocateEmbeddedNgramsInCorpus.O32	\
	${SA_SEARCH_BIN_DIR}/SampleNGramIns.O32	\
	${SA_SCAN_BIN_DIR}/CalcCountOfCounts.O32	\
	${SA_SCAN_BIN_DIR}/OutputHighFreqNgram.O32	\
	${SA_LM_BIN_DIR}/EvaluateLM.O32	\
	${SA_SCAN_BIN_DIR}/TypeTokenFreqInCorpus.O32	\
	${SA_UTIL_BIN_DIR}/InitializeVocabulary.O32	\
	${SA_UTIL_BIN_DIR}/UpdateUniversalVoc.O32

# Target to createall 32-bit binaries with debugging information
allG32 : CREATE_DIR	\
	${SA_INDEX_BIN_DIR}/IndexSA.G32	\
	${SA_SEARCH_BIN_DIR}/FilterDuplicatedSentences.G32	\
	${SA_SEARCH_BIN_DIR}/FrequencyOfNgrams.G32	\
	${SA_SEARCH_BIN_DIR}/CollectNgramFreqCount.G32	\
	${SA_SEARCH_BIN_DIR}/NGramMatchingStat4TestSet.G32	\
	${SA_SEARCH_BIN_DIR}/NgramMatchingFreq4Sent.G32	\
	${SA_SEARCH_BIN_DIR}/NgramTypeInTestSetMatchedInCorpus.G32	\
	${SA_SEARCH_BIN_DIR}/NgramMatchingFreqAndNonCompositionality4Sent.G32	\
	${SA_SEARCH_BIN_DIR}/LocateNgramInCorpus.G32	\
	${SA_SEARCH_BIN_DIR}/LocateEmbeddedNgramsInCorpus.G32	\
	${SA_SEARCH_BIN_DIR}/SampleNGramIns.G32	\
	${SA_SCAN_BIN_DIR}/CalcCountOfCounts.G32	\
	${SA_SCAN_BIN_DIR}/OutputHighFreqNgram.G32	\
	${SA_SCAN_BIN_DIR}/TypeTokenFreqInCorpus.G32	\
	${SA_LM_BIN_DIR}/EvaluateLM.G32	\
	${SA_UTIL_BIN_DIR}/InitializeVocabulary.G32	\
	${SA_UTIL_BIN_DIR}/UpdateUniversalVoc.G32

# Target to create all 64-bit optimized binaries
allO64 :	CREATE_DIR	\
	${SA_INDEX_BIN_DIR}/IndexSA.O64	\
	${SA_SEARCH_BIN_DIR}/FilterDuplicatedSentences.O64	\
	${SA_SEARCH_BIN_DIR}/FrequencyOfNgrams.O64	\
	${SA_SEARCH_BIN_DIR}/CollectNgramFreqCount.O64	\
	${SA_SEARCH_BIN_DIR}/NGramMatchingStat4TestSet.O64	\
	${SA_SEARCH_BIN_DIR}/NgramMatchingFreq4Sent.O64	\
	${SA_SEARCH_BIN_DIR}/NgramTypeInTestSetMatchedInCorpus.O64	\
	${SA_SEARCH_BIN_DIR}/NgramMatchingFreqAndNonCompositionality4Sent.O64	\
	${SA_SEARCH_BIN_DIR}/LocateNgramInCorpus.O64	\
	${SA_SEARCH_BIN_DIR}/LocateEmbeddedNgramsInCorpus.O64	\
	${SA_SCAN_BIN_DIR}/CalcCountOfCounts.O64	\
	${SA_SCAN_BIN_DIR}/OutputHighFreqNgram.O64	\
	${SA_SCAN_BIN_DIR}/TypeTokenFreqInCorpus.O64	\
	${SA_LM_BIN_DIR}/EvaluateLM.O64	\
	${SA_UTIL_BIN_DIR}/InitializeVocabulary.O64	\
	${SA_UTIL_BIN_DIR}/UpdateUniversalVoc.O64

# Target to create the directory hierarchy needed
CREATE_DIR :
	mkdir -p ${OBJ_DIR}
	mkdir -p ${SA_SHARED_OBJ_DIR}
	mkdir -p ${SA_INDEX_OBJ_DIR}
	mkdir -p ${SA_SEARCH_OBJ_DIR}
	mkdir -p ${SA_SCAN_OBJ_DIR}
	mkdir -p ${SA_LM_OBJ_DIR}
	mkdir -p ${SA_UTIL_OBJ_DIR}
	mkdir -p ${BIN_DIR}
	mkdir -p ${SA_INDEX_BIN_DIR}
	mkdir -p ${SA_SEARCH_BIN_DIR}
	mkdir -p ${SA_SCAN_BIN_DIR}
	mkdir -p ${SA_LM_BIN_DIR}
	mkdir -p ${SA_UTIL_BIN_DIR}

#-------------------------------------------------------------------
# Top level executables 32-bit optimized
#-------------------------------------------------------------------
${SA_INDEX_BIN_DIR}/IndexSA.O32: ${SA_INDEX_OBJ_DIR}/IndexSA.o32 ${SA_INDEX_OBJ_DIR}/_MonoCorpus.o32 ${SHARED_OBJ_o32}
	${CC} -o $@ $^

${SA_SEARCH_BIN_DIR}/%.O32 : ${SA_SEARCH_OBJ_DIR}/%.o32 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o32 ${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.o32 ${SHARED_OBJ_o32}
	${CC} -o $@ $^	

${SA_SCAN_BIN_DIR}/%.O32 : ${SA_SCAN_OBJ_DIR}/%.o32 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o32 ${SA_SCAN_OBJ_DIR}/_SuffixArrayScanningBase.o32 ${SHARED_OBJ_o32}
	${CC} -o $@ $^

${SA_LM_BIN_DIR}/%.O32 : ${SA_LM_OBJ_DIR}/%.o32 ${SA_LM_OBJ_DIR}/_SuffixArrayLanguageModel.o32 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o32 ${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.o32 ${SHARED_OBJ_o32}
	${CC} -o $@ $^

${SA_UTIL_BIN_DIR}/%.O32 : ${SA_UTIL_OBJ_DIR}/%.o32 ${SA_UTIL_OBJ_DIR}/_UniversalVocabulary.o32 ${SHARED_OBJ_o32}
	${CC} -o $@ $^

# 32-bit objects
${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o32 : ${SA_APPLICATION_SRC_DIR}/_SuffixArrayApplicationBase.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.o32 : ${SA_SEARCH_SRC_DIR}/_SuffixArraySearchApplicationBase.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_SCAN_OBJ_DIR}/_SuffixArrayScanningBase.o32 : ${SA_SCAN_SRC_DIR}/_SuffixArrayScanningBase.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_LM_OBJ_DIR}/_SuffixArrayLanguageModel.o32 : ${SA_LM_SRC_DIR}/_SuffixArrayLanguageModel.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_SHARED_OBJ_DIR}/%.o32 : ${SA_SHARED_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_INDEX_OBJ_DIR}/%.o32 : ${SA_INDEX_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_SEARCH_OBJ_DIR}/%.o32 : ${SA_SEARCH_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_SCAN_OBJ_DIR}/%.o32 : ${SA_SCAN_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_LM_OBJ_DIR}/%.o32 : ${SA_LM_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O32} -o $@ $<

${SA_UTIL_OBJ_DIR}/%.o32 : ${SA_UTIL_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O32} -o $@ $<


#-------------------------------------------------------------------
# Top level executables 32-bit debug
#-------------------------------------------------------------------
${SA_INDEX_BIN_DIR}/IndexSA.G32: ${SA_INDEX_OBJ_DIR}/IndexSA.g32 ${SA_INDEX_OBJ_DIR}/_MonoCorpus.g32 ${SHARED_OBJ_g32}
	${CC} -o $@ $^

${SA_SEARCH_BIN_DIR}/%.G32 : ${SA_SEARCH_OBJ_DIR}/%.g32 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.g32 ${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.g32 ${SHARED_OBJ_g32}
	${CC} -o $@ $^	

${SA_SCAN_BIN_DIR}/%.G32 : ${SA_SCAN_OBJ_DIR}/%.g32 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.g32 ${SA_SCAN_OBJ_DIR}/_SuffixArrayScanningBase.g32 ${SHARED_OBJ_g32}
	${CC} -o $@ $^

${SA_LM_BIN_DIR}/%.G32 : ${SA_LM_OBJ_DIR}/%.g32 ${SA_LM_OBJ_DIR}/_SuffixArrayLanguageModel.g32 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.g32 ${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.g32 ${SHARED_OBJ_g32}
	${CC} -o $@ $^

${SA_UTIL_BIN_DIR}/%.G32 : ${SA_UTIL_OBJ_DIR}/%.g32 ${SA_UTIL_OBJ_DIR}/_UniversalVocabulary.g32 ${SHARED_OBJ_g32}
	${CC} -o $@ $^

# 32-bit objects with debug information
${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.g32 : ${SA_APPLICATION_SRC_DIR}/_SuffixArrayApplicationBase.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.g32 : ${SA_SEARCH_SRC_DIR}/_SuffixArraySearchApplicationBase.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_SCAN_OBJ_DIR}/_SuffixArrayScanningBase.g32 : ${SA_SCAN_SRC_DIR}/_SuffixArrayScanningBase.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_LM_OBJ_DIR}/_SuffixArrayLanguageModel.g32 : ${SA_LM_SRC_DIR}/_SuffixArrayLanguageModel.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_SHARED_OBJ_DIR}/%.g32 : ${SA_SHARED_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_INDEX_OBJ_DIR}/%.g32 : ${SA_INDEX_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_SEARCH_OBJ_DIR}/%.g32 : ${SA_SEARCH_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_SCAN_OBJ_DIR}/%.g32 : ${SA_SCAN_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_LM_OBJ_DIR}/%.g32 : ${SA_LM_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_G32} -o $@ $<

${SA_UTIL_OBJ_DIR}/%.g32 : ${SA_UTIL_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_G32} -o $@ $<


#-------------------------------------------------------------------
# Top level executables of 64-bit optimized version
#-------------------------------------------------------------------
${SA_INDEX_BIN_DIR}/IndexSA.O64: ${SA_INDEX_OBJ_DIR}/IndexSA.o64 ${SA_INDEX_OBJ_DIR}/_MonoCorpus.o64 ${SHARED_OBJ_o64}
	${CC} -o $@ $^	

${SA_SEARCH_BIN_DIR}/%.O64 : ${SA_SEARCH_OBJ_DIR}/%.o64 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o64 ${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.o64 ${SHARED_OBJ_o64}
	${CC} -o $@ $^	

${SA_SCAN_BIN_DIR}/%.O64 : ${SA_SCAN_OBJ_DIR}/%.o64 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o64 ${SA_SCAN_OBJ_DIR}/_SuffixArrayScanningBase.o64 ${SHARED_OBJ_o64}
	${CC} -o $@ $^

${SA_LM_BIN_DIR}/%.O64 : ${SA_LM_OBJ_DIR}/%.o64 ${SA_LM_OBJ_DIR}/_SuffixArrayLanguageModel.o64 ${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o64 ${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.o64 ${SHARED_OBJ_o64}
	${CC} -o $@ $^

${SA_UTIL_BIN_DIR}/%.O64 : ${SA_UTIL_OBJ_DIR}/%.o64 ${SA_UTIL_OBJ_DIR}/_UniversalVocabulary.o64 ${SHARED_OBJ_o64}
	${CC} -o $@ $^

# 64-bit objects
${SA_SEARCH_OBJ_DIR}/_SuffixArrayApplicationBase.o64 : ${SA_APPLICATION_SRC_DIR}/_SuffixArrayApplicationBase.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_SEARCH_OBJ_DIR}/_SuffixArraySearchApplicationBase.o64 : ${SA_SEARCH_SRC_DIR}/_SuffixArraySearchApplicationBase.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_SCAN_OBJ_DIR}/_SuffixArrayScanningBase.o64 : ${SA_SCAN_SRC_DIR}/_SuffixArrayScanningBase.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_LM_OBJ_DIR}/_SuffixArrayLanguageModel.o64 : ${SA_LM_SRC_DIR}/_SuffixArrayLanguageModel.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_SHARED_OBJ_DIR}/%.o64 : ${SA_SHARED_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_INDEX_OBJ_DIR}/%.o64 : ${SA_INDEX_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_SEARCH_OBJ_DIR}/%.o64 : ${SA_SEARCH_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_SCAN_OBJ_DIR}/%.o64 : ${SA_SCAN_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_LM_OBJ_DIR}/%.o64 : ${SA_LM_APP_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

${SA_UTIL_OBJ_DIR}/%.o64 : ${SA_UTIL_SRC_DIR}/%.cpp
	${CC} ${CFLAGS_O64} -o $@ $<

# Target to clean all the executables, objects and directories
clean :	
	rm -rf ${OBJ_DIR}/	
	rm -rf ${BIN_DIR}/


