#if !defined(__STRING_H__INCLUDED_)
#define __STRING_H__INCLUDED_

/**
*	Definition of class C_String
* Revision $Rev: 3794 $
* Last Modified $LastChangedDate: 2007-06-29 02:17:32 -0400 (Fri, 29 Jun 2007) $
**/
#include "stdio.h"

class C_String  
{
public:
	
	char getCharAtPos(int) const;
	void clear();
	char * toString() const;
	int length() const;
	void print2stream(FILE *);

	C_String(const C_String & obj1, const C_String & obj2);
	C_String(C_String const&);
	C_String(char *);
	C_String();

	bool operator==(const C_String &) const;
	bool operator!=(const C_String &) const;
	bool operator<(const C_String &) const;
	void operator=(const C_String &strObj2);
	
	void appending(const C_String & obj);
	void appending(const char nextChar);

	virtual ~C_String();

private:
	void freeContent();
	void copy(const C_String &);
	void copy(const C_String &strObj, int copyLen);

	bool hasContent;
	char * content;
};

#endif // !defined(__STRING_H__INCLUDED_)
