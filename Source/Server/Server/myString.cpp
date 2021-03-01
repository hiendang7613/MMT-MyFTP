#include "myString.h"

string myString::ToString() {
	s[n] = '\0';
	return string(s);
}

myString::myString() {
	s = new char[513];
	s[0] = 0;
	n = 0;
}

myString::myString(const string& s) {
	this->s = (char*)&s[0];
	n = s.size();
}
