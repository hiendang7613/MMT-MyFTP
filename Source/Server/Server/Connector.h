#pragma once
#include <afxsock.h>
#include "myString.h"

using namespace std;

class Connector
{
public:
	myString buf;
	string user;
	string pass;
	bool logined;

	CSocket conn;

	void login(string user, string pass);

	void Send(const myString& data);

	myString Receive();

	void Close();

	Connector();
};

