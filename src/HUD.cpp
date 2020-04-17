#include "HUD.h"

////////////////////////////////////////////////////////////
HUD::HUD(sf::Font& hudFont)
{
	m_gameStateText.setFont(hudFont);
	m_gameStateText.setCharacterSize(30);
	m_gameStateText.setFillColor(sf::Color::White);
	m_gameStateText.setPosition(sf::Vector2f(480, 5));
	m_gameStateText.setString("Game Running");

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
	m_hudOutline[1].position = { m_width,m_height};
	m_hudOutline[2].position = { m_width, m_height + m_outlineThickness };
	m_hudOutline[3].position = { 0.0f, m_height + m_outlineThickness };

	m_hudOutline[0].color = { sf::Color::White};
	m_hudOutline[1].color = { sf::Color::White};
	m_hudOutline[2].color = { sf::Color(128,128,128,255) };
	m_hudOutline[3].color = { sf::Color(128,128,128,255) };
}

////////////////////////////////////////////////////////////
void HUD::update(GameState const& gameState)
{
	switch (gameState)
	{
	case GameState::GamePlay:
		m_gameStateText.setString("GAMEPLAY");
		break;
	case GameState::Paused:
		m_gameStateText.setString("PAUSED");
		break;
	case GameState::GameOver:
		m_gameStateText.setString("GAME OVER");
		break;
	default:
		break;
	}
}

void HUD::render(sf::RenderWindow& window)
{
	window.draw(m_hudBackground);
	window.draw(m_hudOutline);
	window.draw(m_gameStateText);
}