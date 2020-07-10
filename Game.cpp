#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"
#include "Block.h"
#include "Echelle.h"
#include "Player.h"
#include "Barrel.h"
#include "Pauline.h"

const float Game::PlayerSpeed = 150.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);
const float Game::BarrelSpeed = 75.f;
const sf::Time Game::BarrelSpawnDelay = sf::seconds(5.f);

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
		_Echelle[i].setPosition(i % 2 == 0 ? LEFT_LADDER_X : RIGHT_LADDER_X, 0.f + BLOCK_SPACE * (i + 1) + _sizeBlock.y );

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

	// Draw DK
	sf::Sprite spriteDk;
	std::shared_ptr<DK> entityDk = std::make_shared<DK>();
	sf::Vector2f scaleDk;
	scaleDk.x = 2.5;
	scaleDk.y = 2.5;

	_TextureDK.loadFromFile("Media/Textures/dk.png");
	spriteDk.setTexture(_TextureDK);
	spriteDk.setScale(scaleDk);
	spriteDk.setPosition(420.0f, 30.0f);

	entityDk->m_sprite = spriteDk;
	EntityManager::Dk = entityDk;

	// Draw Pauline
	sf::Sprite spritePauline;
	std::shared_ptr<Pauline> entityPauline = std::make_shared<Pauline>();
	sf::Vector2f scalePauline;
	scalePauline.x = 3.0f;
	scalePauline.y = 3.0f;

	_TexturePauline.loadFromFile("Media/Textures/pauline.png");
	spritePauline.setTexture(_TexturePauline);
	spritePauline.setPosition(650.0f, 44.0f);
	spritePauline.setScale(scalePauline);

	entityPauline->m_sprite = spritePauline;
	EntityManager::Pauline = entityPauline;


	// Draw Statistic Font 

	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setString("Welcome to Donkey Kong 1981");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);

	mLivesText.setString(
		"Lives : " + toString(EntityManager::Player->m_lives)
	);
	mLivesText.setFont(mFont);
	mLivesText.setPosition(5.f, 50.f);

	// Load Barrel's texture
	_TextureBarrel.loadFromFile("Media/Textures/Barrel.png");
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time barrelSpawnTimer = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		barrelSpawnTimer += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}

		if (barrelSpawnTimer > BarrelSpawnDelay) {
			barrelSpawnTimer = sf::Time::Zero;
			if(!mIsPaused) {
                spawnBarrel();
			}
		}

		handleCollisions();
		updateStatistics(elapsedTime);
		render();
	}
}

// EVENT PROCESSING

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
	if(mIsPaused) {
	    return;
    }
    updatePlayer(elapsedTime);
    updateBarrels(elapsedTime);
}

void Game::updatePlayer(sf::Time elapsedTime) {
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

void Game::updateBarrels(sf::Time elapsedTime) {
	for (std::shared_ptr<Barrel> barrel : EntityManager::m_Barrels) {
		if (!barrel->m_enabled) continue;
		if (barrel->m_sprite.getPosition().y > 600.0f) {
			barrel->m_enabled = false;
			continue;
		}

		sf::Vector2f movement(0.f, 0.f);

		if (!barrel->m_down && 
			((barrel->m_sprite.getPosition().x >= LEFT_LADDER_X - 1.f && barrel->m_sprite.getPosition().x <= LEFT_LADDER_X + 1.f) ||
			(barrel->m_sprite.getPosition().x >= RIGHT_LADDER_X - 1.f && barrel->m_sprite.getPosition().x <= RIGHT_LADDER_X + 1.f))) {
			barrel->m_down = true;
			barrel->m_downVelocity = BLOCK_SPACE;
		}

		if (barrel->m_down && barrel->m_downVelocity <= 0.f) {
			barrel->m_down = false;
			barrel->m_bLeftToRight = !barrel->m_bLeftToRight;
		}

		if (barrel->m_down) {
			float movementY = BarrelSpeed * elapsedTime.asSeconds() > barrel->m_downVelocity ? barrel->m_downVelocity : BarrelSpeed * elapsedTime.asSeconds();
			barrel->m_downVelocity -= BarrelSpeed * elapsedTime.asSeconds();

			barrel->m_sprite.move(0.f, movementY);
			continue;
		}
		else {
			if (barrel->m_bLeftToRight) {
				movement.x += BarrelSpeed;
			}
			else {
				movement.x -= BarrelSpeed;
			}
		}


		barrel->m_sprite.move(movement * elapsedTime.asSeconds());
	}
}

// RENDERS

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Block> block : EntityManager::m_Blocks) {
		block->RenderEntity(&mWindow);
	}

	for (std::shared_ptr<Echelle> ladder : EntityManager::m_Ladders) {
		ladder->RenderEntity(&mWindow);
	}

	for (std::shared_ptr<Barrel> barrel : EntityManager::m_Barrels) {
		if (!barrel->m_enabled) continue;
		barrel->RenderEntity(&mWindow);
	}

	EntityManager::Player->RenderEntity(&mWindow);
	EntityManager::Dk->RenderEntity(&mWindow);
	EntityManager::Pauline->RenderEntity(&mWindow);

	mWindow.draw(mStatisticsText);
	mWindow.draw(mLivesText);

    if (mIsPaused) {
        displayModal();
    }

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
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us"
		);

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Game::displayModal() {
    mPanelTitle.setFont(mFont);
    mPanelTitle.setPosition(100.f, 150.f);
    mPanelTitle.setCharacterSize(100);

    mPanelInfo.setFont(mFont);
    mPanelInfo.setString("Press ENTER to play again!");
    mPanelInfo.setFillColor(sf::Color::White);
    mPanelInfo.setPosition(110.f, 250.f);
    mPanelInfo.setCharacterSize(50);

    mPanelRectangle.setFillColor(sf::Color::Black);
    mPanelRectangle.setOutlineColor(sf::Color::White);
    mPanelRectangle.setOutlineThickness(5);
    mPanelRectangle.setPosition(50, 130);
    mPanelRectangle.setSize(sf::Vector2f(720, 200));


    mWindow.draw(mPanelRectangle);
    mWindow.draw(mPanelInfo);
    mWindow.draw(mPanelTitle);
}

void Game::hideModal() {
    mPanelTitle.setCharacterSize(0);
    mPanelInfo.setCharacterSize(0);
    mPanelRectangle.setSize(sf::Vector2f(0, 0));
}

void Game::restart() {
    hideModal();
    mIsPaused = false;

    std::shared_ptr<Player> player = EntityManager::Player;
    sf::Vector2f posMario;
    posMario.x = 100.f + 100.f;
    posMario.y = BLOCK_SPACE * 5 - _sizeMario.y;
    mPlayer.setPosition(posMario);
    player->m_sprite = mPlayer;
    player->m_type = EntityType::player;
    player->m_size = mTexture.getSize();
    player->m_position = mPlayer.getPosition();
    player->m_lives = 3;
    mLivesText.setString(
            "Lives : " + toString(EntityManager::Player->m_lives)
    );

    EntityManager::Player = player;
    EntityManager::m_Barrels.clear();
}

void Game::victory() {
    mPanelTitle.setString("YOU WIN !!! :D");
    mPanelTitle.setFillColor(sf::Color::Green);
    mIsPaused = true;

    if (mVictorySoundBuffer.loadFromFile(mVictorySoundPath))
    {
        mVictorySound.setBuffer(mVictorySoundBuffer);
        mVictorySound.play();
    }
}


void Game::gameOver() {
    mPanelTitle.setString("GAME OVER :(");
    mPanelTitle.setFillColor(sf::Color::Red);
    mIsPaused = true;

    if (mGameOverSoundBuffer.loadFromFile(mGameOverSoundPath))
    {
        mGameOverSound.setBuffer(mGameOverSoundBuffer);
        mGameOverSound.setPlayingOffset(sf::milliseconds(500.f));
        mGameOverSound.play();
    }
}

// PLAYER CHECKS

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
	std::shared_ptr<Player> player = EntityManager::Player;
	float lowerBound = -1.f;
	float upperBound = 2.f;
	bool adjust = false;

	if (player->m_velocityY >= 330.0f) {
		upperBound = 6.f;
		adjust = true;
	}

	for (std::shared_ptr<Block> block : EntityManager::m_Blocks) {
		if (!block->m_enabled) {
			continue;
		}

		sf::FloatRect blockBounds = block->m_sprite.getGlobalBounds();
		sf::FloatRect playerBound = player->m_sprite.getGlobalBounds();
		float difference = blockBounds.top - (playerBound.top + playerBound.height);

		if (difference >= lowerBound && difference <= upperBound) {
			if (adjust) adjustPlayerWithBlock(blockBounds);
			return true;
		}
	}

	return false;
}

void Game::adjustPlayerWithBlock(sf::FloatRect blockBounds) {
	std::shared_ptr<Player> player = EntityManager::Player;

	player->m_sprite.setPosition(player->m_sprite.getPosition().x, blockBounds.top - player->m_sprite.getGlobalBounds().height);
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

void Game::checkGameOver() {
	if (EntityManager::Player->m_lives == 0) {
		gameOver();
	}
}

// HANDLE INPUTS

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
    if (key == sf::Keyboard::Enter) {
        restart();
        return;
    }

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
			player->m_velocityY = -215;

            if (mMarioJumpSoundBuffer.loadFromFile(mMarioJumpSoundPath))
            {
                mMarioJumpSound.setBuffer(mMarioJumpSoundBuffer);
                mMarioJumpSound.setVolume(5.f);
                mMarioJumpSound.setPlayingOffset(sf::milliseconds(300.f));
                mMarioJumpSound.play();
            }
		}
	}
}

// SPAWN ENTITIES

void Game::spawnBarrel() {
	sf::Sprite spriteBarrel;
	std::shared_ptr<Barrel> entityBarrel = std::make_shared<Barrel>();

	spriteBarrel.setTexture(_TextureBarrel);
	spriteBarrel.setPosition(383.0f, 85.0f);
	spriteBarrel.setScale(0.75f, 0.75f);

	entityBarrel->m_sprite = spriteBarrel;

	EntityManager::m_Barrels.push_back(entityBarrel);

    if (mBarrelSoundBuffer.loadFromFile(mBarrelSoundPath))
    {
        mBarrelSound.setBuffer(mBarrelSoundBuffer);
        mMarioJumpSound.setVolume(5.f);
        mBarrelSound.play();
    }
}

// COLLISIONS

void Game::handleCollisions() {
    if(!mIsPaused) {
        handlePlayerAndBarrelCollision();
        handlePlayerAndPaulineCollision();
    }
}

void Game::handlePlayerAndBarrelCollision() {
	std::shared_ptr<Player> player = EntityManager::Player;
	sf::FloatRect playerBound = player->m_sprite.getGlobalBounds();

	for (std::shared_ptr<Barrel> barrel : EntityManager::m_Barrels) {
		if (!barrel->m_enabled) {
			continue;
		}
		sf::FloatRect barrelBound = barrel->m_sprite.getGlobalBounds();

		if (playerBound.intersects(barrelBound)) {
			player->m_lives -= 1;
			mLivesText.setString(
				"Lives : " + toString(EntityManager::Player->m_lives)
			);
			checkGameOver();

			player->m_isJumping = true;
			player->m_velocityY = -150;
			player->m_isClimbing = false;

			barrel->m_enabled = false;

            if (mHurtSoundBuffer.loadFromFile(mHurtSoundPath))
            {
                mHurtSound.setBuffer(mHurtSoundBuffer);
                mHurtSound.play();
            }
		}
	}
}

void Game::handlePlayerAndPaulineCollision() {
	sf::FloatRect playerBound = EntityManager::Player->m_sprite.getGlobalBounds();
	sf::FloatRect paulineBound = EntityManager::Pauline->m_sprite.getGlobalBounds();

	if (playerBound.intersects(paulineBound)) {
		victory();
	}
}
