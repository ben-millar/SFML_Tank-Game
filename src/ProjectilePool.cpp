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

///////////////////////////////////////////////////////////////////////////////////////////////

void ProjectilePool::setTexture(sf::Texture const& texture)
{
	m_sprite.setTexture(texture);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ProjectilePool::reset()
{
	for (Projectile& p : m_projectiles)
	{
		if (p.m_active)
		{
			kill(p);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////

void ProjectilePool::update(sf::Time dt)
{
	// for all projectiles
	for (Projectile& p : m_projectiles)
	{
		// which are active
		if (p.m_active)
		{
			// if can't update, then our projectile died this frame
			if (!p.update(dt))
			{
				kill(p);
			}
		}
	}
}

/*

void printNumber(int i)
{
   std::cout << i;
}

std::function<void(int)> f_print = printNumber;
f_print(10);


checkCollisions( std::function<void(const Foo&, int)> f_generateParticle )
{
f_generateParticle(Fooinstance, );
}

*/

///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<sf::Vector2f> ProjectilePool::getActiveProjectilePos()
{
	std::vector<sf::Vector2f> positions;

	for (Projectile& p : m_projectiles)
	{
		if (p.m_active) positions.push_back(p.m_position);
	}

	return positions;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ProjectilePool::checkCollisions(std::vector<GameObject*>& t_gameObjVector, std::function<void(Tank*, sf::Vector2f)> t_smokeFunc, Tank* t_tank)
{
	for (Projectile& p : m_projectiles)
	{
		if (p.m_active)
		{
			for (auto& obj : t_gameObjVector)
			{
				// assign a temp sprite to this location for collisions
				sf::Sprite* tempSprite = new sf::Sprite();
				tempSprite->setPosition(p.m_position);
				tempSprite->setRotation(p.m_rotation);

				// if they collide
				if (CollisionDetector::collision(*tempSprite, obj->getSprite()))
				{
					std::cout << "IMPACT" << std::endl;
					kill(p);

					// pass position to our smoke effect function
					t_smokeFunc(t_tank, p.m_position);

					// tell our object it's been hit
					obj->hit();
				}

				delete tempSprite;
				tempSprite = nullptr;

			} // end for t_spriteVector
		} // end if p.active
	} // end for m_projectiles
}

///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////

void ProjectilePool::kill(Projectile& t_projectile)
{
	// add back to our linked list
	t_projectile.setNext(m_firstAvailable);
	m_firstAvailable = &t_projectile;

	// set inactive
	t_projectile.m_active = false;

	std::cout << "deleting bullet" << std::endl;
}