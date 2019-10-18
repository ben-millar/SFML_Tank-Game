#pragma once
#include "Projectile.h"

class ProjectilePool
{
public:
	/// <summary>
	/// @brief Iterate through our projectile array and draw them
	/// </summary>
	/// <param name="t_window">Reference to SF render window to draw to</param>
	void render(sf::RenderWindow& t_window);

private:
	const static int ARRAY_SIZE = 100;
	Projectile m_projectiles[ARRAY_SIZE];
};