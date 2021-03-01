#include <afxsock.h>
#include "Server.h"
#include <conio.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

void ShowConsoleCursor(bool);

void DrawBoard(int Height, int Width);

void IsExit(Server* server);

void Fixconsolewindow();

CWinApp theApp;



int main()
{
	Fixconsolewindow();
	ShowConsoleCursor(false);
	DrawBoard(Max_Height, Max_Width);
	GotoXY(0, 1);
	HMODULE hModule = GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			cout << "Fatal Error: MFC initialization failed\n";
		}
		else
		{	
			Server* server = Server::Ins();
			thread isExit(IsExit, server);
			server->Init();
			isExit.join();
			isExit.~thread();
		}
	}
	else
	{
		cout << "Fatal Error: GetModuleHandle failed\n";
	}

	return 0;
}


void IsExit(Server* server) 
{
	int c = 0;
	while (c!=27) 
	{
		if (_kbhit())
		{
			c = _getch();
			if (c == 27) {
				server->SendStatus("Server", "off");
				server->alive["Server"] = false;
				cout << "Bye!!";
				server->Close();
			}
		}
	}
}


// cố định màn hình
void Fixconsolewindow()
{
	HWND consolewindow = GetConsoleWindow();
	long style = GetWindowLong(consolewindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME) & ~(WS_VSCROLL);
	SetWindowLong(consolewindow, GWL_STYLE, style);
}

// tắt dấu nháy trên màn hình
void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

// tạo màn hình
void DrawBoard(int Height, int Width)
{
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
		GotoXY(Width / 2, i);
		cout << char(x);
	}
	GotoXY(Width / 2 + 2, 1);
	cout << "========== Online List =============";
}