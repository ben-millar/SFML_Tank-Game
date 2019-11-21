#pragma once
#include <functional>
#include "Projectile.h"
#include "CollisionDetector.h"
#include "GameObject.h"


class Tank;

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
	void update(sf::Time dt);


	void checkCollisions(std::vector<GameObject*>& t_gameObjVector, std::function<void(Tank*, sf::Vector2f)>, Tank* t_tank);

	/// <summary>
	/// @brief Iterate through our projectile array and draw them
	/// </summary>
	/// <param name="t_window">Reference to SF render window to draw to</param>
	void render(sf::RenderWindow& t_window);

private:

	/// <summary>
	/// Make projectile inactive and add back to our linked list
	/// </summary>
	/// <param name="t_projectile">projectile to act on</param>
	void kill(Projectile& t_projectile);


	sf::Texture m_texture;
	sf::Sprite m_sprite;

	Projectile* m_firstAvailable{ nullptr };

	const static int POOL_SIZE = 100;
	Projectile m_projectiles[POOL_SIZE];
};