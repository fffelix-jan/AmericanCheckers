// #define DEBUG	// Comment this out to hide the console window

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <WinUser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "CheckersConstants.h"
#include "CheckersPieces.h"

int RedPieces[12][2] = RED_START;
bool RedKings[12] = { 0 };
int BluePieces[12][2] = BLUE_START;
bool BlueKings[12] = { 0 };

int PieceSelected = -1;
bool BluePlayerTurn = false;
bool ForcedCapture = false;
bool InChainCapture = false;

bool GamePause = false;

int TimeRemaining = TIME_LIMIT;
int TotalTime;

void display(void);

void logToConsole(const char* st);
void logTime(void);

void setTurn(bool blue);
void processWin(bool isBlue);

// Check who won. Returns 0 if no winner, 1 if red and 2 if blue.
int checkWinner(void)
{
    if (checkRemainingPieces(true) == 0) return 1;
    else if (checkRemainingPieces(false) == 0) return 2;
    else return 0;
}

// Resets the game.
void resetGame(void)
{
    logToConsole("Resetting Board");
    int redStart[12][2] = RED_START;
    memcpy(RedPieces, redStart, sizeof(int[12][2]));
    int blueStart[12][2] = BLUE_START;
    memcpy(BluePieces, blueStart, sizeof(int[12][2]));
    memset(RedKings, 0, sizeof(bool[12]));
    memset(BlueKings, 0, sizeof(bool[12]));
    setTurn(false);
    TotalTime = 0;
    GamePause = false;
    display();
}

// Saves a game to a save file so that the game can be continued at a later time.
// Format: 
// FA_USCHKSV
// TotalTime TimeRemaining PieceSelected BluePlayerTurn InChainCapture
// RedPieces...
// RedKings...
// BluePieces...
// BlueKings...
void saveGame(const char* fileName)
{
    FILE* saveFile;
    if (!(saveFile = fopen(fileName, "w")))
    {
        MessageBox((HWND)GetGraphicsWindowHwnd(), "Error opening save file.", "Save Error", MB_ICONERROR);
    }
    else
    {
        // This includes a header to indicate that it is a valid save file
        fprintf(saveFile, "FA_USCHKSV\n%d %d %d %d %d %d\n", TotalTime, TimeRemaining, PieceSelected, BluePlayerTurn, InChainCapture, ForcedCapture);
        int i;
        for (i = 0; i < 12; ++i)
        {
            fprintf(saveFile, "%d %d ", RedPieces[i][0], RedPieces[i][1]);
        }
        fputc('\n', saveFile);
        for (i = 0; i < 12; ++i)
        {
            fprintf(saveFile, "%d ", RedKings[i]);
        }
        fputc('\n', saveFile);
        for (i = 0; i < 12; ++i)
        {
            fprintf(saveFile, "%d %d ", BluePieces[i][0], BluePieces[i][1]);
        }
        fputc('\n', saveFile);
        for (i = 0; i < 12; ++i)
        {
            fprintf(saveFile, "%d ", BlueKings[i]);
        }
        fputc('\n', saveFile);
        fclose(saveFile);
    }
}

// Loads a saved game.
void loadGame(const char* fileName)
{
    FILE* saveFile;
    if (!(saveFile = fopen(fileName, "r")))
    {
        MessageBox((HWND)GetGraphicsWindowHwnd(), "Error opening save file.", "Open Error", MB_ICONERROR);
    }
    else
    {
        // Check whether it is a checkers save file
        char headerLine[50];
        headerLine[0] = '\0';
        fscanf(saveFile, "%49s", headerLine);
        if (strcmp(headerLine, "FA_USCHKSV") != 0)
        {
            MessageBox((HWND)GetGraphicsWindowHwnd(), "The file you opened is not an American Checkers save file.", "Invalid File", MB_ICONERROR);
            return;
        }
        // Discard the rest of the first line
        while (fgetc(saveFile) != '\n');

        fscanf(saveFile, "%d%d%d", &TotalTime, &TimeRemaining, &PieceSelected);

        // Cast the int to bool
        int temp;
        fscanf(saveFile, "%d", &temp);
        BluePlayerTurn = temp;
        fscanf(saveFile, "%d", &temp);
        InChainCapture = temp;
        fscanf(saveFile, "%d", &temp);
        ForcedCapture = temp;

        int i;
        for (i = 0; i < 12; ++i)
        {
            fscanf(saveFile, "%d%d", &RedPieces[i][0], &RedPieces[i][1]);
        }
        for (i = 0; i < 12; ++i)
        {
            fscanf(saveFile, "%d", &temp);
            RedKings[i] = temp;
        }
        for (i = 0; i < 12; ++i)
        {
            fscanf(saveFile, "%d%d", &BluePieces[i][0], &BluePieces[i][1]);
        }
        for (i = 0; i < 12; ++i)
        {
            fscanf(saveFile, "%d", &temp);
            BlueKings[i] = temp;
        }
        fclose(saveFile);
    }
}

// Prompts to save a game.
void promptSave(void)
{
    bool oldStatus = GamePause;
    GamePause = true;

    OPENFILENAME ofn;
    char fileName[1024];
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = (HWND)GetGraphicsWindowHwnd();
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(fileName);
    ofn.lpstrFilter = "Checkers Savegame Files (*.sav)\0*.SAV\0All Files (*.*)\0*.*\0";
    ofn.lpstrDefExt = "sav";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = "Save Game";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    GetSaveFileName(&ofn);
    if (strlen(ofn.lpstrFile) > 0) saveGame(ofn.lpstrFile);

    GamePause = oldStatus;
}

// Prompts to open a game.
void promptOpen(void)
{
    bool oldStatus = GamePause;
    GamePause = true;

    OPENFILENAME ofn;
    char fileName[1024];
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = (HWND)GetGraphicsWindowHwnd();
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(fileName);
    ofn.lpstrFilter = "Checkers Savegame Files (*.sav)\0*.SAV\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = "Open Game";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    GetOpenFileName(&ofn);
    if (strlen(ofn.lpstrFile) > 0) loadGame(ofn.lpstrFile);

    GamePause = oldStatus;
}

// Logs a string to the console with the date and time.
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

// Logs just the time to the console. This function should be followed by a call to printf ending with a '\n' character.
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

// Opens the Help website.
inline void openHelp(void)
{
    ShellExecute(0, 0, "https://www.felixan.ca", 0, 0, SW_SHOW);
}

// Draws the menu bar at the top of the screen.
void drawMenu()
{
    static char* menuListFile[] = { "File",
        "New Game | Ctrl-N",
        "Open Game | Ctrl-O",
        "Open Replay | Ctrl-R",
        "Save Game | Ctrl-S",
        "Pause/Resume | Ctrl-P",
        "Exit  | Ctrl-Q", };
    static char* menuListHelp[] = { "Help",
        "Documentation...  | F1",
        "About" };
    static char* selectedLabel = NULL;

    double fH = GetFontHeight();
    double x = 0; //fH/8;
    double y = GetWindowHeight();
    double h = fH * 1.5;
    double w = TextStringWidth(menuListHelp[0]) * 2;
    double wlist = TextStringWidth(menuListHelp[1]) * 1.2;
    double xindent = GetWindowHeight() / 20;
    int    selection;

    // menu bar
    drawMenuBar(0, y - h, GetWindowWidth(), h);
    // File
    selection = menuList(GenUIID(0), x, y - h, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
    if (selection > 0) selectedLabel = menuListFile[selection];
    if (selection == 1)
    {
        resetGame();
    }
    if (selection == 2)
    {
        promptOpen();
    }
    if (selection == 4)
    {
        promptSave();
    }
    if (selection == 5)
    {
        GamePause = !GamePause;
        display();
    }
    if (selection == 6)
        ExitGraphics(); // choose to exit

    // Help
    selection = menuList(GenUIID(0), x + 1 * w, y - h, w, wlist, h, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]));
    if (selection > 0) selectedLabel = menuListHelp[selection];
    if (selection == 1)
        openHelp();
    if (selection == 2)
    {
        bool oldStatus = GamePause;
        GamePause = true;
        MessageBox((HWND)GetGraphicsWindowHwnd(), "American Checkers\r\nVersion 1.0.0\r\n\r\nCopyright (c) 2022 Felix An\r\nhttps://www.felixan.ca\r\nLicensed under the MIT License", "About American Checkers", MB_ICONASTERISK);
        GamePause = oldStatus;
    }
}

// Draws the text labels.
void drawLabels(void)
{
    int oldSize = GetPointSize();
    char oldFont[50];
    strcpy(oldFont, GetFont());
    SetPenColor("Black");
    MovePen(2.1, 5.9);
    SetPointSize(60);
    SetFont("Georgia");
    DrawTextString("American Checkers");
    SetPointSize(20);
    MovePen(4.25, 5.9 - GetFontHeight());
    SetFont("Tahoma");
    DrawTextString("by Felix An");
    SetPointSize(oldSize);
    SetFont(oldFont);

    // TODO: hide the labels in playback mode
    MovePen(6, 4.5);
    SetFont("Georgia");
    SetPointSize(30);
    DrawTextString("Current Player:");
    MovePen(6, 4);
    SetPointSize(50);
    SetPenColor(BluePlayerTurn ? "Blue" : "Red");
    DrawTextString(BluePlayerTurn ? "Blue" : "Red");
    SetPenColor("Black");
    MovePen(6, 3.5);
    SetPointSize(30);
    DrawTextString("Time Remaining:");
    MovePen(6, 3);
    SetPointSize(50);
    char timeString[20];
    snprintf(timeString, 20, "%d s", TimeRemaining);
    SetFont("Times New Roman");
    DrawTextString(timeString);
    SetFont("Georgia");
    MovePen(6, 2.5);
    SetPointSize(30);
    DrawTextString("Time Elapsed:");
    MovePen(6, 2);
    SetPointSize(50);
    snprintf(timeString, 20, "%02d:%02d:%02d", TotalTime / 3600, TotalTime % 3600 / 60, TotalTime % 3600 % 60);
    SetFont("Times New Roman");
    DrawTextString(timeString);
    SetFont("Georgia");
    MovePen(6, 1);
    if (GamePause)
    {
        DrawTextString("PAUSED");
    }
    else if (InChainCapture)
    {
        DrawTextString("Keep jumping!");
    }
    else if (ForcedCapture)
    {
        DrawTextString("You must jump.");
    }


    SetPointSize(oldSize);
    SetFont(oldFont);
}

// Draws the checkerboard.
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

// Draws a checkers piece at the current pen location.
void drawCheckersPiece(bool selected, bool king)
{
    MovePen(GetCurrentX() + 0.15, GetCurrentY());
    StartFilledRegion(selected ? 0.5 : 1);
    DrawArc(0.15, 0, 360);
    EndFilledRegion();
    MovePen(GetCurrentX() - 0.15, GetCurrentY());
    if (king)
    {
        MovePen(GetCurrentX() - 0.11, GetCurrentY() - 0.37);
        int oldSize = GetPointSize();
        char oldColor[20];
        strcpy(oldColor, GetPenColor());
        SetPointSize(60);
        SetPenColor("White");
        DrawTextString("*");
        SetPointSize(oldSize);
        SetPenColor(oldColor);
        MovePen(GetCurrentX() + 0.11, GetCurrentY() + 0.37);
    }
}

// Draws the game pieces.
void drawPieces(void)
{
    int oldSize = GetPointSize();
    SetPointSize(30);

    SetPenColor("Red");
    // Draw red pieces
    for (int i = 0; i < 12; i++)
    {
        if (RedPieces[i][0] != -1 && RedPieces[i][1] != -1)
        {
            MovePen(boardToWindowX(RedPieces[i][0]) + 0.25, boardToWindowY(RedPieces[i][1]) + 0.25);
            // If it is a king, draw the letter K
            if (RedKings[i])
            {
                if (!BluePlayerTurn && PieceSelected == i)
                    drawCheckersPiece(true, true);
                else
                    drawCheckersPiece(false, true);
            }
            else
            {
                if (!BluePlayerTurn && PieceSelected == i)
                    drawCheckersPiece(true, false);
                else
                    drawCheckersPiece(false, false);
            }
        }
    }

    SetPenColor("Blue");
    // Draw blue pieces
    for (int i = 0; i < 12; i++)
    {
        if (BluePieces[i][0] != -1 && BluePieces[i][1] != -1)
        {
            MovePen(boardToWindowX(BluePieces[i][0]) + 0.25, boardToWindowY(BluePieces[i][1]) + 0.25);
            // If it is a king, draw the letter K
            if (BlueKings[i])
            {
                if (BluePlayerTurn && PieceSelected == i)
                    drawCheckersPiece(true, true);
                else
                    drawCheckersPiece(false, true);
            }
            else
            {
                if (BluePlayerTurn && PieceSelected == i)
                    drawCheckersPiece(true, false);
                else
                    drawCheckersPiece(false, false);
            }
        }
    }
    SetPointSize(oldSize);
}

// Sets the turn, resets the timer, and checks for other conditions.
void setTurn(bool blue)
{
    PieceSelected = -1;
    InChainCapture = false;
    ForcedCapture = false;
    bool movePossible = false;
    int whoWon = checkWinner();
    if (whoWon == 1)
    {
        display();
        processWin(false);
        return;
    }
    else if (whoWon == 2)
    {
        display();
        processWin(true);
        return;
    }
    TimeRemaining = TIME_LIMIT;
    if (blue)
    {
        BluePlayerTurn = true;
        for (int i = 0; i < 12; ++i)
        {
            if (onGrid(BluePieces[i][0], BluePieces[i][1]))
            {
                if (checkMovePossible(true, BluePieces[i][0], BluePieces[i][1], BlueKings[i])) movePossible = true;
                if (checkCapturePossible(true, BluePieces[i][0], BluePieces[i][1], BlueKings[i]))
                {
                    ForcedCapture = true;
                    display();
                    break;
                }
            }
        }
        if (!movePossible)
        {
            display();
            GamePause = true;
            MessageBox((HWND)GetGraphicsWindowHwnd(), "No moves are possible. Blue has lost the game.", "No Moves Possible", MB_ICONEXCLAMATION);
            processWin(false);
        }
    }
    else // if red
    {
        BluePlayerTurn = false;
        for (int i = 0; i < 12; ++i)
        {
            if (onGrid(RedPieces[i][0], RedPieces[i][1]))
            {
                if (checkMovePossible(false, RedPieces[i][0], RedPieces[i][1], RedKings[i])) movePossible = true;
                if (checkCapturePossible(false, RedPieces[i][0], RedPieces[i][1], RedKings[i]))
                {
                    ForcedCapture = true;
                    display();
                    break;
                }
            }
        }
        if (!movePossible)
        {
            display();
            GamePause = true;
            MessageBox((HWND)GetGraphicsWindowHwnd(), "No moves are possible. Red has lost the game.", "No Moves Possible", MB_ICONEXCLAMATION);
            processWin(true);
        }
    }
}

// Processes a move when the board is clicked.
void processMovePieceClick(int x, int y)
{
    logTime();
    printf("Player %d Piece selected: %d\n", BluePlayerTurn, getPieceIndex(BluePlayerTurn, x, y));
    printf("Is it possible to move from this location? %s as man, %s as king\n", checkMovePossible(BluePlayerTurn, x, y, false) ? "true" : "false", checkMovePossible(BluePlayerTurn, x, y, true) ? "true" : "false");

    // Return if outside of the board range
    if (!onGrid(x, y)) return;

    if (PieceSelected == -1)
    {
        PieceSelected = getPieceIndex(BluePlayerTurn, x, y);
        // Deselect the piece if forced capture is active and it is not possible to capture
        if (ForcedCapture && !checkCapturePossible(BluePlayerTurn, x, y, BluePlayerTurn ? BlueKings[PieceSelected] : RedKings[PieceSelected]))
        {
            PieceSelected = -1;
        }
    }
    else
    {
        if (BluePlayerTurn)
        {
            // same square, deselect the piece
            if (BluePieces[PieceSelected][0] == x && BluePieces[PieceSelected][1] == y && !InChainCapture)
            {
                PieceSelected = -1;
                return;
            }
            // regular move forward
            else if (!InChainCapture && !ForcedCapture && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && (x == BluePieces[PieceSelected][0] + 1 || x == BluePieces[PieceSelected][0] - 1) && y == BluePieces[PieceSelected][1] - 1)
            {
                BluePieces[PieceSelected][0] = x;
                BluePieces[PieceSelected][1] = y;
                if (BluePieces[PieceSelected][1] == 0) BlueKings[PieceSelected] = true;
                setTurn(false);
                return;
            }
            // king move backward
            else if (!InChainCapture && !ForcedCapture && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && (x == BluePieces[PieceSelected][0] + 1 || x == BluePieces[PieceSelected][0] - 1) && y == BluePieces[PieceSelected][1] + 1 && BlueKings[PieceSelected])
            {
                BluePieces[PieceSelected][0] = x;
                BluePieces[PieceSelected][1] = y;
                setTurn(false);
                return;
            }
            // capture left move forward
            else if (getPieceIndex(false, x + 1, y + 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == BluePieces[PieceSelected][0] - 2 && y == BluePieces[PieceSelected][1] - 2)
            {
                BluePieces[PieceSelected][0] = x;
                BluePieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(false, x + 1, y + 1);
                RedPieces[toCapture][0] = -1;
                RedPieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);
                if (BluePieces[PieceSelected][1] == 0)
                {
                    BlueKings[PieceSelected] = true;
                    setTurn(false);
                    return;
                }

                // check if chain capture is possible
                if (checkCapturePossible(true, x, y, BlueKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(false);
                    return;
                }
            }
            // capture right move forward
            else if (getPieceIndex(false, x - 1, y + 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == BluePieces[PieceSelected][0] + 2 && y == BluePieces[PieceSelected][1] - 2)
            {
                BluePieces[PieceSelected][0] = x;
                BluePieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(false, x - 1, y + 1);
                RedPieces[toCapture][0] = -1;
                RedPieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);
                if (BluePieces[PieceSelected][1] == 0)
                {
                    BlueKings[PieceSelected] = true;
                    setTurn(false);
                    return;
                }

                // check if chain capture is possible
                if (checkCapturePossible(true, x, y, BlueKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(false);
                    return;
                }
            }
            // king capture left move backward
            else if (getPieceIndex(false, x + 1, y - 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == BluePieces[PieceSelected][0] - 2 && y == BluePieces[PieceSelected][1] + 2 && BlueKings[PieceSelected])
            {
                BluePieces[PieceSelected][0] = x;
                BluePieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(false, x + 1, y - 1);
                RedPieces[toCapture][0] = -1;
                RedPieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);

                // check if chain capture is possible
                if (checkCapturePossible(true, x, y, BlueKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(false);
                    return;
                }
            }
            // king capture right move backward
            else if (getPieceIndex(false, x - 1, y - 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == BluePieces[PieceSelected][0] + 2 && y == BluePieces[PieceSelected][1] + 2 && BlueKings[PieceSelected])
            {
                BluePieces[PieceSelected][0] = x;
                BluePieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(false, x - 1, y - 1);
                RedPieces[toCapture][0] = -1;
                RedPieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);

                // check if chain capture is possible
                if (checkCapturePossible(true, x, y, BlueKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(false);
                    return;
                }
            }
        }
        else // if red player turn
        {
            // same square, deselect the piece
            if (RedPieces[PieceSelected][0] == x && RedPieces[PieceSelected][1] == y && !InChainCapture)
            {
                PieceSelected = -1;
                return;
            }
            // regular move forward
            else if (!InChainCapture && !ForcedCapture && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && (x == RedPieces[PieceSelected][0] + 1 || x == RedPieces[PieceSelected][0] - 1) && y == RedPieces[PieceSelected][1] + 1)
            {
                RedPieces[PieceSelected][0] = x;
                RedPieces[PieceSelected][1] = y;
                if (RedPieces[PieceSelected][1] == 7) RedKings[PieceSelected] = true;
                setTurn(true);
                return;
            }
            // king move backward
            else if (!InChainCapture && !ForcedCapture && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && (x == RedPieces[PieceSelected][0] + 1 || x == RedPieces[PieceSelected][0] - 1) && y == RedPieces[PieceSelected][1] - 1 && RedKings[PieceSelected])
            {
                RedPieces[PieceSelected][0] = x;
                RedPieces[PieceSelected][1] = y;
                setTurn(true);
                return;
            }
            // capture left move forward
            else if (getPieceIndex(true, x + 1, y - 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == RedPieces[PieceSelected][0] - 2 && y == RedPieces[PieceSelected][1] + 2)
            {
                RedPieces[PieceSelected][0] = x;
                RedPieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(true, x + 1, y - 1);
                BluePieces[toCapture][0] = -1;
                BluePieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);
                if (RedPieces[PieceSelected][1] == 7)
                {
                    RedKings[PieceSelected] = true;
                    setTurn(true);
                    return;
                }

                // check if chain capture is possible
                if (checkCapturePossible(false, x, y, RedKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(true);
                    return;
                }
            }
            // capture right move forward
            else if (getPieceIndex(true, x - 1, y - 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == RedPieces[PieceSelected][0] + 2 && y == RedPieces[PieceSelected][1] + 2)
            {
                RedPieces[PieceSelected][0] = x;
                RedPieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(true, x - 1, y - 1);
                BluePieces[toCapture][0] = -1;
                BluePieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);
                if (RedPieces[PieceSelected][1] == 7)
                {
                    RedKings[PieceSelected] = true;
                    setTurn(true);
                    return;
                }

                // check if chain capture is possible
                if (checkCapturePossible(false, x, y, RedKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(true);
                    return;
                }
            }
            // king capture left move backward
            else if (getPieceIndex(true, x + 1, y + 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == RedPieces[PieceSelected][0] - 2 && y == RedPieces[PieceSelected][1] - 2 && RedKings[PieceSelected])
            {
                RedPieces[PieceSelected][0] = x;
                RedPieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(true, x + 1, y + 1);
                BluePieces[toCapture][0] = -1;
                BluePieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);

                // check if chain capture is possible
                if (checkCapturePossible(false, x, y, RedKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(true);
                    return;
                }
            }
            // king capture right move backward
            else if (getPieceIndex(true, x - 1, y + 1) >= 0 && getPieceIndex(false, x, y) == -1 && getPieceIndex(true, x, y) == -1 && x == RedPieces[PieceSelected][0] + 2 && y == RedPieces[PieceSelected][1] - 2 && RedKings[PieceSelected])
            {
                RedPieces[PieceSelected][0] = x;
                RedPieces[PieceSelected][1] = y;
                int toCapture = getPieceIndex(true, x - 1, y + 1);
                BluePieces[toCapture][0] = -1;
                BluePieces[toCapture][1] = -1;
                PlaySound("ChkCapturePiece.wav", NULL, SND_ASYNC);

                // check if chain capture is possible
                if (checkCapturePossible(false, x, y, RedKings[PieceSelected]))
                {
                    InChainCapture = true;
                    return;
                }
                else
                {
                    setTurn(true);
                    return;
                }
            }
        }
    }
}

// Displays everything and refreshes the screen.
void display(void)
{
    SetFont("Microsoft Sans Serif");
    DisplayClear();
    drawBoard();
    drawPieces();
    drawLabels();
    drawMenu();
    UpdateDisplay();
}

// Processes a win.
void processWin(bool isBlue)
{
    logTime();
    printf("%s Player Wins\n", isBlue ? "Blue" : "Red");

    GamePause = true;
    PlaySound("ChkIntro.wav", NULL, SND_ASYNC);
    char message[300];
    snprintf(message, 300, "%s PLAYER WINS!\r\nThis game lasted for %d h %d min %d s.\r\nThanks for playing!", isBlue ? "BLUE" : "RED", TotalTime / 3600, TotalTime % 3600 / 60, TotalTime % 3600 % 60);
    MessageBox((HWND)GetGraphicsWindowHwnd(), message, "Winner!", MB_ICONASTERISK);

    resetGame();
}

// Processes a mouse event.
void processMouseEvent(int x, int y, int button, int ev)
{
    uiGetMouse(x, y, button, ev);
    switch (ev)
    {
    case BUTTON_DOWN:
        logTime();
        printf("Mouse event at %d %d\n", windowToBoardX(x), windowToBoardY(y));
        logTime();
        printf("Red Piece Index: %d; Blue Piece Index: %d\n", getPieceIndex(false, windowToBoardX(x), windowToBoardY(y)), getPieceIndex(true, windowToBoardX(x), windowToBoardY(y)));
        logTime();
        printf("On Board? %d\n", onGrid(windowToBoardX(x), windowToBoardY(y)));
        if (GamePause == false) processMovePieceClick(windowToBoardX(x), windowToBoardY(y));    // Only respond to clicks if the game is not paused.
        display();
    case BUTTON_DOUBLECLICK:
        break;
    case BUTTON_UP:
        break;
    case MOUSEMOVE:
        break;
    }
    display();
}

// Processes a keyboard event.
void processKeyboardEvent(int key, int ev)
{
    uiGetKeyboard(key, ev);

    // debug stuff
    if (ev == KEY_DOWN)
    {
        switch (key)
        {
        case VK_F1:
            openHelp();
            logToConsole("F1 Key Pressed");
            break;
        case VK_SPACE:
            logToConsole("Space Key Pressed");
            break;
        case VK_RETURN:
            logToConsole("Return Key Pressed");
            break;
        case VK_CONTROL:
            logToConsole("Ctrl Key Pressed");
            break;
        case VK_SHIFT:
            logToConsole("Shift Key Pressed");
            break;
        default:
            logTime();
            if (65 <= key && key <= 90)
            {
                printf("%c Key Pressed\n", key);
            }
            else
            {
                printf("Key %d Pressed\n", key);
            }
            break;
        }
    }

    display();
}

// Process a char event.
void processCharEvent(char ch)
{
    uiGetChar(ch);
    display();
}

// Timers
// TURN_TIMER: A timer to limit the amount of time in a turn.
#define TURN_TIMER 1

// Processes a timer event.
void processTimerEvent(int timerID)
{
    if (timerID == TURN_TIMER && GamePause == false)
    {
        if (TimeRemaining <= 0)
        {
            GamePause = true;
            MessageBox((HWND)GetGraphicsWindowHwnd(), "You ran out of time. Therefore, you have lost the game.", "Time's up!", MB_ICONEXCLAMATION);
            processWin(!BluePlayerTurn);
        }
        else
        {
            --TimeRemaining;
        }
        ++TotalTime;
        display();
    }
}

// Main graphics window function.
void Main()
{
    TotalTime = 0;
    SetWindowTitle("American Checkers");
    InitGraphics();

#ifdef DEBUG
    InitConsole();
    SetConsoleTitle("Checkers Log Window");
#endif // DEBUG

    puts("*** LOG WINDOW ***");

    registerCharEvent(processCharEvent);
    registerMouseEvent(processMouseEvent);
    registerKeyboardEvent(processKeyboardEvent);
    registerTimerEvent(processTimerEvent);

    // Start the turn timer
    startTimer(TURN_TIMER, 1000);
    display();

    // Play the theme song
    PlaySound("ChkIntro.wav", NULL, SND_ASYNC);
}