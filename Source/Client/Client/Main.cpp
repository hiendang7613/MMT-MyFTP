#include "Client.h"
#include "Main.h"
#include <fstream>
#include <windows.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


// cố định màn hình
void Fixconsolewindow()
{
	HWND consolewindow = GetConsoleWindow();
	long style = GetWindowLong(consolewindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consolewindow, GWL_STYLE, style);
}

// tắt dấu nháy trên màn hình
void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

// tạo màn hình
void DrawBoard(int Height, int Width)
{
	system("cls");
	int x = 177;
	GotoXY(0, 0);
	for (int i = 0; i < Width; i++)
	{
		cout << char(x);
	}
	GotoXY(0, Height);
	for (int i = 0; i < Width; i++)
	{
		cout << char(x);
	}
	for (int i = 1; i < Height; i++)
	{
		GotoXY(Width / 3, i);
		cout << char(x);
		GotoXY(Width * 2 / 3, i);
		cout << char(x);
	}
}

int DrawMenu1(int Height, int Width)
{
	system("cls");
	int x = 177;
	GotoXY(0, 0);
	for (int i = 0; i < Width; i++)
	{
		cout << char(x);
	}
	GotoXY(0, Height);
	for (int i = 0; i < Width; i++)
	{
		cout << char(x);
	}
	for (int i = 1; i < Height / 2 - 5; i++)
	{
		GotoXY(Width / 3, i);
		cout << char(x);
		GotoXY(Width * 2 / 3, i);
		cout << char(x);
	}
	for (int i = Height / 2 + 5; i < Height; i++)
	{
		GotoXY(Width / 3, i);
		cout << char(x);
		GotoXY(Width * 2 / 3, i);
		cout << char(x);
	}

	GotoXY(Width / 3 - 1, Height / 2 - 5);
	for (int i = 0; i < Width / 3 + 3; i++)
	{
		cout << char(x);
	}
	GotoXY(Width / 3 - 1, Height / 2 + 5);
	for (int i = 0; i < Width / 3 + 3; i++)
	{
		cout << char(x);
	}
	GotoXY(Width / 3 + 3, Height / 2 - 4);
	cout << "1. Log In.";
	GotoLnX(Width / 3 + 3);
	cout << "2. Sign Up.";
	GotoLnX(Width / 3 + 3);
	cout << "0. EXIT!!.";
	GotoLnX(Width / 3 + 3);
	cout << "Choose: ";
	int choice = 0;
	cin >> choice;
	return choice;
}

void DrawMenu(int Height, int Width, Client* c)
{
	bool serverAlive = true;
	bool clientAlive = true;
	bool finishProcess = false;
	char temp;
	int type = 0;
	int count = 0;
	int countMax = 0;
	vector<string> fileOnServer;
	vector<string> fileOnClient;
	vector<string> history;
	do
	{
		finishProcess = false;
		temp = ' ';
		system("cls");
		int x = 177;
		GotoXY(0, 0);
		for (int i = 0; i < Width; i++)
		{
			cout << char(x);
		}
		GotoXY(0, Height - 2);
		for (int i = 0; i < Width; i++)
		{
			cout << char(x);
		}
		GotoXY(0, Height);
		cout << "\t1. EXIT!!\t2. Upload\t3. Download\t4. Refresh";
		for (int i = 1; i < Height - 2; i++)
		{
			GotoXY(Width / 3, i);
			cout << char(x);
			GotoXY(Width * 2 / 3, i);
			cout << char(x);
		}
		/*if (!c->alive["Server"])
		{
			GotoXY(HistoryPos, 1);
			cout << "Server is offline";
			GotoLnX(HistoryPos);
			cout << "Press any key to quit Client Socket...";
			_getch();
			c->Close();
			exit(0);
		}*/
		if (c->alive["Server"] && c->alive["me"])
		{
			GotoXY(ListFilePos, 1);
			cout << "===== List File On Server =====";
			fileOnServer = c->GetFileList();
			if (!c->alive["Server"])
				continue;
			int len = fileOnServer.size();
			for (int i = 0; i < len; i++)
			{
				GotoLnX(ListFilePos);
				if (fileOnServer[i].size() > Max_Width/3 - ListFilePos)
					cout << fileOnServer[i].substr(0, Max_Width/3 - ListFilePos - 4) << "...";
				else
					cout << fileOnServer[i];
			}
			GotoXY(ListMyFilePos, 1);
			cout << "====== List File On Client ======";
			fileOnClient = c->GetMyFileList();
			len = fileOnClient.size();
			for (int i = 0; i < len; i++)
			{
				GotoLnX(ListMyFilePos);
				if (fileOnClient[i].size() > Max_Width*2/3 - ListMyFilePos)
					cout << fileOnClient[i].substr(0, Max_Width*2/3 - ListMyFilePos - 4) << "...";
				else
					cout << fileOnClient[i];
				
			}
			/*GotoXY(HistoryPos, 1);
			cout << "==========History===========";
			history = c->history;
			len = history.size();
			for (int i = 0; i < len; i++)
			{
				GotoLnX(HistoryPos);
				cout << history[i];
			}*/
		}
		if (!c->alive["me"])
		{
			GotoXY(HistoryPos, 1);
			cout << "Successfully unconnected to Server";
			c->Close();
			return;
		}
		int xtmp, ytmp;
		do
		{
			xtmp = GetPosCursor().X;
			ytmp = GetPosCursor().Y;
			GotoXY(HistoryPos, 1);
			cout << "========== History ===========";
			history = c->history;
			int len = history.size();
			for (int i = 0; i < len; i++)
			{
				GotoLnX(HistoryPos);
				if (history[i].size() > Max_Width - HistoryPos)
				{
					cout << history[i].substr(0, Max_Width - HistoryPos -1);
					GotoLnX(HistoryPos);
					cout << history[i].substr(Max_Width - HistoryPos, history[i].size());
				}
				else
					cout << history[i];
				
			}
			if (!c->alive["Server"])
			{
				GotoLnX(HistoryPos);
				cout << "Server is offline";
				GotoLnX(HistoryPos);
				cout << "Press any key to quit Client Socket...";
				_getch();
				c->Close();
				exit(0);
			}
			GotoXY(xtmp, ytmp);
			temp = ' ';
			if (_kbhit())
			{
				while (temp == ' ' | (temp != '1' && temp != '2' && temp != '3' && temp != '4' && temp != 72 && temp != 80 && temp != 13))
					temp = _getch();
			}
			if (temp == '4')
			{
				break;
			}
			if (temp == '1')
			{
				GotoXY(Max_Width / 2 - 5, Max_Height / 2);
				cout << "Exiting...";
				c->client->Send((myString)"off");
				c->alive["me"] = false;
				c->statusPortThread->join();
				c->statusPortThread->~thread();
				finishProcess = true;
			}
			else
				if (temp == '2' && fileOnClient.size() != 0)
				{
					GotoXY(ListMyFilePos - 1, 2);
					cout << ">";
					count = 1;
					countMax = fileOnClient.size();
					type = 2;
					GotoXY(8, Max_Height - 1);
					cout << fileOnClient[count - 1];
					GotoXY(ListMyFilePos - 1, 2);
				}
				else
					if (temp == '3' && fileOnServer.size() != 0)
					{
						GotoXY(ListFilePos - 1, 2);
						cout << ">";
						count = 1;
						countMax = fileOnServer.size();
						type = 3;
						GotoXY(8, Max_Height - 1);
						cout << fileOnServer[count - 1];
						GotoXY(ListFilePos - 1, 2);
					}
					else
						if (temp == 72)
						{
							count--;
							if (count < 1)
								count = 1;
							if (type == 2)
							{

								GotoXY(ListMyFilePos - 1, GetPosCursor().Y);
								cout << " ";
								GotoXY(ListMyFilePos - 1, count + 1);
								cout << ">";
								GotoXY(8, Max_Height - 1);
								cout << fileOnClient[count - 1];
								cout << "                               ";
								GotoXY(ListMyFilePos - 1, count + 1);
							}
							if (type == 3)
							{
								GotoXY(ListFilePos - 1, GetPosCursor().Y);
								cout << " ";
								GotoXY(ListFilePos - 1, count + 1);
								cout << ">";
								GotoXY(8, Max_Height - 1);
								cout << fileOnServer[count - 1];
								cout << "                               ";
								GotoXY(ListFilePos - 1, count + 1);
							}
						}
						else
							if (temp == 80)
							{
								if (type == 2)
									countMax = fileOnClient.size();
								if (type == 3)
									countMax = fileOnServer.size();
								count++;
								if (count > countMax)
									count = countMax;
								if (type == 2)
								{
									GotoXY(ListMyFilePos - 1, GetPosCursor().Y);
									cout << " ";
									GotoXY(ListMyFilePos - 1, count + 1);
									cout << ">";
									GotoXY(8, Max_Height - 1);
									cout << fileOnClient[count - 1];
									cout << "                               ";
									GotoXY(ListMyFilePos - 1, count + 1);
								}
								if (type == 3)
								{
									GotoXY(ListFilePos - 1, GetPosCursor().Y);
									cout << " ";
									GotoXY(ListFilePos - 1, count + 1);
									cout << ">";
									GotoXY(8, Max_Height -1);
									cout << fileOnServer[count - 1];
									cout << "                               ";
									GotoXY(ListFilePos - 1, count + 1);
								}
							}
							else
								if (temp == 13 && type == 2)
								{
									if (c->Upload(fileOnClient[count - 1]))
									{
									}
									finishProcess = true;
									type = 0;
								}
								else
									if (temp == 13 && type == 3)
									{
										if (c->Download(fileOnServer[count - 1]))
										{
										}
										finishProcess = true;
										type = 0;
									}
		} while (!finishProcess);
	} while (1);
}

int main()
{
	Fixconsolewindow();
	ShowConsoleCursor(false);
	DrawBoard(Max_Height, Max_Width);

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
	}
	else
	{

		Client* client = Client::Ins();

		if (client->Init()) {

			while (1)
			{
				int choice = DrawMenu1(Max_Height, Max_Width);
				if (choice == 1) {
					if (Login(client)) {
						DrawMenu(Max_Height, Max_Width, client);
					}
				}
				else if (choice == 2) {
					Signup(client);
				}
				else
				{
					client->Close();
				}

				if (!client->alive["Server"]) {
					break;
				}
			}
		}
		GotoLnX(HistoryPos);
		cout << "Bye!!";
	}
	_getch();
	return 0;
}

void Signup(Client* c)
{
	GotoLnX(Max_Width / 3 + 3);
	cout << "========= SIGNUP =========";
	string user;
	string pass;
	GotoLnX(Max_Width / 3 + 3);
	cout << "Username : ";
	cin >> user;
	GotoXY(Max_Width / 3 + 3, GetPosCursor().Y);
	cout << "Password : ";
	cin >> pass;
	GotoXY(Max_Width / 3 + 3, GetPosCursor().Y);
	if (c->Signup(user, pass))
	{
		cout << "Successfull SignUp !! Press any key to continue";
	}
	else
	{
		cout << "Username has already existed!! Press any key to continue";
	}
	_getch();
}

bool Login(Client* c)
{
	bool flag = false;
	GotoLnX(Max_Width / 3 + 3);
	cout << "========= LOGIN =========";
	string user;
	string pass;
	GotoLnX(Max_Width / 3 + 3);
	cout << "Username : ";
	cin >> user;
	GotoXY(Max_Width / 3 + 3, GetPosCursor().Y);
	cout << "Password : ";
	cin >> pass;
	GotoXY(Max_Width / 3 + 3, GetPosCursor().Y);
	if (c->Login(user, pass))
	{
		cout << "Successfully LogIn !! Press any key to continue";
		flag = true;
	}
	else
	{
		cout << "Unsuccessfully LogIn !!! Press any key to continue";
	}
	_getch();
	return flag;
}