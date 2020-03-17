#pragma once
#define DEBUG

#include "MathUtility.h"
#include "Tank.h"
#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>
#include "GameState.h"
#include <iostream>
#include <queue>

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
	/// @brief Steers the AI tank towards the player tank avoiding obstacles along the way.
	/// Gets a vector to the player tank and sets steering and velocity vectors towards
	/// the player if current behaviour is seek. If behaviour is stop, the velocity vector
	/// is set to 0. Then compute the correct rotation angle to point towards the player tank. 
	/// If the distance to the player tank is < MAX_SEE_AHEAD, then the behaviour is changed from seek to stop.
	/// Finally, recalculate the new position of the tank base and turret sprites.
	/// </summary>
	/// <param name="playerTank">A reference to the player tank</param>
	/// <param name="dt">update delta time</param>
	void update(Tank const & playerTank, double dt);

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
	/// 
	/// </summary>
	void hit() override;

	/// <summary>
	/// @brief Draws the tank base and turret.
	///
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow & window);

	/// <summary>
	/// @brief Initialises the obstacle container and sets the tank base/turret sprites to the specified position.
	/// <param name="position">An x,y position</param>
	/// </summary>
	void init(sf::Vector2f position);

	enum class AIState
	{
		PATROL_MAP,
		ATTACK_PLAYER
	} m_currentState{ AIState::PATROL_MAP };

private:

	/// <summary>
	/// @brief Setup the bounding box and origin of our base and turret sprites
	/// </summary>
	void initSprites();

	/// <summary>
	/// @brief Updates our position and calculates our net steering force
	/// </summary>
	/// <param name="dt">Time since last frame</param>
	void updateMovement(double dt);

	/// <summary>
	/// @brief Finds a vector along which we must travel to get to the player
	/// </summary>
	/// <param name="playerPosition">Current position of the player</param>
	/// <returns>Steering force which will guide us to the player</returns>
	sf::Vector2f seek(sf::Vector2f playerPosition) const;

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

	// Cells we should check for collisions in
	std::set<int> m_activeCells;

	// A reference to the sprite sheet texture.
	sf::Texture const & m_texture;

	// A sprite for the tank base.
	sf::Sprite m_tankBase;

	// A sprite for the turret
	sf::Sprite m_turret;

	// A reference to the container of wall sprites.
	std::map<int, std::list<GameObject*>>& ref_obstacles;

	// A container of circles that represent the obstacles to avoid.
	std::vector<sf::CircleShape> m_obstacles;

	// The current rotation as applied to tank base and turret.
	double m_rotation{ 0.0 };

	const float MASS{ 5.0f };

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

	

	enum class AiBehaviour
	{
		SEEK_PLAYER,
		STOP,
		RETREAT
	} m_aiBehaviour;
};