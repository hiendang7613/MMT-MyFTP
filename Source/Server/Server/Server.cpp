#define _CRT_SECURE_NO_WARNINGS

#include "Server.h"
#include <fstream>
#include <windows.h>
#include <conio.h>

using namespace std;

Server* Server::_ins;

COORD GetPosCursor()
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	GetConsoleScreenBufferInfo(out, &cbsi);
	return cbsi.dwCursorPosition;
}
// di chuyển đến vị trí x , y trên màn hình với x là cột y là dòng
void GotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
// di chuyển đến vị trí x , y trên màn hình với x là cột y là dòng
void GotoLnX(int x)
{
	COORD coord;
	coord.X = x;
	coord.Y = GetPosCursor().Y + 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Server::Init()
{
	if (AfxSocketInit() == FALSE)
	{
		cout << "Cannot setup Socket!!\n";
		return;
	}

	server.Create(port, SOCK_STREAM, NULL);
	server.Listen(10);

	server2.Create(port2, SOCK_STREAM, NULL);
	server2.Listen(10);

	cout << "Successfully setup Server\n";
	cout << "Server listening ...\n";

	//Tao mang chua cac socket client
	Connector* connector;
	thread* connectorThread;

	Connector* connector2;
	thread* connectorThread2;


	while (1)
	{
		connector = new Connector;
		connector2 = new Connector;

		server.Accept(connector->conn);
		server2.Accept(connector2->conn);

		if (!alive["Server"]) {
			return;
		}

		connectors.push_back(connector);
		connectors2.push_back(connector2);

		connectorThread = new thread(&Server::ConnectorThread, this, connector, connector2);
		connectorThreads.push_back(connectorThread);

		numOfClient++;
	}
	for (int i = 0; i < numOfClient; i++)
	{
		connectorThreads[i]->join();
	}
}

void Server::ConnectorThread(Connector* connector, Connector* connector2)
{
	string cmd;
	while (1)
	{
		
		if (!alive["Server"]) { return; }

		if (!Command(connector, connector2)) { break; }
		
	}
}

bool Server::Signup(const string& user, const string& pass)
{
	map<string, string>::iterator it;

	it = account.find(user);
	if (it != account.end()) {

		return false;
	}
	account[user] = pass;
	return true;
}

bool Server::Check_ClientLogin(const string& user, const string& pass)
{
	map<string, string>::iterator it;

	it = account.find(user);
	if (it != account.end()) {
		if (pass == it->second) { return true; }
	}
	return false;
}

void Server::GetFileList(Connector* connector)
{
	TCHAR NPath[MAX_PATH];
	int len = GetCurrentDirectory(MAX_PATH, NPath);
	_tcscpy(&NPath[len], _T("\\files"));
	if (alive["Server"]) {
		connector->Send((myString)"ok");
	}
	else {
		//Close();
		return;
	}
	string s;
	for (const auto& file : std::filesystem::directory_iterator(NPath))
	{
		s = file.path().u8string();
		s = s.substr(wcslen(NPath) + 1);
		connector->Send(s);
	}
	connector->Send((myString)"end_of_file");
}

bool Server::Upload(Connector* connector, Connector* connector2, const string& filename)
{
	try
	{
		fstream f;
		f.open("files\\" + filename, fstream::out | fstream::binary);
		if (alive["Server"]) {
			connector->Send((myString)"ok");
		}
		else {
			return false;
		}
		
		SendStatus(connector->user, "Uploading file " + filename);
		while (1)
		{
			connector->buf = connector->Receive();
			if (connector->buf.n < 512) {
				if (connector->buf.ToString() == "end_of_file") {
					break;
				}
			}
			f.write(connector->buf.s, connector->buf.n);
		}
		SendStatus(connector->user, "Uploaded file " + filename);
		string tmp = "user: " + connector->user + "   uploaded file \"" + filename + "\"";
		if (tmp.size() > Max_Width / 2)
		{
			cout << tmp.substr(0, Max_Width / 2 - 1) << endl;
			cout << tmp.substr(Max_Width / 2 -1, tmp.size() - Max_Width / 2)<<endl;
		}
		else
			cout << tmp<<endl;
		
	}
	catch (const std::exception&)
	{
		cout << "user: " + connector->user + " Cant upload file " + filename;

		return false;
	}
	return true;
}

bool Server::Download(Connector* connector, Connector* connector2, const string& filename)
{

	fstream f;
	f.open("files\\" + filename, fstream::binary | fstream::in);
	if (alive["Server"]) {
		connector->Send((myString)"ok");
	}
	else {
		//Close();
		return false;
	}

	if (!f.good()) {
		cout << "user: " + connector->user + " Cant download file " + filename;
		connector->Send((myString)"end_of_file");
		return false;
	}

	while (1)
	{
		f.read(connector->buf.s, 512);
		connector->buf.n = f.gcount();
		connector->Send(connector->buf);
		if (f.eof()) {
			break;
		}
	}

	connector->Send((myString)"end_of_file");

	SendStatus(connector->user, "Downloaded file " + filename);

	string tmp = "user: " + connector->user + "   downloaded file \"" + filename +"\"";
	if (tmp.size() > Max_Width / 2)
	{
		cout << tmp.substr(0, Max_Width / 2 - 1) << endl;
		cout << tmp.substr(Max_Width / 2 - 1, tmp.size() - Max_Width / 2)<<endl;
	}
	else
		cout << tmp << endl;

	return true;
}

void Server::Close()
{
	PutAccounts("account.txt");
	alive["Server"] = false;
	
	for (int i = 0; i < connectors.size(); i++)
	{
		connectors[i]->Send((myString)"Server");
		connectors2[i]->Send((myString)"off");
		connectors[i]->conn.Close();
		connectors2[i]->conn.Close();
		delete connectors[i];
		connectorThreads[i]->join();
		connectorThreads[i]->~thread();
	}
	server.Close();
	server2.Close();
	delete _ins;
}

bool Server::Command(Connector* connector, Connector* connector2)
{
	string cmd, filename;
	cmd = connector->Receive().ToString();

	if (!connector->logined) {

		//====== login / signup =====
		if (cmd == "login")
		{
			string user;
			string pass;

			user = connector->Receive().ToString();
			pass = connector->Receive().ToString();

			if (Check_ClientLogin(user, pass)) {
				connector->Send((myString)"ok");
				connector->login(user, pass);
				alive[user] = true;
				cout << "user: \"" + connector->user + "\" login .\n";
				SendStatus(connector->user, "login");
				connector->logined = true;
				
				int xtmp = GetPosCursor().X;
				int ytmp = GetPosCursor().Y;
				GotoXY(Max_Width/2 + 2, 1);
				for (auto x : alive)
				{
					if (x.second)
					{
						GotoLnX(Max_Width / 2 + 2);
						cout << x.first;
						for (int i = 0; i < 50 - x.first.size(); i++)
							cout << " ";
						
					}
				}
				GotoXY(xtmp, ytmp);

				return true;
			}
			else
			{
				connector->Send((myString)"no");
				return true;
			}
		}
		if (cmd == "signup")
		{
			string user;
			string pass;

			user = connector->Receive().ToString();
			pass = connector->Receive().ToString();

			if (Signup(user, pass)) {
				connector->Send((myString)"ok");
				connector->login(user, pass);
				cout << "user: \"" + connector->user + "\" signup .\n";
				return true;
			}
			else
			{
				connector->Send((myString)"no");
				return true;
			}
		}


	}
	else
	{

		//======= other command ====
		if (cmd == "up")
		{
			filename = connector->Receive().ToString();
			Upload(connector, connector2, filename);
			return true;
		}
		else if (cmd == "down")
		{
			filename = connector->Receive().ToString();
			Download(connector, connector2, filename);
			return true;

		}
		else if (cmd == "getlist")
		{
			GetFileList(connector);
			return true;

		}
		else if (cmd == "off")
		{
			cout << "user: \"" << connector->user << "\" logout \n";
			alive[connector->user] = false;
			SendStatus(connector->user, "off");
			DeleteConnector(connector);
			connector->Close();

			int xtmp = GetPosCursor().X;
			int ytmp = GetPosCursor().Y;
			GotoXY(Max_Width / 2 + 2, 1);
			for (auto x : alive)
			{
				if (x.second)
				{
					GotoLnX(Max_Width / 2 + 2);
					cout << x.first;
					for (int i = 0; i < 50 - x.first.size(); i++)
						cout << " ";
				}
			}
			GotoLnX(Max_Width / 2 + 2);
			cout << "                                                 ";
			GotoXY(xtmp, ytmp);
			return false;
		}
	}
}

void Server::DeleteConnector(Connector* connector) {
	vector<Connector*>::iterator iter = find(connectors.begin(), connectors.end(), connector);
	int n = distance(connectors.begin(), iter);

	connectors2[n]->Close();

	connectors.erase(connectors.begin() + n);
	connectorThreads.erase(connectorThreads.begin() + n);
	connectors2.erase(connectors2.begin() + n);

}

void Server::SendStatus(const string& user, const string& status)
{
	for (int i = 0; i < connectors2.size(); i++)
	{
		connectors2[i]->Send((myString)user);
		connectors2[i]->Send((myString)status);
	}
}


bool Server::GetAccounts(const string& filename) {
	fstream f;
	f.open(filename, fstream::in);
	if (!f.good()) {
		cout << "Can not found " + filename + "\n";
		return false;
	}

	string user;
	string pass;
	while (!f.eof())
	{
		f >> user;
		f >> pass;
		account[user] = pass;
	}
	f.close();
	return true;
}

bool Server::PutAccounts(const string& filename)
{
	fstream f;
	f.open(filename, fstream::out);
	if (!f.good()) {
		cout << "Cant open AccountsFile ( " + filename + " ).\n";
		return false;
	}

	map<string, string>::iterator it;
	for (it = account.begin(); it != account.end(); ++it)
	{
		f << it->first;
		f << '\n';
		f << it->second;
		f << '\n';
	}
	f.close();
	return true;
}

Server* Server::Ins() {
	if (_ins == nullptr) {
		_ins = new Server();
	}
	return _ins;
}

Server::Server() {
	port = PORT;
	port2 = PORT2;

	numOfClient = 0;
	alive["Server"] = true;

	account["root"] = "root";
	GetAccounts("account.txt");
	filesystem::create_directories("./files");
}


