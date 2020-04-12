#include "MathUtility.h"

namespace MathUtility
{
	////////////////////////////////////////////////////////////
	
	double distance(sf::Vector2f v1, sf::Vector2f v2)
	{
		return std::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
	}

	////////////////////////////////////////////////////////////

	bool isLeft(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r)
	{
		return ((q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x)) > 0;
	}

	////////////////////////////////////////////////////////////
	
	bool lineIntersectsCircle(sf::Vector2f ahead, sf::Vector2f halfAhead, sf::CircleShape circle)
	{
		return distance(circle.getPosition(), ahead) <= circle.getRadius() ||
			distance(circle.getPosition(), halfAhead) <= circle.getRadius();
	}

	////////////////////////////////////////////////////////////
	
	sf::Vector2f truncate(sf::Vector2f v, float const max)
	{		
		if (thor::length(v) > max)
		{
			v = thor::unitVector(v);			
			v *= max;
		}	

		return v;
	}

}
