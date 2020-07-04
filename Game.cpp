#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"
#include "Block.h"
#include "Echelle.h"
#include "Player.h"
#include "Barrel.h"

const float Game::PlayerSpeed = 150.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(840, 600), "Donkey Kong 1981", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);

	// Set min and max limit
	minX = 840;
	maxX = 0;

	// Set gravity
	mGravity = 10;

	// Draw blocks

	_TextureBlock.loadFromFile("Media/Textures/Block.png");
	_sizeBlock = _TextureBlock.getSize();

	for (int i = 0; i < BLOCK_COUNT_X; i++)
	{
		for (int j = 0; j < BLOCK_COUNT_Y; j++)
		{
			_Block[i][j].setTexture(_TextureBlock);
			_Block[i][j].setPosition(100.f + 70.f * (i + 1), 0.f + BLOCK_SPACE * (j + 1));

			std::shared_ptr<Block> se = std::make_shared<Block>();
			se->m_sprite = _Block[i][j];
			se->m_type = EntityType::block;
			se->m_size = _TextureBlock.getSize();
			se->m_position = _Block[i][j].getPosition();

			sf::FloatRect bounds = _Block[i][j].getGlobalBounds();
			if (bounds.left < minX)
				minX = bounds.left;

			if (bounds.left + bounds.width > maxX)
				maxX = bounds.left + bounds.width;

			EntityManager::m_Blocks.push_back(se);
		}
	}
	std::cout << minX << "  " << maxX;
	// Draw Echelles

	_TextureEchelle.loadFromFile("Media/Textures/Echelle.png");

	for (int i = 0; i < ECHELLE_COUNT; i++)
	{
		_Echelle[i].setTexture(_TextureEchelle);
		_Echelle[i].setPosition(100.f + 70.f * (i + 1), 0.f + BLOCK_SPACE * (i + 1) + _sizeBlock.y );

		std::shared_ptr<Echelle> se = std::make_shared<Echelle>();
		se->m_sprite = _Echelle[i];
		se->m_type = EntityType::echelle;
		se->m_size = _TextureEchelle.getSize();
		se->m_position = _Echelle[i].getPosition();
		EntityManager::m_Ladders.push_back(se);
	}

	// Draw Mario

	mTexture.loadFromFile("Media/Textures/Mario_small_transparent.png"); // Mario_small.png");
	_sizeMario = mTexture.getSize();
	mPlayer.setTexture(mTexture);
	sf::Vector2f posMario;
	sf::Vector2f originSprite;
	posMario.x = 100.f + 100.f;
	posMario.y = BLOCK_SPACE * 5 - _sizeMario.y;

	originSprite = mPlayer.getOrigin();
	originSprite.x = _sizeMario.x / 2;

	mPlayer.setPosition(posMario);
	mPlayer.setOrigin(originSprite);

	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::Player = player;

	// Draw Statistic Font 

	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setString("Welcome to Donkey Kong 1981");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime)
{
	std::shared_ptr<Player> player = EntityManager::Player;
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp) {
		if (!player->m_isClimbing && CanClimbUp())
			player->m_isClimbing = true;
		
		if (player->m_isClimbing) {
			movement.y -= PlayerSpeed;
			player->m_isJumping = false;
			player->m_velocityY = 0.0f;
		}
	}
	if (mIsMovingDown) {
		if (player->m_isClimbing)
			movement.y += PlayerSpeed;
	}
	if (mIsMovingLeft) {
		if (player->m_facingRight) {
			player->m_sprite.setScale(-1.f, 1.f);
			player->m_facingRight = false;
		}

		if (!player->m_isClimbing && CheckXMin())
			movement.x -= PlayerSpeed;
	}
	if (mIsMovingRight) {
		if (!player->m_facingRight) {
			player->m_sprite.setScale(1.f, 1.f);
			player->m_facingRight = true;
		}

		if (!player->m_isClimbing && CheckXMax())
			movement.x += PlayerSpeed;
	}
		
	if (player->m_isJumping) {
		player->m_velocityY += mGravity;
	}
	movement.y += player->m_velocityY;

	player->m_sprite.move(movement * elapsedTime.asSeconds());

	if (player->m_isClimbing && PlayerReachedFloor()) {
		player->m_isClimbing = false;
	}

	if (player->m_isJumping && player->m_velocityY > 0 && PlayerReachedFloor()) {
		player->m_isJumping = false;
		player->m_velocityY = 0;
	}
}

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Block> block : EntityManager::m_Blocks) {
		block->RenderEntity(&mWindow);
	}

	for (std::shared_ptr<Echelle> ladder : EntityManager::m_Ladders) {
		ladder->RenderEntity(&mWindow);
	}

	EntityManager::Player->RenderEntity(&mWindow);

	mWindow.draw(mStatisticsText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	//
	// Handle collision
	//

	if (mStatisticsUpdateTime >= sf::seconds(0.050f))
	{
		// Handle collision weapon enemies
	}
}

bool Game::CanClimbUp() {
	for (std::shared_ptr<Echelle> ladder : EntityManager::m_Ladders) {
		if (!ladder->m_enabled) {
			continue;
		}

		sf::FloatRect playerBound;
		sf::FloatRect ladderBound;
		
		playerBound = EntityManager::Player->m_sprite.getGlobalBounds();
		ladderBound = ladder->m_sprite.getGlobalBounds();

		if (playerBound.intersects(ladderBound)) {
			return true;
		}
	}

	return false;
}

bool Game::PlayerReachedFloor() {
	for (std::shared_ptr<Block> block : EntityManager::m_Blocks) {
		if (!block->m_enabled) {
			continue;
		}

		sf::FloatRect blockBounds = block->m_sprite.getGlobalBounds();
		sf::FloatRect playerBound = EntityManager::Player->m_sprite.getGlobalBounds();
		float difference = blockBounds.top - (playerBound.top + playerBound.height);
		
		if (difference >= -1.0 && difference <= 2.0) {
			return true;
		}
	}

	return false;
}

bool Game::CheckXMin() {
	std::shared_ptr<Player> player = EntityManager::Player;
	sf::FloatRect playerBounds = player->m_sprite.getGlobalBounds();

	return playerBounds.left > minX;
}

bool Game::CheckXMax() {
	std::shared_ptr<Player> player = EntityManager::Player;
	sf::FloatRect playerBounds = player->m_sprite.getGlobalBounds();

	return playerBounds.left + playerBounds.width < maxX;
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Right)
		mIsMovingRight = isPressed;

	if (key == sf::Keyboard::Space)
	{
		std::shared_ptr<Player> player = EntityManager::Player;
		if (!player->m_isClimbing && !player->m_isJumping) {
			player->m_isJumping = true;
			player->m_velocityY = -200;
		}
	}
}
