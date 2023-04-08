#include "TreasureHunt.h"
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <atomic>
#include <sstream>

queue<Msg> player1_MSG_Queue;       // Separate queue for Player 1
queue<Msg> player2_MSG_Queue;     // Separate queue for Player 2
queue<Msg> bot_MSG_Queue;     // Separate queue for Bot
int player1Pts = 0;
int player2Pts = 0;
int BotPts = 0;

// NON MEMBER INITIALIZERS
void* initPlayer1(void* args)   // THREAD 1
{
	TreasureHunt* game = (TreasureHunt*)args;
	
	float playerSize = 50.f;  // Radius
	float playerPosX = 30.f;
	float playerPosY = 5.f;

	game->player1.setScale(sf::Vector2f(0.5f, 0.5f));
	game->player1.setRadius(playerSize);
	game->player1.setFillColor(sf::Color::Yellow);
	game->player1.setOutlineColor(sf::Color::Black);
	game->player1.setOutlineThickness(3.f);
	game->player1.setPosition(playerPosX, playerPosY);

	// PLAYER 1 MOVEMENT
	sf::Event P1event = game->getEvent();
	while (game->getWindow()->pollEvent(P1event)) {
		if (P1event.type == sf::Event::Closed)
			game->getWindow()->close();
		else if (P1event.type == sf::Event::KeyPressed) {
			if (P1event.key.code == sf::Keyboard::Escape)
				game->getWindow()->close();
			else if (P1event.key.code == sf::Keyboard::W)
				game->player1.move(0.f, -50.f);
			else if (P1event.key.code == sf::Keyboard::A)
				game->player1.move(-50.f, 0.f);
			else if (P1event.key.code == sf::Keyboard::S)
				game->player1.move(0.f, 50.f);
			else if (P1event.key.code == sf::Keyboard::D)
				game->player1.move(50.f, 0.f);
		}
	}

	// PLAYER 1 COLLISIONS
	sf::Vector2f pos = game->player1.getPosition();
	for(int i=0; i<17; i++){
		if((pos.x >= game->items.offset[i].X && pos.x <= game->items.offset[i].X + 50.f) && (pos.y >= game->items.offset[i].Y && pos.y <= game->items.offset[i].Y + 20.f))
		{
			Msg msg;
			msg.offset.X = pos.x;
			msg.offset.Y = pos.y;
			msg.item = i;
			player1_MSG_Queue.push(msg);
		}
	}


	pthread_exit(0);
}
void* initPlayer2(void* args)   // THREAD 2
{	TreasureHunt* game = (TreasureHunt*)args;
	
	float playerSize = 50.f;  // Radius
	float playerPosX = 142.f;
	float playerPosY = 5.f;

	game->player2.setScale(sf::Vector2f(0.5f, 0.5f));
	game->player2.setRadius(playerSize);
	game->player2.setFillColor(sf::Color::Green);
	game->player2.setOutlineColor(sf::Color::Black);
	game->player2.setOutlineThickness(3.f);
	game->player2.setPosition(playerPosX, playerPosY);

	// PLAYER 2 MOVEMENT
	sf::Event P2event = game->getEvent();
	while (game->getWindow()->pollEvent(P2event)) {
		if (P2event.type == sf::Event::Closed)
			game->getWindow()->close();
		else if (P2event.type == sf::Event::KeyPressed) {
			if (P2event.key.code == sf::Keyboard::Escape)
				game->getWindow()->close();
			else if (P2event.key.code == sf::Keyboard::Up)
				game->player2.move(0.f, -50.f);
			else if (P2event.key.code == sf::Keyboard::Down)
				game->player2.move(0.f, 50.f);
			else if (P2event.key.code == sf::Keyboard::Left)
				game->player2.move(-50.f, 0.f);
			else if (P2event.key.code == sf::Keyboard::Right)
				game->player2.move(50.f, 0.f);
		}
	}

	// PLAYER 2 COLLISIONS
	sf::Vector2f pos = game->player2.getPosition();
	for(int i=0; i<17; i++){
		if((pos.x >= game->items.offset[i].X && pos.x <= game->items.offset[i].X + 50.f) && (pos.y >= game->items.offset[i].Y && pos.y <= game->items.offset[i].Y + 20.f))
		{
			Msg msg;
			msg.offset.X = pos.x;
			msg.offset.Y = pos.y;
			msg.item = i;
			player2_MSG_Queue.push(msg);
		}
	}

	pthread_exit(0);
}
void* initBot(void* args)   // THREAD 3
{
	TreasureHunt* game = (TreasureHunt*)args;
	
	game->spawnTimerMax = 100.f;
	game->spawnTimer = game->spawnTimerMax;
	game->spawnBot();
	float playerSize = 50.f;  // Radius

	game->Bot.setScale(sf::Vector2f(0.5f, 0.5f));
	game->Bot.setRadius(playerSize);
	game->Bot.setFillColor(sf::Color::Red);
	game->Bot.setOutlineColor(sf::Color::Black);
	game->Bot.setOutlineThickness(3.f);

	// BOT COLLISIONS
	sf::Vector2f pos = game->Bot.getPosition();
	for(int i=0; i<17; i++){
		if((pos.x >= game->items.offset[i].X && pos.x <= game->items.offset[i].X + 50.f) && (pos.y >= game->items.offset[i].Y && pos.y <= game->items.offset[i].Y + 20.f))
		{
			Msg msg;
			msg.offset.X = pos.x;
			msg.offset.Y = pos.y;
			msg.item = i;
			bot_MSG_Queue.push(msg);
		}
	}

	pthread_exit(0);
}



// INITIALIZER FUNCTIONS
void TreasureHunt::initAll()
{
	// WINDOW
	int height = 1904;  // Calculated by dividing screen size equally on 17 spaces
	int width = 1071;
	this->window = new sf::RenderWindow(sf::VideoMode(height, width), "Treasure Hunt By Muhammad Affan (21i-0474)", sf::Style::Fullscreen);
	this->window->setFramerateLimit(144);


	//END GAME
	this->endGame = 0;
}
void TreasureHunt::initBoard()
{
	this->Boardsize = 17; //this->generateRandomNumber();
	this->offsetsBoard = this->OffsetsGenerator(this->Boardsize);

	float OffsetX = 0, OffsetY = 0;
	for (int i = 0; i < this->Boardsize; i++) {
		for (int j = 0; j < this->Boardsize; j++) {
			offsetsBoard[i][j] = { OffsetX,OffsetY };
			OffsetX += 112.f;  // (1904 / 17) = 112
		}
		OffsetY += 63.f;     // (1071 / 17) = 63
		OffsetX = 0;
	}

	// Size generated is 17 x 17
	this->Board = new sf::RectangleShape * [this->Boardsize];
	for (int i = 0; i < this->Boardsize; i++)
		this->Board[i] = new sf::RectangleShape[this->Boardsize];

	for (int i = 0; i < this->Boardsize; i++) {
		for (int j = 0; j < this->Boardsize; j++) {
			this->Board[i][j].setPosition(this->offsetsBoard[i][j].X, this->offsetsBoard[i][j].Y);
			this->Board[i][j].setSize(sf::Vector2f(112.f, 63.f));
			this->Board[i][j].setFillColor(sf::Color::Cyan);
			this->Board[i][j].setOutlineColor(sf::Color::Black);
			this->Board[i][j].setOutlineThickness(1.f);
		}
	}
}
void TreasureHunt::initItems()
{
	this->items.totalItems = 30;
	this->items.itemArr = new sf::CircleShape[this->items.totalItems];
	this->items.ITEMS_ON_BOARD = this->items.totalItems;
	this->items.offset = new Offset[this->items.totalItems];
	
	for(int i=0; i< this->items.totalItems;i++){
		int randomX = rand() % 16; // Generating random locations on Board
		int randomY = rand() % 16;
		this->items.offset[i] = {this->offsetsBoard[randomX][randomY].X + 45.f, this->offsetsBoard[randomX][randomY].Y + 20.f};
	}
	
	float itemSize = 40.f;  // Radius

	for(int i=0; i<this->items.totalItems;i++){
		this->items.itemArr[i].setScale(sf::Vector2f(0.25f, 0.25f));
		this->items.itemArr[i].setRadius(itemSize);
		this->items.itemArr[i].setFillColor(sf::Color::Magenta);
		this->items.itemArr[i].setOutlineColor(sf::Color::Black);
		this->items.itemArr[i].setOutlineThickness(3.f);
		this->items.itemArr[i].setPosition(this->items.offset[i].X, this->items.offset[i].Y);
	}
}
void TreasureHunt::initFonts(){
	if(!this->font.loadFromFile("Paul-le1V.ttf")){
		cout << "Failed to load Font!"<<endl;
	}
}

void TreasureHunt::initTexts()
{
	this->text.setFont(this->font);
	this->text.setFillColor(sf::Color::Black);
	this->text.setCharacterSize(24);
	this->text.setStyle(sf::Text::Bold);
	this->text.setPosition(sf::Vector2f(950.f, 5.f));
	this->text.setString("NONE");

}

// CONSTRUCTOR & DESTRUCTOR
TreasureHunt::TreasureHunt()
{
	initAll();
	initBoard();
	initItems();
	initFonts();
	initTexts();
}
TreasureHunt::~TreasureHunt()
{
	delete this->window;
}

// GAME FUNCTIONS
void TreasureHunt::Render()
{
	if(!this->endGame){
		this->window->clear();  // clears current frame
		
		// BOARD
		for (int i = 0; i < this->Boardsize; i++) {
			for (int j = 0; j < this->Boardsize; j++) {
				this->window->draw(this->Board[i][j]);
			}
		}

		// READING MESSAGES FROM PLAYER'S QUEUES AND DELETING ITEMS
		if(!player1_MSG_Queue.empty()){
			Msg msg = player1_MSG_Queue.front();
			for(int i=0; i<this->items.totalItems;i++){
				if(i == msg.item){
					this->items.itemArr[i].setPosition(-20.f,-20.f);
					this->items.offset[i] = {-20.f,-20.f};
				}

			}
			this->items.ITEMS_ON_BOARD--;
			cout << "Items left: "<<this->items.ITEMS_ON_BOARD << endl;
			player1Pts++;
			cout << "P1 points: " << player1Pts<< endl;

			player1_MSG_Queue.pop();
		}
		else if(!player2_MSG_Queue.empty()){
			Msg msg = player2_MSG_Queue.front();
			for(int i=0; i<this->items.totalItems;i++){
				if(i == msg.item){
					this->items.itemArr[i].setPosition(-20.f,-20.f);
					this->items.offset[i] = {-20.f,-20.f};
				}
			}
			this->items.ITEMS_ON_BOARD--;
			cout << "Items left: "<<this->items.ITEMS_ON_BOARD << endl;
			player2Pts++;
			cout << "P2 points: " << player2Pts<< endl;

			player2_MSG_Queue.pop();
		}
		else if(!bot_MSG_Queue.empty()){
			Msg msg = bot_MSG_Queue.front();
			for(int i=0; i<this->items.totalItems;i++){
				if(i == msg.item){
					this->items.itemArr[i].setPosition(-20.f,-20.f);
					this->items.offset[i] = {-20.f,-20.f};
				}
			}
			this->items.ITEMS_ON_BOARD--;
			cout << "Items left: "<<this->items.ITEMS_ON_BOARD << endl;
			BotPts++;
			cout << "Bot points: " << BotPts<< endl;

			bot_MSG_Queue.pop();
		}
		else{
			for(int i=0; i<this->items.totalItems;i++){
				this->window->draw(this->items.itemArr[i]);
			}
		}

		

		// PLAYER 1
		this->window->draw(this->player1);

		// PLAYER 2
		this->window->draw(this->player2);

		// BOT
		this->window->draw(this->Bot);

		// TEXT
		this->RenderText(*this->window);

		this->window->display();  // draw again
	}
}
void TreasureHunt::Update()
{

	// Updating Poll Events
	this->PollEvents();

	// Collisions
	this->BotCollisions();
	this->P1Collisions();
	this->P2Collisions();

	// Setting Endgame Conditions
	if(this->items.ITEMS_ON_BOARD == 0)
		this->endGame = true;

	if(!this->endGame){
		// Update Bot
		this->UpdateBot();
		this->UpdateText();
	}
	// Update Mouse Position
	//cout << "Mouse Pos : (" << sf::Mouse::getPosition(*this->window).x << "," << sf::Mouse::getPosition(*this->window).y << ")" << endl;
}
void TreasureHunt::UpdateBot()
{
	if(this->spawnTimer >= this->spawnTimerMax)
	{
		this->spawnTimer = 0.f;
		this->spawnBot();
	}
	else
		this->spawnTimer+= 1.f;

	// Movement for the Bot
	if(BotPts < this->items.totalItems){
		float x, y;
		x = this->Bot.getPosition().x;
		y = this->Bot.getPosition().y;
		int signed_OR_notX = random() % 2;
		int signed_OR_notY = random() % 2;
		int movX = 1.f;
		int movY = 1.f;
		if( (this->Bot.getPosition().x + 2.f) <= 1800 &&
			(this->Bot.getPosition().y + 2.f) <= 1015 &&
			(this->Bot.getPosition().x - 2.f) >= 0 &&
			(this->Bot.getPosition().y - 2.f) >= 0)
		{
			if(signed_OR_notX == 0 && signed_OR_notY == 0){
				for(int i=0;i<40;i++)
					this->Bot.move(movX,movY);
			}
			else if(signed_OR_notX == 0 && signed_OR_notY == 1){
				for(int i=0;i<40;i++)
					this->Bot.move(movX,-movY);
			}
            else if (signed_OR_notX == 1 && signed_OR_notY == 0)
            {
                for(int i=0;i<40;i++)
					this->Bot.move(-movX,movY);
            }
            else{
				for(int i=0;i<40;i++)
					this->Bot.move(-movX,-movY);
			}
		}
		else if((this->Bot.getPosition().x + 2.f) > 1800)
			this->Bot.move(-movX,movY);
		else if((this->Bot.getPosition().y + 2.f) > 1015)
			this->Bot.move(movX,-movY);
		else if((this->Bot.getPosition().x - 2.f) < 0)
			this->Bot.move(movX,movY);
		else if((this->Bot.getPosition().y - 2.f) < 0)
			this->Bot.move(movX,movY);
	}
}
void TreasureHunt::spawnBot()
{
	this->Bot.setPosition(
		static_cast<float>(rand()% static_cast<int>(this->window->getSize().x - this->Bot.getRadius()- 70.f)),
		static_cast<float>(rand()% static_cast<int>(this->window->getSize().y - this->Bot.getRadius()- 5.f))
		);
}
void TreasureHunt::PollEvents()
{
	while (this->window->pollEvent(this->e)) {
		if (this->e.type == sf::Event::Closed)
			this->window->close();
		else if (this->e.type == sf::Event::KeyPressed) {
			if (this->e.key.code == sf::Keyboard::Escape)
				this->window->close();
			else if (this->e.key.code == sf::Keyboard::W)
				this->player1.move(0.f, -30.f);
			else if (this->e.key.code == sf::Keyboard::A)
				this->player1.move(-30.f, 0.f);
			else if (this->e.key.code == sf::Keyboard::S)
				this->player1.move(0.f, 30.f);
			else if (this->e.key.code == sf::Keyboard::D)
				this->player1.move(30.f, 0.f);
			else if (this->e.key.code == sf::Keyboard::Up)
				this->player2.move(0.f, -30.f);
			else if (this->e.key.code == sf::Keyboard::Down)
				this->player2.move(0.f, 30.f);
			else if (this->e.key.code == sf::Keyboard::Left)
				this->player2.move(-30.f, 0.f);
			else if (this->e.key.code == sf::Keyboard::Right)
				this->player2.move(30.f, 0.f);
		}
	}
}
int TreasureHunt::generateRandomNumber()
{
	int num = rand() % 89 + 10;   // generates between (10 - 99)
	cout << "The random number is: " << num << endl;
	num *= 4;			  // Roll no: 0474
	int num2 = 474 / num;
	int rem = num2 % 25;
	rem < 10 ? rem += 15 : rem;
	cout << "Board Size: (" << rem << " x " << rem << ")" << endl;
	return rem;
}
Offset** TreasureHunt::OffsetsGenerator(int size)
{
	Offset** Board = new Offset * [size];
	for (int i = 0; i < size; i++)
		Board[i] = new Offset[size];
	return Board;
}
void TreasureHunt::P1Collisions()
{
    sf::Vector2f pos = this->player1.getPosition();
	for(int i=0; i<this->items.totalItems; i++){
		if((pos.x >= this->items.offset[i].X - 50.f && pos.x <= this->items.offset[i].X + 50.f) && (pos.y >= this->items.offset[i].Y - 20.f && pos.y <= this->items.offset[i].Y + 20.f))
		{
			cout << "P1 hit item: " << i << endl;
			Msg msg;
			msg.offset.X = pos.x;
			msg.offset.Y = pos.y;
			msg.item = i;
			player1_MSG_Queue.push(msg);
		}
	}
}
void TreasureHunt::P2Collisions()
{
	sf::Vector2f pos = this->player2.getPosition();
	for(int i=0; i<this->items.totalItems; i++){
		if((pos.x >= this->items.offset[i].X - 50.f && pos.x <= this->items.offset[i].X + 50.f) && (pos.y >= this->items.offset[i].Y - 20.f && pos.y <= this->items.offset[i].Y + 20.f))
		{
			cout << "P2 hit item: " << i << endl;
			Msg msg;
			msg.offset.X = pos.x;
			msg.offset.Y = pos.y;
			msg.item = i;
			player2_MSG_Queue.push(msg);
		}
	}
}
void TreasureHunt::BotCollisions()
{
	sf::Vector2f pos = this->Bot.getPosition();
	for(int i=0; i<this->items.totalItems; i++){
		if((pos.x >= this->items.offset[i].X - 50.f && pos.x <= this->items.offset[i].X + 50.f) && (pos.y >= this->items.offset[i].Y - 20.f && pos.y <= this->items.offset[i].Y + 20.f))
		{
			cout << "Bot hit item: " << i << endl;
			Msg msg;
			msg.offset.X = pos.x;
			msg.offset.Y = pos.y;
			msg.item = i;
			bot_MSG_Queue.push(msg);
		}
	}
}
void TreasureHunt::RenderText(sf::RenderTarget& target)
{
	target.draw(this->text);
}

void TreasureHunt::UpdateText()
{
	stringstream ss;
	ss << "Player 1 Points:  " << player1Pts << "\nPlayer 2 Points: " << player2Pts << "\nBot Points:        " << BotPts << "\n"; 
	this->text.setString(ss.str());
}


// ACCESSOR FUNCTIONS
const bool TreasureHunt::isRunning() const
{
	return this->window->isOpen();
}
const bool TreasureHunt::endGameAccessor() const
{
    return this->endGame;
}
const sf::Event TreasureHunt::getEvent() const
{
    return this->e;
}
sf::RenderWindow *TreasureHunt::getWindow() const
{
    return this->window;
}


// MAIN FUNCTION
int main() {
	srand(time(0));

	// Initialize Game object
	TreasureHunt* game = new TreasureHunt;

	// Making three threads one for each player and bot
	pthread_t pid1, pid2, pid3;

	pthread_create(&pid1, NULL, initPlayer1, (void*)game);
	pthread_create(&pid2, NULL, initPlayer2, (void*)game);
	pthread_create(&pid3, NULL, initBot, (void*)game);

	
	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);
	pthread_join(pid3, NULL);

	// Game Loop
	while (game->isRunning() && !game->endGameAccessor()) {
		game->Update();  // checks within the main while loop for the event Poll
		game->Render();  // Render the sprites after reseting all
	}

	// RESULTS
	cout << "Player 1 Points: "<< player1Pts << endl;
	cout << "Player 2 Points: "<< player2Pts << endl;
	cout << "Bot Points: "<< BotPts <<endl;
	if(player1Pts > player2Pts && player1Pts > BotPts){
		cout << "Player 1 WON! Congratulations!!" << endl;
	}
	else if(player2Pts > player1Pts && player2Pts > BotPts){
		cout << "Player 2 WON! Congratulations!!" << endl;
	}
	else{
		cout << "Bot WON! Congratulations!!" << endl;
	}
	return 0;
}

