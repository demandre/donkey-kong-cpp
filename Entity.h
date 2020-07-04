#pragma once

enum EntityType
{
	player,
	block,
	echelle
};

class Entity
{
public:
	Entity() { };
	~Entity() { };

public:
	sf::Sprite m_sprite;
	sf::Vector2u m_size;
	sf::Vector2f m_position;
	EntityType m_type;
	bool m_enabled = true;

	void RenderEntity(sf::RenderTarget* target);
};
