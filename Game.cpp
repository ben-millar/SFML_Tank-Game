#include "Game.h"
#include <iostream>

// Updates per milliseconds
static double const MS_PER_UPDATE = 10.0;

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_height, ScreenSize::s_width, 32), "SFML Playground", sf::Style::Default)
{
	m_window.setVerticalSyncEnabled(true);

	loadTextures();
	setupSprites();

	m_tankHeadingDegrees = 0.0f;
	m_turnRate = 1.0f;
	m_moveSpeed = 1.5f;

	m_movementVector = { 0.0f,0.0f };
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Int32 lag = 0;

	while (m_window.isOpen())
	{
		sf::Time dt = clock.restart();

		lag += dt.asMilliseconds();

		processEvents();

		while (lag > MS_PER_UPDATE)
		{
			update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}

		// this could run faster than 100ups, but still passes 10ms to the update loop
		// could this mess up game logic that relies on dTime?
		update(MS_PER_UPDATE);

		render();
	}
}

////////////////////////////////////////////////////////////
void Game::loadTextures()
{
	// load texture from file
	if (!m_texture.loadFromFile("E-100.png"))
	{
		std::string s("Error loading texture 'E-100.png' to myTexture");
		throw std::exception(s.c_str());
	}
}

////////////////////////////////////////////////////////////
void Game::setupSprites()
{
	// apply texture to sprite
	m_sprite.setTexture(m_texture);

	m_sprite.setOrigin({ m_sprite.getGlobalBounds().width / 2.0f, m_sprite.getGlobalBounds().height / 2.0f });
	m_sprite.setPosition({ 400.0f, 400.0f });
	m_sprite.setRotation(90.0f);
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		processGameEvents(event);
	}
}

////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_window.close();
			break;
		case sf::Keyboard::Up:
			// Up key was pressed...
			break;
		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	// rotate our tank clockwise and counter-clockwise
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_tankHeadingDegrees -= m_turnRate;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_tankHeadingDegrees += m_turnRate;
	}

	// Normalise our heading to the range 0 - 360
	if (m_tankHeadingDegrees > 360.0f)
	{
		m_tankHeadingDegrees -= 360.0f;
	}

	if (m_tankHeadingDegrees < 0.0f)
	{
		m_tankHeadingDegrees += 360.0f;
	}

	// set the rotation of our sprite based on our heading
	m_sprite.setRotation(m_tankHeadingDegrees);

	// get our heading in radians to pass to cos/sin
	m_tankHeadingRadians = m_tankHeadingDegrees * (3.14159f / 180.0f);

	// determine a movement vector based off out movement speed, and our heading
	m_movementVector = { m_moveSpeed * cos(m_tankHeadingRadians), m_moveSpeed * sin(m_tankHeadingRadians) };

	// move our tank forward and backward along its movement vector
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_sprite.move(m_movementVector);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_sprite.move(-m_movementVector);
	}
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));

	m_window.draw(m_sprite);

	m_window.display();
}