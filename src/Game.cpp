#include "Game.h"
#include <iostream>

// Updates per milliseconds
static const sf::Time MS_PER_UPDATE = sf::seconds(1.0f/60.0f);

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_height, ScreenSize::s_width, 32), "SFML Playground", sf::Style::Default),
	m_tank(m_tankTexture, m_sprites)
{
	// Game runs much faster with this commented out. Why?
	// Seems to limit our refresh rate to that of the monitor
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
	loadFonts();
	generateWalls();
	generateTargets();
	setupSprites();

	// set state to GamePlay
	m_gameState = state::GamePlay;
	m_gameClock.restart();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::run()
{
	sf::Clock clock;
	sf::Time lag = sf::Time::Zero;

	while (m_window.isOpen())
	{
		sf::Time dt = clock.restart();

		lag += dt;

		processEvents();

		while (lag > MS_PER_UPDATE)
		{
			update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}

		// this could run faster than 100ups, but still passes 10ms to the update loop
		// could this mess up game logic that relies on dTime?
		update(dt);

		render();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// @brief Loads all game textures from file
/// </summary>
void Game::loadTextures()
{
	try
	{
		if (!m_tankTexture.loadFromFile(".\\resources\\images\\SpriteSheet.png"))
		{
			throw std::exception("Error loading tank texture from file in game.cpp:76");
		}

		if (!m_bgTexture.loadFromFile(m_level.m_background.m_fileName))
		{
			throw std::exception("Error loading background texture from file");
		}
		if (!m_spriteSheetTexture.loadFromFile(".\\resources\\images\\SpriteSheet.png"))
		{
			throw std::exception("Error loading SpriteSheet texture from file in game.cpp:85");
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::loadFonts()
{
	try
	{
		if (!m_font.loadFromFile(".\\resources\\fonts\\joystix.monospace.ttf"))
		{
			throw std::exception("Error loading joystix font from file in game.cpp:100");
		}

		m_text.setFont(m_font);
		m_text.setCharacterSize(16U);
		m_text.setPosition({ 10.0f,10.0f });
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// @brief Assigns textures to sprites and sets up sprite parameters
/// </summary>
void Game::setupSprites()
{
	// Now the level data is loaded, set the tank position in a random corner.
	m_tank.setPosition(m_level.m_tank.m_position[rand() % 4]);

	m_bgSprite.setTexture(m_bgTexture);
	m_bgSprite.setPosition({ 0.0f,0.0f });
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::generateWalls()
{
	sf::IntRect wallRect(2, 129, 33, 23);
	// Create the Walls 
	for (ObstacleData const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_spriteSheetTexture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0f, wallRect.height / 2.0f);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_sprites.push_back(sprite);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::generateTargets()
{
	sf::IntRect targetRect(0, 90, 38, 38);

	for (TargetData const& target : m_level.m_targets)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_spriteSheetTexture);
		sprite.setTextureRect(targetRect);
		sprite.setOrigin(targetRect.width / 2.0f, targetRect.height / 2.0f);
		sprite.setPosition(target.m_position);

		float offsetX = rand() % target.m_randomOffset.x;
		float offsetY = rand() % target.m_randomOffset.y;

		sprite.move({ offsetX, offsetY });

		m_targets.push_back(sprite);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::getTurretRotation()
{
	sf::Vector2f tankPos = m_tank.position();
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);

	sf::Vector2f vec = { mousePos.x - tankPos.x, mousePos.y - tankPos.y };

	m_tank.setTurretHeading(atan2(vec.y, vec.x));
}

///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::processGameEvents(sf::Event& event)
{
	switch (m_gameState)
	{
	///////////////////////////////////////// GAMEPLAY
	case state::GamePlay:
		// check if the event is a a mouse button release
		if (sf::Event::KeyPressed == event.type)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_window.close();
				break;
			case sf::Keyboard::Space:
				m_tank.fire();
			default:
				break;
			}
		}

		if (sf::Event::KeyReleased == event.type)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::C:
				m_tank.toggleTurretFree();
				break;
			default:
				break;
			}
		}

		if (sf::Event::MouseButtonPressed == event.type)
		{
			if (sf::Mouse::Left == event.mouseButton.button)
			{
				m_tank.fire();
			}
		}
		break;
		///////////////////////////////////////// DEFAULT
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || // UP or W
		sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_tank.increaseSpeed();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || // DOWN or S
		sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_tank.decreaseSpeed();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || // LEFT or A
		sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_tank.decreaseRotation();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || // RIGHT or D
		sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_tank.increaseRotation();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::update(sf::Time dt)
{
	// if we've hit our maximum game time, set gamestate to GameOver
	if (m_gameClock.getElapsedTime() > m_maxGameTime)
	{
		m_gameState = state::GameOver;
	}

	switch (m_gameState)
	{
	case state::Loading:
		break;
	case state::GamePlay:

		handleKeyInput();

		getTurretRotation();

		m_tank.update(dt);

		break;
	case state::GameOver:
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::render()
{
	m_window.clear(sf::Color::Black);

	int timeRemaining = (m_maxGameTime - m_gameClock.getElapsedTime()).asSeconds();

	switch (m_gameState)
	{
	case state::Loading:
		m_text.setString("Loading . . .");
		m_window.draw(m_text);
		break;
	case state::GamePlay:

		m_window.draw(m_bgSprite);

		m_text.setString("Time Remaining: " + std::to_string(timeRemaining) );

		m_window.draw(m_text);

		for (auto& sprite : m_sprites)
		{
			m_window.draw(sprite);
		}

		for (auto& target : m_targets)
		{
			m_window.draw(target);
		}

		m_tank.render(m_window);

		break;
	case state::GameOver:
		break;
	default:
		break;
	}

	m_window.display();
}