#pragma once
#include <SFML/Graphics.hpp>
#include <Thor/Particles.hpp>
#include <Thor/Math.hpp>
#include "CollisionDetector.h"
#include "ProjectilePool.h"

/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
/// <summary>
/// @brief Constructor that stores drawable state (texture, sprite) for the tank.
/// Stores references to the texture and container of wall sprites. 
/// Creates sprites for the tank base and turret from the supplied texture.
/// </summary>
/// <param name="texture">A reference to the sprite sheet texture</param>
///< param name="texture">A reference to the container of wall sprites</param>
	Tank(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites);

	inline sf::Vector2f position() { return m_tankBase.getPosition(); }

	/// <summary>
	/// @brief Sets the initial position of our tank and turret
	/// </summary>
	/// <param name="m_pos">Position for tank and turret</param>
	void setPosition(sf::Vector2f const& m_pos);


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

	/// <summary>
	/// @brief Fires a projectile
	/// </summary>
	void fire();

	void update(sf::Time dt);
	void render(sf::RenderWindow & window);
	
private:

	/// <summary>
	/// @brief Checks for collisions between the tank and the walls.
	/// </summary>
	/// <returns>True if collision detected between tank and wall.</returns>
	bool checkWallCollision();

	/// <summary>
	/// @brief Stops the tank if moving and applies a small increase in speed in the opposite direction of travel.
	/// - If the tank speed is currently 0, the rotation is set to a value that is less than the previous rotation value
	///   (scenario: tank is stopped and rotates into a wall, so it gets rotated towards the opposite direction).
	/// - If the tank is moving, further rotations are disabled and the previous tank position is restored.
	///   The tank speed is adjusted so that it will travel slowly in the opposite direction. The tank rotation 
	///   is also adjusted as above if necessary (scenario: tank is both moving and rotating, upon wall collision it's 
	///   speed is reversed but with a smaller magnitude, while it is rotated in the opposite direction of it's 
	///   pre-collision rotation).
	/// </summary>
	void deflect();

	/// <summary>
	/// @brief Adjusts the rotation of the tank following a collision with an obstacle
	/// </summary>
	void adjustRotation();

	/// <summary>
	/// @brief
	/// </summary>
	void initSprites();

	/// <summary>
	/// @brief Set up THOR particle system/emitters
	/// </summary>
	void initParticles();

	// ########## THOR PARTICLES ##########
	
	sf::Texture m_particleTexture;
	thor::ParticleSystem m_particleSystem;

	thor::UniversalEmitter m_emitter = thor::UniversalEmitter();

	// ####################################


	// ####### SPRITES AND TEXTURES #######

	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	sf::Texture const & m_texture;

	// A reference to the container of wall sprites.
	std::vector<sf::Sprite>& m_wallSprites;

	// ####################################


	// ########### PROJECTILES ############

	ProjectilePool m_projectilePool = ProjectilePool();

	// ####################################


	// ########## TANK ATTRIBUTES #########
	
	static constexpr double M_MAX_SPEED = 100.0;
	static constexpr double M_MIN_SPEED = -100.0;
	static constexpr double M_FRICTION = 0.1;

	// The tank speed.
	double m_speed{ 0.0 };

	// can our tank rotate?
	bool m_enableRotation{ true };

	// Controls whether or not the turret can rotate independent of the base.
	bool m_turretFree{ false };

	// The current rotation as applied to tank base.
	double m_baseRotation{ 0.0 };
	double m_previousBaseRotation{ 0.0 };

	// The current rotation as applied to turret base.
	double m_turretRotation{ 0.0 };
	double m_previousTurretRotation{ 0.0 };

	// rolling storage of our last position
	sf::Vector2f m_previousPosition{ 0.0f,0.0f };
	
	// rolling storage of our last speed
	double m_previousSpeed;

	// ####################################
};
