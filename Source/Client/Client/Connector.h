#pragma once
#include <afxsock.h>
#include "myString.h"

using namespace std;

class Connector
{
	myString buf;
public:

	CSocket conn;

	void Send(const myString& data);

	myString Receive();

	Connector() {
	}

	void Close() {
		//delete buf.s;
		conn.Close();
	}
};

