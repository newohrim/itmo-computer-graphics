#pragma once

class Window;

class ScoreBoard {
public:
	static void AddScoreA(int score, Window* window);
	static void AddScoreB(int score, Window* window);
	static void UpdateScoreBoard(Window* window);

private:
	static int scoreA;
	static int scoreB;
};
