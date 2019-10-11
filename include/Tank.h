#pragma once
#include <SFML/Graphics.hpp>

/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	Tank(sf::Texture const & texture, sf::Vector2f const & pos);
	inline void setPosition(sf::Vector2f const& m_pos) { initSprites(m_pos); }
	inline sf::Vector2f position() { return m_tankBase.getPosition(); }

	/// <summary>
	/// @brief Increases the speed by 1, max speed is capped at 100.
	/// </summary>
	void increaseSpeed();

	/// <summary>
	/// @brief Decreases the speed by 1, min speed is capped at -100.
	/// </summary>
	void decreaseSpeed();

	/// <summary>
	/// @brief Increases the rotation by 1 degree, wraps to 0 degrees after 359.
	/// </summary>
	void increaseRotation();

	/// <summary>
	/// @brief Decreases the rotation by 1 degree, wraps to 359 degrees after 0.
	/// </summary>
	void decreaseRotation();

	/// <summary>
	/// @brief Sets the absolute rotation of the turret.
	/// </summary>
	void setTurretHeading(float t_heading);

	/// <summary>
	/// @brief Toggles whether turret can rotate freely of base or not
	/// </summary>
	void toggleTurretFree();

	void update(double dt);
	void render(sf::RenderWindow & window);
	
private:
	void initSprites(sf::Vector2f const & pos);
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	sf::Texture const & m_texture;

	static constexpr double M_MAX_SPEED = 100.0;
	static constexpr double M_MIN_SPEED = -100.0;
	static constexpr double M_FRICTION = 0.05;

	// The tank speed.
	double m_speed{ 0.0 };

	// The current rotation as applied to tank base.
	double m_baseRotation{ 0.0 };
	// The current rotation as applied to turret base.
	double m_turretRotation{ 0.0 };
	// Controls whether or not the turret can rotate independent of the base.
	bool m_turretFree{ true };
};
