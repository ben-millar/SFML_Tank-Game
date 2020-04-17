#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"


class HUD
{
public:
	/// <summary>
	/// @brief Default constructor that stores a font for the HUD and initialises the general HUD appearance.
	/// </summary>
	HUD(sf::Font& hudFont);

	/// <summary>
	/// @brief Checks the current game state and sets the appropriate status text on the HUD.
	/// </summary>
	/// <param name="gameState">The current game state</param>
	void update(GameState const& gameState);

	/// <summary>
	/// @brief Draws the HUD outline and text.
	///
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow& window);

private:
	// The font for this HUD.
	sf::Font m_textFont;

	// A container for the current HUD text.
	sf::Text m_gameStateText;

	// A simple background shape for the HUD.
	sf::VertexArray m_hudBackground{ sf::Quads, 4U };
	sf::VertexArray m_hudOutline{ sf::Quads, 4U };

	float m_width{ 1440.0f }, m_height{ 80.0f };
	float m_outlineThickness{ 5.0f };
};