#include <iostream>
#include <cmath>
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

const sf::String kAssetRoot = "../SpaceShooter/";
<<<<<<< HEAD

	====== =
	>>>>>> > 332d38f7f512fa49d3c583cc4d2ed08430606841
#endif

	using namespace std;
using namespace sf;

Vector2u windowSize;

const float Drone_Vy = 0.08f;
const float cruiserSpeed = 0.3f;
const float Bullet_speed = 0.4f;
const float viper_Vy = 0.08f;
const float viper_Amp = 60.f;   // smaller sine wave amplitude
const float seeker_Speed = 0.2f;
const float viper_Freq = 0.1f;
const float drone_Scale = 0.06f;
const float viper_Scale = 0.03f;
const float cruiser_Scale = 0.12f;
const float seeker_Scale = 0.12f;
const float player_Scale = 0.12f;
const double pi = 3.14159;


String resolvePath(const String& relative) {
	return kAssetRoot + relative;
}

const String kPlayerTexturePath = resolvePath("Images/Player/defaultPlayer.png");
const String kBulletTexturePath = resolvePath("Images/Player/defaultBullet.png");
const String kDroneTexturePath = resolvePath("Images/Enemy/defaultDrone.png");
const String kViperTexturePath = resolvePath("Images/Enemy/defaultViper.png");
const String kSeekerTexturePath = resolvePath("Images/Enemy/defaultSeeker.png");
const String ExpolsionTexturePath = resolvePath("Images/defaultExplosion.png");
const String kCruiserTexturePath = resolvePath("Images/Enemy/defaultCruiser.png");
const String kCruiserLaserTexturePath = resolvePath("Images/Enemy/cruiserLaser.png");


IntRect ExplosionFrames(0, 0, 182, 182);
const int explosionFrames = 7;


class Entity {
	float xPos;
	float yPos;
	float V_x;
	float V_y;
	Texture Tex;
	Sprite sprite;
public:
	Entity() {
	}
	Entity(float xPos, float yPos, float V_x, float V_y) :xPos(xPos), yPos(yPos), V_x(V_x), V_y(V_y) {
	}
	Entity(const Entity& other) {
		this->xPos = other.xPos;
		this->yPos = other.yPos;
		this->V_x = other.V_x;
		this->V_y = other.V_y;
		this->Tex = other.Tex;
		this->sprite = other.sprite;
		this->sprite.setTexture(this->Tex);
	}
	Entity& operator=(const Entity& other) {
		if (this != &other) {
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
	bool loadTexture(const String& filename, IntRect frame = IntRect()) {
		if (!Tex.loadFromFile(filename)) {
			std::cout << "Failed to load: " << filename.toAnsiString() << std::endl;
			return false;
		}
		bool resetRect = (frame == sf::IntRect());
		sprite.setTexture(Tex, resetRect);

		if (!resetRect) {
			sprite.setTextureRect(frame);
		}

		return true;
	}
	void setX(float x) {
		xPos = x;
	}
	void setY(float y) {
		yPos = y;
	}
	void setVx(float v) {
		V_x = v;
	}
	void setVy(float v) {
		V_y = v;
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
	float getVy() const {
		return V_y;
	}
	float getVx() const {
		return V_x;
	}
	float getxPos()const {
		return xPos;
	}
	float getyPos()const {
		return yPos;
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

class ShootableCharacter : public Entity {
protected:
	int  health;
	Clock  clock;
	Bullet** bullets;
	int bulletCount;

public:
	ShootableCharacter()
		: Entity(), bullets(nullptr), bulletCount(0) {
	}

	ShootableCharacter(float xPos, float yPos, float V_x, float V_y,int health): Entity(xPos, yPos, V_x, V_y), bullets(nullptr), bulletCount(0) {
		this->health = health;
	}

	virtual ~ShootableCharacter() {
		if (bullets) {
			for (int i = 0; i < bulletCount; i++) {
				delete bullets[i];
			}
			delete[] bullets;
		}
	}
	Bullet** getBullets() {
		return bullets;
	}

	int getBulletCount() const {
		return bulletCount;
	}

	void addBullet(Bullet* b) {
		Bullet** temp = new Bullet * [bulletCount + 1];
		for (int i = 0; i < bulletCount; i++) {
			temp[i] = bullets[i];
		}
		temp[bulletCount] = b;
		delete[] bullets;
		bullets = temp;
		bulletCount++;
		cout << "Bullet " << bulletCount << " added\n";
	}

	void deleteBullet(int i) {
		delete bullets[i];
		Bullet** temp = nullptr;
		if (bulletCount > 1) {
			temp = new Bullet * [bulletCount - 1];
			for (int j = 0, k = 0; j < bulletCount; j++) {
				if (i == j)
					continue;
				temp[k++] = bullets[j];
			}
		}
		delete[] bullets;
		bullets = temp;
		bulletCount--;
		cout << "Bullet deleted\n";
	}

	void drawBullets(RenderWindow& window) {
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->draw(window);
		}
	}

	void destroy() {
		this->loadTexture(ExpolsionTexturePath, ExplosionFrames);
		SetScale(0.7f, 0.7f);
		SetOrigin();
	}
	void takeDamage() {
		health--;
		if (health == 0) {
			this->destroy();
		}
	}
	
};

class Player : public ShootableCharacter {

public:
	Player(int health,float xPos, float yPos, float V_x, float V_y, const String& filename)
		: ShootableCharacter(xPos, yPos, V_x, V_y,health) {
		this->loadTexture(filename);
		this->SetOrigin();
		this->SetScale(player_Scale, player_Scale);
		this->setPosition();
	}
	void moveRight() {
		this->getSprite().move(this->getVx(), 0);
	}
	void moveLeft() {
		this->getSprite().move(-this->getVx(), 0);
	}
	void moveUp() {
		this->getSprite().move(0, -this->getVy());
	}
	void moveDown() {
		this->getSprite().move(0, this->getVy());
	}
	void shoot(FloatRect& Mybounds) {
		Bullet* b = new Bullet(3, false, Mybounds.left + (Mybounds.width / 2.0f), Mybounds.top, 0.5f, .30f, kBulletTexturePath);
		b->SetScale(0.2f, 0.15f);
		b->SetOrigin();
		b->setPosition();
		addBullet(b);
	}

	Bullet** getPlayerBullets() {
		return getBullets();
	}

	void draw(RenderWindow& window) {
		Entity::draw(window);
		drawBullets(window);
	}

	void update() {
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->moveUp();
		}
		for (int i = 0; i < bulletCount; i++) {
			if (bullets[i]->getSprite().getGlobalBounds().top <= 0) {
				this->deleteBullet(i);
			}
		}
	}
};

class Enemy : public ShootableCharacter {
	
	bool isDead;
public:
	Enemy(float x, float y, float Vy, float Vx, int health, const String& filename)
		: ShootableCharacter(x, y, Vx, Vy,health) {
		isDead = 0;
		this->loadTexture(filename);
	}

	virtual void shoot(FloatRect Bounds) = 0;
	virtual void move() = 0;
	virtual void update() = 0;
	virtual void draw(RenderWindow& window) = 0;

	bool checkStatus() {
		return isDead;
	}
	void setStatus() {
		if(health == 0)
		   isDead = 1;
	}
	bool getStatus() {
		return isDead;
	}

	virtual ~Enemy() {}
};

class Drone : public Enemy {
public:
	Drone(float x, float y, float Vy, float Vx, int health, const String& filename)
		: Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(drone_Scale, drone_Scale);
	}

	void move() override {
		this->getSprite().move(0, this->getVy());
	}

	void shoot(FloatRect Bounds) override {
		if (clock.getElapsedTime().asSeconds() >= 1.5f) {
			Bullet* b = new Bullet(3, false, Bounds.left + (Bounds.width / 2.0f), Bounds.top + Bounds.height, 0.5f, Bullet_speed, kBulletTexturePath);
			b->SetScale(0.2f, -0.15f);
			b->SetOrigin();
			b->setPosition();
			addBullet(b);
			clock.restart();
		}
	}

	void update() override {
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->moveDown();
		}
		for (int i = 0; i < bulletCount; i++) {
			if (bullets[i]->getSprite().getGlobalBounds().top >= windowSize.y) {
				this->deleteBullet(i);
			}
		}
	}

	void draw(RenderWindow& window) override {
		Entity::draw(window);
		drawBullets(window);
	}

};

class Viper : public Enemy {
	float currentY; 
	float time;
public:
	Viper(float x, float y, float Vy, float Vx, int health, const String& filename): Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(viper_Scale, -viper_Scale);
		currentY = y;
		time = 0;
	}
	void move() override {
		time += 0.05f;
		float x = getxPos() + viper_Amp * sin(time * viper_Freq);
		currentY += getVy();
		this->getSprite().setPosition(x, currentY);
	}
	void shoot(FloatRect Bounds) override {
		if (clock.getElapsedTime().asSeconds() >= 1.5f) {
			Bullet* b = new Bullet(3, false, Bounds.left + (Bounds.width / 2.0f), Bounds.top + Bounds.height, 0.5f, Bullet_speed, kBulletTexturePath);
			b->SetScale(0.2f, -0.15f);
			b->SetOrigin();
			b->setPosition();
			addBullet(b);
			clock.restart();
		}
	}
	void update() override {
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->moveDown();
		}
		for (int i = 0; i < bulletCount; i++) {
			if (bullets[i]->getSprite().getGlobalBounds().top >= windowSize.y) {
				this->deleteBullet(i);
			}
		}
	}
	void draw(RenderWindow& window) override {
		Entity::draw(window);
		drawBullets(window);
	}

};

class Seeker :public Enemy {
	float lockedX;
   double theta;
public:

	Seeker(float lockedX,float lockedY,float x, float y, float Vy, float Vx, int health, const String& filename) : Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(seeker_Scale, seeker_Scale);
		this->lockedX = lockedX;
		theta = atan2((double)(lockedY-y), (double)(lockedX - x));
		setVx(seeker_Speed * cos(theta));
		setVy(seeker_Speed * sin(theta));
		this->getSprite().setRotation(theta*((double)180 / pi)-90);
	}
	void move() override {
		this->getSprite().move(getVx(), getVy());
	}
	inline void shoot(FloatRect Bounds)override{
	 
	
	}
	inline void update() override{}
	void draw(RenderWindow& window) override {
		Entity::draw(window);
	}
};

class Cruiser:public Enemy {
	Clock laserClock;
	int laserState ;
	int gap;
	RectangleShape *Laser;
public:
	Cruiser(float x, float y, float Vy, float Vx, int health, const String& filename) : Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(cruiser_Scale, -cruiser_Scale);
		laserState = 0;
		gap = 0;
		Laser = nullptr;
	}
	void move() override {
		if (this->getSprite().getGlobalBounds().left <= 0) {
			setVx(fabs(getVx()));
		}
		else if (this->getSprite().getGlobalBounds().left + this->getSprite().getGlobalBounds().width >= windowSize.x) {
			setVx(-getVx());
		}
		this->getSprite().move(getVx(), 0);
	}

	void shoot(FloatRect Bounds) override {
		if (clock.getElapsedTime().asSeconds() >= 1.5f) {
			Bullet* bleft = new Bullet(3, false, Bounds.left, Bounds.top + Bounds.height, 0.5f, Bullet_speed, kCruiserLaserTexturePath);//left side bullet
			Bullet* bright = new Bullet(3, false, Bounds.left + Bounds.width, Bounds.top + Bounds.height, 0.5f, Bullet_speed, kCruiserLaserTexturePath);//right side bullet
			bleft->SetScale(0.2f, -0.15f);
			bleft->SetOrigin();
			bleft->setPosition();
			bright->SetScale(0.2f, -0.15f);
			bright->SetOrigin();
			bright->setPosition();
			addBullet(bright);
			addBullet(bleft);
			clock.restart();
		}
		if (laserState == 0) {
		
			if (laserClock.getElapsedTime().asSeconds() >= 10.f) {
				gap = rand() % 5;
				Laser = new RectangleShape[4];
				
				for (int i = 0,k=0; i < 5; i++) {
					if (i != gap) {
						Laser[k].setFillColor(Color::Color(255,0,0,30));
						Laser[k].setSize(Vector2f(30, windowSize.y));
						Laser[k].setPosition(i * 112, Bounds.top + Bounds.height);
						k++;
					}
				}
				laserState = 1;        
				laserClock.restart();   
			}
		}
		else if (laserState == 1) {
			
			if (laserClock.getElapsedTime().asSeconds() >= 1.5f) {
				
				for (int i = 0; i < 5; i++) {
					if (i != gap) {
						Bullet* laser = new Bullet(3, false, 112.f * i, Bounds.top + Bounds.height,0.5f, 2*Bullet_speed, kCruiserLaserTexturePath);
						laser->SetScale(0.3f, -0.3f);
						laser->SetOrigin();
						laser->setPosition();
						addBullet(laser);
					}
				}
				laserState = 0;         
				laserClock.restart();   
				delete[] Laser;
				Laser = nullptr;
			}
		}
	}
	void update() override {
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->moveDown();
		}
		for (int i = 0; i < bulletCount; i++) {
			if (bullets[i]->getSprite().getGlobalBounds().top >= windowSize.y) {
				this->deleteBullet(i);
			}
		}
		
	}
	void draw(RenderWindow& window) override {
		Entity::draw(window);
		drawBullets(window);
		if (laserState == 1 && laserClock.getElapsedTime().asSeconds() <= 1.5) {
			for (int i = 0,k=0; i < 5; i++) {
				if (i != gap) {
					window.draw(Laser[k]);
					k++;
				}
			}
		}
	}

};

void collisionsManager(Player& Me, Enemy**& enemies, int enemyCount) {

	FloatRect playerBounds = Me.getSprite().getGlobalBounds();

	for (int k = 0; k < enemyCount; k++) {

		if (!enemies[k]) continue;

		FloatRect enemyBounds = enemies[k]->getSprite().getGlobalBounds();

	
		for (int i = enemies[k]->getBulletCount() - 1; i >= 0; i--) {
			FloatRect bulletBounds = enemies[k]->getBullets()[i]->getSprite().getGlobalBounds();
			if (bulletBounds.intersects(playerBounds)) {
				enemies[k]->deleteBullet(i);        
				enemies[k]->getBullets();            
				Me.takeDamage();
				cout << "Player hit\n";
				playerBounds = Me.getSprite().getGlobalBounds(); 
			}
		}

		// Player vs Bullet
		for (int i = Me.getBulletCount() - 1; i >= 0; i--) {
			FloatRect bulletBounds = Me.getPlayerBullets()[i]->getSprite().getGlobalBounds();
			if (bulletBounds.intersects(enemyBounds)) {
				Me.deleteBullet(i);                 
				enemies[k]->takeDamage();
				enemies[k]->setStatus();
				if (enemies[k]->getStatus()) {
					for (int r = enemies[k]->getBulletCount() - 1; r >= 0; r--) {
						enemies[k]->deleteBullet(r);
					}
					break;
				}
				
			}
		}

		//Enemy body vs player body
		if (!enemies[k]->checkStatus() && enemyBounds.intersects(playerBounds)) {
		
			enemies[k]->destroy();
			enemies[k]->setStatus();

			for (int r = enemies[k]->getBulletCount() - 1; r >= 0; r--) {
				enemies[k]->deleteBullet(r);
			}

			Me.destroy();
			//instantKill();               
			cout << "Player Died\n";
		}

		
		if (!enemies[k]->checkStatus()) {
			float enemyBottom = enemies[k]->getSprite().getGlobalBounds().top
				+ enemies[k]->getSprite().getGlobalBounds().height;
			if (enemyBottom >= windowSize.y) {
				enemies[k]->destroy();
				enemies[k]->setStatus();
				cout << "Enemy reached bottom\n";
			}
		}

		if (!enemies[k]->checkStatus()) {
			enemies[k]->move();
			enemies[k]->shoot(enemies[k]->getSprite().getGlobalBounds());
			enemies[k]->update();
		}
	}
}

int main()
{
	RenderWindow window(VideoMode(560, 854), "Space Shooter", Style::Close);
	windowSize = window.getSize();
	Player Me(5, windowSize.x / 2, windowSize.y/2 , .4f, .4f, kPlayerTexturePath);
	Enemy** enemy = new Enemy * [1];

	//enemy[0] = new Seeker(Me.getSprite().getGlobalBounds().left , Me.getSprite().getGlobalBounds().top , 100, 100, seeker_Speed, seeker_Speed, 1, kSeekerTexturePath);
	//enemy[0] = new Viper{ 300, 0, viper_Vy,0, 1, kViperTexturePath }; // Viper
	//enemy[0] = new Drone{ 100, 100, Drone_Vy, 0, 1, kDroneTexturePath }; // Drone

	enemy[0] = new Cruiser{ 100, 50, 0, cruiserSpeed, 10, kCruiserTexturePath };

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

		if (Keyboard::isKeyPressed(Keyboard::D) && Mybounds.left + Mybounds.width < windowSize.x) {
			Me.moveRight();
		}
		if (Keyboard::isKeyPressed(Keyboard::A) && Mybounds.left > 0) {
			Me.moveLeft();
		}
		if (Keyboard::isKeyPressed(Keyboard::W) && Mybounds.top > 0) {
			Me.moveUp();
		}
		if (Keyboard::isKeyPressed(Keyboard::S) && Mybounds.top < windowSize.y) {
			Me.moveDown();
		}

		Me.update();
		collisionsManager(Me, enemy,1);
		window.clear();
		Me.draw(window);
		for (int i = 0; i < 1; i++)
			enemy[i]->draw(window);
		window.display();
	}

	delete enemy[0];
	delete[] enemy;

	return 0;
}

