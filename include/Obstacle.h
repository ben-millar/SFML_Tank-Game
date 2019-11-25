#pragma once
#include "GameObject.h"
class Obstacle :
	public GameObject
{
public:
	Obstacle(sf::Sprite);

	void hit() override;

	inline bool isHit() { return m_isHit; }
	inline void reset() { m_isHit = false; }

private:

	bool m_isHit{ false };
};