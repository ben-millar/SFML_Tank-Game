#include "ProjectilePool.h"

void ProjectilePool::render(sf::RenderWindow& t_window)
{
	for (Projectile i : m_projectiles)
	{
		if (i.m_active)
		{
			i.draw(t_window);
		}
	}
}