#include "CellResolution.h"

const sf::Vector2f CellResolution::CELL_SIZE{ ScreenSize::s_width / static_cast<float>(NUM_ROWS), ScreenSize::s_height / static_cast<float>(NUM_COLS) };

////////////////////////////////////////////////////////////

int CellResolution::getGridRef(sf::Vector2f t_pos)
{
	return (floor(t_pos.x / CELL_SIZE.x) * 10) + floor(t_pos.y / CELL_SIZE.y);
}

////////////////////////////////////////////////////////////

std::array<sf::Vector2f, 4> CellResolution::getCorners(sf::Sprite & t_obj)
{
	std::array<sf::Vector2f, 4> corners;

	sf::FloatRect bounds{ t_obj.getGlobalBounds() };
	sf::Vector2f pos{ bounds.left, bounds.top };
	float rotation = t_obj.getRotation() * MathUtility::DEG_TO_RAD;

	float horizontalComponentOfWidth = bounds.width * cos(rotation);
	float verticalComponentOfWidth = bounds.width * sin(rotation);
	float horizontalComponentOfHeight = bounds.height * cos(rotation);
	float verticalComponentOfHeight = bounds.height * sin(rotation);

	corners.at(0) = pos;
	corners.at(1) = pos + sf::Vector2f(horizontalComponentOfWidth, verticalComponentOfWidth);
	corners.at(2) = corners.at(1) + sf::Vector2f(horizontalComponentOfHeight, verticalComponentOfHeight);
	corners.at(3) = pos + sf::Vector2f(horizontalComponentOfHeight, verticalComponentOfHeight);

	return corners;
}
