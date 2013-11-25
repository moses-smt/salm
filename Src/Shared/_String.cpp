/**
* _String.cpp: implementation of the C_String class.
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/

#include "_String.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C_String::C_String()
{
	this->content = (char *) malloc(sizeof(char));
	this->content[0]='\0';
	this->hasContent = true;
}

void C_String::freeContent()
{
	if(this->hasContent){
		this->hasContent = false;
		free(this->content);
	}
}

C_String::~C_String()
{
	this->freeContent();
}

/**
* Copy constructor from a char string
**/
C_String::C_String(char * str1)
{

	this->content = (char *) malloc(sizeof(char)*strlen(str1)+1);
	if(this->content==NULL){
		fprintf(stderr,"Memory allocation error, Quit.\n");
	}

	strcpy(this->content, str1);

	this->hasContent = true;
}


C_String::C_String(C_String const &strObj1)
{
	this->hasContent = false;
	copy(strObj1);
}

C_String::C_String(const C_String & obj1, const C_String & obj2)
{
	this->freeContent();

	int len1 = strlen(obj1.content);
	int len2 = strlen(obj2.content);

	int fullLen = len1+len2;
	this->content = (char *)  malloc(sizeof(char)*len1 + sizeof(char)*len2 + 1);

	if(this->content==NULL){
		fprintf(stderr,"Memory allocation error, Quit.\n");
	}

	char * pointer = (char*) this->content; 
	strcpy(pointer, obj1.content);	//copy first part
	pointer += len1;
	strcpy(pointer, obj2.content);	//copy second part
	
	this->content[fullLen]='\0';

	this->hasContent = true;
}

void C_String::operator=(const C_String &strObj2)
{
	copy(strObj2);
}

void C_String::copy(const C_String &strObj)
{
	this->freeContent();

	this->content = (char *)  malloc(sizeof(char)*strlen(strObj.content)+1);
	if(this->content==NULL){
		fprintf(stderr,"Memory allocation error, Quit.\n");
	}

	strcpy(this->content, strObj.content);
	this->hasContent = true;
}

void C_String::copy(const C_String &strObj, int copyLen)
{
	this->freeContent();

	this->content = (char *)  malloc(sizeof(char)*(copyLen+1) );
	if(this->content==NULL){
		fprintf(stderr,"Memory allocation error, Quit.\n");
	}
	
	for(int i=0;i<copyLen;i++){
		this->content[i]=strObj.getCharAtPos(i);
	}

	this->content[copyLen]='\0';

	this->hasContent = true;
	
}

void C_String::print2stream(FILE *stream)
{
	fprintf(stream, content);
}


int C_String::length() const
{
	if(this->hasContent){
		return strlen(this->content);
	}

	return 0;
}

bool C_String::operator==(const C_String &obj1) const
{
	if(strcmp(this->content, obj1.content)==0){
		return true;
	};

	return false;
}

bool C_String::operator!=(const C_String &obj1) const
{
	if(strcmp(this->content, obj1.content)!=0){
		return true;
	};

	return false;
}

bool C_String::operator<(const C_String &obj1) const
{
	if(strcmp(this->content, obj1.content)<0){
		return true;
	};

	return false;
}

char * C_String::toString() const
{
	return this->content;
}

void C_String::clear()
{
	this->freeContent();

	this->content = (char *) malloc(sizeof(char));
	this->content[0]='\0';
	this->hasContent = true;
}


char C_String::getCharAtPos(int pos) const
{
	if(pos>=this->length()){
		fprintf(stderr,"Can not get char at pos %d, out of bound! Exit.\n", pos);
		exit(0);
	}

	return this->content[pos];
}


void C_String::appending(const C_String &obj)
{
	int len1 = 0;
	
	if(this->hasContent){
		len1 = strlen(this->content);
	}
	
	int len2 = strlen(obj.content);

	int fullLen = len1+len2;

	char * newContent = (char *)  malloc(sizeof(char)*fullLen + 1);
	
	if(newContent==NULL){
		fprintf(stderr,"Memory allocation error, Quit.\n");
	}

	char * pointer = newContent; 
	if(this->hasContent){
		strcpy(pointer, content);	//copy first part
		pointer += len1;
	}

	strcpy(pointer, obj.content);	//copy second part
	newContent[fullLen]='\0';

	//free old content
	this->freeContent();

	//point to new content
	this->content = newContent;

	this->hasContent = true;
}

void C_String::appending(const char nextChar)
{
	int len1 = 0;
	
	if(this->hasContent){
		len1 = strlen(this->content);
	}

	int fullLen = len1+1;

	char * newContent = (char *)  malloc(sizeof(char)*fullLen + 1);
	
	if(newContent==NULL){
		fprintf(stderr,"Memory allocation error, Quit.\n");
	}
	
	strcpy(newContent, content);	//copy first part	

	newContent[len1]=nextChar;	//copy second part
	newContent[fullLen]='\0';

	//free old content
	this->freeContent();

	//point to new content
	this->content = newContent;

	this->hasContent = true;
}
