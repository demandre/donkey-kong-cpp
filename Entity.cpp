#include "pch.h"
#include "Entity.h"

void Entity::RenderEntity(sf::RenderTarget* target) {
	target->draw(this->m_sprite);
}
