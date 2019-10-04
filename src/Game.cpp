#include "Game.h"
#include <iostream>

// Updates per milliseconds
static double const MS_PER_UPDATE = 10.0;

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_height, ScreenSize::s_width, 32), "SFML Playground", sf::Style::Default),
	m_tank(m_tankTexture, m_level.m_tank.m_position)
{
	m_window.setVerticalSyncEnabled(true);

	int currentLevel = 1;

	// Will generate an exception if level loading fails
	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception& e)
	{
		std::cout << "Level loading failure" << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}

	loadTextures();

	setupSprites();
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
			lag -= static_cast<sf::Int32>(MS_PER_UPDATE);
		}

		// this could run faster than 100ups, but still passes 10ms to the update loop
		// could this mess up game logic that relies on dTime?
		update(MS_PER_UPDATE);

		render();
	}
}

/// <summary>
/// @brief Loads all game textures from file
/// </summary>
void Game::loadTextures()
{
	try
	{
		if (!m_tankTexture.loadFromFile(".\\resources\\images\\SpriteSheet.png"))
		{
			throw std::exception("Error loading tank texture from file");
		}

		if (!m_bgTexture.loadFromFile(m_level.m_background.m_fileName))
		{
			throw std::exception("Error loading background texture from file");
		}
		//if (!m_spriteSheetTexture.loadFromFile(".\\resources\\images\\SpriteSheet.png"))
		//{
		//	throw std::exception("Error loading SpriteSheet texture from file");
		//}
	}
	catch(std::exception& e)
	{
		std::cout << e.what();
	}
}

/// <summary>
/// @brief Assigns textures to sprites and sets up sprite parameters
/// </summary>
void Game::setupSprites()
{
	// Now the level data is loaded, set the tank position.
	m_tank.setPosition(m_level.m_tank.m_position);

	m_bgSprite.setTexture(m_bgTexture);
	m_bgSprite.setPosition({ 0.0f,0.0f });

	/*m_wallSprite.setTexture(m_spriteSheetTexture);
	m_wallSprite.setTextureRect(m_wallRect);*/

	for (auto& obstacle : m_level.m_obstacles)
	{
		// Position the wall sprite using the obstacle data
		m_wallSprite.setPosition(obstacle.m_position);
		m_wallSprite.rotate(static_cast<float>(obstacle.m_rotation));
	//	m_sprites.push_back(m_wallSprite);
	}
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
			m_tank.increaseSpeed();
			break;
		case sf::Keyboard::Down:
			m_tank.decreaseSpeed();
			break;
		case sf::Keyboard::Left:
			m_tank.decreaseRotation();
			break;
		case sf::Keyboard::Right:
			m_tank.increaseRotation();
			break;
		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	m_tank.update(dt);
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));

	m_window.draw(m_bgSprite);
	//m_window.draw(m_tankSprite);

	/*for (auto& sprite : m_sprites)
	{
		m_window.draw(sprite);
	}*/

	m_tank.render(m_window);

	m_window.display();
}