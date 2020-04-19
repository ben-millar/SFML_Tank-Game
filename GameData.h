#pragma once

/// <summary>
/// @brief This struct contains game data to communicate to the HUD class
/// </summary>
struct GameData
{
	int totalTargets{ 0 };
	int targetsCollected{ 0 };
	float timeElapsed{ 0.0f };
	std::vector<float> timePerTarget;
	float score{ 0.0f };
};