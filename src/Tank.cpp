#include "Tank.h"
#include "MathUtility.h"
#include "Thor/Animations.hpp"
#include <iostream>

Tank::Tank(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites)
	: m_texture(texture)
	, m_wallSprites(wallSprites)
{
	f_impactSmoke = &Tank::impactSmoke;
	initSprites();
	initParticles();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::setPosition(sf::Vector2f const& m_pos)
{
	m_tankBase.setPosition(m_pos);
	m_turret.setPosition(m_pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool Tank::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		// Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::deflect()
{
	// In case tank was rotating.
	adjustRotation();

	// If tank was moving.
	if (m_speed != 0)
	{
		// Temporarily disable turret rotations on collision.
		m_enableRotation = false;
		// Back up to position in previous frame.
		m_tankBase.setPosition(m_previousPosition);
		m_turret.setPosition(m_previousPosition);
		// Apply small force in opposite direction of travel.
		if (m_previousSpeed < 0)
		{
			m_speed = 20.0;
		}
		else
		{
			m_speed = -20.0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::adjustRotation()
{
	// If tank was rotating...
	if (m_baseRotation != m_previousBaseRotation)
	{
		// Work out which direction to rotate the tank base post-collision.
		if (m_baseRotation > m_previousBaseRotation)
		{
			m_baseRotation = m_previousBaseRotation - 1;
		}
		else
		{
			m_baseRotation = m_previousBaseRotation + 1;
		}
	}
	// If turret was rotating while tank was moving
	if (m_turretRotation != m_previousTurretRotation)
	{
		// Set the turret rotation back to it's pre-collision value.
		m_turretRotation = m_previousTurretRotation;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::increaseSpeed()
{
	if (m_speed < (M_MAX_SPEED - 1.0))
	{
		m_speed += 0.6;
	}
	else
	{
		m_speed = M_MAX_SPEED;
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::decreaseSpeed()
{
	if (m_speed > (M_MIN_SPEED + 1.0))
	{
		m_speed -= 0.6;
	}
	else
	{
		m_speed = M_MIN_SPEED;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::increaseRotation()
{
	m_previousBaseRotation = m_baseRotation;
	(m_baseRotation > 360.0) ? m_baseRotation -= 360.0 : m_baseRotation += 1.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::decreaseRotation()
{
	m_previousBaseRotation = m_baseRotation;
	(m_baseRotation < 0.0) ? m_baseRotation += 360.0 : m_baseRotation -= 1.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::setTurretHeading(float t_heading)
{
	m_previousTurretRotation = m_turretRotation;

	if (m_enableRotation)
	{
		if (m_turretFree)
		{
			m_turretRotation = t_heading * MathUtility::RAD_TO_DEG;
		}
		else
		{
			m_turretRotation = m_baseRotation;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::toggleTurretFree()
{
	(m_turretFree) ? m_turretFree = false : m_turretFree = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::fire()
{
	if (m_fireClock.getElapsedTime() > m_fireDelay)
	{
		sf::Vector2f targetVector{
			static_cast<float>(cos(MathUtility::DEG_TO_RAD * m_turretRotation)),
			static_cast<float>(sin(MathUtility::DEG_TO_RAD * m_turretRotation))
		};

		m_projectilePool.create(m_turret.getPosition(), targetVector, 180);

		muzzleFlash(targetVector);

		m_fireClock.restart();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::muzzleFlash(sf::Vector2f t_fireDir)
{
	thor::FadeAnimation fade{ 0.0f,1.0f };

	// Muzzle Flash/Sparks effects
	m_sparksEmitter.setParticlePosition(m_tankBase.getPosition() + t_fireDir * 60.0f);
	m_sparksEmitter.setEmissionRate(500);
	m_sparksEmitter.setParticleVelocity(thor::Distributions::deflect(t_fireDir * 250.0f, 10.0f));
	m_sparksEmitter.setParticleLifetime(thor::Distributions::uniform(sf::seconds(0.1), sf::seconds(2.0)));

	m_sparkParticleSystem.addEmitter(m_sparksEmitter, sf::seconds(0.05f));
	m_sparkParticleSystem.addAffector(thor::AnimationAffector(fade));

	// Smoke/Dust effects
	m_smokeEmitter.setParticlePosition(m_tankBase.getPosition() + t_fireDir * 60.0f);
	m_smokeEmitter.setEmissionRate(500);
	m_smokeEmitter.setParticleVelocity(thor::Distributions::deflect(t_fireDir * 60.0f, 120.0f));
	m_smokeEmitter.setParticleLifetime(thor::Distributions::uniform(sf::seconds(0.1), sf::seconds(1.5)));

	
	thor::ScaleAffector scale({ 1.1f,1.1f });
	m_smokeParticleSystem.addEmitter(m_smokeEmitter, sf::seconds(0.1f));
	m_smokeParticleSystem.addAffector(thor::AnimationAffector(fade));
	m_smokeParticleSystem.addAffector(scale, sf::seconds(1));
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::impactSmoke(sf::Vector2f t_impactPos)
{
	std::cout << "I'm making smoke!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::update(sf::Time dt)
{
	// update projectiles
	m_projectilePool.update(dt);

	m_projectilePool.checkCollisions(m_wallSprites, f_impactSmoke);

	// update particles
	m_smokeParticleSystem.update(dt);
	m_sparkParticleSystem.update(dt);

	// keep track of previous position
	m_previousPosition = m_tankBase.getPosition();

	m_tankBase.move(cos(MathUtility::DEG_TO_RAD * m_baseRotation) * m_speed * dt.asSeconds(), sin(MathUtility::DEG_TO_RAD * m_baseRotation) * m_speed * dt.asSeconds());
	m_turret.move(cos(MathUtility::DEG_TO_RAD * m_baseRotation) * m_speed * dt.asSeconds(), sin(MathUtility::DEG_TO_RAD * m_baseRotation) * m_speed * dt.asSeconds());

	m_tankBase.setRotation(m_baseRotation);
	m_turret.setRotation(m_turretRotation);

	if (checkWallCollision())
	{
		deflect();
	}
	else
	{
		m_enableRotation = true;
	}

	// keep track of previous speed
	m_previousSpeed = m_speed;

	if (m_speed > 0.0)
	{
		m_speed -= M_FRICTION;
	}

	if (m_speed < 0.0)
	{
		m_speed += M_FRICTION;
	}

	// Doesn't work very smoothly with acceleration :(
	//m_speed = std::clamp(m_speed, M_MIN_SPEED, M_MAX_SPEED);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_smokeParticleSystem);
	window.draw(m_sparkParticleSystem);

	// draw projectiles
	m_projectilePool.render(window);

	window.draw(m_tankBase);
	window.draw(m_turret);	
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	//m_tankBase.setPosition(pos);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	//m_turret.setPosition(pos);

	m_projectilePool.setTexture(m_texture);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::initParticles()
{
	try
	{
		if (!m_smokeTexture.loadFromFile(".\\resources\\images\\smoke.png"))
		{
			throw std::exception("Error loading 'smoke.png' from within Tank.cpp >> initParticles");
		}

		if (!m_sparkTexture.loadFromFile(".\\resources\\images\\spark.png"))
		{
			throw std::exception("Error loading 'spark.png' from within Tank.cpp >> initParticles");
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	m_smokeParticleSystem.setTexture(m_smokeTexture);
	m_sparkParticleSystem.setTexture(m_sparkTexture);
}