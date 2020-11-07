#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <thread>

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
		map[bPos].Character = L'X';
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

			map[pos].Character = (wchar_t)(0x30 + bCounter);
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
	
	Slot* mineMap = generate_map(150, ScreenWidth, ScreenHeight);

	while (true)
	{
		
		for (int px = 0; px < ScreenWidth * ScreenHeight; px++)
		{
			screen[px] = L'#';
		}

		for (int x = 0; x < ScreenWidth; x++)
		{
			for (int y = 0; y < ScreenHeight; y++)
			{
				int pos = y * ScreenWidth + x;

				screen[pos] = mineMap[pos].Character;
			}
		}

		WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

}
