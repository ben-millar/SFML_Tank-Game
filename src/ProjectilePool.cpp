#include "ProjectilePool.h"
#include "MathUtility.h"
#include <iostream>
#include <cassert>

ProjectilePool::ProjectilePool()
{
	// init first in list
	m_firstAvailable = &m_projectiles[0];

	// init middle of list
	for (int i = 0; i < POOL_SIZE - 1; i++)
	{
		m_projectiles[i].setNext( &m_projectiles[i + 1] );
	}

	// init last in list
	m_projectiles[POOL_SIZE - 1].setNext(nullptr);

	m_sprite.setTextureRect({ 8,177,9,6 });
}

void ProjectilePool::setTexture(sf::Texture const& texture)
{
	m_sprite.setTexture(texture);
}

void ProjectilePool::create(sf::Vector2f t_pos, sf::Vector2f t_vel, int t_timeToLive)
{
	// ensure there is a projectile available
	assert(m_firstAvailable != nullptr); 

	// move our first available to next available projectile
	Projectile* newProjectile = m_firstAvailable;
	m_firstAvailable = newProjectile->getNext();

	// initialise our new projectile
	newProjectile->init(t_pos, t_vel, t_timeToLive);

	// set rotation of our new projectile
	newProjectile->m_rotation = (atan2(t_vel.y, t_vel.x)) * MathUtility::RAD_TO_DEG;

	std::cout << "FIRING" << std::endl;
}

void ProjectilePool::update(double t_dT)
{
	// for all projectiles
	for (Projectile& i : m_projectiles)
	{
		// which are active
		if (i.m_active)
		{
			// if can't update, then our projectile died this frame
			if (!i.update(t_dT))
			{
				// add back to our linked list
				i.setNext(m_firstAvailable);
				m_firstAvailable = &i;

				// set inactive
				i.m_active = false;

				std::cout << "deleting bullet" << std::endl;
			}
		}
	}
}

void ProjectilePool::render(sf::RenderWindow& t_window)
{
	for (Projectile& i : m_projectiles)
	{
		if (i.m_active)
		{
			m_sprite.setPosition(i.m_position);
			m_sprite.setRotation(i.m_rotation);
			t_window.draw(m_sprite);
		}
	}
}