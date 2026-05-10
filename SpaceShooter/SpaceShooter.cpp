#include <iostream>
#include <cmath>
#include <random>
#include <map>
#include <fstream>
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
const float Player_bullet_speed = 0.8f;
const float viper_Vy = 0.08f;
const float viper_Amp = 60.f;   // smaller sine wave amplitude
const float seeker_Speed = 0.7f;
const float viper_Freq = 0.1f;
const float drone_Scale = 0.04f;
const float viper_Scale = 0.03f;
const float cruiser_Scale = 0.12f;
const float seeker_Scale = 0.12f;
const float player_Scale = 0.12f;
const float twinCannon_Scale = 0.2f;
const float MotherShip_Scale = 0.6f;

// powerup
const int puSpread = 0; // different vars just for readability
const int puPierce = 1;
const int puShield = 2;
const int puEmp = 3;
const int weaponStandard = 0;
const int weaponSpread = 1;
const int weaponPierce = 2;

const int BASE_DRONES = 3;
const int BASE_SEEKERS = 2;
const int BASE_VIPERS = 1;

const int SEEKERS_START = 4;   // wave at which seekers begin spawning
const int VIPERS_START = 7;
int wave = 0;

const int SCORE_DRONE = 10;
const int SCORE_VIPER = 15;
const int SCORE_SEEKER = 20;
const int SCORE_BOSS = 50;
const String kHighScorePath = "highscores.txt";

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
const String kSpreadPlayerPath = resolvePath("Images/Player/spreadPlayer.png");
const String kPiercePlayerPath = resolvePath("Images/Player/piercePlayer.png");
const String kPierceBulletPath = resolvePath("Images/Player/pierceBullet.png");
const String kEmpBulletPath = resolvePath("Images/Player/empBullet.png");
const String kHeartTexturePath = resolvePath("Images/HUD/heart.png");
const String kShieldTexturePath = resolvePath("Images/HUD/shield.png");
const String kEmpTexturePath = resolvePath("Images/HUD/emp.png");
const String kAsteroid1Path = resolvePath("Images/Asteroids/ast1.png");
const String kAsteroid2Path = resolvePath("Images/Asteroids/ast2.png");
const String kAsteroid3Path = resolvePath("Images/Asteroids/ast3.png");

const float ASTEROID_SPEED = 0.08f;
const int ASTEROID_MIN_COUNT = 3;
const int ASTEROID_MAX_COUNT = 5;

const String kPlayerShieldTexturePath = resolvePath("Images/Player/Shield.png");
//replace this with your font path 
const String FontPath = resolvePath("/assets/fonts/Orbitron Medium.ttf");
std::map<String, Texture> textureCache;


float RandomValue(float n) {
	return static_cast<float>(rand()) / RAND_MAX * n;
}
float RandomValue(float min, float max) {
	return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

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
	const Sprite& getSprite() const {
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
	bool isDead;
	int  health;
	int  maxHealth;
	Clock  clock;
	bool isExploding;
	Clock explosionClock;
	Bullet** bullets;
	int bulletCount;
	int bulletCapacity;
	int currentFrame;
public:
	ShootableCharacter()
		: Entity(), bullets(nullptr), bulletCount(0), bulletCapacity(0) {
		health = 0;
		maxHealth = 0;
		isExploding = 0;
		currentFrame = 0;
	}

	ShootableCharacter(float xPos, float yPos, float V_x, float V_y, int health) : Entity(xPos, yPos, V_x, V_y), bullets(nullptr), bulletCount(0), bulletCapacity(0) {
		this->health = health;
		this->maxHealth = health;
	}
	bool isExpolode() {
		return isExploding;
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

	void updateExplosion() {
		if (!isExploding) return;
		if (explosionClock.getElapsedTime().asMilliseconds() >= 80) { 
			currentFrame++;
			if (currentFrame >= explosionFrames) {
				isExploding = false;
				setStatus(); //dead delete
				return;
			}
			IntRect frame(currentFrame * 182, 0, 182, 182);
			sprite.setTextureRect(frame);
			explosionClock.restart();
		}
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
		isExploding = true;
		currentFrame = 0;
		this->loadTexture(ExpolsionTexturePath, ExplosionFrames); // frame 0
		SetScale(0.5f, 0.5f);
		SetOrigin();
		explosionClock.restart();
	}

	int getHealth() const {
		return health;
	}
	virtual void takeDamage() {
		if (isExploding) return;
		health--;
		if (health <= 0) {
			destroy();
		}
	}
	int getMaxHealth() const {
		return maxHealth;
	}
	void setStatus() {
			isDead = 1;
	}
	bool getStatus() {
		return isDead;
	}
	void instantKill() {
		health = 0;
	}
	
};

class Player : public ShootableCharacter {

public:
	// weapon edit
	int currentWeapon; // three types, standard, spreading, piercing
	int shieldHits; // capped to 2 even if one more shield is acquired
	Sprite shield;
	int empCount; // 3 max
	bool pierce;
	int pierceCount;
	bool empShot;
	float dt;
	Sprite EMP;
	int score;
	int scoreMultiplier;
	Clock lastKillTime;

// dash system
	Clock dashCooldownClock;
	Clock invincibleClock;
	bool isInvincible;
	float dashDistance;

	// dash animation
	Sprite* dashTrail;
	int dashTrailCount;
	Clock dashTrailClock;

	Player(int health,float xPos, float yPos, float V_x, float V_y, const String& filename)
		: ShootableCharacter(xPos, yPos, V_x, V_y,health), currentWeapon(weaponStandard), shieldHits(0), empCount(1), score(0), scoreMultiplier(1), isInvincible(false), dashDistance(150.f), dashTrailCount(0) {
		this->loadTexture(filename);
		this->SetOrigin();
		this->SetScale(player_Scale, player_Scale);
		this->setPosition();
		shield.setTexture(textureCache[kPlayerShieldTexturePath]);
		shield.setOrigin(shield.getLocalBounds().width / 2.f, shield.getLocalBounds().height / 2.f);
		shield.setScale(0.05, 0.05);
		isDead = 0;
		lastKillTime.restart();
		dashCooldownClock.restart();
		invincibleClock.restart();
		EMP.setTexture(textureCache[kEmpBulletPath]);
		EMP.setOrigin(EMP.getLocalBounds().width / 2, EMP.getLocalBounds().height / 2);
		EMP.setScale(0.05, 0.05);
		dashTrail = new Sprite [3];
		pierce = 0;
		pierceCount = 0;
		empShot = 0;
		dt = 0.01;
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
		if (currentWeapon == weaponSpread) { //triple bullets!!! 
			float cx = Mybounds.left + Mybounds.width / 2.0f;
			Bullet* b1 = new Bullet(3, false, cx - 15, Mybounds.top, -.09f, Player_bullet_speed, kBulletTexturePath); // left diagonal
			Bullet* b2 = new Bullet(3, false, cx, Mybounds.top, 0.0f, Player_bullet_speed, kBulletTexturePath); // straight
			Bullet* b3 = new Bullet(3, false, cx + 15, Mybounds.top, .09f, Player_bullet_speed, kBulletTexturePath); // right diagonal
			b1->SetScale(0.2f, 0.15f); b1->SetOrigin(); b1->setPosition(); addBullet(b1);
			b2->SetScale(0.2f, 0.15f); b2->SetOrigin(); b2->setPosition(); addBullet(b2);
			b3->SetScale(0.2f, 0.15f); b3->SetOrigin(); b3->setPosition(); addBullet(b3);
			pierce = false;
		}
		else if (currentWeapon == weaponPierce) {
			Bullet* b = new Bullet(6, true, Mybounds.left + (Mybounds.width / 2.0f), Mybounds.top, 0, Player_bullet_speed, kPierceBulletPath);
			b->SetScale(0.03f, 0.03f); b->SetOrigin(); b->setPosition(); addBullet(b);
			pierce = true;
		}
		else {
			Bullet* b = new Bullet(3, false, Mybounds.left + (Mybounds.width / 2.0f), Mybounds.top, 0, Player_bullet_speed, kBulletTexturePath);
			b->SetScale(0.2f, 0.15f); b->SetOrigin(); b->setPosition(); addBullet(b);
			pierce = false;
		}
	}
	bool getPierceFlag() {
		return pierce;
	}
	//roblox damage sound puhleeeeeez?!? ok
	void takeDamage() override {
		if (isInvincible) return;
		if (shieldHits > 0) {
			shieldHits--;
			return;
		}
		resetMultiplier();
		ShootableCharacter::takeDamage();
	}

	void changeSprite(const String& path, float scale) {
		loadTexture(path);
		SetScale(scale, scale);
		SetOrigin();
	}

	int getHealth() const { 
		return health; 
	}

	void empShoot(FloatRect& Mybounds) {
		if (empCount > 0) {
			empCount--; 
			dt = 0.06f;     
		}
	}
	
	void applyPowerUp(int t) {
		switch (t) {
			case puSpread: 
				currentWeapon = weaponSpread; 
				changeSprite(kSpreadPlayerPath, 0.03f);
				break;
			case puPierce: 
				currentWeapon = weaponPierce; 
				changeSprite(kPiercePlayerPath, 0.15f);
				break;
			case puShield: 
				shieldHits = 2; 
				break;
			case puEmp: 
				if (empCount < 3) empCount++;
				break;
		}
	}

	Bullet** getPlayerBullets() {
		return getBullets();
	}

	int getScore() const { 
		return score; 
	}
	int getScoreMultiplier() const { 
		return scoreMultiplier; 
	}

	void addScore(int basePoints) {
		float timeSinceLastKill = lastKillTime.getElapsedTime().asSeconds();
		if (timeSinceLastKill <= 3.0f) {
			if (scoreMultiplier == 1) scoreMultiplier = 2;
			else if (scoreMultiplier == 2) scoreMultiplier = 4;
		}
		else {
			scoreMultiplier = 1;
		}
		score += basePoints * scoreMultiplier;
		lastKillTime.restart();
	}

	void resetMultiplier() {
		scoreMultiplier = 1;
	}

	bool canDash() const {
		return dashCooldownClock.getElapsedTime().asSeconds() >= 3.0f;
	}

	float getDashCooldown() const {
		float elapsed = dashCooldownClock.getElapsedTime().asSeconds();
		return (elapsed >= 3.0f) ? 0.0f : 3.0f - elapsed;
	}

	bool getIsInvincible() const {
		return isInvincible;
	}

	void dash(float dx, float dy) {
		if (canDash()) {
			FloatRect bounds = getSprite().getGlobalBounds();
			Vector2f startPos = getSprite().getPosition();

			float endX = startPos.x + dx * dashDistance;
			float endY = startPos.y + dy * dashDistance;

			endX = max(bounds.width / 2.f, min(endX, windowSize.x - bounds.width / 2.f));
			endY = max(bounds.height / 2.f, min(endY, windowSize.y - bounds.height / 2.f));

			dashTrail[0] = getSprite();
			dashTrail[1] = getSprite();
			dashTrail[1].setPosition(startPos.x + (endX - startPos.x) * 0.2f, startPos.y + (endY - startPos.y) * 0.2f);
			dashTrail[2] = getSprite();
			dashTrail[2].setPosition(startPos.x + (endX - startPos.x) * 0.6f, startPos.y + (endY - startPos.y) * 0.6f);

			dashTrailCount = 3;
			dashTrailClock.restart();

			getSprite().setPosition(endX, endY);
			dashCooldownClock.restart();
			invincibleClock.restart();
			isInvincible = true;
		}
	}

	void updateDash() {
		if (dashTrailCount > 0 && dashTrailClock.getElapsedTime().asSeconds() >= 0.2f) {
			dashTrailCount = 0;
		}
		if (isInvincible && invincibleClock.getElapsedTime().asSeconds() >= 0.5f) {
			isInvincible = false;
		}
	}

	void draw(RenderWindow& window) {
		for (int i = 0; i < dashTrailCount; i++) {
			window.draw(dashTrail[i]);
		}
		if (shieldHits > 0) {
			window.draw(shield);
		}
		if (empShot) {
			window.draw(EMP);
		}
		Entity::draw(window);
		drawBullets(window);
	}
	void update() {
		updateExplosion();
		shield.setPosition(sprite.getPosition().x, sprite.getPosition().y);
		EMP.setPosition(sprite.getPosition().x, sprite.getPosition().y);

		if (empShot) {
			dt += 0.001f;
			EMP.setScale(dt, dt);
			if (dt > 1) {
				empShot = false;
				dt = 0.01f;
			}
		}
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->getSprite().move(bullets[i]->getVx(), -bullets[i]->getVy());
		}
		for (int i = 0; i < bulletCount; i++) {
			if (empShot) break; // don't delete EMP bullet during animation
			if (bullets[i]->getSprite().getGlobalBounds().top <= 0) {
				this->deleteBullet(i);
				i--;
			}
		}
	}

	~Player() {
		delete[] dashTrail;
	}
};

class Enemy : public ShootableCharacter {
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
	virtual int getScoreValue() const { 
		return SCORE_DRONE;
	}
	virtual ~Enemy() {}
	float getHealth() {
		return health;
	}

};
class Drone : public Enemy {
public:
	Drone(float x, float y, float Vy, float Vx, int health, const String& filename)
		: Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(drone_Scale, drone_Scale);
	}
	int getScoreValue() const override { 
		return SCORE_DRONE; 
	}

	void move() override {
		this->getSprite().move(0, this->getVy());
	}

	void shoot(FloatRect Bounds) override {
		if (clock.getElapsedTime().asSeconds() >= 1.5f && this->getSprite().getGlobalBounds().top >= 0) {
			Bullet* b = new Bullet(3, false, Bounds.left + (Bounds.width / 2.0f), Bounds.top + Bounds.height, 0.5f, Bullet_speed, kBulletTexturePath);
			b->SetScale(0.1f, -0.1f);
			b->SetOrigin();
			b->setPosition();
			addBullet(b);
			clock.restart();
		}
	}

	void update() override {
		updateExplosion();
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
	int getScoreValue() const override { 
		return SCORE_VIPER; 
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
			b->SetScale(0.1f, -0.1f);
			b->SetOrigin();
			b->setPosition();
			addBullet(b);
			clock.restart();
		}
	}
	void update() override {
		updateExplosion();
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
	int getScoreValue() const override {
		return SCORE_SEEKER; 
	}

	inline void move() override {
		this->getSprite().move(getVx(), getVy());
	}
	inline void shoot(FloatRect Bounds)override{}
	inline void update() override{
		updateExplosion();
	}
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
	int getScoreValue() const override { 
		return SCORE_BOSS; 
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
						Bullet* laser = new Bullet(3, false, 112*i, Bounds.top + Bounds.height,0.5f, 4*Bullet_speed, kCruiserLaserTexturePath);
						laser->SetScale(0.2f, -0.2f);
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
		updateExplosion();
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
	int getScoreValue() const override { 
		return SCORE_BOSS; 
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
		updateExplosion();
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
	int getScoreValue() const override { 
		return SCORE_BOSS; 
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

				for (int i = 0; i < 3; i++) {
					
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
		updateExplosion();
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
	int getScoreValue() const override { 
		return SCORE_BOSS; 
	}

	void move() override {
		
	}
	inline void Homing(FloatRect PlayerBounds) {
		theta = atan2((double)(PlayerBounds.top - getyPos()), (double)(PlayerBounds.left - getxPos()));
		
	}
	void spawnSeekers(FloatRect PlayerBounds) {
		if (minionClock.getElapsedTime().asSeconds() >= 1.5f) {
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
			for (int i = 0; i < 3; i++) {
				Bullet* b = new Bullet(3, false, Bounds.left - Bounds.width / 2 + i * (Bounds.width / 2)+20, Bounds.top + Bounds.height, Bullet_speed, Bullet_speed, kCruiserLaserTexturePath);//right side bullet
				b->SetScale(0.2f, -0.15f);
				b->SetOrigin();
				b->setPosition();
				addBullet(b);
			}
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
		updateExplosion();
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

class PowerUp : public Entity {
	int powerType;
public:
	PowerUp(float x, float y, int t) : Entity(x, y, 0, 0.15f), powerType(t) {
		String path;
		float s = 0.02f;
		if (t == puSpread) { 
			path = resolvePath("Images/Drops/spread.png"); 
			
		} else if (t == puPierce) { 
			path = resolvePath("Images/Drops/piercing.png"); 
		
		} else if (t == puShield) { 
			path = resolvePath("Images/Drops/shield.png"); 
			
		} else { 
			path = resolvePath("Images/Drops/emp.png"); 
		}
		loadTexture(path);
		SetScale(s, s);
		SetOrigin();
		setPosition();
	}
	int getType() const {
		return powerType;
	}
	void moveDown() {
		getSprite().move(0, getVy());
	}
	void draw(RenderWindow& window) {
		Entity::draw(window);
	}
	FloatRect getBounds() const {
		return getSprite().getGlobalBounds();
	}
};

class Asteroid : public Entity {
public:
	Asteroid(float x, float y, int sizeType) : Entity(x, y, 0, ASTEROID_SPEED) {
		String path;
		float scale;
		float mul = 1.2f; //how many times bigger than player
		if (sizeType == 0) {
			path = kAsteroid1Path;
			scale = 0.1f * mul;
		} else if (sizeType == 1) {
			path = kAsteroid2Path;
			scale = 0.025f * mul;
		} else {
			path = kAsteroid3Path;
			scale = 0.1f * mul;
		}
		loadTexture(path);
		SetScale(scale, scale);
		SetOrigin();
		setPosition();
	}
	void moveDown() {
		sprite.move(0, getVy());
		sprite.rotate(.1f);
	}
	void draw(RenderWindow& window) {
		Entity::draw(window);
	}
	FloatRect getBounds() const {
		return getSprite().getGlobalBounds();
	}
};

// what did you comment up there bro, what discombombulating abomination is that function ;-;
void spawnPowerup(float x, float y, PowerUp**& powerups, int& powerupCount) {
	bool isEmp = false;
	if (rand() % 100 < 5) { //could be more efficient...
		//essentially what this does is that if random percentage is more than 5%
		//it spawns an EMP
		PowerUp** temp = new PowerUp * [powerupCount + 1];
		for (int i = 0; i < powerupCount; i++) 
			temp[i] = powerups[i];
		temp[powerupCount] = new PowerUp(x, y, puEmp);
		delete[] powerups;
		powerups = temp;
		powerupCount++;
		isEmp = true;
	}
	if (rand() % 100 < 15 && !isEmp) { //spawns something other than emp
		int pick = rand() % 3;
		int t;
		if (pick == 0) 
			t = puSpread;
		else if (pick == 1)
			t = puPierce;
		else 
			t = puShield;

		PowerUp** temp = new PowerUp * [powerupCount + 1];
		for (int i = 0; i < powerupCount; i++) 
			temp[i] = powerups[i];
		temp[powerupCount] = new PowerUp(x, y, t);
		delete[] powerups;
		powerups = temp;
		powerupCount++;
	} //need to make this more efficient, this recreation again and again is terrible.Dynamic arrays suck
}

void spawnAsteroids(Asteroid**& asteroids, int& asteroidCount) {
	if (asteroidCount != 0) {
		for (int i = 0; i < asteroidCount; i++) {
			delete asteroids[i];
		}
		delete[] asteroids;
	}

	asteroidCount = 3;
	asteroids = new Asteroid * [asteroidCount];

	float segmentWidth = windowSize.x / 3.f;
	for (int i = 0; i < asteroidCount; i++) {
		float x = segmentWidth * (i + 0.5f);
		float y = -100.f - (i * 400.f);
		int sizeType = rand() % 3;
		asteroids[i] = new Asteroid(x, y, sizeType);
	}
}

void updateAsteroids(Asteroid**& asteroids, int& asteroidCount) {
	for (int i = 0; i < asteroidCount; i++) {
		asteroids[i]->moveDown();
		FloatRect bounds = asteroids[i]->getBounds();
		if (bounds.top > windowSize.y) {
			delete asteroids[i];
			float segmentWidth = windowSize.x / asteroidCount;
			float x = segmentWidth * (i + 0.5f);
			float y = RandomValue(-800.f, -100.f);
			int sizeType = rand() % 3;
			asteroids[i] = new Asteroid(x, y, sizeType);
		}
	}
}

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

class FPS //idk what this is, copied from stackoverflow
{
public:
   FPS() : mFrame(0), mFps(0) {}


   const unsigned int getFPS() const { return mFps; }
private:
   unsigned int mFrame;
   unsigned int mFps;
   sf::Clock mClock;

public:
   void update()
   {
       if (mClock.getElapsedTime().asSeconds() >= 1.f)
       {
           mFps = mFrame;
           mFrame = 0;
           mClock.restart();
       }

       ++mFrame;
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
void collisionsManager(Player& Me, Enemy**& enemies, int& enemyCount, PowerUp**& powerups, int& powerupCount, Asteroid** asteroids, int asteroidCount) {

	FloatRect playerBounds = Me.getSprite().getGlobalBounds();

	for (int k = 0; k < enemyCount; k++) {

		if (!enemies[k]) continue;

		if (enemies[k]->getStatus()) {
			Vector2f pos = enemies[k]->getSprite().getPosition(); //try spawning powerup
			spawnPowerup(pos.x, pos.y, powerups, powerupCount);
			Me.addScore(enemies[k]->getScoreValue());
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

						if (turrets[t]->getStatus()) {
							for (int r = turrets[t]->getBulletCount() - 1; r >= 0; r--)
								turrets[t]->deleteBullet(r);

							Me.addScore(turrets[t]->getScoreValue());
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
						if (!Me.getPierceFlag()) {
							Me.deleteBullet(i);
							bulletDeleted = true;
						}
						seekers[s]->takeDamage();
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
				}
			}
			else {
				if (bulletBounds.intersects(enemies[k]->getSprite().getGlobalBounds()) && !enemies[k]->isExpolode()) {
					enemies[k]->takeDamage();
					if (!Me.getPierceFlag()) {
						Me.deleteBullet(i);
						bulletDeleted = true;
					}
					else {
						Me.pierceCount++;
						if (Me.pierceCount >= 2) {
							Me.deleteBullet(i);
							bulletDeleted = true;
							Me.pierceCount = 0;
						}
					}
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
					seekers[i]->takeDamage();
					Me.instantKill();
					Me.takeDamage();
				}
			}
		}
		else {
			if (enemies[k]->getSprite().getGlobalBounds().intersects(playerBounds) && !enemies[k]->isExpolode()) {
				enemies[k]->takeDamage();
				for (int r = enemies[k]->getBulletCount() - 1; r >= 0; r--)
					enemies[k]->deleteBullet(r);
				Me.instantKill();
				Me.takeDamage();
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
				Vector2f pos = enemies[k]->getSprite().getPosition();
				spawnPowerup(pos.x, pos.y, powerups, powerupCount); //should it spawn now? ye
				Me.addScore(enemies[k]->getScoreValue());
				enemies[k]->destroy();
				deleteEnemy<Enemy>(k, enemies, enemyCount);
				k--;
				continue;
			}
		}
	}

	
	for (int i = 0; i < powerupCount; i++) {
		powerups[i]->moveDown(); //powerup move down :0
	}
	for (int i = 0; i < powerupCount; i++) { //power up get collect
		if (powerups[i]->getBounds().intersects(Me.getSprite().getGlobalBounds())) {
			Me.applyPowerUp(powerups[i]->getType());
			delete powerups[i];
			for (int j = i; j < powerupCount - 1; j++) // can use swap and pop, dk if that changes order
				powerups[j] = powerups[j + 1];
			powerupCount--;
			i--;
		}
		else if (powerups[i]->getBounds().top >= windowSize.y) {
			delete powerups[i];
			for (int j = i; j < powerupCount - 1; j++)
				powerups[j] = powerups[j + 1];
			powerupCount--;
			i--;
		}
	}

	for (int a = 0; a < asteroidCount; a++) {
		FloatRect asteroidBounds = asteroids[a]->getBounds();

		for (int i = Me.getBulletCount() - 1; i >= 0; i--) {
			FloatRect bulletBounds = Me.getPlayerBullets()[i]->getSprite().getGlobalBounds();
			if (bulletBounds.intersects(asteroidBounds)) {
				if (!Me.getPierceFlag()) {
					Me.deleteBullet(i);
					break;
				}
			}
		}

		for (int k = 0; k < enemyCount; k++) {
			if (!enemies[k]) continue;
			for (int i = enemies[k]->getBulletCount() - 1; i >= 0; i--) {
				FloatRect bulletBounds = enemies[k]->getBullets()[i]->getSprite().getGlobalBounds();
				if (bulletBounds.intersects(asteroidBounds)) {
					enemies[k]->deleteBullet(i);
				}
			}
		}

		if (playerBounds.intersects(asteroidBounds)) {
			Me.takeDamage();
			playerBounds = Me.getSprite().getGlobalBounds();
		}
	}
}
Enemy* getActiveBoss(int enemyCount,Enemy** enemies) {
	for (int i = 0; i < enemyCount; i++) {
		if (dynamic_cast<Cruiser*>(enemies[i]) ||
			dynamic_cast<TwinCannon*>(enemies[i]) ||
			dynamic_cast<MotherShip*>(enemies[i]))
			return enemies[i];
	}
	return nullptr;
}
enum class GameMode {Survival,Arcade};
enum class GameState {
	Playing, Main_Menu, Paused,Game_Over,Game_win
};
void InputManager(Player& Me, FloatRect Mybounds, Enemy**& enemies, int& enemyCount, PowerUp**& powerups, int& powerupCount) {
	static int keyFrame = 0;
	float dx = 0, dy = 0;
	if (Keyboard::isKeyPressed(Keyboard::D) && Mybounds.left + Mybounds.width < windowSize.x) {
		Me.moveRight();
		dx = 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::A) && Mybounds.left > 0) {
		Me.moveLeft();
		dx = -1;
	}
	if (Keyboard::isKeyPressed(Keyboard::W) && Mybounds.top > 0) {
		Me.moveUp();
		dy = -1;
	}
	if (Keyboard::isKeyPressed(Keyboard::S) && Mybounds.top < windowSize.y) {
		Me.moveDown();
		dy = 1;
	}

	static bool eWasPressed = false;
	bool eNowPressed = Keyboard::isKeyPressed(Keyboard::E);
	if (eNowPressed && !eWasPressed && (dx != 0 || dy != 0)) {
		Me.dash(dx, dy);
	}
	eWasPressed = eNowPressed;

	if (Keyboard::isKeyPressed(Keyboard::Space) && keyFrame >= 150) {
		keyFrame = 0;
		Me.shoot(Mybounds);
	}
	if(keyFrame < 150)
	keyFrame++;

	static bool nWasPressed = false;
	bool nNowPressed = Keyboard::isKeyPressed(Keyboard::N);
	if (nNowPressed && !nWasPressed && Me.empCount > 0) {
		Me.empShot = true;
		Me.empShoot(Mybounds);
		for (int i = enemyCount - 1; i >= 0; i--) {
			if (enemies[i]->getStatus()) continue;
			if (dynamic_cast<Cruiser*>(enemies[i]) || dynamic_cast<TwinCannon*>(enemies[i]) || dynamic_cast<MotherShip*>(enemies[i])) {
				for (int d = 0; d < 15; d++) enemies[i]->takeDamage();
			} 
			else {
				Vector2f pos = enemies[i]->getSprite().getPosition();
				spawnPowerup(pos.x, pos.y, powerups, powerupCount);
				deleteEnemy(i, enemies, enemyCount);
			}
		}
		
	}
	nWasPressed = nNowPressed;
}

void spawnWave(Enemy**& enemies, int& enemyCount,FloatRect Me,GameMode mode)
{
	if (mode == GameMode::Survival) {
		int droneCount = BASE_DRONES + (wave / 2) * BASE_DRONES;
		int seekerCount = (wave >= SEEKERS_START) ? BASE_SEEKERS + (wave / 3) * BASE_SEEKERS : 0;
		int viperCount = (wave >= VIPERS_START) ? BASE_VIPERS + (wave / 4) * BASE_VIPERS : 0;
		enemyCount = droneCount + seekerCount + viperCount;

		delete[] enemies;
		enemies = new Enemy * [enemyCount];

		int i = 0;

		for (int d = 0; d < droneCount; d++)
			enemies[i++] = new Drone{ RandomValue(windowSize.x),RandomValue(-500.f,0.f), Drone_Vy, 0, 1, kDroneTexturePath };

		// Spawn seekers (only after SEEKERS_START wave)
		for (int s = 0; s < seekerCount; s++)
			enemies[i++] = new Seeker(Me.left, Me.top, RandomValue(windowSize.x), RandomValue(-500.f, 0.f), seeker_Speed, seeker_Speed, 1, kSeekerTexturePath);

		// Spawn vipers (only after VIPERS_START wave)
		for (int v = 0; v < viperCount; v++)
			enemies[i++] = new Viper{ RandomValue(windowSize.x),RandomValue(-400.f,0.f), viper_Vy,0, 1, kViperTexturePath };
	}
	if (mode == GameMode::Arcade) {
		int droneCount = (wave>= 0 && wave % 5 != 0) ? BASE_DRONES + (wave / 3) * BASE_DRONES : 0;
		int seekerCount = (wave >= SEEKERS_START && wave % 5 != 0) ? BASE_SEEKERS + (wave / 3) * BASE_SEEKERS : 0;
		int viperCount = (wave >= VIPERS_START && wave % 5 != 0) ? BASE_VIPERS + (wave / 4) * BASE_VIPERS : 0;
		int cruiserCount = (wave == 5) ? 1 : 0;
		int TwinCannonCount = (wave == 10) ? 1 : 0;
		int MotherShipCount = (wave == 15) ? 1 : 0;

		enemyCount = droneCount + seekerCount + viperCount + MotherShipCount + TwinCannonCount + cruiserCount;

		delete[] enemies;
		enemies = new Enemy * [enemyCount];

		int i = 0;

		for (int d = 0; d < droneCount; d++)
			enemies[i++] = new Drone{ RandomValue(windowSize.x),RandomValue(-500.f,0.f), Drone_Vy, 0, 1, kDroneTexturePath };

		// Spawn seekers (only after SEEKERS_START wave)
		for (int s = 0; s < seekerCount; s++)
			enemies[i++] = new Seeker(Me.left, Me.top, RandomValue(windowSize.x), RandomValue(-500.f, 0.f), seeker_Speed, seeker_Speed, 1, kSeekerTexturePath);

		// Spawn vipers (only after VIPERS_START wave)
		for (int v = 0; v < viperCount; v++)
			enemies[i++] = new Viper{ RandomValue(windowSize.x),RandomValue(-400.f,0.f), viper_Vy,0, 1, kViperTexturePath };

		if (wave == 5) {
			enemies[i++] = new Cruiser{ 100, 50, 0, cruiserSpeed, 20, kCruiserTexturePath };
		}
		if (wave == 10) {
			enemies[i++] = new TwinCannon(300, 100, 0, 0, 30, kTwinCannonTexturePath);
		}
		if (wave == 15) {
			enemies[i++]= new MotherShip(300, 100, 0, 0, 40, kMotherShipTexturePath);
	
		}
	}
}

class Screen
{
public:
	static const int OPTION_COUNT = 3;

	RectangleShape bars[OPTION_COUNT];
	Text labels[OPTION_COUNT];
	Text title;
	Font font;

	int   hovered = -1;  // -1 = no hover
	float glowTimer = 0.f;
	Screen(sf::RenderWindow& window, const char* Header,const char* Option1, const char* Option2, const char* Option3)
	{
		font.loadFromFile(FontPath);

		const char* labelStr[OPTION_COUNT] = {
			Option1,
			Option2,
			Option3
		};
		float centerX = window.getSize().x / 2.f;  // 280
		float startY = 300.f;                      // where first bar begins
		float spacing = 80.f;
		// Title
		title.setFont(font);
		title.setString(Header);
		title.setCharacterSize(20);
		title.setLetterSpacing(6.f);
		title.setFillColor(sf::Color(192, 200, 255, 178));
		sf::FloatRect tb = title.getLocalBounds();
		title.setOrigin(tb.width / 2.f, 0.f);
		title.setPosition(centerX, 220.f);

		for (int i = 0; i < OPTION_COUNT; i++)
		{
			// Bar — width capped to fit 560px window
			bars[i].setSize(sf::Vector2f(300.f, 54.f));
			bars[i].setFillColor(sf::Color(13, 13, 32));
			bars[i].setOutlineThickness(1.5f);
			bars[i].setOutlineColor(sf::Color(30, 30, 68));
			bars[i].setOrigin(150.f, 0.f);                        // half of 300
			bars[i].setPosition(centerX, startY + i * spacing);

			// Label
			labels[i].setFont(font);
			labels[i].setString(labelStr[i]);
			labels[i].setCharacterSize(12);
			labels[i].setLetterSpacing(2.f);
			labels[i].setFillColor(sf::Color(100, 119, 170));
			sf::FloatRect lb = labels[i].getLocalBounds();
			labels[i].setOrigin(lb.width / 2.f, lb.top + lb.height / 2.f);  // centered
			labels[i].setPosition(centerX, startY + i * spacing + 27.f);
		}
	}

	void update(float dt,RenderWindow &window)
	{
		// Get mouse position
		sf::Vector2i mouse = sf::Mouse::getPosition(window);

		int newHovered = -1;
		for (int i = 0; i < OPTION_COUNT; i++)
		{
			if (bars[i].getGlobalBounds().contains(static_cast<float>(mouse.x),static_cast<float>(mouse.y)))
			{
				newHovered = i;
				break;
			}
		}

		// Reset previous hovered bar if hover changed
		if (newHovered != hovered)
		{
			if (hovered != -1)
			{
				bars[hovered].setOutlineColor(sf::Color(30, 30, 68));
				bars[hovered].setOutlineThickness(1.5f);
				labels[hovered].setFillColor(sf::Color(100, 119, 170));
			}
			hovered = newHovered;
			glowTimer = 0.f;
		}
		// Animate glow on hovered bar
		if (hovered != -1)
		{
			glowTimer += dt;
			float t = (std::sin(glowTimer * 3.f) + 1.f) / 2.f;

			sf::Uint8 r = static_cast<sf::Uint8>(51 + t * 76);
			sf::Uint8 g = static_cast<sf::Uint8>(68 + t * 68);
			sf::Uint8 b = static_cast<sf::Uint8>(187 + t * 68);
			bars[hovered].setOutlineColor(sf::Color(r, g, b));
			bars[hovered].setOutlineThickness(2.f + t * 1.5f);
			labels[hovered].setFillColor(sf::Color(221, 232, 255));
		}
	}

	void draw(sf::RenderWindow& window)
	{
		sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x),static_cast<float>(window.getSize().y)));
		overlay.setFillColor(sf::Color(0, 0, 0, 160));
		window.draw(overlay);

		window.draw(title);
		for (int i = 0; i < OPTION_COUNT; i++)
		{
			window.draw(bars[i]);
			window.draw(labels[i]);
		}
	}
};

class MainMenuScreen : public Screen {
public:
	Text arcadeBest;
	Text survivalBest;

	MainMenuScreen(sf::RenderWindow& window, Font& font, int arcadeHigh, int survivalHigh)
		: Screen(window, "Main Menu", "Arcade Mode", "Survival Mode", "Exit")
	{
	arcadeBest.setFont(font);
	arcadeBest.setString("Best: " + std::to_string(arcadeHigh));
	arcadeBest.setCharacterSize(11);
	arcadeBest.setFillColor(sf::Color(150, 180, 255, 200));
	arcadeBest.setOrigin(arcadeBest.getLocalBounds().width / 2.f, arcadeBest.getLocalBounds().height / 2.f);
	arcadeBest.setPosition(window.getSize().x / 2.f, 341.f);

	survivalBest.setFont(font);
	survivalBest.setString("Best: " + std::to_string(survivalHigh));
	survivalBest.setCharacterSize(11);
	survivalBest.setFillColor(sf::Color(150, 180, 255, 200));
	survivalBest.setOrigin(survivalBest.getLocalBounds().width / 2.f, survivalBest.getLocalBounds().height / 2.f);
	survivalBest.setPosition(window.getSize().x / 2.f, 422.f);
	}

void updateHighScores(int arcadeHigh, int survivalHigh) {
	arcadeBest.setString("Best: " + std::to_string(arcadeHigh));
	arcadeBest.setOrigin(arcadeBest.getLocalBounds().width / 2.f, arcadeBest.getLocalBounds().height / 2.f);
	survivalBest.setString("Best: " + std::to_string(survivalHigh));
	survivalBest.setOrigin(survivalBest.getLocalBounds().width / 2.f, survivalBest.getLocalBounds().height / 2.f);
}

	void draw(sf::RenderWindow& window) {
		Screen::draw(window);
		window.draw(arcadeBest);
		window.draw(survivalBest);
	}
};

class GameObject {
private:
	void loadTextures() {
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
		textureCache[kSpreadPlayerPath].loadFromFile(kSpreadPlayerPath);
		textureCache[kPiercePlayerPath].loadFromFile(kPiercePlayerPath);
		textureCache[kPierceBulletPath].loadFromFile(kPierceBulletPath);
		textureCache[kEmpBulletPath].loadFromFile(kEmpBulletPath);
		textureCache[kHeartTexturePath].loadFromFile(kHeartTexturePath);
		textureCache[kShieldTexturePath].loadFromFile(kShieldTexturePath);
		textureCache[kEmpTexturePath].loadFromFile(kEmpTexturePath);
		textureCache[kAsteroid1Path].loadFromFile(kAsteroid1Path);
		textureCache[kAsteroid2Path].loadFromFile(kAsteroid2Path);
		textureCache[kAsteroid3Path].loadFromFile(kAsteroid3Path);
		textureCache[kPlayerShieldTexturePath].loadFromFile(kPlayerShieldTexturePath);

	}
public:
	GameState state;
	GameMode mode;
	Screen pauseScreen;
	MainMenuScreen* mainMenuScreen;
	Screen gameOver;
	Screen gameWin;
	Player* player;
	Enemy** enemies;
	int enemyCount;
	Star stars[STAR_COUNT];
	Clock clock;
	PowerUp** powerups;
	int powerupCount;
	Asteroid** asteroids;
	int asteroidCount;
	FPS fps;
	Text fpsNum;
	Text scoreText;
	Text multiplierText;
	Text dashText;
	Font font;
	Text waveText;
	RectangleShape healthBarBg;
	RectangleShape healthBarFill;
	Text healthBarLabel;

	int survivalHighScore;
	int arcadeHighScore;

	GameObject(GameState s,RenderWindow &window) : 
		state(s), pauseScreen(window,"Paused","Continue","Back To Main Menu","Exit"), 
		gameOver(window, "Game Over", "Retry", "Back To Main Menu", "Exit"),
		gameWin(window, "You Win!", "Play Again", "Back To Main Menu", "Exit"), 
		survivalHighScore(0), arcadeHighScore(0) {

		loadTextures();
		loadHighScores();
		player = nullptr;
		enemies = nullptr;
		enemyCount = 0;
		powerups = nullptr;
		powerupCount = 0;
		asteroids = nullptr;
		asteroidCount = 0;
		font.loadFromFile(FontPath);
		mainMenuScreen = new MainMenuScreen(window, font, survivalHighScore, arcadeHighScore);
		fpsNum.setFont(font);
		fpsNum.setCharacterSize(75);
		fpsNum.setFillColor(Color::Green);
		fpsNum.setPosition(480, 50);
		// Wave text
		waveText.setFont(font);
		waveText.setCharacterSize(25);
		waveText.setFillColor(Color(180, 180, 255));
		waveText.setPosition(10, 100);

		healthBarBg.setSize(Vector2f(200.f, 18.f));
		healthBarBg.setFillColor(Color(40, 0, 0));
		healthBarBg.setOutlineColor(Color(150, 0, 0));
		healthBarBg.setOutlineThickness(1.5f);
		healthBarBg.setPosition(windowSize.x / 2.f - 100.f, 18.f);
		healthBarFill.setSize(Vector2f(200.f, 18.f));
		healthBarFill.setFillColor(Color(220, 30, 30));
		healthBarFill.setPosition(windowSize.x / 2.f - 100.f, 18.f);
		healthBarLabel.setFont(font);
		healthBarLabel.setCharacterSize(10);
		healthBarLabel.setFillColor(Color::White);
		healthBarLabel.setLetterSpacing(2.f);

		dashText.setFont(font);
		dashText.setCharacterSize(20);
		dashText.setFillColor(Color::Cyan);
		dashText.setPosition(10, 140);

		scoreText.setFont(font);
		scoreText.setCharacterSize(20);
		scoreText.setFillColor(Color::White);
		scoreText.setPosition(10, 160);

		multiplierText.setFont(font);
		multiplierText.setCharacterSize(20);
		multiplierText.setFillColor(Color::Yellow);
		multiplierText.setPosition(10, 180);
	}

	~GameObject() {
		for (int i = 0; i < powerupCount; i++)
			delete powerups[i];
		delete[] powerups;
		for (int i = 0; i < asteroidCount; i++)
			delete asteroids[i];
		delete[] asteroids;
		delete mainMenuScreen;
	}

	void loadHighScores() {
		ifstream file("highscores.txt");
		if (!file.is_open()) return;
		file >> survivalHighScore >> arcadeHighScore;
		file.close();
	}

	void saveHighScores() {
		ofstream file("highscores.txt");
		file << survivalHighScore << " " << arcadeHighScore << "\n";
		file.close();
	}

	int& getHighScoreRef(GameMode m) {
		return (m == GameMode::Survival) ? survivalHighScore : arcadeHighScore;
	}

	void startGame(RenderWindow &window) {

		if (state == GameState::Playing) {
			FloatRect Mybounds = player->getSprite().getGlobalBounds();
			InputManager(*player, Mybounds, enemies, enemyCount, powerups, powerupCount);
			player->update();
			player->updateDash();
			if (enemyCount == 0) {
				wave++;
				cout << "Wave No: " << wave << endl;
				if (wave > 15) {
					state = GameState::Game_win;
				}
				spawnWave(enemies, enemyCount, Mybounds,mode);
			}
			collisionsManager(*player, enemies, enemyCount, powerups, powerupCount, asteroids, asteroidCount);
			updateAsteroids(asteroids, asteroidCount);
			if (player->getStatus()) {
				int currentScore = player->getScore();
				int& highScore = getHighScoreRef(mode);
				if (currentScore > highScore) {
					highScore = currentScore;
					saveHighScores();
				}
				state = GameState::Game_Over;
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				state = GameState::Paused;
			}
			window.clear();
			for (int i = 0; i < STAR_COUNT; i++) {
				stars[i].move();
				stars[i].draw(window);
			}
			player->draw(window);
			// draw hearts and shields ok i get it bro
			{
				Sprite heart;
				heart.setTexture(textureCache[kHeartTexturePath]);
				heart.setScale(0.007f, 0.007f);
				Sprite shield;
				shield.setTexture(textureCache[kShieldTexturePath]);
				shield.setScale(0.02f, 0.02f);
				int healthCount = player->getHealth();
				for (int i = 0; i < healthCount; i++) {
					heart.setPosition(10 + i * 40, 10);
					window.draw(heart);
				}
				for (int i = healthCount; i < healthCount + player->shieldHits; i++) {
					shield.setPosition(10 + i * 40, 0);
					window.draw(shield);
				}
			}
			// draw EMP
			{
				Sprite emp;
				emp.setTexture(textureCache[kEmpTexturePath]);
				emp.setScale(0.055f, 0.055f);
				for (int i = 0; i < player->empCount; i++) {
					emp.setPosition(10 + i * 45, 42);
					window.draw(emp);
				}
			}

			scoreText.setString("Score: " + std::to_string(player->getScore()));
			window.draw(scoreText);

			multiplierText.setString("Multiplier: " + std::to_string(player->getScoreMultiplier()) + "x");
			window.draw(multiplierText);

			float dashCooldown = player->getDashCooldown();
			if (dashCooldown > 0) {
				dashText.setString("Dash: " + std::to_string(static_cast<int>(dashCooldown + 0.99f)) + "s");
				dashText.setFillColor(Color(255, 100, 100));
			} else {
				dashText.setString("Dash: Ready (E)");
				dashText.setFillColor(Color::Cyan);
			}
			window.draw(dashText);

			for (int i = 0; i < enemyCount; i++)
				enemies[i]->draw(window);
			for (int i = 0; i < powerupCount; i++)
				powerups[i]->draw(window);
			for (int i = 0; i < asteroidCount; i++)
				asteroids[i]->draw(window);

			fps.update();
			fpsNum.setString(sf::String(std::to_string(fps.getFPS())));
			waveText.setString("WAVE  " + to_string(wave));
			Enemy* boss = getActiveBoss(enemyCount,enemies);
			if (boss && !boss->getStatus()) { // POINT OF FOCUS
				int bossHealth = boss->getHealth();
				int bossMaxHealth = 20; // match what you pass in spawnWave

				float ratio = max(0.f, min(1.f, bossHealth / (float)bossMaxHealth));
				healthBarFill.setSize(Vector2f(200.f * ratio, 18.f));

				// color shifts red -> yellow -> green
				Uint8 r = static_cast<Uint8>(220 * (1.f - ratio));
				Uint8 g = static_cast<Uint8>(200 * ratio);
				healthBarFill.setFillColor(Color(220 - r / 2, g, 30));

				string bossName = dynamic_cast<Cruiser*>(boss) ? "CRUISER" :
					dynamic_cast<TwinCannon*>(boss) ? "TWIN CANNON" :
					"MOTHERSHIP";
				FloatRect lb = healthBarLabel.getLocalBounds();
				healthBarLabel.setString(bossName);
				healthBarLabel.setOrigin(lb.width / 2.f, 0);
				healthBarLabel.setPosition(windowSize.x / 2.f, 2.f);

				window.draw(healthBarBg);
				window.draw(healthBarFill);
				window.draw(healthBarLabel);
			}
			window.draw(waveText);
			window.draw(fpsNum);
			window.display();
		}
		if (state == GameState::Paused) {
		    playPaused(window);
		}
		if (state == GameState::Main_Menu) {
			if (!player) delete player;
			player = nullptr;
			playMainMenu(window);
		}
		if (state == GameState::Game_Over) {
			delete player;
			player = nullptr;
			playGameOver(window);
		}
		if (state == GameState::Game_win) {
			playGameWin(window);
		}
	}
	void playPaused(RenderWindow& window)
	{
		// Use just-pressed, not held
		static bool wasPressed = false;
		bool nowPressed = Mouse::isButtonPressed(Mouse::Left);

		if (nowPressed && !wasPressed && pauseScreen.hovered != -1)
		{
			if (pauseScreen.hovered == 0) state = GameState::Playing;
			if (pauseScreen.hovered == 1) state = GameState::Main_Menu;
			if (pauseScreen.hovered == 2) window.close();
		}
		wasPressed = nowPressed;

		window.clear();
		pauseScreen.update(clock.restart().asSeconds(), window);
		pauseScreen.draw(window);
		window.display();
	}
	void playMainMenu(RenderWindow& window)
	{   
		static bool wasPressed = false;
		bool nowPressed = Mouse::isButtonPressed(Mouse::Left);

		if (nowPressed && !wasPressed && mainMenuScreen->hovered != -1)
		{
			if (mainMenuScreen->hovered == 0) mode = GameMode::Arcade;
			if (mainMenuScreen->hovered == 1) mode = GameMode::Survival;
			if (mainMenuScreen->hovered == 2) window.close();

			powerups = nullptr;
			powerupCount = 0;
			if (enemyCount != 0) {
				for (int i = 0; i < enemyCount; i++) {
					delete enemies[i];
				}
				delete[] enemies;
				enemies = nullptr;
				enemyCount = 0;
				wave = 0;
			}
			if (asteroidCount != 0) {
				for (int i = 0; i < asteroidCount; i++) {
					delete asteroids[i];
				}
				delete[] asteroids;
				asteroids = nullptr;
				asteroidCount = 0;
			}
			player = new Player(5, windowSize.x / 2, windowSize.y / 2, .4f, .4f, kPlayerTexturePath);
			spawnAsteroids(asteroids, asteroidCount);
			state = GameState::Playing;
		}
		wasPressed = nowPressed;
		window.clear();
		mainMenuScreen->updateHighScores(arcadeHighScore, survivalHighScore);
		mainMenuScreen->update(clock.restart().asSeconds(), window);
		mainMenuScreen->draw(window);
		window.display();
	}
	void playGameOver(RenderWindow& window)
	{


		static bool wasPressed = false;
		bool nowPressed = Mouse::isButtonPressed(Mouse::Left);

		if (nowPressed && !wasPressed && gameOver.hovered != -1)
		{
			if (gameOver.hovered == 0) {
				for (int i = 0; i < powerupCount; i++)
					delete powerups[i];
				delete[] powerups;
				powerups = nullptr;
				powerupCount = 0;
				if (enemyCount != 0) {
					for (int i = 0; i < enemyCount; i++) {
						delete enemies[i];
					}
					delete[] enemies;
					enemies = nullptr;
					enemyCount = 0;
					wave = 0;
				}
				if (asteroidCount != 0) {
					for (int i = 0; i < asteroidCount; i++) {
						delete asteroids[i];
					}
					delete[] asteroids;
					asteroids = nullptr;
					asteroidCount = 0;
				}
				player = new Player(5, windowSize.x / 2, windowSize.y / 2, .4f, .4f, kPlayerTexturePath);
				spawnAsteroids(asteroids, asteroidCount);
				state = GameState::Playing;
			}
			if (gameOver.hovered == 1) state = GameState::Main_Menu;
			if (gameOver.hovered == 2) window.close();

		}
		wasPressed = nowPressed;

		window.clear();
		gameOver.update(clock.restart().asSeconds(), window);
		gameOver.draw(window);
		window.display();
	}
	void playGameWin(RenderWindow& window)
	{
		static bool wasPressed = false;
		bool nowPressed = Mouse::isButtonPressed(Mouse::Left);

		if (nowPressed && !wasPressed && gameWin.hovered != -1)
		{
			if (gameWin.hovered == 0) {
				for (int i = 0; i < powerupCount; i++)
					delete powerups[i];
				delete[] powerups;
				powerups = nullptr;
				powerupCount = 0;
				if (enemyCount != 0) {
					for (int i = 0; i < enemyCount; i++) {
						delete enemies[i];
					}
					delete[] enemies;
					enemies = nullptr;
					enemyCount = 0;
					wave = 0;
				}
				player = new Player(5, windowSize.x / 2, windowSize.y / 2, .4f, .4f, kPlayerTexturePath);
				state = GameState::Playing;
			}
			if (gameWin.hovered == 1) state = GameState::Main_Menu;
			if (gameWin.hovered == 2) window.close();

		}
		wasPressed = nowPressed;

		window.clear();
		gameWin.update(clock.restart().asSeconds(), window);
		gameWin.draw(window);
		window.display();
	}
};

int main() {
	RenderWindow window(VideoMode(560, 854), "Space Shooter", Style::Close);
	windowSize = window.getSize();

	GameObject SpaceShooter(GameState::Main_Menu,window);
	while (window.isOpen()) {

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}
		SpaceShooter.startGame(window);

	}

	return 0;
}

