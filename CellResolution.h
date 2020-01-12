#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include "GameObject.h"
#include "ScreenSize.h"
#include "MathUtility.h"

class CellResolution
{
public:
	/// <summary>
	/// @brief Find the grid position of a given point in 2D space
	/// </summary>
	/// <param name="t_pos">position of the object to check</param>
	/// <returns>grid reference as vector2i</returns>
	static int getGridRef(sf::Vector2f t_pos);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="t_obj"></param>
	/// <returns></returns>
	static std::array<sf::Vector2f, 4> getCorners(sf::Sprite& t_obj);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	static sf::Vector2f temp_getCellSize() { return CELL_SIZE; }

private:

	const static int NUM_ROWS{ 10 }, NUM_COLS{ 10 };
	const static sf::Vector2f CELL_SIZE;
};

