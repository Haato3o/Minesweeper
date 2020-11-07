#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <thread>

#define BOMB_ICON L'¤'
#define FLAG_ICON L'!'
#define WALL_ICON L'█'
#define HIDDEN_ICON L'░'

using namespace std;

int ScreenWidth = 50;
int ScreenHeight = 20;

struct Slot
{
	wchar_t Character;
	bool IsBomb;
	bool IsShown;
};

Slot* generate_map(int nBombs, int mWidth, int mHeight)
{
	if (nBombs > mWidth * mHeight)
	{
		nBombs = mWidth * mHeight;
	}

	Slot* map = (Slot*)malloc(mWidth * mHeight * sizeof(Slot));

	for (int i = 0; i < mWidth * mHeight; i++)
	{
		map[i].Character = L' ';
		map[i].IsBomb = false;
		map[i].IsShown = false;
	}

	for (int bomb = 0; bomb < nBombs; bomb++)
	{
		int bPos;
		do
		{
			bPos = rand() % (mWidth * mHeight);
		} while (map[bPos].IsBomb);

		map[bPos].IsBomb = true;
		map[bPos].Character = BOMB_ICON;
	}

	for (int x = 0; x < mWidth; x++)
	{
		for (int y = 0; y < mHeight; y++)
		{
			int pos = y * mWidth + x;
			int bCounter = 0;
			
			if (map[pos].IsBomb)
			{
				continue;
			}

			// Check top		
			if (y > 0)
			{
				bCounter += map[(y - 1) * mWidth + x].IsBomb ? 1 : 0;

				if (x > 0)
				{
					bCounter += map[(y - 1) * mWidth + (x - 1)].IsBomb ? 1 : 0;
				}
				if (x < mWidth - 1)
				{
					bCounter += map[(y - 1) * mWidth + (x + 1)].IsBomb ? 1 : 0;
				}
			}

			// Check bottom
			if (y < mHeight - 1)
			{
				bCounter += map[(y + 1) * mWidth + x].IsBomb ? 1 : 0;

				if (x > 0)
				{
					bCounter += map[(y + 1) * mWidth + (x - 1)].IsBomb ? 1 : 0;
				}
				if (x < mWidth - 1)
				{
					bCounter += map[(y + 1) * mWidth + (x + 1)].IsBomb ? 1 : 0;
				}
			}

			// Check left
			if (x > 0)
			{
				bCounter += map[y * mWidth + (x - 1)].IsBomb ? 1 : 0;
			}

			// Check right
			if (x < mWidth - 1)
			{
				bCounter += map[y * mWidth + (x + 1)].IsBomb ? 1 : 0;
			}

			if (bCounter == 0)
			{
				map[pos].Character = L' ';
			} else
			{
				map[pos].Character = (wchar_t)(0x30 + bCounter);

			}

		}
	}

	return map;
}

int main()
{
	// Creates screen buffer
	wchar_t* screen = (wchar_t*)malloc(ScreenWidth * ScreenHeight * sizeof(wchar_t));

	for (int i = 0; i < ScreenWidth * ScreenHeight; i++)
	{
		screen[i] = L' ';
	}

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	DWORD lpNumberOfEventsRead;
	INPUT_RECORD lpBuffer[256];

	// Block selection from console
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD lpMode;
	GetConsoleMode(hInput, &lpMode);
	
	SetConsoleMode(hInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | (lpMode & ~ENABLE_QUICK_EDIT_MODE));

	int mMapWidth = 46;
	int mMapHeight = 16;
	int mNumberBombs = 10;

	Slot* mineMap = generate_map(mNumberBombs, mMapWidth, mMapHeight);

	// Game states
	COORD mPosition{};
	bool IsLeftButtonPressed;
	bool IsRightButtonPressed;

	while (true)
	{
		IsLeftButtonPressed = false;
		IsRightButtonPressed = false;

		// Draw border
		for (int px = 0; px < ScreenWidth * ScreenHeight; px++)
		{
			screen[px] = WALL_ICON;
		}

		// Gets cursor position
		if (ReadConsoleInput(hInput, lpBuffer, 256, &lpNumberOfEventsRead))
		{
			for (int i = 0; i < lpNumberOfEventsRead; i++)
			{
				if (lpBuffer[i].EventType == MOUSE_EVENT)
				{
					MOUSE_EVENT_RECORD e = lpBuffer[i].Event.MouseEvent;
					// Left button press
					if (e.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
					{
						IsLeftButtonPressed = true;
					}
					if (e.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
					{
						IsRightButtonPressed = true;
					}
					mPosition = e.dwMousePosition;
				}
			}
		}
		/*
		if (GetConsoleScreenBufferInfo(hConsole, &lpConsoleScreenBufferInfo))
		{
			mPosition = lpConsoleScreenBufferInfo.dwCursorPosition;
			swprintf_s(screen, 16, L"%d %d", mPosition.X, mPosition.Y);
		}*/
		
		// Draw map
		for (int x = 0; x < mMapWidth; x++)
		{
			for (int y = 0; y < mMapHeight; y++)
			{
				int posMap = y * mMapWidth + x;
				int posScreen = (y + 2) * ScreenWidth + (x + 2);

				Slot currentSlot = mineMap[posMap];

				if (!currentSlot.IsShown)
				{
					screen[posScreen] = HIDDEN_ICON;
				} else
				{
					screen[posScreen] = currentSlot.Character;
				}
			}
		}

		if (IsLeftButtonPressed && screen[mPosition.Y * ScreenWidth + mPosition.X] != WALL_ICON)
		{
			mineMap[(mPosition.Y - 2) * mMapWidth + (mPosition.X - 2)].IsShown = true;
		}

		WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

}
