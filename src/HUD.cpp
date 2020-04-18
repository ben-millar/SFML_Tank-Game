#include "HUD.h"

////////////////////////////////////////////////////////////
HUD::HUD(sf::Font& hudFont, GameData& t_gameData, GameState& t_state) :
	m_gameData{t_gameData},
	m_gameState{t_state}
{
	m_gameText.setFont(hudFont);
	m_gameText.setCharacterSize(30);
	m_gameText.setFillColor(sf::Color::White);
	m_gameText.setPosition(sf::Vector2f(ScreenSize::s_width / 2.0f, 10));
	m_gameText.setString("Game Running");
}

////////////////////////////////////////////////////////////

void HUD::init()
{
	//Setting up our hud properties 
	m_hudBackground[0].position = { 0.0f,0.0f };
	m_hudBackground[1].position = { m_width,0.0f };
	m_hudBackground[2].position = { m_width, m_height };
	m_hudBackground[3].position = { 0.0f, m_height };

	m_hudBackground[0].color = { sf::Color(128,128,128,128) };
	m_hudBackground[1].color = { sf::Color(128,128,128,128) };
	m_hudBackground[2].color = { sf::Color(0,0,0,128) };
	m_hudBackground[3].color = { sf::Color(0,0,0,128) };

	m_hudOutline[0].position = { 0.0f,m_height };
	m_hudOutline[1].position = { m_width, m_height };
	m_hudOutline[2].position = { m_width, m_height + m_outlineThickness };
	m_hudOutline[3].position = { 0.0f, m_height + m_outlineThickness };

	m_hudOutline[0].color = { sf::Color::White };
	m_hudOutline[1].color = { sf::Color::White };
	m_hudOutline[2].color = { sf::Color(128,128,128,255) };
	m_hudOutline[3].color = { sf::Color(128,128,128,255) };
}

////////////////////////////////////////////////////////////

void HUD::update()
{
	if (GameState::GameOver == m_gameState)
	{
		m_hudBackground[2].position = { m_width, ScreenSize::s_height };
		m_hudBackground[3].position = { 0.0f, ScreenSize::s_height };

		m_hudOutline[1].position = { 0.0f,0.0f };
		m_hudOutline[2].position = { 0.0f,0.0f };
	}
}

////////////////////////////////////////////////////////////

void HUD::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_hudBackground);
	t_window.draw(m_hudOutline);

	// Game State Text
	setText(m_gameStateStrings.at(static_cast<int>(m_gameState)), { 550.0f, 10.0f }, 48U);
	t_window.draw(m_gameText);

	// Score Text
	setText("Score: " + std::to_string(int(m_gameData.score)), { 20.0f, 10.0f }, 24U);
	t_window.draw(m_gameText);

	setText(std::to_string(m_gameData.targetsCollected) + "/" + std::to_string(m_gameData.totalTargets) + " targets", { 20.0f, 40.0f }, 24U);
	t_window.draw(m_gameText);
}

////////////////////////////////////////////////////////////

void HUD::setText(std::string t_str, sf::Vector2f t_position, unsigned int t_fontSize)
{
	m_gameText.setCharacterSize(t_fontSize);
	m_gameText.setString(t_str);
	m_gameText.setPosition(t_position);
}