#pragma once

// TODO: Setup all asset file paths in YAML file

#include <SFML/Graphics.hpp>
#include <Thor/Time.hpp>
#include "ScreenSize.h"
#include "Tank.h"
#include "Target.h"
#include "Obstacle.h"

#include <map>
#include <list>

/// <summary>
/// @author RP
/// @date June 2017
/// @version 1.0
/// 
/// </summary>

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

#include "LevelLoader.h"

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop results (lag) is stored. If this value is 
	///  greater than the notional time for one loop (MS_PER_UPDATE), then additional updates will be 
	///  performed until the lag is less than the notional time for one loop.
	/// The target is one update and one render cycle per game loop, but slower PCs may 
	///  perform more update than render operations in one loop.
	/// </summary>
	void run();

private: 

	/// <summary>
	/// @brief Keep track of the state of our game
	/// </summary>
	enum class state 
	{ 
		Null, 
		Loading,
		GamePlay, 
		Paused,
		GameOver 
	} m_gameState{ state::Loading };

	/// <summary>
	/// @brief Loads all game textures from file
	/// </summary>
	void loadTextures();

	/// <summary>
	/// @brief Loads all fonts from file
	/// </summary>
	void loadFonts();

	/// <summary>
	/// @brief Assigns textures to sprites and sets up sprite parameters
	/// </summary>
	void setupSprites();

	/// <summary>
	/// @brief Sets up a new game environment
	/// </summary>
	void init();

	/// <summary>
	/// @brief Pull our wall data from file and assign it to our obstacles
	/// </summary>
	void generateWalls();

	/// <summary>
	/// @brief Pull our target data from file and assign it to our target vector
	/// </summary>
	void generateTargets();

	/// <summary>
	/// @brief Takes a vector of game objects, calculates their grid position, and associates them with our spacial map
	/// </summary>
	void buildMap();

	/// <summary>
	/// @brief Find the grid position of a given point in 2D space
	/// </summary>
	/// <param name="t_pos">position of the object to check</param>
	/// <returns>grid reference as vector2i</returns>
	int getGridRef(sf::Vector2f t_pos);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="t_obj"></param>
	/// <returns></returns>
	std::array<sf::Vector2f, 4> getCorners(GameObject & t_obj);

	/// <summary>
	/// @brief Checks if any active targets have been hit
	/// </summary>
	void checkTargetsHit();

	/// <summary>
	/// @brief Shakes the sf::view based off our trauma variable
	/// </summary>
	void shakeScreen();

	/// <summary>
	/// @brief Decrease deltaScoreText opacity until it disappears
	/// </summary>
	void fadeDeltaScoreText();

	/// <summary>
	/// @brief pulls the next target into our active target array
	/// </summary>
	void nextTarget();

	/// <summary>
	/// Gets the angle between the mouse and the tank, and assign the turret that angle
	/// </summary>
	void getTurretRotation();

	/// <summary>
	/// Poll SF keyboard to check current state of keys
	/// </summary>
	void handleKeyInput();

	/// <summary>
	/// @brief Draws our targets to the screen
	/// </summary>
	void drawTargets();

	/// <summary>
	/// @brief Draw our time/score/accuracy
	/// </summary>
	void drawUI();

	/// <summary>
	/// @brief Draws our PAUSE overlay with flashing text
	/// </summary>
	void drawPauseScreen();

	/// <summary>
	/// @brief Draws our game over screen with game stats
	/// </summary>
	void drawGameOverScreen();

	// stores the data for our level
	LevelData m_level;

	sf::Texture m_menuBackgroundTexture;
	sf::Sprite m_menuBackgroundSprite;

	// used for game space partitioning 
	const int NUM_ROWS{ 10 }, NUM_COLS{ 10 };
	const sf::Vector2f CELL_SIZE{ ScreenSize::s_width / static_cast<float>(NUM_ROWS), ScreenSize::s_height / static_cast<float>(NUM_COLS)};

	// a mapping of our partition spaces to the sprites occupying them
	std::map<int, std::list<GameObject*>> m_spacialMap;

	// keeps track of game time
	thor::StopWatch m_gameClock;
	sf::Time m_maxGameTime{ sf::seconds(60.0f) };

	
	// keeps track of target timing
	thor::StopWatch m_targetClock;
	sf::Time m_targetDuration{ sf::seconds(5.0f) };

	// background sprite
	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;

	// obstacle sprites
	std::vector<Obstacle> m_obstacles;

	// target sprites
	std::vector<sf::Sprite> m_allTargets;
	std::vector<Target> m_activeTargets;
	int m_targetIndex{ 0 }; // track which target is active

	// visual representation of how long a given target has left on the screen
	sf::RectangleShape m_targetLoadingBar;
	float const MAX_BAR_WIDTH{ 80.0f };

	// A texture for the spritesheet
	sf::Texture m_spriteSheetTexture;

	// font and text
	sf::Font m_font;
	sf::Text m_text;

	// An instance representing the player controlled tank.
	Tank m_tank;

	// Player Statistics
	int m_score{ 0 };
	float m_accuracy{ 0.0f };

	// highscores
	int m_highscore{ 0 };
	float m_bestAccuracy{ 0.0f };

	// Used to calculate accuracy
	int m_shotsFired{ 0 };
	int m_targetsHit{ 0 };

	// Shows the player how much time was added on destroying target
	sf::Text m_deltaScoreText;

	// Used for fading of delta score text
	const sf::Time DELTA_SCORE_TIME{ sf::seconds(1.0f) };
	thor::StopWatch m_deltaScoreClock;

	// SCREENSHAKE variables
	float m_trauma{ 0.0f }; // scalar for screenshake, normalised to the range 0-1
	const float MAX_OFFSET = 50.0f; // max translational offset for screenshake
	const float MAX_ANGLE = 5.0f; // max rotational offset for screenshake
	sf::Text m_traumaMeter;

protected:
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(sf::Time dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	void processGameEvents(sf::Event&);

	// main window
	sf::RenderWindow m_window;
};
