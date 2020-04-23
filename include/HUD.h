#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "ScreenSize.h"
#include "GameState.h"
#include "GameData.h"


class HUD
{
public:
	/// <summary>
	/// @brief Default constructor that stores a font for the HUD and initialises the general HUD appearance.
	/// </summary>
	HUD(sf::Font& hudFont, GameData& t_gameData, GameState& t_state);

	/// <summary>
	/// @brief Initialise the HUD position/colours
	/// </summary>
	void init();

	/// <summary>
	/// @brief Checks the current game state and sets the appropriate status text on the HUD.
	/// </summary>
	/// <param name="gameState">The current game state</param>
	void update();

	/// <summary>
	/// @brief Draws the HUD outline and text.
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow& window);

private:

	/// <summary>
	/// @brief Loads in our icon spritesheet
	/// </summary>
	void loadIcons();

	sf::Texture m_iconTexture;
	sf::Sprite m_iconSprite;
	sf::Vector2i m_iconSize{ 64,64 };

	// This will cover up the icons if their effects are inactive
	sf::RectangleShape m_iconOcclusion;

	/// <summary>
	/// @brief Set the string and position for our text object
	/// </summary>
	void setText(std::string t_str, sf::Vector2f t_position, unsigned int t_fontSize, bool t_center);

	/// <summary>
	/// @brief Converts seconds to MM:SS notation
	/// </summary>
	/// <param name="t_time">Time in seconds</param>
	/// <returns>MM:SS in string form</returns>
	std::string timeAsString(float t_time);

	// Keep track of the game state
	GameState& m_gameState;

	std::vector<std::string> m_gameStateStrings
	{
		"NULL",
		"Loading",
		"Gameplay",
		"Paused",
		"Game Over"
	};

	// Contains information on score, time, etc. to display
	GameData& m_gameData;

	// The font for this HUD.
	sf::Font m_textFont;

	// A container for the current HUD text.
	sf::Text m_gameText;

	// A simple background shape for the HUD.
	sf::VertexArray m_hudBackground{ sf::Quads, 4U };
	sf::VertexArray m_hudOutline{ sf::Quads, 4U };

	sf::VertexArray m_healthBar{ sf::Quads, 4U };
	sf::VertexArray m_healthBarBackground{ sf::Quads, 4U };

	const sf::Vector2f HEALTH_BAR_POS{ 925.0f,37.5f };
	const sf::Vector2f HEALTH_BAR_SIZE{ 300.0f,35.0f };

	float m_width{ ScreenSize::s_width }, m_height{ 80.0f };
	float m_outlineThickness{ 5.0f };
};