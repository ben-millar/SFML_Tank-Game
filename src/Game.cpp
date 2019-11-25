#include "Game.h"
#include <iostream>

// Updates per milliseconds
static const sf::Time MS_PER_UPDATE = sf::seconds(1.0f/60.0f);

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "SFML Playground", sf::Style::Default),
	m_tank(m_spriteSheetTexture, m_obstacles, m_activeTargets)
{
	// Game runs much faster with this commented out. Why?
	// Seems to limit our refresh rate to that of the monitor
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);

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

	// add first target to our active targets
	m_activeTargets.push_back(Target(m_allTargets[m_targetIndex]));

	// set state to GamePlay
	m_gameState = state::GamePlay;

	// restart game timer
	m_gameClock.restart();

	// restart target timer
	m_targetClock.restart();

	m_targetLoadingBar.setFillColor(sf::Color::Red);
	m_targetLoadingBar.setSize({ 50.0f,10.0f });
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

		m_traumaMeter.setFont(m_font);
		m_traumaMeter.setPosition({ 10.0f,30.0f });
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

	// overdraw the background slightly to account for later screenshake
	m_bgSprite.setScale(1.1f, 1.1f);
	m_bgSprite.setOrigin(m_bgSprite.getGlobalBounds().width / 2.0f, 
						 m_bgSprite.getGlobalBounds().height / 2.0f);
	m_bgSprite.setPosition({ ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f });
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

		m_obstacles.push_back(Obstacle(sprite));
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

		m_allTargets.push_back(sprite);
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
	// GAMEPLAY
	if (state::GamePlay == m_gameState)
	{
		if (sf::Event::MouseButtonPressed == event.type)
		{
			if (sf::Mouse::Left == event.mouseButton.button)
			{
				if (m_tank.fire())
				{
					(m_trauma < 0.5f) ? m_trauma += 0.5f : m_trauma = 1.0f;
				}
			}
		}

		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::Space == event.key.code)
			{
				if (m_tank.fire())
				{
					(m_trauma < 0.5f) ? m_trauma += 0.5f : m_trauma = 1.0f;
				}
			}

			if (sf::Keyboard::P == event.key.code)
			{
				m_gameState = state::Paused;
				m_gameClock.stop();
				m_targetClock.stop();
			}

			if (sf::Keyboard::C == event.key.code)
			{
				m_tank.toggleTurretFree();
			}
		}
	}
	// PAUSED
	else if (state::Paused == m_gameState)
	{
		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::P == event.key.code)
			{
				m_gameState = state::GamePlay;
				m_gameClock.start();
				m_targetClock.start();
			}
		}
	}

	// ALL STATES
	if (sf::Event::KeyPressed == event.type)
	{
		if (sf::Keyboard::Escape == event.key.code)
		{
			m_window.close();
		}
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

	// setup our new target
	if (m_targetClock.getElapsedTime() > m_targetDuration)
	{
		m_targetIndex++;

		// take previous target out of the array
		m_activeTargets.clear();

		// add new target to the array
		m_activeTargets.push_back(m_allTargets[m_targetIndex % m_allTargets.size()]);

		// restart our target clock
		m_targetClock.restart();
	}

	switch (m_gameState)
	{
	case state::Loading:
		break;
	case state::GamePlay:

		handleKeyInput();

		getTurretRotation();

		m_tank.update(dt);

		// reduce trauma linearly to zero
		(m_trauma > 0.005f) ? m_trauma -= 0.005f : m_trauma = 0.0f;
		m_traumaMeter.setString(std::to_string(m_trauma));

		shakeScreen();

		break;
	case state::GameOver:
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::shakeScreen()
{
	sf::View view = m_window.getDefaultView();

	// translational shake
	float offsetX = (rand() % 201 - 100) / 100.0f;
	float offsetY = (rand() % 201 - 100) / 100.0f;

	offsetX *= MAX_OFFSET * (m_trauma * m_trauma);
	offsetY *= MAX_OFFSET * (m_trauma * m_trauma);

	view.move(offsetX, offsetY);

	// rotational shake
	float angleOffset = (rand() % 201 - 100) / 100.0f;

	angleOffset *= MAX_ANGLE * (m_trauma * m_trauma);

	view.setRotation(angleOffset);

	m_window.setView(view);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::render()
{
	m_window.clear(sf::Color::Black);

	int timeRemaining = (m_maxGameTime - m_gameClock.getElapsedTime()).asSeconds();

	// LOADING
	if (state::Loading == m_gameState)
	{
		m_text.setString("Loading . . .");
		m_window.draw(m_text);
	}

	// GAMEPLAY OR PAUSED
	if (state::GamePlay == m_gameState || state::Paused == m_gameState)
	{
		m_window.draw(m_bgSprite);

		m_text.setString("Time Remaining: " + std::to_string(timeRemaining));

		m_window.draw(m_text);

		for (auto& i : m_obstacles)
		{
			m_window.draw(i.getSprite());
		}

		for (auto& target : m_activeTargets)
		{
			// set up target loading bar
			float barWidth = 10.0f * (m_targetDuration.asSeconds() - m_targetClock.getElapsedTime().asSeconds());
			m_targetLoadingBar.setOrigin({ barWidth / 2.0f, 0.0f });
			m_targetLoadingBar.setPosition(target.getSprite().getPosition().x, 
										   target.getSprite().getPosition().y + 25.0f);

			m_targetLoadingBar.setSize({ barWidth, 10.0f });

			m_window.draw(target.getSprite());
			m_window.draw(m_targetLoadingBar);
		}

		m_tank.render(m_window);

		m_window.draw(m_traumaMeter);

		// PAUSED
		if (state::Paused == m_gameState)
		{
			drawPauseScreen();
		}
	}
		
	m_window.display();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::drawPauseScreen()
{
	sf::Vector2f windowSize{ static_cast<float>(ScreenSize::s_width),
							 static_cast<float>(ScreenSize::s_height) };

	// SETUP grey overlay
	sf::RectangleShape pauseScreenCover;
	pauseScreenCover.setFillColor(sf::Color(196, 196, 196, 128));
	pauseScreenCover.setSize(windowSize);

	m_window.draw(pauseScreenCover);

	// SETUP pause text
	sf::Text pauseText("PAUSED", m_font, 70U);

	pauseText.setOrigin(pauseText.getGlobalBounds().width / 2.0f, pauseText.getGlobalBounds().height / 2.0f);
	pauseText.setPosition({ windowSize.x / 2.0f, windowSize.y / 2.0f });

	m_window.draw(pauseText);
}