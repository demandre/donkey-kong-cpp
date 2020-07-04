#pragma once
#include "Entity.h"
class Player :
	public Entity
{
public:
	bool m_isClimbing = false;
	bool m_isJumping = false;
	bool m_facingRight = true;
	float m_velocityY = 0;
};

