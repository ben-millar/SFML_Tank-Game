#pragma once

/// <summary>
/// @brief This struct contains game data to communicate to the HUD class
/// </summary>
struct GameData
{
	float playerHealth{ -1.0f };
	int totalTargets{ -1 };
	int targetsCollected{ -1 };
	float timeElapsed{ 0-1.0f };
	std::vector<float> timePerTarget;
	float score{ -1.0f };
};