#pragma once

#include <filesystem>
#include <afxsock.h>
#include <iostream>
#include <vector>
#include <string>
#include "myString.h"
#include <fstream>
#include <thread>
#include <map>
#include "Connector.h"
#include <mutex>
#include <conio.h>


#include <atomic>


#define PORT 3030
#define PORT2 3031

using namespace std;

static mutex mu;
static int Max_Width = 120;
static int Max_Height = 29;
static int ListFilePos = 2;
static int HistoryPos = Max_Width * 2 / 3 + 3;
static int ListMyFilePos = Max_Width / 3 + 3;

void GotoXY(int x, int y);
void GotoLnX(int x);
COORD GetPosCursor();


class Client {
	myString buf;

public:
	Connector* client;
	Connector* client2;
	thread *statusPortThread;

	map<string, bool> alive; 
	vector<string> history;
	atomic<bool> Refresh_status;

	atomic<bool> IsClientUploading;



	void StatusThread(Connector* client2);

	bool Init();

	bool Signup(const string& user, const string& pass);

	bool Login(const string& user, const string& pass);

	vector<string> GetMyFileList();

	vector<string> GetFileList();

	bool Upload(const string& filename);

	bool Download(const string& filename);

	void Close();


#pragma region singleton
private:
	static Client* _ins;

	Client() {

		client = new Connector;
		client2 = new Connector;
		filesystem::create_directories("./files");
		alive["Server"] = true;
		alive["me"] = true;
		Refresh_status = false;
		IsClientUploading = false;

	}


public:
	static Client* Ins();
#pragma endregion

};


