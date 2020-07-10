#pragma once

#define ECHELLE_COUNT 5
#define BLOCK_COUNT_X 8
#define BLOCK_COUNT_Y 5
#define BLOCK_SPACE 110.f
#define LEFT_LADDER_X 200.0f
#define RIGHT_LADDER_X 680.0f
#include <SFML/Audio.hpp>

class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void updatePlayer(sf::Time elpasedTime);
	void updateBarrels(sf::Time elapsedTime);
	void handleCollisions();
	void handlePlayerAndBarrelCollision();
	void handlePlayerAndPaulineCollision();
	void displayModal();
	void hideModal();
	void restart();
	void victory();
	void gameOver();
	void checkGameOver();
	void render();

	bool CanClimbUp();
	bool PlayerReachedFloor();
	bool CheckXMin();
	bool CheckXMax();
	void updateStatistics(sf::Time elapsedTime);
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void spawnBarrel();
	void adjustPlayerWithBlock(sf::FloatRect blockBounds);

private:
	static const float		PlayerSpeed;
	static const sf::Time	TimePerFrame;
	static const sf::Time   BarrelSpawnDelay;
	static const float      BarrelSpeed;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Font	mFont;
	sf::Text	mStatisticsText;
	sf::Text    mLivesText;
	sf::Text    mPanelTitle;
	sf::Text    mPanelInfo;
    sf::RectangleShape mPanelRectangle;
	sf::Time	mStatisticsUpdateTime;
    sf::SoundBuffer mVictorySoundBuffer;
    sf::Sound mVictorySound;
    sf::SoundBuffer mGameOverSoundBuffer;
    sf::Sound mGameOverSound;
    sf::SoundBuffer mHurtSoundBuffer;
    sf::Sound mHurtSound;
    sf::SoundBuffer mMarioJumpSoundBuffer;
    sf::Sound mMarioJumpSound;
    sf::SoundBuffer mBarrelSoundBuffer;
    sf::Sound mBarrelSound;
    std::string mVictorySoundPath = "Media/Audio/zelda-secret.wav";
    std::string mGameOverSoundPath = "Media/Audio/sitcom-laugh.wav";
    std::string mHurtSoundPath = "Media/Audio/minecraft-hurt.wav";
    std::string mMarioJumpSoundPath = "Media/Audio/mario-jump.wav";
    std::string mBarrelSoundPath = "Media/Audio/barrel.wav";


	std::size_t	mStatisticsNumFrames;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;
	bool mIsPaused;

	float mGravity;
	float minX;
	float maxX;


	sf::Sprite	_Echelle[ECHELLE_COUNT];
	sf::Sprite	_Block[BLOCK_COUNT_X][BLOCK_COUNT_Y];
	sf::Sprite	_Weapon;
	
	sf::Vector2u _sizeBlock;
	sf::Vector2u _sizeMario;

	sf::Texture _TextureDK;
	sf::Texture _TexturePauline;
	sf::Texture	_TextureWeapon;
	sf::Texture	_TextureEchelle;
	sf::Texture	_TextureBlock;
	sf::Texture _TextureBarrel;
};

