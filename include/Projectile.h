#pragma once
#include <SFML/Graphics.hpp>

class Projectile
{
	friend class ProjectilePool;
public:
	/// <summary>
	/// @brief Draw projectile sprite to the window passed
	/// </summary>
	/// <param name="t_window">Reference to SF render window to draw to</param>
	void draw(sf::RenderWindow& t_window);
private:

	bool m_active{ false };
	sf::Sprite m_sprite;
};

