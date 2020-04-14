#pragma once
#define DEBUG

#include "MathUtility.h"
#include "Tank.h"
#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>
#include <Thor/Particles.hpp>
#include <Thor/Animations.hpp>

#include "GameState.h"
#include "ProjectilePool.h"
#include <iostream>
#include <queue>
#include <thread>

class TankAi : public GameObject
{
public:
	/// <summary>
	/// @brief Constructor that stores a reference to the obstacle container.
	/// Initialises steering behaviour to seek (player) mode, sets the AI tank position and
	///  initialises the steering vector to (0,0) meaning zero force magnitude.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="wallSprites">A reference to the container of wall sprites</param>
	TankAi(sf::Texture const & texture, std::map<int, std::list<GameObject*>>& t_obstacleMap);

	/// <summary>
	/// @brief Gives the AI tank a vector of obstacle sprites for raycast checks
	/// </summary>
	/// <param name="t_obstacleSprites">Vector of sprites</param>
	void setupObstaclePositions(std::vector<sf::Sprite> t_obstacleSprites);

	/// <summary>
	/// @brief Steers the AI tank towards the player tank avoiding obstacles along the way.
	/// Gets a vector to the player tank and sets steering and velocity vectors towards
	/// the player if current behaviour is seek. If behaviour is stop, the velocity vector
	/// is set to 0. Then compute the correct rotation angle to point towards the player tank. 
	/// If the distance to the player tank is < MAX_SEE_AHEAD, then the behaviour is changed from seek to stop.
	/// Finally, recalculate the new position of the tank base and turret sprites.
	/// </summary>
	/// <param name="playerTank">A reference to the player tank</param>
	/// <param name="dt">update delta time</param>
	void update(Tank & playerTank, sf::Time dt);

	/// <summary>
	/// @brief Checks for collision between the AI and player tanks.
	/// </summary>
	/// <param name="player">The player tank instance</param>
	/// <returns>True if collision detected between AI and player tanks.</returns>
	bool collidesWithPlayer(Tank const& playerTank) const;

	/// <summary>
	/// @brief Returns a reference to our base sprite
	/// </summary>
	sf::Sprite& getSprite() override { return m_tankBase; }

	/// <summary>
	/// @brief Called when we're hit by a projectile
	/// </summary>
	void hit() override;

	/// <summary>
	/// @brief Draws the tank base and turret.
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow & window);

	/// <summary>
	/// @brief Sets the tank base/turret sprites to the specified position.
	/// <param name="position">An x,y position</param>
	/// </summary>
	void init(sf::Vector2f position);

	enum class AIState
	{
		PATROL_MAP,
		FOLLOW_PLAYER,
		ATTACK_PLAYER
	} m_currentState{ AIState::FOLLOW_PLAYER };

private:

	/// <summary>
	/// @brief Setup the bounding box and origin of our base and turret sprites
	/// </summary>
	void initSprites();

	/// <summary>
	/// @brief Sets up the vision cone
	/// </summary>
	void initVisionCone();

	/// <summary>
	/// @brief Updates our position and calculates our net steering force
	/// </summary>
	/// <param name="dt">Time since last frame</param>
	void updateMovement(sf::Time dt);

	/// <summary>
	/// @brief Aim our turret at the player position
	/// </summary>
	/// <param name="t_playerPos">Position of the player</param>
	void aimTurret(sf::Vector2f t_playerPos);

	/// <summary>
	/// @brief Calculate our steering vector
	/// </summary>
	void steer();

	/// <summary>
	/// @brief Determine which way we should be facing given our movement vector
	/// </summary>
	void determineHeading();

	/// <summary>
	/// @brief Calculate a steering vector such that we follow the player
	/// </summary>
	void followPlayer(sf::Vector2f t_playerPos);

	/// <summary>
	/// @brief Roam around the map searching for the player
	/// </summary>
	void search();

	/// <summary>
	/// @brief Checks which obstacle corners could be within our vision cone
	/// </summary>
	void prioritiseCorners();

	/// <summary>
	/// @brief Updates the position of the vision cone
	/// </summary>
	void updateVisionCone();

	/// <summary>
	/// @brief Determines the angle between ray casts given their magnitude and arc length
	/// </summary>
	/// <returns>Angle between two rays</returns>
	float calculatePitch();

	/// <summary>
	/// @brief Finds a vector along which we must travel to get to the player
	/// </summary>
	/// <param name="playerPosition">Current position of the player</param>
	/// <returns>Steering force which will guide us to the player</returns>
	sf::Vector2f seek(sf::Vector2f playerPosition) const;

	/// <summary>
	/// @brief Fires a projectile in the direction of the turret
	/// </summary>
	void fire();

	/// <summary>
	/// @brief Looks ahead for obstacles that we're going to it
	/// </summary>
	/// <returns>A steering force which will prevent us from hitting some obstacle</returns>
	sf::Vector2f collisionAvoidance();

	/// <summary>
	/// @brief Identifies which obstacle is the most immediate threat to us (I.e., we're about to hit it)
	/// </summary>
	/// <returns>Bounding circle of the obstacle</returns>
	const sf::CircleShape findMostThreateningObstacle();

	/// <summary>
	/// @brief Updates which game objects are in our local area (spatial partitioning code)
	/// </summary>
	void updateGameObjects();

	// ########### THOR VFX ############

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
	std::function<void(TankAi*, sf::Vector2f)> f_projectileImpact;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="t_impactPos"></param>
	void impactSmoke(sf::Vector2f t_impactPos);

	/// <summary>
	/// @brief Alias for impactSmoke function
	/// </summary>
	std::function<void(TankAi*, sf::Vector2f)> f_impactSmoke;

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

	// Cells we should check for collisions in
	std::set<int> m_activeCells;

	// A reference to the sprite sheet texture.
	sf::Texture const & m_texture;

	// A sprite for the tank base.
	sf::Sprite m_tankBase;

	// A sprite for the turret
	sf::Sprite m_turret;

	// Projectile pool for firing
	ProjectilePool m_projectilePool;

	// A reference to the container of wall sprites.
	std::map<int, std::list<GameObject*>>& ref_obstacleMap;

	// A container of circles that represent the obstacles to avoid.
	std::vector<sf::CircleShape> m_obstacleColliders;
	std::vector<GameObject*> m_obstacles;

	// Positions on-screen of each corner of each obstacle
	std::vector<sf::Vector2f> m_obstacleCorners;

	// The current rotation in degrees as applied to tank base.
	float m_baseRotation{ 0.0f };

	// The current rotation in degrees as applied to turret.
	float m_turretRotation{ 0.0f };

	// How far our turret can turn per game tick
	const float TURN_RATE{ 0.1f };

	// Throttle the tank's fire rate to one shot per delay time
	sf::Clock m_fireClock;
	sf::Time m_fireDelay{ sf::seconds(2.0f) };

	const float MASS{ 4.0f };

	// Current velocity.
	sf::Vector2f m_velocity;

	// Steering vector.
	sf::Vector2f m_steering;

	// The ahead vector.
	sf::Vector2f m_ahead;

	// The half-ahead vector.
	sf::Vector2f m_halfAhead;

	// The maximum see ahead range.
	static float constexpr MAX_SEE_AHEAD{ 50.0f };

	// The maximum avoidance turn rate.
	static float constexpr MAX_AVOID_FORCE{ 50.0f };

	// 
	static float constexpr MAX_FORCE{ 10.0f };

	// The maximum speed for this tank.
	float MAX_SPEED = 50.0f;


	// ##### VISION CONE #####


	// Distance the vision cone projects out
	float m_visionDistance{ 200.0f };

	// Arc subtended by the cone (inversely related to the length)
	float m_visionArc{ thor::Pi / 3.0f }; // 60 degrees

	// Angle between each ray
	float m_arcPerRay;

	// Distance between distal ends of ray casts; vision arc will reduce at longer range to maintain this.
	// We will also check for collisions at this interval along the ray, creating a 'net' with a 20px mesh
	float m_visionAcuity{ 20.0f }; 
	const float MIN_ACUITY{ 10.0f };
	const float MAX_ACUITY{ 30.0f };

	// How many ray casts we'll put out
	static const int NUM_RAYS{ 10 };

	// The actual 'rays' we're casting out, origin is tank position!
	std::array<sf::Vector2f, NUM_RAYS> m_visionRayCasts;

	// Used for drawing the ray casts on-screen.
	// We will replace this with a convex shape
	sf::VertexArray m_visionCone{ sf::TriangleFan };
};