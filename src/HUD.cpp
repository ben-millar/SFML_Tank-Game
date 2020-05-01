#include "HUD.h"

////////////////////////////////////////////////////////////
HUD::HUD(sf::Font& hudFont, GameData& t_gameData, GameState& t_state) :
	m_gameData{t_gameData},
	m_gameState{t_state}
{
	loadIcons();

	m_HUDTankSprite.setPosition({ 1260.0f,6.0f });

	m_gameText.setFont(hudFont);
	m_gameText.setCharacterSize(30);
	m_gameText.setFillColor(sf::Color::White);
	m_gameText.setPosition(sf::Vector2f(ScreenSize::s_width / 2.0f, 10));
	m_gameText.setString("Game Running");
}

////////////////////////////////////////////////////////////

void HUD::loadIcons()
try
{
	if (!m_HUDTankTexture.loadFromFile(".\\resources\\images\\HUD_Tank.png"))
	{
		std::string msg{ "Error loading icon texture in HUD>>loadIcons" };
		throw std::exception(msg.c_str());
	}
	else
	{
		m_HUDTankSprite.setTexture(m_HUDTankTexture);
		m_HUDTankSprite.setTextureRect({ 0,0,148,79 });

		m_damagedTrackSprite.setTexture(m_HUDTankTexture);
		m_damagedTrackSprite.setTextureRect({ 0,79,88,13 });
	}
}
catch (const std::exception& e)
{
	std::cout << e.what() << std::endl;
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

	m_healthBarOutline.setFillColor(sf::Color(0, 0, 0, 0));
	m_healthBarOutline.setOutlineColor(sf::Color::White);
	m_healthBarOutline.setOutlineThickness(2.0f);
	m_healthBarOutline.setPosition(HEALTH_BAR_POS);
	m_healthBarOutline.setSize(HEALTH_BAR_SIZE);


	m_healthBar[0].position = { HEALTH_BAR_POS };
	m_healthBar[1].position = { HEALTH_BAR_POS + sf::Vector2f{HEALTH_BAR_SIZE.x, 0.0f} };
	m_healthBar[2].position = { HEALTH_BAR_POS + HEALTH_BAR_SIZE};
	m_healthBar[3].position = { HEALTH_BAR_POS + sf::Vector2f{0.0f, HEALTH_BAR_SIZE.y} };

	m_healthBar[0].color = sf::Color::Red;
	m_healthBar[1].color = sf::Color::Red;
	m_healthBar[2].color = sf::Color(128, 0, 0, 255);
	m_healthBar[3].color = sf::Color(128, 0, 0, 255);
}

////////////////////////////////////////////////////////////

void HUD::update()
{
	if (GameState::GameOver == m_gameState || GameState::GameWin == m_gameState)
	{
		m_hudBackground[2].position = { m_width, ScreenSize::s_height };
		m_hudBackground[3].position = { 0.0f, ScreenSize::s_height };

		m_hudOutline[1].position = { 0.0f,0.0f };
		m_hudOutline[2].position = { 0.0f,0.0f };
	}
	else
	{
		m_healthBar[1].position = { HEALTH_BAR_POS + sf::Vector2f{HEALTH_BAR_SIZE.x * (m_gameData.playerHealth / 100.0f), 0.0f} };
		m_healthBar[2].position = { HEALTH_BAR_POS + sf::Vector2f{HEALTH_BAR_SIZE.x * (m_gameData.playerHealth / 100.0f), HEALTH_BAR_SIZE.y} };
	}
}

////////////////////////////////////////////////////////////

void HUD::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_hudOutline);
	t_window.draw(m_hudBackground);

	if (GameState::GameOver == m_gameState || GameState::GameWin == m_gameState)
	{
		// Game State Text
		setText(m_gameStateStrings.at(static_cast<int>(m_gameState)), { ScreenSize::s_width / 2.0f, 100.0f }, 72U, true);
		t_window.draw(m_gameText);

		// Score Text
		setText("Score: " + std::to_string(int(m_gameData.score)), { 200.0f, 350.0f }, 36U, false);
		t_window.draw(m_gameText);

		// Targets Collected
		setText("You collected: " + std::to_string(m_gameData.targetsCollected) + "/" + std::to_string(m_gameData.totalTargets) + " targets", { 200.0f, 430.0f }, 36U, false);
		t_window.draw(m_gameText);

		// Total Time
		setText("Total Time: " + timeAsString(m_gameData.timeElapsed), { 200.0f, 510.0f }, 36U, false);
		t_window.draw(m_gameText);

		// Restart
		setText("Press [R] to Restart!", { ScreenSize::s_width / 2.0f, 700.0f }, 42U, true);
		t_window.draw(m_gameText);
	}
	else
	{
		// ##### HEALTH BAR #####

		
		t_window.draw(m_healthBar);
		t_window.draw(m_healthBarOutline);

		setText("Health:", { 925.0f,5.0f }, 24U, false);
		t_window.draw(m_gameText);

		// ###### TANK ICON ######

		t_window.draw(m_HUDTankSprite);
		
		if (m_gameData.tankDamage.m_leftTrackDamaged)
		{
			flashIcon();

			m_damagedTrackSprite.setPosition(LEFT_TRACK_POS);
			t_window.draw(m_damagedTrackSprite);
		}

		if (m_gameData.tankDamage.m_rightTrackDamaged)
		{
			flashIcon();

			m_damagedTrackSprite.setPosition(RIGHT_TRACK_POS);
			t_window.draw(m_damagedTrackSprite);
		}


		// ###### TEXT ######

		// Game State Text
		setText(m_gameStateStrings.at(static_cast<int>(m_gameState)), { ScreenSize::s_width / 2.0f, 0.0f }, 48U, true);
		t_window.draw(m_gameText);

		// Score Text
		setText("Score: " + std::to_string(int(m_gameData.score)), { 20.0f, 10.0f }, 24U, false);
		t_window.draw(m_gameText);

		// Targets Collected
		setText(std::to_string(m_gameData.targetsCollected) + "/" + std::to_string(m_gameData.totalTargets) + " targets", { 20.0f, 40.0f }, 24U, false);
		t_window.draw(m_gameText);

		// Total time
		setText("Time: " + timeAsString(m_gameData.timeElapsed), { ScreenSize::s_width / 2.0f, 50.0f }, 24U, true);
		t_window.draw(m_gameText);
	}
}

////////////////////////////////////////////////////////////

void HUD::flashIcon()
{
	// Take the absolute value of a sine wave (range 0.0 to 1.0) and use it as a coefficient for our alpha
	sf::Uint8 alpha = std::abs(std::sin(MathUtility::DEG_TO_RAD*(m_sineFlash+=10))) * 255;

	m_damagedTrackSprite.setColor(sf::Color(255U, 255U, 255U, alpha));
}

////////////////////////////////////////////////////////////

void HUD::setText(std::string t_str, sf::Vector2f t_position, unsigned int t_fontSize, bool t_center)
{
	m_gameText.setCharacterSize(t_fontSize);
	m_gameText.setString(t_str);

	if (t_center)
	{
		m_gameText.setOrigin(m_gameText.getLocalBounds().width / 2.0f, 0.0f);
	}
	else
	{
		m_gameText.setOrigin(0.0f, 0.0f);
	}

	m_gameText.setPosition(t_position);
}

////////////////////////////////////////////////////////////

std::string HUD::timeAsString(float t_time)
{
	int minutes, seconds;

	minutes = t_time / 60;

	seconds = static_cast<int>(t_time) % 60;

	std::string output{ "" };

	if (minutes < 10)
	{
		output += "0";
	}

	output += std::to_string(minutes) + ":";

	if (seconds < 10)
	{
		output += "0";
	}

	output += std::to_string(seconds);

	return output;
}
