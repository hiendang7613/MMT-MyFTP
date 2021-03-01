#pragma once
#include <string>

using namespace std;

class myString
{
public:
	char* s;
	int n;


	string ToString() {
		s[n] = '\0';
		return string(s);
	}


	myString() {
		s = new char[513];
		s[0] = 0;
		n = 0;
	}

	myString(const string& s) {
		this->s = (char*)&s[0];
		n = s.size();
	}
};

