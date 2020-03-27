#include "TankAi.h"

////////////////////////////////////////////////////////////

TankAi::TankAi(sf::Texture const & texture, std::map<int, std::list<GameObject*>>& t_obstacleMap) :
	  m_texture(texture)
	, ref_obstacles(t_obstacleMap)
	, m_steering(0, 0)
{
	// Initialises the tank base and turret sprites.
	initSprites();

	initVisionCone();
}

////////////////////////////////////////////////////////////

void TankAi::update(Tank const & playerTank, float dt)
{
	updateGameObjects();
	updateVisionCone();

	// ***** TEMP
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		m_visionDistance += 5.0f;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		m_visionDistance -= 5.0f;
	}

	// *************

	sf::Vector2f vectorToPlayer = seek(playerTank.position());

	// Point our gun at the player
	m_turretRotation = thor::polarAngle(vectorToPlayer);
	m_visionDistance = thor::length(vectorToPlayer) + 100.0f;

	sf::Vector2f acceleration;

	switch (m_currentState)
	{
	case AIState::ATTACK_PLAYER:
		m_steering += thor::unitVector(vectorToPlayer);
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);

		acceleration = m_steering / MASS;

		//m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		m_velocity = MathUtility::truncate(m_velocity + acceleration, MAX_SPEED);

		break;
	case AIState::STOP:
		m_velocity = sf::Vector2f(0, 0);
		//motion->m_speed = 0;
	default:
		break;
	}

	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

	auto currentRotation = m_baseRotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}
	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_baseRotation = static_cast<int>((m_baseRotation) + 1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		m_baseRotation = static_cast<int>((m_baseRotation) - 1) % 360;
	}


	if (thor::length(vectorToPlayer) < MAX_SEE_AHEAD)
	{
		m_currentState = AIState::STOP;
	}
	else
	{
		m_currentState = AIState::ATTACK_PLAYER;
	}

	updateMovement(dt);
}

////////////////////////////////////////////////////////////

void TankAi::render(sf::RenderWindow& window)
{
	// TODO: Don't draw if off-screen...
	window.draw(m_tankBase);

	window.draw(m_visionCone);

	window.draw(m_turret);

	if (DEBUG_mode)
	{
		for (sf::CircleShape& c : m_obstacles)
		{
			window.draw(c);
		}

		// DEBUG STUFF
		// Draw my velocity on screen
		sf::VertexArray heading(sf::Lines, 2);

		heading[0] = sf::Vertex(m_tankBase.getPosition(), sf::Color::Blue);
		heading[1] = sf::Vertex(m_tankBase.getPosition() + m_velocity * 2.0f, sf::Color::Blue);

		window.draw(heading);

		// Draw my steering force on screen
		sf::VertexArray steering(sf::Lines, 2);

		steering[0] = sf::Vertex(m_tankBase.getPosition(), sf::Color::Red);
		steering[1] = sf::Vertex(m_tankBase.getPosition() + m_steering * 5.0f, sf::Color::Red);

		window.draw(steering);
	}
}

////////////////////////////////////////////////////////////

void TankAi::init(sf::Vector2f position)
{
	m_tankBase.setPosition(position);
	m_turret.setPosition(position);
}

////////////////////////////////////////////////////////////

sf::Vector2f TankAi::seek(sf::Vector2f playerPosition) const
{
	return sf::Vector2f(playerPosition - m_tankBase.getPosition());
}

////////////////////////////////////////////////////////////

sf::Vector2f TankAi::collisionAvoidance()
{
	float headingRadians = static_cast<float>(thor::toRadian(m_baseRotation));
	sf::Vector2f headingVector(std::cosf(headingRadians) * MAX_SEE_AHEAD, std::sinf(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle();
	sf::Vector2f avoidance(0, 0);

	if (MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, mostThreatening))
	{		
		avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
		avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
	}
	else
	{
		avoidance *= 0.0f;
	}
	return avoidance;
}

////////////////////////////////////////////////////////////

const sf::CircleShape TankAi::findMostThreateningObstacle()
{
	sf::CircleShape mostThreatening;
	
	if (m_obstacles.size())
	{
		mostThreatening = m_obstacles.at(0);

		for (sf::CircleShape c : m_obstacles)
		{
			// if this object is closer than our most threatening
			if (thor::length(c.getPosition() - m_tankBase.getPosition()) < 
				thor::length(mostThreatening.getPosition() - m_tankBase.getPosition()))
			{
				mostThreatening = c;
			}
		}
	}
	
	return mostThreatening;
}

////////////////////////////////////////////////////////////

bool TankAi::collidesWithPlayer(Tank const& playerTank) const
{
	// Checks if the AI tank has collided with the player tank.
	if (CollisionDetector::collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_tankBase, playerTank.getBase()))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////

void TankAi::hit()
{
	std::cout << "\n*~~~~~~~~~~~~~~~~*" << std::endl;
	std::cout << "* enemy tank hit *" << std::endl;
	std::cout << "*~~~~~~~~~~~~~~~~*\n" << std::endl;
}

////////////////////////////////////////////////////////////

void TankAi::updateGameObjects()
{
	// clear our array of circles
	m_obstacles.clear();

	// work out which cells we occupy
	m_activeCells.clear();

	int gridPos{ -1 };

	// add positions of tank base
	for (sf::Vector2f pos : CellResolution::getCorners(m_tankBase))
	{
		gridPos = CellResolution::getGridRef(pos);

		// If we get an error value back, don't add to set
		if (gridPos == -1) continue;

		m_activeCells.insert(gridPos);
	}

	// add positions for tank turret
	for (sf::Vector2f pos : CellResolution::getCorners(m_turret))
	{
		gridPos = CellResolution::getGridRef(pos);

		// If we get an error value back, don't add to set
		if (gridPos == -1) continue;

		m_activeCells.insert(gridPos);
	}

	// populate our vector of obstacle bounding circles
	for (int i : m_activeCells)
	{
		// will return 0 if key not in map
		if (ref_obstacles.count(i))
		{
			for (GameObject* obj : ref_obstacles.at(i))
			{
				sf::CircleShape circle(obj->getSprite().getTextureRect().width * 1.5f);
				circle.setOrigin(circle.getRadius(), circle.getRadius());
				circle.setPosition(obj->getSprite().getPosition());

				circle.setFillColor(sf::Color(255, 255, 255, 128));
				m_obstacles.push_back(circle);
			}
		}
	}
}

////////////////////////////////////////////////////////////

void TankAi::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(103, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0f, baseRect.height / 2.0f);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0f, turretRect.height / 2.0f);
}

////////////////////////////////////////////////////////////

void TankAi::initVisionCone()
{
	// Populate our vision cone with coloured vertices (should be 1 more than the number of rays)
	for (int i = 0; i <= NUM_RAYS; i++)
	{
		m_visionCone.append(sf::Vertex({ -1.0f, -1.0f }, sf::Color(255,255,0,128)));
	}
}

////////////////////////////////////////////////////////////

void TankAi::updateMovement(float dt)
{
	float speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(thor::toRadian(m_baseRotation)) * speed * dt,
		m_tankBase.getPosition().y + std::sin(thor::toRadian(m_baseRotation)) * speed * dt);
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_baseRotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_turretRotation);
}

////////////////////////////////////////////////////////////

void TankAi::updateVisionCone()
{
	// Position of our turret
	sf::Vector2f pos{ m_turret.getPosition() };

	// Adjust the acuity of our ray casts
	m_visionAcuity = std::clamp(50.0f - (m_visionDistance / 20.0f), MIN_ACUITY, MAX_ACUITY);

	// Angle between each ray, preventing int division
	m_arcPerRay = calculatePitch();

	// Recalculate our vision arc, given the distance
	m_visionArc = m_arcPerRay * NUM_RAYS;

	// At what angle do we start drawing
	float startAngle{ thor::toRadian(m_turretRotation) - m_visionArc / 2.0f };

	// Initialise each ray, tracked in local coordinates (i.e., origin @ 0,0)
	for (sf::Vector2f& ray : m_visionRayCasts)
	{
		ray = { m_visionDistance * cosf(startAngle), m_visionDistance * sinf(startAngle) };
		startAngle += m_arcPerRay;
	}

	// Set the beginning of our triangle cone
	m_visionCone[0].position = pos;

	// Set the end positions of our vertices
	for (int i = 1; i <= NUM_RAYS; i++)
	{
			m_visionCone[i].position = pos + m_visionRayCasts.at(i-1);
	}
}

float TankAi::calculatePitch()
{
	// We split the isosceles triangle of our ray into 2 right angled triangles
	// and determine the base angle of one given its length and arc, and double
	// the result to get the angle of our original isosceles
	return atanf((m_visionAcuity / 2.0f) / m_visionDistance) * 2.0f;
}