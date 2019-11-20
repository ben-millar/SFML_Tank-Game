#pragma once

// TODO: Setup all asset file paths in YAML file

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "Tank.h"

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
	/// @brief Pull our wall data from file and assign it to our obstacles
	/// </summary>
	void generateWalls();

	/// <summary>
	/// @brief Pull our target data from file and assign it to our target vector
	/// </summary>
	void generateTargets();

	/// <summary>
	/// Gets the angle between the mouse and the tank, and assign the turret that angle
	/// </summary>
	void getTurretRotation();

	/// <summary>
	/// Poll SF keyboard to check current state of keys
	/// </summary>
	void handleKeyInput();

	// stores the data for our level
	LevelData m_level;

	// keeps track of game time
	sf::Clock m_gameClock;
	sf::Time m_maxGameTime{ sf::seconds(60.0f) };

	// tank sprite
	sf::Texture m_tankTexture;
	//sf::Sprite m_tankSprite;

	// background sprite
	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;

	// obstacle sprites
	std::vector<sf::Sprite> m_sprites;

	// target sprites
	std::vector<sf::Sprite> m_targets;

	// A texture for the spritesheet
	sf::Texture m_spriteSheetTexture;

	// font and text
	sf::Font m_font;
	sf::Text m_text;

	// An instance representing the player controlled tank.
	Tank m_tank;

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
