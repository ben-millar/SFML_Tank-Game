#include "Game.h"
#include "MathUtility.h"
#include <iostream>

// Updates per milliseconds
static const sf::Time MS_PER_UPDATE = sf::seconds(1.0f/60.0f);

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "SFML Playground", sf::Style::Default),
	m_tank(m_spriteSheetTexture, m_spatialMap, m_activeTargets, m_aiTank, m_trauma),
	m_aiTank(m_spriteSheetTexture, m_spatialMap, m_obstacles),
	m_HUD(m_font, m_gameData, m_gameState)
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

	init();

	m_aiTank.init(m_level.m_aiTank.m_position.at(0));

	// set state to GamePlay
	m_gameState = GameState::GamePlay;

	m_deltaScoreClock.start();
	m_targetClock.start();
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
			throw std::exception("Error loading background texture from file in game.cpp>loadTextures");
		}
		if (!m_spriteSheetTexture.loadFromFile(".\\resources\\images\\SpriteSheet.png"))
		{
			throw std::exception("Error loading SpriteSheet texture from file in game.cpp>loadTextures");
		}
		if (!m_menuBackgroundTexture.loadFromFile(".\\resources\\images\\MainMenuBackground.png"))
		{
			throw std::exception("Error loading menuBackgroundTexture from file in game.cpp>loadTextures");
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

		m_deltaScoreText.setFont(m_font);
		m_deltaScoreText.setCharacterSize(16U);
		m_deltaScoreText.setFillColor(sf::Color::Yellow);
		m_deltaScoreText.setOutlineColor(sf::Color::Black);
		m_deltaScoreText.setOutlineThickness(2.0f);
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

	m_menuBackgroundSprite.setTexture(m_menuBackgroundTexture);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::init()
{
	// Restart game clocks
	m_gameClock.restart();

	// Reset trauma/screenshake variable
	m_trauma = 0.0f;

	// Reset score counters
	m_targetIndex = 0;

	m_gameData.totalTargets = 10;
	m_gameData.targetsCollected = 0;
	m_gameData.timePerTarget.clear();
	m_gameData.score = 0;

	// Clear targets array and push back first target
	m_activeTargets.clear();
	m_activeTargets.push_back(m_allTargets[m_targetIndex]);

	// Get rid of delta score text
	m_deltaScoreText.setPosition({ -100.0f,-100.0f });

	buildMap();

	m_HUD.init();

	m_aiTank.init({ 720.0f,450.0f });
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::generateWalls()
{
	//sf::IntRect wallRect(2, 129, 33, 23);

	sf::IntRect wallRect;

	// Create the Walls 
	for (ObstacleData const& obstacle : m_level.m_obstacles)
	{
		// Randomly choose one of our rock sprites
		switch (rand() % 3)
		{
		case 0:
			wallRect = { 48, 90, 64, 64 };
			break;
		case 1:
			wallRect = { 112, 90, 96, 64 };
			break;
		case 2:
			wallRect = { 48, 160, 80, 64 };
			break;
		default:
			break;
		}	

		// Random rotation
		int rotation = rand() % 360;

		sf::Sprite sprite;
		sprite.setTexture(m_spriteSheetTexture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0f, wallRect.height / 2.0f);
		sprite.setPosition(obstacle.m_position);
		//sprite.setRotation(obstacle.m_baseRotation);
		sprite.setRotation(rotation);

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

		float offsetX = static_cast<float>(rand() % target.m_randomOffset.x);
		float offsetY = static_cast<float>(rand() % target.m_randomOffset.y);

		sprite.move({ offsetX, offsetY });

		m_allTargets.push_back(sprite);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::buildMap()
{
	// for each obstacle in our vector
	for (GameObject& obs : m_obstacles)
	{
		// for each corner of our obstacle
		for (sf::Vector2f pos : CellResolution::getCorners(obs.getSprite()))
		{
			// find the grid ref
			int grid{ CellResolution::getGridRef(pos) };

			// If we get an error value back, continue
			if (grid == -1) continue;

			// check if we already account for this object at this position in our map
			bool alreadyInList{ false };
			for (GameObject* mapObject : m_spatialMap[grid])
			{
				if (mapObject == &obs)
				{
					alreadyInList = true;
					break;
				}
			}

			// associate our obstacle with the grid ref in our map if unaccounted for
			if (!alreadyInList) (m_spatialMap[grid].push_back(&obs));
		}
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

		// Toggle between command line window and game
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				m_window.close();
			}
		}

		processGameEvents(event);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::processGameEvents(sf::Event& event)
{
	// GAMEPLAY
	if (GameState::GamePlay == m_gameState)
	{
		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::P == event.key.code)
			{
				m_gameState = GameState::Paused;
				m_gameClock.stop();
			}

			if (sf::Keyboard::C == event.key.code)
			{
				m_tank.toggleTurretFree();
			}
		}
	}
	// PAUSED
	else if (GameState::Paused == m_gameState)
	{
		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::P == event.key.code)
			{
				m_gameState = GameState::GamePlay;
				m_gameClock.start();
			}
		}
	}
	// GAME OVER
	else if (GameState::GameOver == m_gameState)
	{
		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::R == event.key.code)
			{
				init();
				m_gameState = GameState::GamePlay;
			}
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
	switch (m_gameState)
	{
	case GameState::Loading:
		break;
	case GameState::GamePlay:

		handleKeyInput();

		getTurretRotation();

		// Check for collisions with AI tank
		if (m_aiTank.collidesWithPlayer(m_tank))
		{
			gameOver();
		}

		if (m_gameData.targetsCollected >= m_gameData.totalTargets)
		{
			gameOver();
		}

		if (m_tank.getHealth() <= 0.0f)
		{
			gameOver();
		}

		checkTargetsHit();

		m_tank.update(dt);

		m_aiTank.update(m_tank, dt);

		// update game time for HUD
		m_gameData.timeElapsed = m_gameClock.getElapsedTime().asSeconds();

		// update player health for HUD
		m_gameData.playerHealth = m_tank.getHealth();

		// reduce trauma linearly to zero
		(m_trauma > 0.005f) ? m_trauma -= 0.005f : m_trauma = 0.0f;
		m_traumaMeter.setString(std::to_string(m_trauma));

		fadeDeltaScoreText();

		shakeScreen();

		break;
	case GameState::GameOver:
		break;
	default:
		break;
	}

	m_HUD.update();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::checkTargetsHit()
{
	// setup our new target
	for (auto& t : m_activeTargets)
	{
		if (t.isHit())
		{
			// How long did it take the player to hit the target?
			sf::Time targetTime{ m_targetClock.getElapsedTime() };
			m_gameData.timePerTarget.push_back(targetTime.asSeconds());

			m_targetClock.restart();

			m_gameData.targetsCollected++;

			int deltaScore{ calculateScore(targetTime) };
			m_gameData.score += deltaScore;
			
			t.reset();

			m_deltaScoreText.setString("+" + std::to_string(deltaScore));
			m_deltaScoreText.setPosition(t.getSprite().getPosition());

			m_deltaScoreClock.restart();

			nextTarget();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

int Game::calculateScore(sf::Time t_timeToHit)
{
	// Maximum score it's possible to get per target
	int maxScore{ 1000 };

	// Minimum score it's possible to get for a target
	int minScore{ 100 };
	
	// Time at which score will plateau
	sf::Time lowestScoreTime{ sf::seconds(20.0f) };

	// What proportion of the 'max' time did we take, normalised to the range 1 - 0
	float proportionOfTimeTaken{ 1 - (t_timeToHit.asSeconds() / lowestScoreTime.asSeconds()) };

	// Square easing function; score drops off inversely proportional to the square of the time taken
	float scoreScale{ std::pow(proportionOfTimeTaken,2) };
	
	int score{ static_cast<int>(maxScore * scoreScale) };

	// Put a lower cap on the score
	return (score > minScore) ? score : minScore;
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

	// View target
	sf::Vector2f target = (m_tank.getBase().getPosition());
	
	target = ((target * 0.75f) + (view.getCenter() * 0.25f));

	// Center cannot go outside of the background
	target.x = std::clamp(target.x, ScreenSize::s_width / 2.0f, ScreenSize::s_width * 1.5f);
	target.y = std::clamp(target.y, ScreenSize::s_height / 2.0f, ScreenSize::s_height * 1.5f);

	// Asymptotic average of target and current view; lazy camera
	view.setCenter(target);

	m_window.setView(view);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::fadeDeltaScoreText()
{
	if (m_deltaScoreClock.getElapsedTime() < DELTA_SCORE_TIME)
	{
		// non-linear fading (when time remaining is 1, coefficient is 1. when time remaining is 0.5, coefficient is 0.25)
		float timeCoefficient = m_deltaScoreClock.getElapsedTime() / DELTA_SCORE_TIME;

		sf::Uint8 alpha = static_cast<sf::Uint8>(255U * pow(timeCoefficient, 2));

		m_deltaScoreText.setFillColor(sf::Color(255U, 255U, 0U, 255U - alpha));
		m_deltaScoreText.setOutlineColor(sf::Color(0U, 0U, 0U, 255U - alpha));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::nextTarget()
{
	m_targetIndex++;

	// take previous target out of the array
	m_activeTargets.clear();

	// add new target to the array
	m_activeTargets.push_back(m_allTargets[m_targetIndex % m_allTargets.size()]);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::render()
{
	m_window.clear(sf::Color::Black);

	// LOADING
	if (GameState::Loading == m_gameState)
	{
		m_text.setString("Loading . . .");
		m_window.draw(m_text);
	}

	// GAMEPLAY OR PAUSED
	if (GameState::GamePlay == m_gameState || GameState::Paused == m_gameState)
	{
		m_window.draw(m_bgSprite);

		for (auto& i : m_obstacles)
		{
			m_window.draw(i.getSprite());
		}

		for (auto& target : m_activeTargets)
		{
			m_window.draw(target.getSprite());
		}

		m_tank.render(m_window);

		m_aiTank.render(m_window);

		if (m_deltaScoreClock.getElapsedTime() < DELTA_SCORE_TIME) m_window.draw(m_deltaScoreText);

		// PAUSED
		if (GameState::Paused == m_gameState)
		{
			drawPauseScreen();
		}
	}

	// Store the current view transform
	sf::View currentView = m_window.getView();

	// we want to draw the HUD such that it ignores the global view transforms
	m_window.setView(m_window.getDefaultView()); 
	m_HUD.render(m_window);

	// Restore the view transforms
	m_window.setView(currentView);

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

///////////////////////////////////////////////////////////////////////////////////////////////

void Game::gameOver()
{
	m_gameState = GameState::GameOver;

	//if (m_score > m_highscore) m_highscore = m_score;

	m_tank.reset();
}