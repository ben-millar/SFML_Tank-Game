#include "TankAi.h"

////////////////////////////////////////////////////////////

TankAi::TankAi(sf::Texture const & texture, std::map<int, std::list<GameObject*>>& t_obstacleMap)
	: m_aiBehaviour(AiBehaviour::SEEK_PLAYER)
	, m_texture(texture)
	, ref_obstacles(t_obstacleMap)
	, m_steering(0, 0)
{
	// Initialises the tank base and turret sprites.
	initSprites();
}

////////////////////////////////////////////////////////////

void TankAi::update(Tank const & playerTank, double dt)
{
	updateGameObjects();

	sf::Vector2f vectorToPlayer = seek(playerTank.position());

	switch (m_aiBehaviour)
	{
	case AiBehaviour::SEEK_PLAYER:
		m_steering += thor::unitVector(vectorToPlayer);
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);

		break;
	case AiBehaviour::STOP:
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

	auto currentRotation = m_rotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}
	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_rotation = static_cast<int>((m_rotation) + 1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		m_rotation = static_cast<int>((m_rotation) - 1) % 360;
	}


	if (thor::length(vectorToPlayer) < MAX_SEE_AHEAD)
	{
		m_aiBehaviour = AiBehaviour::STOP;
	}
	else
	{
		m_aiBehaviour = AiBehaviour::SEEK_PLAYER;
	}

	updateMovement(dt);
}

////////////////////////////////////////////////////////////

void TankAi::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	window.draw(m_tankBase);
	window.draw(m_turret);

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
	auto headingRadians = thor::toRadian(m_rotation);
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle();
	sf::Vector2f avoidance(0, 0);
	if (mostThreatening.getRadius() != 0.0)
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
				sf::CircleShape circle(obj->getSprite().getTextureRect().width);
				circle.setOrigin(circle.getRadius(), circle.getRadius());
				circle.setPosition(obj->getSprite().getPosition());
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
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
}


////////////////////////////////////////////////////////////

void TankAi::updateMovement(double dt)
{
	double speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD  * m_rotation) * speed * dt,
		m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD  * m_rotation) * speed * dt);
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_rotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);
}