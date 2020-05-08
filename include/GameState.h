#pragma once

/// <summary>
/// @brief Keep track of the state of our game
/// </summary>
enum class GameState
{
	Null,
	Loading,
	GamePlay,
	Paused,
	GameWin,
	GameOver
};

static bool DEBUG_mode{ false };