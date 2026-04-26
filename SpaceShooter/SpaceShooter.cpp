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

const float Drone_Vy = 0.1f;
const float Bullet_speed = 0.3f;
const float viper_Vy = 0.1f;
const float amplitude = 100.f;
const float frequency = 0.1f;

String resolvePath(const String& relative) {
	return kAssetRoot + relative;
}

const String kPlayerTexturePath = resolvePath("Images/Player/defaultPlayer.png");
const String kBulletTexturePath = resolvePath("Images/Player/defaultBullet.png");
const String kDroneTexturePath = resolvePath("Images/Enemy/defaultDrone.png");
const String kViperTexturePath = resolvePath("Images/Enemy/defaultViper.png");
const String ExpolsionTexturePath = resolvePath("Images/defaultExplosion.png");
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
	void setPosition() {
		sprite.setPosition(xPos, yPos);
	}
	void SetOrigin() {
		sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	}
	void SetScale(float x, float y) {
		sprite.setScale(x, y);
	}
	void destroy() {
		this->loadTexture(ExpolsionTexturePath, ExplosionFrames);
		SetScale(0.7f, 0.7f);
		SetOrigin();
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
	Clock  clock;
	Bullet** bullets;
	int    bulletCount;

public:
	ShootableCharacter()
		: Entity(), bullets(nullptr), bulletCount(0) {
	}

	ShootableCharacter(float xPos, float yPos, float V_x, float V_y)
		: Entity(xPos, yPos, V_x, V_y), bullets(nullptr), bulletCount(0) {
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
	}

	void drawBullets(RenderWindow& window) {
		for (int i = 0; i < bulletCount; i++) {
			bullets[i]->draw(window);
		}
	}
	
};

class Player : public ShootableCharacter {
	static int lives;
public:
	Player(float xPos, float yPos, float V_x, float V_y, const String& filename)
		: ShootableCharacter(xPos, yPos, V_x, V_y) {
		this->loadTexture(filename);
		this->SetOrigin();
		this->SetScale(.2f, .2f);
		this->setPosition();
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
int Player::lives = 5;

class Enemy : public ShootableCharacter {
	int  health;
	bool isDead;
public:
	Enemy(float x, float y, float Vy, float Vx, int health, const String& filename)
		: ShootableCharacter(x, y, Vx, Vy) {
		this->health = health;
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
		isDead = 1;
	}

	virtual ~Enemy() {}
};

class Drone : public Enemy {
public:
	Drone(float x, float y, float Vy, float Vx, int health, const String& filename)
		: Enemy(x, y, Vy, Vx, health, filename) {
		this->SetOrigin();
		this->setPosition();
		this->SetScale(0.1f, 0.1f);
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
		this->SetScale(0.05f, -0.05f);
		currentY = y;
		time = 0;
	}
	void move() override {
		time += 0.05f;
		float x = getxPos() + amplitude * sin(time * frequency);
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

void collisionsManager(Player& Me, Enemy**& drone) {
	for (int k = 0; k < 1; k++) {
		FloatRect P = Me.getSprite().getGlobalBounds();
		FloatRect E = drone[k]->getSprite().getGlobalBounds();
		Bullet** MyBullets_Boundary = Me.getPlayerBullets();
		Bullet** EnemyBullets_Boundary = drone[k]->getBullets();

		for (int i = 0; i < drone[k]->getBulletCount(); i++) {
			if (EnemyBullets_Boundary[i]->getSprite().getGlobalBounds().intersects(P)) {
				drone[i]->deleteBullet(i);
				Player::takeDamage();
			}
		}

		for (int i = 0; i < Me.getBulletCount(); i++) {
			if (MyBullets_Boundary[i]->getSprite().getGlobalBounds().intersects(E) && !(drone[k]->checkStatus())) {
				Me.deleteBullet(i);
				drone[k]->destroy();
				drone[k]->setStatus();
				for (int r = 0; r < drone[k]->getBulletCount(); r++) {
					drone[k]->deleteBullet(r);
				}
			}
		}

		if (drone[k]->checkStatus()) {
			// Implement explosion animation here
		}
		else {
			drone[k]->move();
			if (drone[k]->getSprite().getGlobalBounds().top + drone[k]->getSprite().getGlobalBounds().height >= windowSize.y) {
				cout << "Drone deleted\n";
				cout << "Player died\n";
			}
			drone[k]->shoot(drone[k]->getSprite().getGlobalBounds());
			drone[k]->update();
		}
	}
}

int main()
{
	RenderWindow window(VideoMode::getDesktopMode(), "SFML Works!", Style::Fullscreen);

	windowSize = window.getSize();

	Player Me(windowSize.x / 2, windowSize.y / 1.1f, .5f, .5f, kPlayerTexturePath);
	Enemy** enemy = new Enemy * [1];
	enemy[0] = new Viper{ 300, 0, viper_Vy,0, 1, kViperTexturePath };

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

		Me.update();
		collisionsManager(Me, enemy);
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

