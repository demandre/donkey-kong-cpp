#pragma once
#include "Entity.h"
#include "Player.h"
#include "Block.h"
#include "Echelle.h"
#include "Barrel.h"

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

public:
	static std::vector<std::shared_ptr<Block>> m_Blocks;
	static std::vector<std::shared_ptr<Echelle>> m_Ladders;
	static std::vector<std::shared_ptr<Barrel>> m_Barrels;
	static std::shared_ptr<Player> Player;
};

