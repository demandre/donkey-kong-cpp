#pragma once
#include "Entity.h"
class Barrel :
	public Entity
{
public:
	bool m_bLeftToRight = true;
	int m_times = 0;
};

