#pragma once
#include "GameObject.h"
class Obstacle :
	public GameObject
{
public:
	Obstacle(sf::Sprite);

	sf::Sprite& getSprite() override { return m_sprite; }

	void hit() override;

	inline bool isHit() { return m_isHit; }
	inline void reset() { m_isHit = false; }

private:

	bool m_isHit{ false };
};