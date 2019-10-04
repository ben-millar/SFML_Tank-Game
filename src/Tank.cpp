#include "Tank.h"
#include "MathUtility.h"

Tank::Tank(sf::Texture const & texture, sf::Vector2f const & pos)
: m_texture(texture)
{
	initSprites(pos);
}

void Tank::increaseSpeed()
{
	if (m_speed < M_MAX_SPEED)
	{
		m_speed += 1.0;
	}
}

void Tank::decreaseSpeed()
{
	if (m_speed > M_MIN_SPEED)
	{
		m_speed -= 1.0;
	}
}

void Tank::increaseRotation()
{
	(m_rotation > 360.0) ? m_rotation -= 360.0 : m_rotation += 1.0;
}

void Tank::decreaseRotation()
{
	(m_rotation < 0.0) ? m_rotation += 360.0 : m_rotation -= 1.0;
}

void Tank::update(double dt)
{
	m_tankBase.move(cos(MathUtility::DEG_TO_RAD * m_rotation) * m_speed * (dt / 1000.0), sin(MathUtility::DEG_TO_RAD * m_rotation) * m_speed * (dt / 1000.0));
	m_turret.move(cos(MathUtility::DEG_TO_RAD * m_rotation) * m_speed * (dt / 1000.0), sin(MathUtility::DEG_TO_RAD * m_rotation) * m_speed * (dt / 1000.0));

	m_tankBase.setRotation(m_rotation);
	m_turret.setRotation(m_rotation);

	if (m_speed > 0.0)
	{
		m_speed -= M_FRICTION;
	}

	if (m_speed < 0.0)
	{
		m_speed += M_FRICTION;
	}
}

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
}


void Tank::initSprites(sf::Vector2f const & pos)
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	m_tankBase.setPosition(pos);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	m_turret.setPosition(pos);

}