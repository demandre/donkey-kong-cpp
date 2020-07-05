#pragma once
#include "Entity.h"
class Player :
	public Entity
{
public:
	bool m_isClimbing = false;
	bool m_isJumping = false;
	bool m_facingRight = true;
	int m_lives = 3;
	float m_velocityY = 0;
};

