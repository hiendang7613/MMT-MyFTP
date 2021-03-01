#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include <iostream>
#include <afxsock.h>
#include <vector>
#include <map>
#include <thread>
#include <filesystem>

#include "Connector.h"
#include "myString.h"

#define PORT 3030
#define PORT2 3031

using namespace std;

static int Max_Width = 120;
static int Max_Height = 29;

COORD GetPosCursor();

void GotoXY(int x, int y);

void GotoLnX(int x);

class Server
{
	CSocket server;		// port transport data 
	vector<Connector*> connectors;
	vector<thread*> connectorThreads;

	CSocket server2;	// port status server to clients
	vector<Connector*> connectors2;

	int port;
	int port2;

	int numOfClient;
	map<string, string> account;

public:
	map<string, bool> alive;

	void Init();

	void DeleteConnector(Connector* connector);

	void SendStatus(const string& user, const string& status);

	void ConnectorThread(Connector* connector, Connector* connector2);

	bool Signup(const string& user, const string& pass);

	bool Check_ClientLogin(const string& user, const string& pass);

	void GetFileList(Connector* connector);

	bool Upload(Connector* connector, Connector* connector2, const string& filename);

	bool Download(Connector* connector, Connector* connector2, const string& filename);

	bool Command(Connector* connector, Connector* connector2);

	void Close();

#pragma region singleton
private:
	static Server* _ins;

	Server();

	bool GetAccounts(const string& filename);
	bool PutAccounts(const string& filename);

public:
	static Server* Ins();
#pragma endregion

};

