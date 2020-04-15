#include "Tank.h"
#include "MathUtility.h"
#include <iostream>

Tank::Tank(sf::Texture const& t_texture, std::map<int, std::list<GameObject*>>& t_obstacleMap, std::vector<Target>& t_targetVector, TankAi& t_enemyTank, float& t_screenShake)
	: m_texture(t_texture),
	ref_obstacles(t_obstacleMap),
	ref_targets(t_targetVector),
	ref_enemyTank(t_enemyTank),
	m_screenShake(t_screenShake)
{
	initSprites();
	loadParticleTextures();

	temp_debugInit();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::loadParticleTextures()
{
	try
	{
		if (!m_smokeTexture.loadFromFile(".\\resources\\images\\smoke.png"))
		{
			throw std::exception("Error loading 'smoke.png' from within Tank.cpp >> loadParticleTextures");
		}

		if (!m_sparkTexture.loadFromFile(".\\resources\\images\\spark.png"))
		{
			throw std::exception("Error loading 'spark.png' from within Tank.cpp >> loadParticleTextures");
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	m_smokeParticleSystem.setTexture(m_smokeTexture);
	m_sparkParticleSystem.setTexture(m_sparkTexture);

	// Smoke/Dust effects
	m_smokeEmitter.setEmissionRate(m_smokeEmissionRate);
	m_smokeEmitter.setParticleVelocity(thor::Distributions::deflect({ 30.0f,30.0f }, 360.0f));
	m_smokeEmitter.setParticleLifetime(thor::Distributions::uniform(sf::seconds(0.1f), sf::seconds(1.5f)));

	thor::ScaleAffector scale({ 1.1f,1.25f });
	m_smokeParticleSystem.addAffector(scale);

	thor::FadeAnimation fade{ 0.0f,1.0f };
	m_smokeParticleSystem.addAffector(thor::AnimationAffector(fade));
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::reset()
{
	setPosition({ 80.0f,80.0f });
	m_speed = 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::setPosition(sf::Vector2f const& m_pos)
{
	m_tankBase.setPosition(m_pos);
	m_turret.setPosition(m_pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::temp_debugInit()
{
	temp_activeCellRect.setSize(CellResolution::temp_getCellSize());

	temp_activeCellRect.setFillColor(sf::Color(255, 0, 0, 128));
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool Tank::checkWallCollision()
{
	for (auto& wall : m_obstacles)
	{
		// Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, wall->getSprite()) ||
			CollisionDetector::collision(m_tankBase, wall->getSprite()))
		{
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::checkTargetCollision()
{
	for (GameObject* t : m_targets)
	{
		sf::FloatRect bounds{ t->getSprite().getGlobalBounds() };

		if (m_tankBase.getGlobalBounds().intersects(bounds) ||
			m_turret.getGlobalBounds().intersects(bounds))
		{
			t->hit();
		}
	}
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
	if (m_speed < (M_MAX_SPEED - 1.0f))
	{
		m_speed += 0.6f;
	}
	else
	{
		m_speed = M_MAX_SPEED;
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::decreaseSpeed()
{
	if (m_speed > (M_MIN_SPEED + 1.0f))
	{
		m_speed -= 0.6f;
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
			m_turretRotation = thor::toDegree(t_heading);
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

void Tank::hit()
{
	std::cout << "I'm hit!" << std::endl;

	(m_screenShake < 0.5f) ? m_screenShake += 0.5f : m_screenShake = 1.0f;

	// Increase the amount of smoke coming from our tank
	(m_smokeEmissionRate < 10) ? m_smokeEmissionRate++ : m_smokeEmissionRate = 10;
	m_smokeEmitter.setEmissionRate(m_smokeEmissionRate);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::update(sf::Time dt)
{
	updateParticles(dt);

	// keep track of previous position
	m_previousPosition = m_tankBase.getPosition();

	float deltaX = cosf(thor::toRadian(m_baseRotation)) * m_speed * dt.asSeconds();
	float deltaY = sinf(thor::toRadian(m_baseRotation)) * m_speed * dt.asSeconds();
	m_tankBase.move(deltaX, deltaY);
	m_turret.move(deltaX, deltaY);

	m_tankBase.setRotation(static_cast<float>(m_baseRotation));
	m_turret.setRotation(static_cast<float>(m_turretRotation));

	if (checkWallCollision())
	{
		deflect();
	}
	else
	{
		m_enableRotation = true;
	}

	checkTargetCollision();

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

	updateGameObjects();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_smokeParticleSystem);

	window.draw(m_tankBase);
	window.draw(m_turret);	
	
	window.draw(m_sparkParticleSystem);

	if (DEBUG_mode)
	{
		// DEBUG highlight active cells TEMP
		for (int i : m_activeCells)
		{
			temp_activeCellRect.setPosition({ (i / 10) * CellResolution::temp_getCellSize().x, i % 10 * CellResolution::temp_getCellSize().y });
			window.draw(temp_activeCellRect);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0f, baseRect.height / 2.0f);
	//m_tankBase.setPosition(pos);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0f, turretRect.height / 2.0f);
	//m_turret.setPosition(pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::updateGameObjects()
{
	m_obstacles.clear();
	m_targets.clear();
	m_enemyTanks.clear();

	// work out which cells we occupy
	m_activeCells.clear();

	int gridPos{ -1 };

	// add positions of tank base and turret
	for (sf::Vector2f pos : CellResolution::getCorners(m_tankBase))
	{
		gridPos = CellResolution::getGridRef(pos);

		// If we get an error value back, don't add to set
		if (gridPos == -1) continue;

		m_activeCells.insert(gridPos);
	}
	
	for (sf::Vector2f pos : CellResolution::getCorners(m_turret))
	{
		gridPos = CellResolution::getGridRef(pos);

		// If we get an error value back, don't add to set
		if (gridPos == -1) continue;

		m_activeCells.insert(gridPos);
	}

	// populate our vector of obstacle pointers
	for (int i : m_activeCells)
	{
		// will return 0 if key not in map
		if (ref_obstacles.count(i))
		{ 
			for (GameObject* obj : ref_obstacles.at(i))
			{
				m_obstacles.push_back(obj);
			}
		}
	}

	// populate our vector of target pointers
	for (Target& i : ref_targets)
	{
		m_targets.push_back(&i);
	}

	// populate our vector of tank pointers (right now it's just one!)
	m_enemyTanks.push_back(&ref_enemyTank);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Tank::updateParticles(sf::Time t_dt)
{
	// Update position for particle FX
	m_smokeEmitter.setParticlePosition(m_turret.getPosition());

	m_smokeParticleSystem.addEmitter(m_smokeEmitter, sf::seconds(0.5f));

	m_smokeParticleSystem.update(t_dt);
	m_sparkParticleSystem.update(t_dt);
}