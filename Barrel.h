#pragma once
#include "Entity.h"
class Barrel :
	public Entity
{
public:
	bool m_bLeftToRight = false;
	bool m_down = false;
	float m_downVelocity = 0.f;
	int m_times = 0;
};

