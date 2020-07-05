#include "pch.h"
#include "EntityManager.h"

std::vector<std::shared_ptr<Block>> EntityManager::m_Blocks;
std::vector<std::shared_ptr<Echelle>> EntityManager::m_Ladders;
std::vector<std::shared_ptr<Barrel>>  EntityManager::m_Barrels;
std::shared_ptr<DK> EntityManager::Dk;
std::shared_ptr<Player> EntityManager::Player;
std::shared_ptr<Pauline> EntityManager::Pauline;


EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
}
