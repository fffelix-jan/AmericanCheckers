#define _USE_MATH_DEFINES
#define DEBUG	// Comment this out to hide the console window

#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <WinUser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#define boardToWindowX(coord) (double)(coord + 1) / 2.0 + 0.5
#define boardToWindowY(coord) (double)(coord + 1) / 2.0 + 0.5
#define windowToBoardX(coord) (int)(ScaleXInches(x) * 2) - 2
#define windowToBoardY(coord) (int)(ScaleYInches(y) * 2) - 2
#define onGrid(x, y) 0 <= x && x < 8 && 0 <= y && y < 8
#define RED_START { {0, 0}, {0, 2}, {1, 1}, {2, 0}, {2, 2}, {3, 1}, {4, 0}, {4, 2}, {5, 1}, {6, 0}, {6, 2}, {7, 1} }
#define BLUE_START { {0, 6}, {1, 5}, {1, 7}, {2, 6}, {3, 5}, {3, 7}, {4, 6}, {5, 5}, {5, 7}, {6, 6}, {7, 5}, {7, 7} }
#define RESET_KINGS { false, false, false, false, false, false, false, false, false, false, false, false }

int RedPieces[12][2] = RED_START;
bool RedKings[12] = RESET_KINGS;
int BluePieces[12][2] = BLUE_START;
bool BlueKings[12] = RESET_KINGS;

//double Angle = 0;
//void forward(double distance)
//{
//	DrawLine(distance * cos(M_PI / 180 * Angle), distance * sin(M_PI / 180 * Angle));
//}
//
//void turn(double angle)
//{
//	Angle += angle;
//}
//
//void move(double distance)
//{
//	MovePen(GetCurrentX() + distance * cos(M_PI / 180 * Angle), GetCurrentY() + distance * sin(M_PI / 180 * Angle));
//}

void logToConsole(const char* st)
{
	time_t currentTime;
	time(&currentTime);
	struct tm* timeTm;
	timeTm = localtime(&currentTime);
	char timeStr[50];
	strftime(timeStr, 49, "%Y-%m-%d %H:%M:%S", timeTm);
	printf("<%s> %s\n", timeStr, st);
}

void logTime(void)
{
	time_t currentTime;
	time(&currentTime);
	struct tm* timeTm;
	timeTm = localtime(&currentTime);
	char timeStr[50];
	strftime(timeStr, 49, "%Y-%m-%d %H:%M:%S", timeTm);
	printf("<%s> ", timeStr);
}

void drawMenu()
{
	static char* menuListFile[] = { "File",
		"Open  | Ctrl-O", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"Close",
		"Exit   | Ctrl-E" };
	static char* menuListTool[] = { "Tool",
		"Triangle",
		"Circle",
		"Stop Rotation | Ctrl-T" };
	static char* menuListHelp[] = { "Help",
		"About" };
	static char* selectedLabel = NULL;

	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = GetWindowHeight();
	double h = fH * 1.5; // 控件高度
	double w = TextStringWidth(menuListHelp[0]) * 2; // 控件宽度
	double wlist = TextStringWidth(menuListTool[3]) * 1.2;
	double xindent = GetWindowHeight() / 20; // 缩进
	int    selection;

	// menu bar
	drawMenuBar(0, y - h, GetWindowWidth(), h);
	// File 菜单
	selection = menuList(GenUIID(0), x, y - h, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
	if (selection > 0) selectedLabel = menuListFile[selection];
	if (selection == 1)
	{
		OPENFILENAME openFileInfo;
		char *fileName = calloc(1024, sizeof(char));
		ZeroMemory(&openFileInfo, sizeof(OPENFILENAME));

		openFileInfo.lStructSize = sizeof(OPENFILENAME);
		openFileInfo.hwndOwner = NULL; // TODO: fix hwnd
		openFileInfo.lpstrFile = fileName;
		openFileInfo.lpstrFile[0] = '\0';
		openFileInfo.nMaxFile = 1;
		openFileInfo.lpstrFilter = "Checkers Files (*.txt)\0*.TXT\0";
		openFileInfo.nFilterIndex = 1;

		GetOpenFileName(&openFileInfo);
		MessageBox(GetGraphicsWindowHwnd(), openFileInfo.lpstrFileTitle, "You Selected", MB_ICONASTERISK);
		free(fileName);
	}
	if (selection == 3)
		ExitGraphics(); // choose to exit

	// Help 菜单
	menuListHelp[1] = "About";
	selection = menuList(GenUIID(0), x + 2 * w, y - h, w, wlist, h, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]));
	if (selection > 0) selectedLabel = menuListHelp[selection];
	if (selection == 1)
		MessageBox(GetGraphicsWindowHwnd(), "Test Checkers Program", "About Box", MB_ICONEXCLAMATION);

	// 显示菜单运行结果
	x = GetWindowWidth() / 15;
	y = GetWindowHeight() / 8 * 7;
	SetPenColor("Blue");
	drawLabel(x, y -= h, "Most recently selected menu is:");
	SetPenColor("Red");
	drawLabel(x + xindent, y -= h, selectedLabel);
	SetPenColor("Blue");
	//drawLabel(x, y -= h, "Control Variable Status");
	//SetPenColor("Red");
	//drawLabel(x + xindent, y -= h, enable_rotation ? "Rotation Enabled" : "Rotation Disabled");
	//drawLabel(x + xindent, y -= h, show_more_buttons ? "More Buttons" : "Less Button");
}

void drawBoard(void)
{
	SetPenColor("Black");
	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			drawRectangle(i / 2.0 + 0.5, j / 2.0 + 0.5, 0.5, 0.5, (i + ((j + 1) % 2)) % 2);
		}
	}
}

void drawPieces(void)
{
	int oldSize = GetPointSize();
	SetPointSize(36);

	SetPenColor("Red");
	// Draw red pieces
	for (int i = 0; i < 12; i++)
	{
		MovePen(boardToWindowX(RedPieces[i][0]) + 0.1, boardToWindowY(RedPieces[i][1]) + 0.1);
		// If it is a king, draw the letter K
		if (RedKings[i])
		{
			DrawTextString("X");
		}
		else
		{
			DrawTextString("X");
		}
	}

	SetPenColor("Blue");
	// Draw blue pieces
	for (int i = 0; i < 12; i++)
	{
		MovePen(boardToWindowX(BluePieces[i][0]) + 0.1, boardToWindowY(BluePieces[i][1]) + 0.1);
		// If it is a king, draw the letter K
		if (BlueKings[i])
		{
			DrawTextString("X");
		}
		else
		{
			DrawTextString("O");
		}
	}

	SetPointSize(oldSize);
}

void display(void)
{
	SetFont("Microsoft Sans Serif");
	DisplayClear();
	drawBoard();
	drawPieces();
	MovePen(1, 6);
	DrawTextString("Western Checkers");
	MovePen(1, 6 - GetFontHeight());
	DrawTextString("by Felix An");
	drawMenu();
	UpdateDisplay();
}

void processMouseEvent(int x, int y, int button, int ev)
{
	uiGetMouse(x, y, button, ev);
	switch (ev)
	{
	case BUTTON_DOWN:
		logTime();
		printf("Mouse event at %d %d, button=%d\n", windowToBoardX(x), windowToBoardY(y), button);
		//MovePen(ScaleXInches(x), ScaleYInches(y));
		//DrawArc(1, 0, 360);
	case BUTTON_DOUBLECLICK:
		break;
	case BUTTON_UP:
		break;
	case MOUSEMOVE:
		break;
	}
	display();
}

void processKeyboardEvent(int key, int ev)
{
	uiGetKeyboard(key, ev);

	// debug stuff
	if (ev == KEY_DOWN)
	{
		switch (key)
		{
		case VK_SPACE:
			logToConsole("Space Bar Pressed");
			break;
		case VK_RETURN:
			logToConsole("Return Key Pressed");
			break;
		default:
			logTime();
			printf("Key %d Pressed\n", key);
			// Check if Ctrl+O was pressed
			if (GetKeyboardState(VK_CONTROL) && key == 'O')
			{
				logToConsole("Ctrl+O Pressed");
			}

			break;
		}
	}

	display();
}

void processCharEvent(char ch)
{
	uiGetChar(ch);
	display();
}


#define MY_TIMER 1

void processTimerEvent(int timerID)
{
	if (timerID == MY_TIMER)
	{
		display();
	}
}

void Main()
{
	SetWindowTitle("Western Checkers");
	InitGraphics();

#ifdef DEBUG
	InitConsole();
#endif // DEBUG

	puts("*** LOG WINDOW ***");
	MovePen(GetWindowWidth() / 2, GetWindowHeight() / 2);
	// DrawArc(2, 0, 360);

	registerCharEvent(processCharEvent);
	registerMouseEvent(processMouseEvent);
	registerKeyboardEvent(processKeyboardEvent);
	registerTimerEvent(processTimerEvent);

	// startTimer(MY_TIMER, 50);
	display();
}