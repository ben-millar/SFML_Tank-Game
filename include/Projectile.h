#pragma once
#include <SFML/Graphics.hpp>


class Projectile
{
	friend class ProjectilePool;
public:
	
private:
	Projectile()
    : 
	m_active{ false },
	m_timeToLive{ 0 }
	{
	};

	Projectile* getNext() { return m_next; }
	void setNext(Projectile* t_next) { m_next = t_next; }

	/// <summary>
	/// @brief Initialise a projectile on-screen with a given position/velocity
	/// </summary>
	/// <param name="t_pos">initial position</param>
	/// <param name="t_vel">initial velocity</param>
	/// <param name="t_lifetime">lifetime of projectile</param>
	void init(sf::Vector2f t_pos, sf::Vector2f t_vel, int t_timeToLive);
	
	/// <summary>
	/// @brief Update our position and time remaining
	/// </summary>
	/// <param name="t_dT">time since last update<</param>
	/// <returns>false if our projectile died</returns>
	bool update(sf::Time dt);


	// ####### DATA MEMBERS #######

	bool m_active;

	int m_timeToLive;

	// pixels/second
	float m_speed{ 250.0f };
	float m_baseRotation{ 0.0f };

	sf::Vector2f m_position{ 0.0f,0.0f };
	sf::Vector2f m_velocity{ 0.0f,0.0f };

	Projectile* m_next{ nullptr };
};