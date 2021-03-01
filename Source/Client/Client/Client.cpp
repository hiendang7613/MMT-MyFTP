#include "client.h"


using namespace std;


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

bool Client::Init()
{
	GotoXY(HistoryPos, 1);
	if (AfxSocketInit() == FALSE)
	{
		cout << "Cannot setup Socket Libraray";
		return false;
	}

	client->conn.Create();
	client2->conn.Create();

	fstream f;
	string ip, port, port2;

	f.open("config.txt", fstream::in);
	if (f.fail()) {
		ip = "127.0.0.1";
		port = "3030";
		port2 = "3031";
	}
	else
	{
		f >> ip;
		f >> port;
		f >> port2;
	}

	if (
		client->conn.Connect(CA2W(&ip[0]), stoi(port))
		&& client2->conn.Connect(CA2W(&ip[0]), stoi(port2))
		)
	{
		statusPortThread = new thread(&Client::StatusThread, this, client2);
		cout << "Successfully Connect to Server!!!";
		GotoLnX(HistoryPos);
		cout << "Press any key to continue..";
		_getch();
		return true;
	}
	else
	{
		cout << "Cannot connect to Server !!!";
		return false;
	}
	
}

void Client::StatusThread(Connector* client2) {

	string s1, s2;
	while (true)
	{
		s1 = client2->Receive().ToString();
		s2 = client2->Receive().ToString();

		if (s1 == "Server" && s2 == "off")
		{
			alive["Server"] = false;
			return;
		}
		history.push_back("user " + s1 + " " + s2);

		if (s2 == "login") 
		{
			alive[s1] = true;
		}
		else 
			if (s2 == "off") 
			{
				alive[s1] = false;
				//if (!alive["Server"]) 
				//{
				//	/*GotoLnX(HistoryPos);
				//	cout << "Server is offline";
				//	GotoLnX(HistoryPos);
				//	cout << "Press any key to quit Client Socket...";
				//	_getch();
				//	Close();*/
				//	return;
				//}
				if (!alive["me"])
					return;
			}
		if (s2[0] == 'U') {

			mu.lock();
			if (s2[6] == 'i') {
				IsClientUploading = true;
			}
			else
			{
				IsClientUploading = false;
			}
			mu.unlock();
			//cout << "zzzzz: " << IsClientUploading << endl;
		}

		mu.lock();
		Refresh_status = true;
		mu.unlock();


	}

}

bool Client::Signup(const string& user, const string& pass)
{
	client->Send((myString)"signup");

	client->Send((myString)user);
	client->Send((myString)pass);

	buf = client->Receive();
	if (buf.ToString() == "ok")
	{
		return true;
	}
	return false;
}

bool Client::Login(const string& user, const string& pass)
{
	client->Send((myString)"login");
	client->Send((myString)user);
	client->Send((myString)pass);

	buf = client->Receive();
	if (buf.ToString() == "ok")
	{
		return true;
	}
	return false;
}

vector<string> Client::GetMyFileList()
{
	vector<string> filenames;
	TCHAR NPath[MAX_PATH];
	int len = GetCurrentDirectory(MAX_PATH, NPath);
	_tcscpy(&NPath[len], _T("\\files"));

	string s;
	for (const auto& file : filesystem::directory_iterator(NPath))
	{
		s = file.path().u8string();
		s = s.substr(wcslen(NPath) + 1);
		filenames.push_back(s);
	}
	return filenames;
}

vector<string> Client::GetFileList()
{
	vector<string> filenames;
	client->Send((myString)"getlist");

	buf = client->Receive();

	if (buf.ToString() == "off") {
		return filenames;
	}
	else if (buf.ToString() == "ok")
	{

	}
	else
	{
		//cout << "Loi duong truyen!!\n";
		return filenames;
	}

	while (1)
	{
		buf = client->Receive();
		if (buf.n < 512) {
			if (buf.ToString() == "end_of_file") {
				break;
			}
		}
		filenames.push_back(buf.ToString());
	}
	return filenames;
}

bool Client::Upload(const string& filename)
{
	while(IsClientUploading) {
		//cout << "Wait another client upload a file ...";
		//Sleep(2000);
	}
	fstream f;
	f.open("files\\" + filename, fstream::in | fstream::binary);
	if (!f.good()) {
		//cout << "Cant open file " + filename + "  for upload \n";
		return false;
	}

	client->Send((myString)"up");
	client->Send(filename);

	buf = client->Receive();
	if (buf.ToString() == "off") {
		return false;
	}
	else if (buf.ToString() == "ok")
	{
		//cout << "                                          ";
		GotoXY(Max_Width / 2 - 8, Max_Height / 2);
		cout << "Uploading ... \n";
	}
	else
	{
		//cout << "Loi duong truyen!!\n";
		return false;
	}

	while (1)
	{
		f.read(buf.s, 512);
		buf.n = f.gcount();
		client->Send(buf);
		if (f.eof()) {
			break;
		}
	}
	f.close();
	client->Send((myString)"end_of_file");

	//cout << "Uploaded file " + filename + "\n";

	return true;
}

bool Client::Download(const string& filename)
{
	try
	{
		fstream f;
		f.open("files\\" + filename, fstream::binary | fstream::out);

		client->Send((myString)"down");
		client->Send(filename);

		buf = client->Receive();
		if (buf.ToString() == "off") {
			return false;
		}
		else if (buf.ToString() == "ok")
		{
			GotoXY(Max_Width / 2-9, Max_Height / 2);
			cout << "Downloading ...";
		}
		else
		{
			//cout << "Loi duong truyen!!\n";
			return false;
		}

		while (1)
		{
			buf = client->Receive();
			if (buf.n < 512) {
				if (buf.ToString() == "end_of_file") {
					break;
				}
			}
			f.write(buf.s, buf.n);
		}
		f.close();
		//cout << "Downloaded file " + filename + "\n";

	}
	catch (const std::exception&)
	{
		//cout << "Cant open file " + filename + "  for download \n";
		return false;
	}

	return true;
}

void Client::Close()
{
	delete buf.s;
	client->Close();
	client2->Close();
	delete _ins;
}

Client* Client::Ins() {
	if (_ins == nullptr) {
		_ins = new Client();
	}
	return _ins;
}
