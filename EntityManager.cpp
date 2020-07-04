#include "pch.h"
#include "EntityManager.h"

std::vector<std::shared_ptr<Block>> EntityManager::m_Blocks;
std::vector<std::shared_ptr<Echelle>> EntityManager::m_Ladders;
std::shared_ptr<Player> EntityManager::Player;

EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
}
