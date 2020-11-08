#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <thread>

#define BOMB_ICON L'¤'
#define FLAG_ICON L'!'
#define WALL_ICON L'█'
#define HIDDEN_ICON L'░'
#define EMPTY_ICON L' '

using namespace std;

int ScreenWidth = 50;
int ScreenHeight = 20;

struct Slot
{
	wchar_t Character;
	bool IsBomb;
	bool IsShown;
	bool IsFlagged;
};

void recursive_check_bounds(Slot* map, int posX, int posY, int mWidth, int mHeight)
{

	if (map[posY * mWidth + posX].IsShown || map[posY * mWidth + posX].Character != EMPTY_ICON)
	{
		return;
	} else
	{
		map[posY * mWidth + posX].IsShown = true;
	}

	// Check top		
	if (posY > 0)
	{
		if (map[(posY - 1) * mWidth + posX].Character == EMPTY_ICON)
		{
			recursive_check_bounds(map, posX, posY - 1, mWidth, mHeight);
		}

		if (posX > 0)
		{
			if (map[(posY - 1) * mWidth + (posX - 1)].Character == EMPTY_ICON)
			{
				recursive_check_bounds(map, posX - 1, posY - 1, mWidth, mHeight);
			}
		}
		if (posX < mWidth - 1)
		{
			if (map[(posY - 1) * mWidth + (posX + 1)].Character == EMPTY_ICON)
			{
				recursive_check_bounds(map, posX + 1, posY - 1, mWidth, mHeight);
			}
		}
	}

	// Check bottom
	if (posY < mHeight - 1)
	{
		if (map[(posY + 1) * mWidth + posX].Character == EMPTY_ICON)
		{
			recursive_check_bounds(map, posX, posY + 1, mWidth, mHeight);
		}

		if (posX > 0)
		{
			if (map[(posY + 1) * mWidth + (posX - 1)].Character == EMPTY_ICON)
			{
				recursive_check_bounds(map, posX - 1, posY + 1, mWidth, mHeight);
			}

			if (map[(posY + 1) * mWidth + (posX + 1)].Character == EMPTY_ICON)
			{
				recursive_check_bounds(map, posX + 1, posY + 1, mWidth, mHeight);
			}
		}
	}

	if (posX > 0)
	{
		if (map[posY * mWidth + (posX - 1)].Character == EMPTY_ICON)
		{
			recursive_check_bounds(map, posX - 1, posY, mWidth, mHeight);
		}
	}
	
	if (posX < mWidth - 1)
	{
		if (map[posY * mWidth + (posX + 1)].Character == EMPTY_ICON)
		{
			recursive_check_bounds(map, posX + 1, posY, mWidth, mHeight);
		}
	}

}

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
		map[i].IsFlagged = false;
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
				map[pos].Character = EMPTY_ICON;
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
	srand(time(0));

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
	int mNumberBombs = 40;

	Slot* mineMap = generate_map(mNumberBombs, mMapWidth, mMapHeight);

	// Game states
	COORD mPosition{};
	bool IsLeftButtonPressed;
	bool IsRightButtonPressed;
	int nScore = 0;
	int nBombsLeft = 0;
	int nFlagsLeft = mNumberBombs;
	bool bGameOver = false;

	while (!bGameOver)
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
					screen[posScreen] = currentSlot.IsFlagged ? FLAG_ICON : HIDDEN_ICON;
				} else
				{
					screen[posScreen] = currentSlot.Character;
				}
			}
		}

		if (IsLeftButtonPressed && screen[mPosition.Y * ScreenWidth + mPosition.X] != WALL_ICON)
		{
			int position = (mPosition.Y - 2) * mMapWidth + (mPosition.X - 2);

			if (!mineMap[position].IsShown && mineMap[position].Character == EMPTY_ICON)
			{
				recursive_check_bounds(mineMap, mPosition.X, mPosition.Y, mMapWidth, mMapHeight);
			} else if (!mineMap[position].IsBomb)
			{
				mineMap[position].IsShown = true;
			} else if (mineMap[position].IsBomb)
			{
				bGameOver = true;
			}
		}

		if (IsRightButtonPressed && nFlagsLeft > 0 && screen[mPosition.Y * ScreenWidth + mPosition.X] != WALL_ICON)
		{
			int position = (mPosition.Y - 2) * mMapWidth + (mPosition.X - 2);

			mineMap[position].IsFlagged = !mineMap[position].IsFlagged;
			nFlagsLeft -= mineMap[position].IsFlagged ? 1 : -1;
		}
		
		swprintf_s(&screen[0], 16, L"Score: %8d", nScore);
		swprintf_s(&screen[1 * ScreenWidth + 0], 11, L"Flags: %3d", nFlagsLeft);

		WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

}
