#include <iostream>
#ifdef _WIN32
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <SFML\Window.hpp>
#include <SFML\System.hpp>

const sf::String kAssetRoot = "./";

#else
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

<<<<<<< HEAD
const sf::String kAssetRoot = "../../SpaceShooter/";
=======
const sf::String kAssetRoot = "../SpaceShooter/";
>>>>>>> 59e84c1d8bb37b0fe727fc3c66018d70c88a0bfe
#endif

using namespace std;
using namespace sf;

Vector2u windowSize;

const float Drone_Vy = 0.1f;
const float Bullet_speed = 0.3f;

String resolvePath(const String& relative) {
	return kAssetRoot + relative;
}

const String kPlayerTexturePath = resolvePath("Images/Player/defaultPlayer.png");
const String kBulletTexturePath = resolvePath("Images/Player/defaultBullet.png");
const String kDroneTexturePath = resolvePath("Images/Enemy/defaultDrone.png");

// --- yaha tak. This is path resolving to make it compatible for both mac vs code and windows vs studio
// do lmk if this does not work for you, i have tried to suit to your expected project and binary root directory



class Entity {
	float xPos;
	float yPos;
	float V_x;
	float V_y;
	Texture Tex;
	Sprite sprite;
public:
	Entity() {}
	Entity(float xPos,float yPos,float V_x,float V_y):xPos(xPos),yPos(yPos),V_x(V_x),V_y(V_y) {
		
	}
	Entity(const Entity& other) {
		this->xPos = other.xPos;
		this->yPos = other.yPos;
		this->V_x = other.V_x;
		this->V_y = other.V_y;
		this->Tex = other.Tex;          // Copy the actual texture pixels
		this->sprite = other.sprite;    // Copy the sprite settings
		this->sprite.setTexture(this->Tex); // RE-LINK the sprite to the new texture
	}
	Entity& operator=(const Entity& other) {
		if (this != &other) { // Prevent self-assignment
			this->xPos = other.xPos;
			this->yPos = other.yPos;
			this->V_x = other.V_x;
			this->V_y = other.V_y;
			this->Tex = other.Tex;
			this->sprite = other.sprite;
			this->sprite.setTexture(this->Tex); // The most important line!
		}
		return *this;
	}
	bool loadTexture(const String& filename) {
		if (!Tex.loadFromFile(filename)) {
			cout << "Failed to load texture: " << filename.toAnsiString() << endl; //sf::String does not even have << ostream operator overloading. Skill issue?
			return false; 
		}
		sprite.setTexture(Tex);
		return true;
	}
	void setPosition() {
		sprite.setPosition(xPos, yPos);
	}
	void SetOrigin() {
		sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	}
	void SetScale(float x, float y) {
		sprite.setScale(x, y);
	}
	~Entity() {
	}
	float getVy() const{
		return V_y;
	}
	float getVx() const{
		return V_x;
	}
	Sprite& getSprite() {
		return sprite;
	}
	Texture& getTexture() {
		return Tex;
	}
	void draw(RenderWindow& window) {
		window.draw(sprite);
	}
};

class Bullet :public Entity {
	int damage;
	bool pierceFlag;
public:
	Bullet() {}

	Bullet(int d, bool pierce, float xPos, float yPos, float V_x, float V_y, const String& filename) :Entity(xPos, yPos, V_x, V_y) {

		damage = d;
		pierceFlag = pierce;
		this->loadTexture(filename);
	}
	void moveUp() {
		this->getSprite().move(0, -this->getVy());
	}
	void moveDown() {
		this->getSprite().move(0, this->getVy());
	}

};

class Player : public Entity {
	static int lives;
	Bullet** PlayerBullets;
	int PlayerBulletCount;
public:
	Player(float xPos, float yPos, float V_x, float V_y,const String& filename):Entity(xPos,yPos,V_x,V_y) {
		this->loadTexture(filename);
		PlayerBulletCount = 0;
		PlayerBullets = nullptr;
		this->SetOrigin();
		this->SetScale(.2f, .2f);
		this->setPosition();
	}
	~Player() {
		if (PlayerBullets) {
			for (int i = 0; i < PlayerBulletCount; i++) {
				delete PlayerBullets[i];
			}
			delete[] PlayerBullets;
		}
	}
	void moveRight() {
		this->getSprite().move(this->getVx(), 0);
	}
	void moveLeft() {
		this->getSprite().move(-this->getVx(), 0);
	}
	static void takeDamage() {
		lives--;
		if (lives <= 0) {
			cout << "Player died\n";
		}
	}
	void shoot(FloatRect& Mybounds) {
		Bullet** temp = new Bullet * [PlayerBulletCount + 1];
		for (int i = 0; i < PlayerBulletCount; i++) {
			temp[i] = PlayerBullets[i];
		}

		temp[PlayerBulletCount] = new Bullet(3, false, Mybounds.left + (Mybounds.width / 2.0f), Mybounds.top, 0.5f, .30f, kBulletTexturePath);
		temp[PlayerBulletCount]->SetScale(0.2f, 0.15f);
		temp[PlayerBulletCount]->SetOrigin();
		temp[PlayerBulletCount]->setPosition();
		delete[] PlayerBullets;
		PlayerBullets = temp;
		PlayerBulletCount++;
	
	}
	Bullet** getPlayerBullets() {
		return PlayerBullets;
	}
	void draw(RenderWindow& window) {
		Entity::draw(window);
		for (int i = 0; i < PlayerBulletCount; i++) {
			PlayerBullets[i]->draw(window);
		}
	}
	void update() {
		for (int i = 0; i < PlayerBulletCount; i++) {
			PlayerBullets[i]->moveUp();
		}
		for (int i = 0; i < PlayerBulletCount; i++) {
			if (PlayerBullets[i]->getSprite().getGlobalBounds().top <= 0) {
				delete PlayerBullets[i];
				Bullet** temp = nullptr;
				if (PlayerBulletCount > 1) {
					temp = new Bullet * [PlayerBulletCount - 1];
					for (int j = 0, k = 0; j < PlayerBulletCount; j++) {
						if (i == j)
							continue;
						temp[k++] = PlayerBullets[j];
					}
				}
				delete[] PlayerBullets;
				PlayerBullets = temp;
				PlayerBulletCount--;
				i--;
			}
		}
	}
	int getBulletCount() {
		return PlayerBulletCount;
	}
};

int Player::lives = 5;

class Enemy : public Entity {
	int health;
public:
	Enemy(float x, float y, float Vy, float Vx, int health, const String& filename) :Entity(x, y, Vx, Vy) {
		this->health = health;
		this->loadTexture(filename);
	}
	virtual void shoot(FloatRect Bounds) = 0;
	virtual void move() = 0;
	virtual void update() = 0;
	void destroy() {
		cout << "Destroyed\n";
	}
	virtual int getBulletCount() = 0;
	virtual Bullet** getBullets() = 0;

	virtual void draw(RenderWindow& window) = 0;
	virtual ~Enemy() {} //warning aa rahi thi to bana dia
};

class Drone : public Enemy {
	 Clock clock; 
	 Bullet** DroneBullets;
	 int DroneBulletsCount;
public:
	Drone(float x,float y,float Vy,float Vx,int health,const String &filename):Enemy(x,y,Vy,Vx,health,filename) {
		DroneBulletsCount = 0;
		DroneBullets = nullptr;
		this->SetOrigin();
		this->setPosition();
		this->SetScale(0.1f, 0.1f);
	}
	void move() override{
	     	this->getSprite().move(0,this->getVy());
	}
	void shoot(FloatRect Bounds) override {
		if (clock.getElapsedTime().asSeconds() >= 1.5) {
			Bullet** temp = new Bullet * [DroneBulletsCount + 1];
			for (int i = 0; i < DroneBulletsCount; i++) {
				temp[i] = DroneBullets[i];
			}
			temp[DroneBulletsCount] = new Bullet(3, false, Bounds.left + (Bounds.width / 2.0f),Bounds.top+Bounds.height, 0.5f, Bullet_speed, kBulletTexturePath);
			temp[DroneBulletsCount]->SetScale(0.2f, -0.15f);
			temp[DroneBulletsCount]->SetOrigin();
			temp[DroneBulletsCount]->setPosition();
		
			delete[] DroneBullets;
			DroneBullets = temp;
		
			DroneBulletsCount++;

			clock.restart();
		}
	}
	Bullet** getBullets() override {
		return DroneBullets;
	}
	void update() override {

		for (int i = 0; i < DroneBulletsCount; i++) {
			DroneBullets[i]->moveDown();
		}
		
		for (int i = 0; i < DroneBulletsCount; i++) {
			if (DroneBullets[i]->getSprite().getGlobalBounds().top >= windowSize.y) {
				delete DroneBullets[i];
				Bullet** temp = nullptr;
				if (DroneBulletsCount > 1) {
					temp = new Bullet * [DroneBulletsCount - 1];
					for (int j = 0, k = 0; j < DroneBulletsCount; j++) {
						if (i == j)
							continue;
						temp[k++] = DroneBullets[j];
					}
				}
				delete[] DroneBullets;
				DroneBullets = temp;
				DroneBulletsCount--;
				i--;
				cout << "Bullet deleted\n";
				Player::takeDamage();
			}
		}
		
	}
	void draw(RenderWindow & window) override {
		Entity::draw(window);
		for (int i = 0; i < DroneBulletsCount; i++) {
			DroneBullets[i]->draw(window);
		}
	}
	int getBulletCount() override {
		return DroneBulletsCount;
	}
	~Drone() {
		if (DroneBullets) {
			for (int i = 0; i < DroneBulletsCount; i++) {
				delete DroneBullets[i];
			}
			delete[] DroneBullets;
		}
	}

};

void collisionsManager(Player& Me, Enemy* drone) {

	FloatRect P = Me.getSprite().getGlobalBounds();
	FloatRect E = drone->getSprite().getGlobalBounds();
	Bullet** MyBullets_Boundary = Me.getPlayerBullets();
	Bullet** EnemyBullets_Boundary = drone->getBullets();
	for (int i = 0; i < drone->getBulletCount(); i++) {
		if (EnemyBullets_Boundary[i]->getSprite().getGlobalBounds().intersects(P)) {
			Player::takeDamage();
		}
	}
	for (int i = 0; i < Me.getBulletCount(); i++) {
		if (MyBullets_Boundary[i]->getSprite().getGlobalBounds().intersects(E)) {
			drone->destroy();
		}
	}
}

int main()
{
	RenderWindow window(VideoMode::getDesktopMode(), "SFML Works!", Style::Fullscreen);

	windowSize = window.getSize();

	Player Me(windowSize.x/2, windowSize.y/1.1, .5f, .5f, kPlayerTexturePath);

	Enemy** enemy = new Enemy * [1];
	enemy[0] = new Drone{ 100, 100, Drone_Vy, 0, 1,kDroneTexturePath};

	while (window.isOpen()) {
		FloatRect Mybounds = Me.getSprite().getGlobalBounds();
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				window.close();
			}
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
				Me.shoot(Mybounds);
			}
		}
		
		for (int i = 0; i < 1; i++) {
			enemy[i]->move();
			if (enemy[i]->getSprite().getGlobalBounds().top + enemy[i]->getSprite().getGlobalBounds().height >= windowSize.y) {
				cout << "Drone deleted\n";
				cout << "Player died\n";
			}
			enemy[i]->shoot(enemy[i]->getSprite().getGlobalBounds());
			enemy[i]->update();
		}

		if (Keyboard::isKeyPressed(Keyboard::D) && Mybounds.left + Mybounds.width < windowSize.x) {
				Me.moveRight();
		}
		if (Keyboard::isKeyPressed(Keyboard::A) && Mybounds.left > 0) {
			Me.moveLeft();
		}
		
		Me.update();

		for (int i = 0; i < 1; i++) {
			collisionsManager(Me, enemy[i]);
		}

		window.clear();
		Me.draw(window);
		for(int i = 0; i < 1; i++)
			enemy[i]->draw(window);
		window.display();
    }

	delete enemy[0];
	delete[] enemy;
	
	return 0;
}
