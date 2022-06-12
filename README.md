# American Checkers
 A checkers board game for Windows made in C using libgraphics by Zhejiang University.

 Download link: https://github.com/fffelix-jan/AmericanCheckers/releases/download/v1.0.0/AmericanCheckers-1.0.0.zip

## Summary
This program is a version of the classic American checkers (also known as English draughts) board game for Windows. It allows the user to play checkers with a friend sitting beside them. It also enables the user to save partially completed games so that they can be continued later on, and it also has a recording feature to allow users to record and replay games.

## Analysis of requirements
In this project, the goal is to make a user-friendly checkers game that can be easily played by computer users of all ages. Checkers is a popular board game in North America, and this project aims to create a checkers game that can be played on new and old Windows computers.

## Development and design
The program was created using libgraphics and SimpleGUI by Zhejiang University, which I believe is based on the ancient Borland Graphics Interface. The checkerboard is drawn first and is then overlayed with the checkers, the asterisks to indicate that a piece is a king, and the various labels and menus on-screen. Each square on the board is half an "inch" in `graphics.h`, which makes it easy to convert between board coordinates and screen coordinates. The squares on the board are numbered 0, 1, ..., 7 internally. When the board is clicked, the click location is converted to board coordinates, and the function which processes move clicks is called. The function determines whether a piece was previously selected and selects a piece if not, and whether or not the player is allowed to move to the desired destination. All of the positions of the pieces are stored in `int` arrays, accessed by its index in the array. A separate `bool` array with corresponding indices stores whether or not a piece is a king. Functions are included that use the "open file" and "save file" dialogs from the Win32 api to prompt the user to open or save a partially-completed game. The save files themselves (which have a `.SAV` extension, short for "save") consists of a header line `FA_USCHKSV` and space-separated integers in the lines below. These space-separated integers contain various game parameters and the content of the pieces and kings arrays. If the user chooses to record the game, the pieces and kings arrays are written to a file at each step. Replay files end with a `.CKH` extension (short for "checkers huifang"), and start with the line `FA_USCHKRP`.

## Installation
1. On a computer running Windows XP or newer with the Visual C++ Redistributable 2015-2019 installed (Windows 7 and up has it buit-in), *EXTRACT* the contents of the `.zip` file. The two `.wav` files *MUST* be in the same folder as the `.exe` file. (You must extract all the files, otherwise the program will not have music and sound effects.) The VC++ redistributable (x86 version) is included if you don't have it. It works on Windows XP or newer.
2. Turn on your sound.
3. Run the `.exe` file.

## Note for compiling
If you want to compile this project yourself, make sure you have the `Visual Studio 2017 - Windows XP (v141_xp)` toolset in Visual Studio 2019, so that the compiled program will work on XP. (This serves no practial purpose other than nostalgia. I grew up playing MSN Checkers on Windows XP, so I wanted to re-live those days by making a program that works on XP. Don't worry, the program will work just as well on the latest versions of Windows.) If there is garbage text, make sure the system text encoding for non-Unicode programs is set to `Chinese (Simplified, Singapore)`. This is due to the fact that parts of simpleGUI were written in Chinese.

## How to Play
See: https://www.felixan.ca/AmericanCheckersHowToPlay.html