#pragma once
#include "Projectile.h"

class ProjectilePool
{
public:

	ProjectilePool();

	void setTexture(sf::Texture const& texture);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="t_pos"></param>
	/// <param name="t_vel"></param>
	/// <param name="t_timeToLive"></param>
	void create(sf::Vector2f t_pos, sf::Vector2f t_vel, int t_timeToLive);

	/// <summary>
	/// 
	/// </summary>
	void update(double t_dT);

	/// <summary>
	/// @brief Iterate through our projectile array and draw them
	/// </summary>
	/// <param name="t_window">Reference to SF render window to draw to</param>
	void render(sf::RenderWindow& t_window);

private:

	sf::Texture m_texture;
	sf::Sprite m_sprite;

	Projectile* m_firstAvailable{ nullptr };

	const static int POOL_SIZE = 100;
	Projectile m_projectiles[POOL_SIZE];
};