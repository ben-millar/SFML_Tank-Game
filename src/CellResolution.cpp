#include "CellResolution.h"

const sf::Vector2f CellResolution::CELL_SIZE{ ScreenSize::s_width / static_cast<float>(NUM_ROWS), ScreenSize::s_height / static_cast<float>(NUM_COLS) };

////////////////////////////////////////////////////////////

int CellResolution::getGridRef(sf::Vector2f t_pos)
{
	if (t_pos.x > 0.0f && t_pos.y > 0.0f)
	{
		if (t_pos.x < ScreenSize::s_width * 2.0f && t_pos.y < ScreenSize::s_height * 2.0f)
		{
			return (floor(t_pos.x / CELL_SIZE.x) * 10) + floor(t_pos.y / CELL_SIZE.y);
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////

std::array<sf::Vector2f, 4> CellResolution::getCorners(sf::Sprite & t_sprite)
{
	std::array<sf::Vector2f, 4> corners;

	sf::FloatRect bounds{ t_sprite.getGlobalBounds() };

	corners.at(0) = t_sprite.getTransform().transformPoint(0, 0);
	corners.at(1) = t_sprite.getTransform().transformPoint(bounds.width, 0);
	corners.at(2) = t_sprite.getTransform().transformPoint(bounds.width, bounds.height);
	corners.at(3) = t_sprite.getTransform().transformPoint(0, bounds.height);

	return corners;
}