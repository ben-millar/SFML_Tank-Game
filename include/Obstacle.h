#pragma once
#include "GameObject.h"
class Obstacle :
	public GameObject
{
public:
	Obstacle(sf::Sprite);

	void hit() override;
};

