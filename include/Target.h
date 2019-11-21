#pragma once
#include "GameObject.h"
class Target :
	public GameObject
{
public:
	Target(sf::Sprite);

	void hit() override;
};

