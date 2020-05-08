#include "LevelLoader.h"

/// The various operator >> overloads below are non-member functions used to extract
///  the game data from the YAML data structure.

/// <summary>
/// @brief Extracts the obstacle type, position and rotation values.
/// </summary>
/// <param name="obstacleNode">A YAML node</param>
/// <param name="obstacle">A simple struct to store the obstacle data</param>
void operator >> (const YAML::Node& t_obstacleNode, ObstacleData& t_obstacle)
{
	t_obstacle.m_type = t_obstacleNode["type"].as<std::string>();
	t_obstacle.m_position.x = t_obstacleNode["position"]["x"].as<float>();
	t_obstacle.m_position.y = t_obstacleNode["position"]["y"].as<float>();
	t_obstacle.m_baseRotation = t_obstacleNode["rotation"].as<double>();
}

////////////////////////////////////////////////////////////

/// <summary>
/// @brief Extracts the target type, position, and random offset.
/// </summary>
/// <param name="t_targetNode">A YAML node</param>
/// <param name="t_target">A struct holding our target data</param>
void operator >> (const YAML::Node& t_targetNode, TargetData& t_target)
{
	t_target.m_type = t_targetNode["type"].as<std::string>();
	t_target.m_position.x = t_targetNode["position"]["x"].as<float>();
	t_target.m_position.y = t_targetNode["position"]["y"].as<float>();
	t_target.m_randomOffset.x = t_targetNode["position"]["randomOffset"].as<int>();
	t_target.m_randomOffset.y = t_targetNode["position"]["randomOffset"].as<int>();
}

////////////////////////////////////////////////////////////

/// <summary>
/// @brief Extracts the filename for the game background texture.
/// </summary>
/// <param name="backgroundNode">A YAML node</param>
/// <param name="background">A simple struct to store background related data</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& backgroundNode, BackgroundData& background)
{
	background.m_fileName = backgroundNode["file"].as<std::string>();
}

////////////////////////////////////////////////////////////

/// <summary>
/// @brief Extracts the initial screen position for the player tank.
/// </summary>
/// <param name="tankNode">A YAML node</param>
/// <param name="tank">A simple struct to store data related to the player tank</param>
void operator >> (const YAML::Node& tankNode, TankData& tank)
{
	/*tank.m_position.x = tankNode["position"]["x"].as<float>();
	tank.m_position.y = tankNode["position"]["y"].as<float>();*/

	const YAML::Node& tankPosNode = tankNode["spawns"].as<YAML::Node>();
	auto size = tankPosNode.size();

	/*int i = 0;
	for (auto& node : tankPosNode)
	{
		tank.m_position[i].x = node["pos"]["x"].as<float>();
		tank.m_position[i].y = node["pos"]["y"].as<float>();
		i++;
	}*/

	for (unsigned i = 0; i < tankPosNode.size(); ++i)
	{
		tank.m_position[i].x = tankPosNode[i]["pos"]["x"].as<float>();
		tank.m_position[i].y = tankPosNode[i]["pos"]["y"].as<float>();
	}
}

////////////////////////////////////////////////////////////

/// <summary>
/// @brief Top level function that extracts various game data from the YAML data stucture.
/// 
/// Invokes other functions to extract the background, tank, obstacle, and target data.
/// Because there are multiple obstacles, obstacle data are stored in a vector.
/// </summary>
/// <param name="tankNode">A YAML node</param>
/// <param name="tank">A simple struct to store data related to the player tank</param>
void operator >> (const YAML::Node& levelNode, LevelData& level)
{
	levelNode["background"] >> level.m_background;

	levelNode["tank"] >> level.m_tank;

	levelNode["ai_tank"] >> level.m_aiTank;

	// Load our obstacle data from file into our struct, and push a copy into our obstacle vector
	const YAML::Node& obstaclesNode = levelNode["obstacles"].as<YAML::Node>();
	for (unsigned i = 0; i < obstaclesNode.size(); ++i)
	{
		ObstacleData obstacle;
		obstaclesNode[i] >> obstacle;
		level.m_obstacles.push_back(obstacle);
	}

	// Load our target data from file into our struct, and push a copy into our target vector
	const YAML::Node& targetsNode = levelNode["targets"].as<YAML::Node>();
	for (unsigned i = 0; i < targetsNode.size(); ++i)
	{
		TargetData target;
		targetsNode[i] >> target;
		level.m_targets.push_back(target);
	}
}

////////////////////////////////////////////////////////////

/// <summary>
/// @brief Handles loading our YAML file into memory
/// </summary>
/// <param name="nr">Level number to load in</param>
/// <param name="level">LevelData struct to take info into</param>
void LevelLoader::load(int nr, LevelData& level)
{
	std::stringstream ss;
	ss << ".\\resources\\levels\\level";
	ss << nr;
	ss << ".yaml";

	try
	{
		YAML::Node baseNode = YAML::LoadFile(ss.str());
		if (baseNode.IsNull())
		{
			std::string message("File: " + ss.str() + " not found");
			throw std::exception(message.c_str());
		}
		baseNode >> level;
	}
	catch (YAML::ParserException& e)
	{
		std::string message(e.what());
		message = "YAML Parser Error: " + message;
		throw std::exception(message.c_str());
	}
	catch (std::exception& e)
	{
		std::string message(e.what());
		message = "Unexpected Error: " + message;
		throw std::exception(message.c_str());
	}
}