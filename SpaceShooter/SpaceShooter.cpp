#include <iostream>
#include <cmath>
#include <random>
#include <map>
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
#endif

	using namespace std;
using namespace sf;

Vector2u windowSize;

const float Drone_Vy = 0.08f;
const float cruiserSpeed = 0.3f;
const float Bullet_speed = .5f;
const float viper_Vy = 0.08f;
const float viper_Amp = 60.f;   // smaller sine wave amplitude
const float seeker_Speed = 0.7f;
const float viper_Freq = 0.1f;
const float drone_Scale = 0.06f;
const float viper_Scale = 0.03f;
const float cruiser_Scale = 0.12f;
const float seeker_Scale = 0.12f;
const float player_Scale = 0.12f;
const float twinCannon_Scale = 0.2f;
const float MotherShip_Scale = 0.6f;

const double pi = 3.14159;
const int STAR_COUNT = 150;

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
const String kLeftTurretTexturePath = resolvePath("Images/Enemy/LTurret.png");
const String kRightTurretTexturePath = resolvePath("Images/Enemy/RTurret.png");
const String kTwinCannonTexturePath = resolvePath("Images/Enemy/TwinCannon.png");
const String kMotherShipTexturePath = resolvePath("Images/Enemy/MotherShip.png");
const String kheartDropTexturePath = resolvePath("Images/Drops/heartDrop.png");
Texture heartTex, nukeTex, pierceTex, spreadTex;

std::map<String, Texture> textureCache;

IntRect ExplosionFrames(0, 0, 182, 182);
const int explosionFrames = 7;
class Entity {
protected:
	float xPos;
	float yPos;
	float V_x;
	float V_y;
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
		this->sprite = other.sprite;
	}
	Entity& operator=(const Entity& other) {
		if (this != &other) {
			this->xPos = other.xPos;
			this->yPos = other.yPos;
			this->V_x = other.V_x;
			this->V_y = other.V_y;
			this->sprite = other.sprite;
		}
		return *this;
	}
	bool loadTexture(const String& filename, IntRect frame = IntRect()) {
		if (textureCache.find(filename) == textureCache.end()) {
			if (!textureCache[filename].loadFromFile(filename)) {
				std::cout << "Failed to load: " << filename.toAnsiString() << std::endl;
				return false;
			}
		}
		bool resetRect = (frame == sf::IntRect());
		sprite.setTexture(textureCache[filename], resetRect);

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
	void draw(RenderWindow& window) {
		window.draw(sprite);
	}
};

class Bullet :public Entity {
protected:
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
	bool getNormalcy() {
		return pierceFlag;
	}
};

class ShootableCharacter : public Entity {
protected:
	int  health;
	Clock  clock;
	Bullet** bullets;
	int bulletCount;
	int bulletCapacity;

public:
	ShootableCharacter()
		: Entity(), bullets(nullptr), bulletCount(0), bulletCapacity(0) {
		health = 0;
	}

	ShootableCharacter(float xPos, float yPos, float V_x, float V_y,int health): Entity(xPos, yPos, V_x, V_y), bullets(nullptr), bulletCount(0), bulletCapacity(0) {
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
		if (bulletCount >= bulletCapacity) {
			bulletCapacity = (bulletCapacity == 0) ? 10 : bulletCapacity * 2;
			Bullet** temp = new Bullet * [bulletCapacity];
			for (int i = 0; i < bulletCount; i++) {
				temp[i] = bullets[i];
			}
			delete[] bullets;
			bullets = temp;
		}
		bullets[bulletCount] = b;
		bulletCount++;
	}

	void deleteBullet(int i) {
		delete bullets[i];
		for (int j = i; j < bulletCount - 1; j++) {
			bullets[j] = bullets[j + 1];
		}
		bulletCount--;
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
				i--;
			}
		}
	}
};

class Enemy : public ShootableCharacter {
protected:
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
				i--;
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
				i--;
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
	inline void move() override {
		this->getSprite().move(getVx(), getVy());
	}
	inline void shoot(FloatRect Bounds)override{}
	inline void update() override{}
	void draw(RenderWindow& window) override {
		Entity::draw(window);
	}
	
	~Seeker() {
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
						Laser[k].setFillColor(Color(255,0,0,30));
						Laser[k].setSize(Vector2f(112, windowSize.y));
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
						Bullet* laser = new Bullet(3, false, 112*i, Bounds.top + Bounds.height,0.5f, 2*Bullet_speed, kCruiserLaserTexturePath);
						laser->SetScale(0.3f, -0.3f);
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
				i--;
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

class Turret: public Enemy{
	double theta;

public:
	Turret(float x, float y, float Vy, float Vx, int health, const String& filename): Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(twinCannon_Scale, twinCannon_Scale);
		theta = 0;
	}
	inline void move() override {
	}
	void Homing(FloatRect PlayerBounds) {
		theta = atan2((double)(PlayerBounds.top - getyPos()),(double)(PlayerBounds.left - getxPos()));
	}
	void shoot(FloatRect Bounds) override {
		if (clock.getElapsedTime().asSeconds() >= 1.5f) {
			Bullet* b = new Bullet(3, false, Bounds.left + Bounds.width, Bounds.top + Bounds.height, 0.5f, Bullet_speed, kCruiserLaserTexturePath);//right side bullet
			b->getSprite().setRotation(theta * ((double)180 / pi) - 90);
			b->SetScale(0.2f, -0.15f);
			b->SetOrigin();
			b->setPosition();
			addBullet(b);
			clock.restart();
		}
	}
	void update() override {
		float dx = cos(theta) * Bullet_speed;  
		float dy = sin(theta) * Bullet_speed;
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->getSprite().move(dx, dy);
			bullets[i]->getSprite().setRotation(theta * ((double)180 / pi) - 90);
		}
		for (int i = 0; i < bulletCount; i++) {
			if (bullets[i]->getSprite().getGlobalBounds().top >= windowSize.y) {
				this->deleteBullet(i);
				i--;
			}
		}
	}
	void draw(RenderWindow& window) override {
		Entity::draw(window);
		drawBullets(window);
	}
	~Turret() {
		for (int r = this->getBulletCount() - 1; r >= 0; r--) {
			this->deleteBullet(r);
		}
	}
};

class TwinCannon :public Enemy{
	Turret** turret;
	int turretCount;
	double theta;
public:
	TwinCannon(float x, float y, float Vy, float Vx, int health, const String& filename) : Enemy(x, y, Vy, Vx, health, filename) {
		this->setPosition();
		this->SetScale(twinCannon_Scale, twinCannon_Scale);
		turretCount = 2;
		turret = new Turret * [turretCount];
		turret[0] = new Turret{this->getSprite().getGlobalBounds().left - this->getSprite().getGlobalBounds().width, this->getSprite().getGlobalBounds().top , 0, 0, 5, kLeftTurretTexturePath};
		turret[1] = new Turret{this->getSprite().getGlobalBounds().left + this->getSprite().getGlobalBounds().width, this->getSprite().getGlobalBounds().top, 0, 0, 5, kRightTurretTexturePath};
		this->SetOrigin();
		theta = 0;
	}
	inline void move() override {}
	bool allTurretsActive() {

		for (int i = 0; i < turretCount; i++)
			if (!turret[i]->getStatus()) return true;
		return false;
	}
	void Homing(FloatRect PlayerBounds) {
		if (allTurretsActive()) {
			for (int i = 0; i < turretCount; i++) {
				turret[i]->Homing(PlayerBounds);
			}
		}
		else {
			theta = atan2((double)(PlayerBounds.top - getyPos()), (double)(PlayerBounds.left - getxPos()));
		}
	}
	void shoot(FloatRect Bounds) override {

		if (!allTurretsActive()) {
			if (clock.getElapsedTime().asSeconds() >= 2.5f) {

				for (int i = 0; i < 4; i++) {
					
					Bullet* b = new Bullet(3, false, Bounds.left -Bounds.width/2 +i*(Bounds.width/2), Bounds.top + Bounds.height, Bullet_speed, Bullet_speed, kCruiserLaserTexturePath);//right side bullet
					b->SetScale(0.2f, -0.15f);
					b->SetOrigin();
					b->setPosition();
					addBullet(b);
				}
				clock.restart();
			}
		}
		else {
			for (int i = 0; i < turretCount; i++) {
				turret[i]->shoot(turret[i]->getSprite().getGlobalBounds());
			}
		}
	}
	void update() override {
		
		if (!allTurretsActive()) {
			float dx = cos(theta) * Bullet_speed;
			float dy = sin(theta) * Bullet_speed;
			for (int i = 0; i < bulletCount; i++) {
				bullets[i]->getSprite().move(dx, dy);
				bullets[i]->getSprite().setRotation(theta * ((double)180 / pi) - 90);
			}
			for (int i = 0; i < bulletCount; i++) {
				if (bullets[i]->getSprite().getGlobalBounds().top >= windowSize.y || bullets[i]->getSprite().getGlobalBounds().left < 0 ||
					bullets[i]->getSprite().getGlobalBounds().left >= windowSize.x) {
					this->deleteBullet(i);
					i--;
				}
			}
		}
		else {	
			for (int i = 0; i < turretCount; i++) {
				turret[i]->update();
			}
		}
	}

	Turret** getTurret() {
		return turret;
	}
	int getTurretCount() {
		return turretCount;
	}
	void reduceTurret() {
		turretCount--;
	}
	void draw(RenderWindow& window) override {
		for (int i = 0; i < turretCount; i++) {
			turret[i]->draw(window);
		}
		Entity::draw(window);
		drawBullets(window);
	}
};

class MotherShip:public Enemy {
	double theta;
	Clock laserClock;
	Clock minionClock;
	Seeker** Minions;
	int seekerCount;
	int seekerCapacity;
public:
	MotherShip(float x, float y, float Vy, float Vx, int health, const String& filename):Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(MotherShip_Scale, -MotherShip_Scale);
		Minions = nullptr;
		seekerCount = 0;
		seekerCapacity = 0;
		theta = 0;
	}
	void move() override {
		
	}
	inline void Homing(FloatRect PlayerBounds) {
		theta = atan2((double)(PlayerBounds.top - getyPos()), (double)(PlayerBounds.left - getxPos()));
		
	}
	void spawnSeekers(FloatRect PlayerBounds) {
		if (minionClock.getElapsedTime().asSeconds() >= 2.5f) {
			if (seekerCount >= seekerCapacity) {
				seekerCapacity = (seekerCapacity == 0) ? 10 : seekerCapacity * 2;
				Seeker** temp = new Seeker * [seekerCapacity];
				for (int i = 0; i < seekerCount; i++) {
					temp[i] = Minions[i];
				}
				delete[] Minions;
				Minions = temp;
			}
			Minions[seekerCount] = new Seeker(PlayerBounds.left, PlayerBounds.top, rand() % windowSize.x, 100, seeker_Speed, seeker_Speed, 1, kSeekerTexturePath);
			seekerCount++;
			minionClock.restart();
		}
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
        if (laserClock.getElapsedTime().asSeconds() >= 10.f) {
	
			for (int i = 0; i < 5; i++) {
					Bullet* laser = new Bullet(3, true, windowSize.x * i / 5, Bounds.top + Bounds.height, 0.5f, 2 * Bullet_speed, kCruiserLaserTexturePath);
					laser->SetScale(0.3f, -0.3f);
					laser->SetOrigin();
					laser->setPosition();
					addBullet(laser);
			}
			laserClock.restart();
		}
		
	}

	void update() override {
		float dx = cos(theta) * Bullet_speed;
		float dy = sin(theta) * Bullet_speed;
		for (int i = 0; i < bulletCount; i++) {
			if (!bullets[i]->getNormalcy()) {
				bullets[i]->getSprite().move(dx, dy);
				bullets[i]->getSprite().setRotation(theta * ((double)180 / pi) - 90);
			}
			else {
				bullets[i]->getSprite().move(0, Bullet_speed);
			}
		}
		for (int i = bulletCount - 1; i >= 0; i--) {
			if (bullets[i]->getSprite().getGlobalBounds().top >= windowSize.y)
				this->deleteBullet(i);
		}
	}

	void draw(RenderWindow& window) override {
		for (int i = 0; i < seekerCount; i++) {
			Minions[i]->draw(window);
		}
		Entity::draw(window);
		drawBullets(window);
	}
	Seeker** getSeeker() {
		return Minions;
	}
	int& getSeekerCount() {
		return seekerCount;
	}
	void destroySeekers(int i) {
		delete Minions[i];
		for (int j = i; j < seekerCount - 1; j++) {
			Minions[j] = Minions[j + 1];
		}
		seekerCount--;
		cout << "Enemy deleted\n";
	}
	~MotherShip() {

		for (int i = 0; i < seekerCount; i++) {
			delete Minions[i];
		}
		delete[] Minions;
	    for (int r = this->getBulletCount() - 1; r >= 0; r--) {
				this->deleteBullet(r);
		}
	}
};

//void makeDrops(FloatRect enemyBound, int& dropCount, Drops**& drop) {
//	static random_device rd;
//	static mt19937 gen(rd());
//	static discrete_distribution<> d({ 3,10,15,20 });
//	String texturePath = "";
//	int outcome = d(gen);
//	switch (outcome) {
//	case 0:
//		cout << "Nuke dropped ========================\n";
//		break;
//	case 1:
//		cout << "Pierce dropped ========================\n";
//		break;
//
//	case 2:
//		cout << "Heart dropped ========================\n";
//		texturePath = kheartDropTexturePath;
//		break;
//
//	case 3:
//		cout << "Spread dropped ========================\n";
//		break;
//	}
//	if (texturePath=="") return;
//
//	Drops** temp = new Drops * [dropCount + 1];
//	for (int i = 0; i < dropCount; i++) {
//		temp[i] = drop[i];
//	}
//	heartTex.loadFromFile(texturePath);
//	temp[dropCount] = new Drops(enemyBound.left, enemyBound.top, 0, Bullet_speed,heartTex);
//	delete[] drop;
//	drop = temp;
//	dropCount++;
//}

class Star {
	CircleShape shape;
	float speed;
public:
	Star() {
		float radius = (rand() % 3 + 1) * 0.5f; 
		speed = radius * 0.3f;                     

		float brightness = 100 + rand() % 156;    
		Uint8 b = static_cast<Uint8>(brightness);

		shape.setRadius(radius);
		shape.setFillColor(Color(b, b, b, b));   
		shape.setPosition(rand() % windowSize.x, rand() % windowSize.y);
	}
	void move() {
		shape.move(0, speed);
		if (shape.getPosition().y > windowSize.y)
			shape.setPosition(rand() % windowSize.x, 0);
	}
	void draw(RenderWindow& window) {
		window.draw(shape);
	}
};



template<typename T>
void deleteEnemy(int i,T** &enemies,int &enemyCount) {
	delete enemies[i];
	for (int j = i; j < enemyCount - 1; j++) {
		enemies[j] = enemies[j + 1];
	}
	enemyCount--;
	cout << "Enemy deleted\n";
}
void collisionsManager(Player& Me, Enemy**& enemies, int& enemyCount, int &dropCount ) {

	FloatRect playerBounds = Me.getSprite().getGlobalBounds();

	for (int k = 0; k < enemyCount; k++) {

		if (!enemies[k]) continue;

		if (enemies[k]->getStatus()) {
			deleteEnemy(k, enemies, enemyCount);
			k--;
			continue;
		}

		// -------------------------------------------------------
		// PHASE 1: Enemy bullets vs Player
		// -------------------------------------------------------

		if (TwinCannon* tc = dynamic_cast<TwinCannon*>(enemies[k])) {
			if (tc->allTurretsActive()) {
				Turret** turrets = tc->getTurret();
				for (int t = 0; t < tc->getTurretCount(); t++) {
					for (int i = turrets[t]->getBulletCount() - 1; i >= 0; i--) {
						if (turrets[t]->getBullets()[i]->getSprite().getGlobalBounds().intersects(playerBounds)) {
							turrets[t]->deleteBullet(i);
							Me.takeDamage();
							playerBounds = Me.getSprite().getGlobalBounds();
							cout << "Player hit by turret bullet\n";
						}
					}
				}
			}
			else {
				for (int i = tc->getBulletCount() - 1; i >= 0; i--) {
					if (tc->getBullets()[i]->getSprite().getGlobalBounds().intersects(playerBounds)) {
						tc->deleteBullet(i);
						Me.takeDamage();
						playerBounds = Me.getSprite().getGlobalBounds();
						cout << "Player hit by TwinCannon bullet\n";
					}
				}
			}
		}
		else {
			for (int i = enemies[k]->getBulletCount() - 1; i >= 0; i--) {
				if (enemies[k]->getBullets()[i]->getSprite().getGlobalBounds().intersects(playerBounds)) {
					enemies[k]->deleteBullet(i);
					Me.takeDamage();
					playerBounds = Me.getSprite().getGlobalBounds();
					cout << "Player hit\n";
				}
			}
		}

		// -------------------------------------------------------
		// PHASE 2: Player bullets vs Enemy
		// -------------------------------------------------------

		for (int i = Me.getBulletCount() - 1; i >= 0; i--) {

			FloatRect bulletBounds = Me.getPlayerBullets()[i]->getSprite().getGlobalBounds();
			bool bulletDeleted = false;

			if (TwinCannon* tc = dynamic_cast<TwinCannon*>(enemies[k])) {
				if (tc->allTurretsActive()) {
					Turret** turrets = tc->getTurret();
					for (int t = 0; t < tc->getTurretCount(); t++) {
						if (!bulletBounds.intersects(turrets[t]->getSprite().getGlobalBounds())) continue;

						Me.deleteBullet(i);
						bulletDeleted = true;
						turrets[t]->takeDamage();
						turrets[t]->setStatus();

						if (turrets[t]->getStatus()) {
							for (int r = turrets[t]->getBulletCount() - 1; r >= 0; r--)
								turrets[t]->deleteBullet(r);

							delete turrets[t];

							for (int s = t; s < tc->getTurretCount() - 1; s++)
								turrets[s] = turrets[s + 1];
							turrets[tc->getTurretCount() - 1] = nullptr;

							tc->reduceTurret();
							cout << "Turret destroyed\n";
						}
						break;
					}
				}
				else {
					if (bulletBounds.intersects(tc->getSprite().getGlobalBounds())) {
						Me.deleteBullet(i);
						bulletDeleted = true;
						tc->takeDamage();
						tc->setStatus();
						if (tc->getStatus()) {
							for (int r = tc->getBulletCount() - 1; r >= 0; r--)
								tc->deleteBullet(r);
							cout << "TwinCannon destroyed\n";
						}
					}
				}
			}
			else if (MotherShip* M = dynamic_cast<MotherShip*>(enemies[k])) {
				Seeker** seekers = M->getSeeker();
				FloatRect dropPosition;
				for (int s = 0; s < M->getSeekerCount(); s++) {
					if (bulletBounds.intersects(seekers[s]->getSprite().getGlobalBounds())) {
						Me.deleteBullet(i);
						bulletDeleted = true;
						seekers[s]->takeDamage();
						seekers[s]->setStatus();
						dropPosition = seekers[s]->getSprite().getGlobalBounds();
						M->destroySeekers(s);
						s--;
						break;
					}
				}
				if (!bulletDeleted && bulletBounds.intersects(enemies[k]->getSprite().getGlobalBounds())) {
					Me.deleteBullet(i);
					bulletDeleted = true;
					enemies[k]->takeDamage();
					enemies[k]->setStatus();
				}
			}
			else {
				if (bulletBounds.intersects(enemies[k]->getSprite().getGlobalBounds())) {
					Me.deleteBullet(i);
					bulletDeleted = true;
					enemies[k]->takeDamage();
					enemies[k]->setStatus();
				}
			}
		}

		// -------------------------------------------------------
		// PHASE 3: Enemy body vs Player (ram kill)
		// -------------------------------------------------------

		if (MotherShip* M = dynamic_cast<MotherShip*>(enemies[k])) {
			Seeker** seekers = M->getSeeker();
			for (int i = 0; i < M->getSeekerCount(); i++) {
				if (seekers[i]->getSprite().getGlobalBounds().intersects(playerBounds)) {
					seekers[i]->destroy();
					seekers[i]->setStatus();
					Me.destroy();
					cout << "Player Died\n";
				}
			}
		}
		else {
			if (enemies[k]->getSprite().getGlobalBounds().intersects(playerBounds)) {
				enemies[k]->destroy();
				enemies[k]->setStatus();
				for (int r = enemies[k]->getBulletCount() - 1; r >= 0; r--)
					enemies[k]->deleteBullet(r);
				Me.destroy();
				cout << "Player Died\n";
			}
		}

		// -------------------------------------------------------
		// PHASE 4 & 5: Update, move, shoot, boundary check
		// -------------------------------------------------------

		if (TwinCannon* tc = dynamic_cast<TwinCannon*>(enemies[k]))
			tc->Homing(playerBounds);
		else if (MotherShip* M = dynamic_cast<MotherShip*>(enemies[k])) {
			M->Homing(playerBounds);
			M->spawnSeekers(playerBounds);
		}

		if (MotherShip* M = dynamic_cast<MotherShip*>(enemies[k])) {
			Seeker** seekers = M->getSeeker();
			for (int i = 0; i < M->getSeekerCount(); i++) {
				seekers[i]->move();
				seekers[i]->update();
			}
		}

		enemies[k]->move();
		enemies[k]->shoot(enemies[k]->getSprite().getGlobalBounds());
		enemies[k]->update();

		// -------------------------------------------------------
		// PHASE 4: Enemy reached bottom
		// -------------------------------------------------------

		if (MotherShip* M = dynamic_cast<MotherShip*>(enemies[k])) {
			Seeker** seekers = M->getSeeker();
			for (int i = 0; i < M->getSeekerCount(); i++) {
				FloatRect b = seekers[i]->getSprite().getGlobalBounds();
				if (b.top + b.height >= windowSize.y)
					M->destroySeekers(i--);
			}
		}
		else {
			FloatRect b = enemies[k]->getSprite().getGlobalBounds();
			if (b.top + b.height >= windowSize.y) {
				enemies[k]->destroy();
				deleteEnemy<Enemy>(k, enemies, enemyCount);
				k--;
				continue;
			}
		}
	}
}

int main()
{
	textureCache[kPlayerTexturePath].loadFromFile(kPlayerTexturePath);
	textureCache[kBulletTexturePath].loadFromFile(kBulletTexturePath);
	textureCache[kDroneTexturePath].loadFromFile(kDroneTexturePath);
	textureCache[kViperTexturePath].loadFromFile(kViperTexturePath);
	textureCache[kSeekerTexturePath].loadFromFile(kSeekerTexturePath);
	textureCache[ExpolsionTexturePath].loadFromFile(ExpolsionTexturePath);
	textureCache[kCruiserTexturePath].loadFromFile(kCruiserTexturePath);
	textureCache[kCruiserLaserTexturePath].loadFromFile(kCruiserLaserTexturePath);
	textureCache[kLeftTurretTexturePath].loadFromFile(kLeftTurretTexturePath);
	textureCache[kRightTurretTexturePath].loadFromFile(kRightTurretTexturePath);
	textureCache[kTwinCannonTexturePath].loadFromFile(kTwinCannonTexturePath);
	textureCache[kMotherShipTexturePath].loadFromFile(kMotherShipTexturePath);

	RenderWindow window(VideoMode(560, 854), "Space Shooter", Style::Close);
	windowSize = window.getSize();
	Player Me(5, windowSize.x / 2, windowSize.y/2 , .4f, .4f, kPlayerTexturePath);
	Enemy** enemy = new Enemy * [1];
	Star stars[STAR_COUNT];   
	int enemyCount = 1;
	int dropCount = 0;
	//enemy[0] = new Seeker(Me.getSprite().getGlobalBounds().left , Me.getSprite().getGlobalBounds().top , 100, 100, seeker_Speed, seeker_Speed, 1, kSeekerTexturePath);
	//enemy[0] = new Viper{ 300, 0, viper_Vy,0, 1, kViperTexturePath }; // Viper
	//enemy[0] = new Drone{ 100, 100, Drone_Vy, 0, 1, kDroneTexturePath }; // Drone
	//enemy[0] = new Cruiser{ 100, 50, 0, cruiserSpeed, 20, kCruiserTexturePath };
	//enemy[0] = new TwinCannon(300, 100, 0, 0, 20, kTwinCannonTexturePath);
	enemy[0] = new MotherShip(300, 100, 0, 0, 20, kMotherShipTexturePath);
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
		// Draw stars FIRST so everything renders on top
		
		Me.update();
		collisionsManager(Me, enemy,enemyCount, dropCount);
		window.clear();
		for (int i = 0; i < STAR_COUNT; i++) {
			stars[i].move();
			stars[i].draw(window);
		}
		Me.draw(window);
		for (int i = 0; i < enemyCount; i++)
			enemy[i]->draw(window);
		
		window.display();
	}

	return 0;
}

