#define _USE_MATH_DEFINES
#define DEBUG

#include <math.h>
#include <time.h>
#include <Windows.h>
#include <WinUser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

double Angle = 0;

void forward(double distance)
{
	DrawLine(distance * cos(M_PI / 180 * Angle), distance * sin(M_PI / 180 * Angle));
}

void turn(double angle)
{
	Angle += angle;
}

void move(double distance)
{
	MovePen(GetCurrentX() + distance * cos(M_PI / 180 * Angle), GetCurrentY() + distance * sin(M_PI / 180 * Angle));
}

inline bool onGrid(int x, int y)
{
	return 0 <= x && x < 8 && 0 <= y && y < 8;
}

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

void drawMenu(void)
{
	static char* menuListFile[] = { "File",
		"Open  | Ctrl-O", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"Close",
		"Exit   | Ctrl-E" };
	static char* menuListHelp[] = { "Help",
	"About" };
	static char* selectedLabel = NULL;

	double fontHeight = GetFontHeight();
	double x = 0;
	double y = GetWindowHeight();
	double h = fontHeight * 1.5;
	double w = TextStringWidth(menuListHelp[0]) * 2;
	double wlist = TextStringWidth(menuListFile[3]) * 1.75;
	double xindent = GetWindowHeight() / 20;
	int selection;

	drawMenuBar(0, y - h, GetWindowWidth(), h);

	selection = menuList(GenUIID(0), x, y - h, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
	if (selection > 0) selectedLabel = menuListFile[selection];
	switch (selection)
	{
	case 1:
		MessageBox(NULL, "Not implemented yet.", "Not Implemented", MB_ICONHAND);
		break;
	case 3:
		ExitGraphics();
		break;
	default:
		break;
	}
}


void drawBoard(void)
{
	//for (int i = 1; i <= 9; i++)
	//{
	//	MovePen(i / 2.0 + 0.5, 1);
	//	DrawLine(0, 4);
	//}
	//for (int i = 1; i <= 9; i++)
	//{
	//	MovePen(1, i / 2.0 + 0.5);
	//	DrawLine(4, 0);
	//}
	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			drawRectangle(i / 2.0 + 0.5, j / 2.0 + 0.5, 0.5, 0.5, (i + ((j + 1) % 2)) % 2);
		}
	}
}

void display(void)
{
	drawBoard();
	MovePen(1, 6);
	DrawTextString("Western Checkers");
	MovePen(1, 6 - GetFontHeight());
	DrawTextString("by Felix An");
	// drawMenu();
}

void processMouseEvent(int x, int y, int button, int event)
{
	uiGetMouse(x, y, button, event);
	switch (event)
	{
	case BUTTON_DOWN:
		logTime();
		printf("Mouse event at %d %d, button=%d\n", (int)(ScaleXInches(x) * 2) - 2, (int)(ScaleYInches(y) * 2) - 2, button);
		//MovePen(ScaleXInches(x), ScaleYInches(y));
		//DrawArc(1, 0, 360);
	case BUTTON_DOUBLECLICK:
		break;
	case BUTTON_UP:
		break;
	case MOUSEMOVE:
		break;
	}
	drawBoard();
	drawMenu();
}

void processKeyboardEvent(int key, int event)
{
	if (event == KEY_DOWN)
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
	registerMouseEvent(processMouseEvent);
	registerKeyboardEvent(processKeyboardEvent);
	registerTimerEvent(processTimerEvent);

	// startTimer(MY_TIMER, 50);
	display();
}