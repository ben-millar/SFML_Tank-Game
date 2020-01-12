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
	/// @brief Kill all active projectiles
	/// </summary>
	void reset();

	/// <summary>
	/// @brief Activate a new projectile from our pool, update linked list
	/// </summary>
	/// <param name="t_pos">Position to put projectile</param>
	/// <param name="t_vel">Velocity to give projectile</param>
	/// <param name="t_timeToLive">Projectile lifespan</param>
	void create(sf::Vector2f t_pos, sf::Vector2f t_vel, int t_timeToLive);

	/// <summary>
	/// @brief Move all projectiles by their velocity
	/// </summary>
	void update(sf::Time dt);

	/// <summary>
	/// @brief returns the positions of all active projectiles
	/// </summary>
	/// <returns>A vector of sf::vector2fs, one pos per projectile</returns>
	std::vector<sf::Vector2f> getActiveProjectilePos();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="t_gameObjVector"></param>
	/// <param name=""></param>
	/// <param name="t_tank"></param>
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