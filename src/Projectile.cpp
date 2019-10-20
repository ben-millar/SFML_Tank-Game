#include "Projectile.h"
#include <iostream>

void Projectile::init(sf::Vector2f t_pos, sf::Vector2f t_vel, int t_timeToLive)
{
	m_active = true;
	m_position = t_pos;
	m_velocity = t_vel * m_speed;
	m_timeToLive = t_timeToLive;
}

bool Projectile::update(double t_dT)
{
	m_position += m_velocity;

	m_timeToLive--;

	return (m_timeToLive > 0) ? true : false;
}
