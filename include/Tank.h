#pragma once
#define DEBUG

#include <SFML/Graphics.hpp>
#include <Thor/Math.hpp>
#include <Thor/Time.hpp>
#include <Thor/Particles.hpp>
#include <Thor/Animations.hpp>

#include "CollisionDetector.h"
#include "CellResolution.h"

#include "GameState.h"
#include "GameObject.h"
#include "TankDamage.h"

#include "Obstacle.h"
#include "Target.h"

// Forward reference
class TankAi;

/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank : public GameObject
{
public:	
/// <summary>
/// @brief Constructor that stores drawable state (texture, sprite) for the tank.
/// Stores references to the texture and container of wall sprites. 
/// Creates sprites for the tank base and turret from the supplied texture.
/// </summary>
/// <param name="texture">A reference to the sprite sheet texture</param>
///< param name="texture">A reference to the container of wall sprites</param>
	Tank(sf::Texture const & t_texture, 
		std::map<int, std::list<GameObject*>>& t_obstacleMap, 
		std::vector<Target>& t_targetVector,
		TankAi& t_enemyTank,
		float& t_screenShake);

	inline sf::Vector2f position() const { return m_tankBase.getPosition(); }

	/// <summary>
	/// @brief Resets all tank attributes for new game
	/// </summary>
	void reset();

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
	/// @brief Get the tank turret sprite
	/// </summary>
	/// <returns>SF::Sprite object for tank turret</returns>
	inline sf::Sprite const getTurret() const { return m_turret; }

	/// <summary>
	/// @brief Get the tank base sprite
	/// </summary>
	/// <returns>SF::Sprite object for tank base</returns>
	inline sf::Sprite const getBase() const { return m_tankBase; }

	/// <summary>
	/// @brief Get the players health value
	/// </summary>
	/// <returns>Tank's health as a float</returns>
	inline float getHealth() const { return m_health; }

	/// <summary>
	/// @brief Get the players damage data
	/// </summary>
	/// <returns>TankDamage struct</returns>
	inline TankDamage getDamage() const { return m_damageLevels; }

	/// <summary>
	/// @brief Called when the player tank is hit by a projectile
	/// </summary>
	void hit() override;

	/// <summary>
	/// @brief Overload of game object base function
	/// </summary>
	/// <returns>A reference to our base sprite</returns>
	sf::Sprite& getSprite() override { return m_tankBase; }

	void update(sf::Time dt);
	void render(sf::RenderWindow & window);
	
private:

	/// <summary>
	/// Initialise values for debugging
	/// </summary>
	void temp_debugInit();

	/// <summary>
	/// @brief Checks for collisions between the tank and the walls.
	/// </summary>
	/// <returns>True if collision detected between tank and wall.</returns>
	bool checkWallCollision();

	/// <summary>
	/// @brief Checks for collisions between the tank and the targets.
	/// </summary>
	void checkTargetCollision();

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
	/// @brief Loads particle textures from file
	/// </summary>
	void loadParticleTextures();

	/// <summary>
	/// @brief Updates the game objects that are in our current grid space (spacially partitioned)
	/// </summary>
	void updateGameObjects();


	// ####### SPRITES AND TEXTURES #######

	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	sf::Texture const & m_texture;

	// A reference to the container of wall sprites.
	std::map<int, std::list<GameObject*>>& ref_obstacles;
	std::vector<GameObject*> m_obstacles;

	// A reference to the container of target sprites.
	std::vector<Target>& ref_targets;
	std::vector<GameObject*> m_targets;

	// A reference to the container of enemy tanks
	TankAi& ref_enemyTank;
	std::vector<GameObject*> m_enemyTanks;

	// ####################################



	// ########## THOR PARTICLES ##########

	void updateParticles(sf::Time t_dt);

	sf::Texture m_smokeTexture;
	sf::Texture m_sparkTexture;

	int m_smokeEmissionRate{ 0 };

	thor::ParticleSystem m_smokeParticleSystem;
	thor::ParticleSystem m_sparkParticleSystem;

	thor::UniversalEmitter m_sparksEmitter = thor::UniversalEmitter();
	thor::UniversalEmitter m_smokeEmitter = thor::UniversalEmitter();

	// ####################################



	// ########## TANK ATTRIBUTES #########

	// #### DAMAGE ####

	TankDamage m_damageLevels;

	// How long have we been damaged for?
	thor::StopWatch m_damageClock;
	// How long should we be damaged for before it's repaired?
	sf::Time m_damageTime{ sf::seconds(7.5f) };

	// ################
	
	static constexpr float M_MAX_SPEED = 100.0f;
	static constexpr float M_MIN_SPEED = -100.0f;
	static constexpr float M_FRICTION = 0.2f;

	const float MAX_HEALTH{ 100.0f };
	float m_health{MAX_HEALTH};

	// Fire clock 
	sf::Clock m_fireClock;
	sf::Time m_fireDelay{ sf::seconds(1.0f) };

	// The tank speed.
	float m_speed{ 0.0 };

	// can our tank rotate?
	bool m_enableRotation{ true };

	// Controls whether or not the turret can rotate independent of the base.
	bool m_turretFree{ false };

	// The current rotation as applied to tank base.
	float m_baseRotation{ 0.0 };
	float m_previousBaseRotation{ 0.0 };

	// The current rotation as applied to turret base.
	float m_turretRotation{ 0.0 };
	float m_previousTurretRotation{ 0.0 };

	// rolling storage of our last position
	sf::Vector2f m_previousPosition{ 0.0f,0.0f };
	
	// rolling storage of our last speed
	float m_previousSpeed;

	std::set<int> m_activeCells;

	// ####################################

	// Linked to the game trauma variable, controls amount of screenshake
	float& m_screenShake;

	// ############# DEBUGGING ############

	sf::RectangleShape temp_activeCellRect;

	// ####################################
};

#include "TankAI.h"