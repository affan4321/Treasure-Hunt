#pragma once

#include <iostream>
#include <queue>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "SFML/Graphics.hpp"
using namespace std;

// USING THIS STRUCTURE FOR IDENTIFYING THE POSITIONS OF BOXES
struct Offset {
	float X;
	float Y;
};

// USING THIS STRUCTURE FOR STORING THE MESSAGE THAT THREADS SEND
struct Msg {
	Offset offset;
	int item;
};

// USING THIS STRUCTURE TO KEEP TRSCK OF ALL THE ITEMS ON BOARD
struct Items {
	sf::CircleShape* itemArr;
	int totalItems;
	int ITEMS_ON_BOARD;
	Offset* offset;
};

class TreasureHunt
{
private:
	// Main Window Variables
	sf::RenderWindow* window;
	sf::Event e;

	bool endGame;

	// Board Variables
	int Boardsize;
	Offset** offsetsBoard;  // 2d offset array to keep note of their X,Y offsets
	sf::RectangleShape** Board;  // 2d array for all locations
public:	
	Items items; // Info about all the items

	// Player Variables
	sf::CircleShape player1;
	sf::CircleShape player2;
	sf::CircleShape Bot;
	float spawnTimer;
	float spawnTimerMax;

	// Text and Fonts
	sf::Font font;
	sf::Text text;

	// Initializer
	void initAll();
	void initBoard();
	void initItems();
	void initFonts();
	void initTexts();

	// Constructor / Destructor
	TreasureHunt();
	~TreasureHunt();

	// Functions
	void Render();
	void RenderText(sf::RenderTarget&);
	void UpdateText();
	void Update();
	void UpdateBot();
	void spawnBot();
	void PollEvents();
	void P1Collisions();
	void P2Collisions();
	void BotCollisions();
	int generateRandomNumber();
	Offset** OffsetsGenerator(int size);  // Just a function to initialize offset array

	// Accessors
	const bool isRunning() const;
	const bool endGameAccessor() const;
	const sf::Event getEvent() const;
	sf::RenderWindow* getWindow() const;
};

// non-Member Initializers
void* initPlayer1(void*);
void* initPlayer2(void*);
void* initBot(void*);

