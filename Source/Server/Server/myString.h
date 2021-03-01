#pragma once
#include <string>

using namespace std;

class myString
{
public:
	char* s;
	int n;

	string ToString();

	myString();

	myString(const string& s);
};

