#pragma once
#define DEBUG

#include <SFML/Graphics.hpp>
#include <Thor/Particles.hpp>
#include <Thor/Math.hpp>
#include <thread>
#include "CollisionDetector.h"
#include "CellResolution.h"
#include "ProjectilePool.h"
#include "GameState.h"

#include "Obstacle.h"
#include "Target.h"

// Forward reference
class TankAi;

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
	Tank(sf::Texture const & t_texture, 
		std::map<int, std::list<GameObject*>>& t_obstacleMap, 
		std::vector<Target>& t_targetVector,
		TankAi& t_enemyTank);

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
	/// 
	/// </summary>
	/// <returns></returns>
	inline sf::Sprite const getTurret() const { return m_turret; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	inline sf::Sprite const getBase() const { return m_tankBase; }

	/// <summary>
	/// @brief Fires a projectile
	/// </summary>
	bool fire();

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

	/// <summary>
	/// 
	/// </summary>
	void updateGameObjects();

	/// <summary>
	/// @brief Handles turret firing effects
	/// </summary>
	void muzzleFlash(sf::Vector2f t_fireDir);

	/// <summary>
	/// @brief Handles impact smoke effects
	/// </summary>
	/// <param name="t_impactPos">location of impact</param>
	void projectileImpact(sf::Vector2f t_impactPos);

	/// <summary>
	/// @brief Alias for projectileImpact function
	/// </summary>
	std::function<void(Tank*, sf::Vector2f)> f_projectileImpact;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="t_impactPos"></param>
	void impactSmoke(sf::Vector2f t_impactPos);

	/// <summary>
	/// @brief Alias for impactSmoke function
	/// </summary>
	std::function<void(Tank*, sf::Vector2f)> f_impactSmoke;

	// ############# THREADS ##############

	std::thread* m_smokeThread;

	// ####################################
	

	// ########## THOR PARTICLES ##########
	
	sf::Texture m_smokeTexture;
	sf::Texture m_sparkTexture;

	thor::ParticleSystem m_smokeParticleSystem;
	thor::ParticleSystem m_sparkParticleSystem;

	thor::UniversalEmitter m_sparksEmitter = thor::UniversalEmitter();
	thor::UniversalEmitter m_smokeEmitter = thor::UniversalEmitter();

	thor::ParticleSystem m_impactParticleSystem;
	thor::UniversalEmitter m_impactSmokeEmitter;

	// ####################################


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


	// ########### PROJECTILES ############

	ProjectilePool m_projectilePool = ProjectilePool();

	// ####################################


	// ########## TANK ATTRIBUTES #########
	
	static constexpr double M_MAX_SPEED = 100.0;
	static constexpr double M_MIN_SPEED = -100.0;
	static constexpr double M_FRICTION = 0.2;

	// Fire clock 
	sf::Clock m_fireClock;
	sf::Time m_fireDelay{ sf::seconds(1.0f) };

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


	std::set<int> m_activeCells;

	// ####################################



	// ############# DEBUGGING ############

	sf::RectangleShape temp_activeCellRect;

	// ####################################
};

#include "TankAI.h"