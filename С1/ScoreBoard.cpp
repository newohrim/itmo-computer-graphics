#include "ScoreBoard.h"

#include "os/Window.h"

#ifdef _WIN32
#include <windows.h>
#include <WinUser.h>
#include "os/wnd.h"
#endif

int ScoreBoard::scoreA{0};
int ScoreBoard::scoreB{0};

void ScoreBoard::AddScoreA(int score, Window* window)
{
	scoreA += score;
	UpdateScoreBoard(window);
}

void ScoreBoard::AddScoreB(int score, Window* window)
{
	scoreB += score;
	UpdateScoreBoard(window);
}

void ScoreBoard::UpdateScoreBoard(Window* window)
{
#ifdef _WIN32
	WCHAR text[256];
	swprintf_s(text, TEXT("Pong: (%d, %d)"), scoreA, scoreB);
	SetWindowText(wndGetHWND(window), text);
#endif
}
