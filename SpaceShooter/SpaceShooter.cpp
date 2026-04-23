
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <SFML\Window.hpp>
#include<SFML\System.hpp>

using namespace std;
using namespace sf;

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
	bool loadTexture(const string& filename) {
		if (!Tex.loadFromFile(filename)) {
			return false; 
		}
		sprite.setTexture(Tex);
		return true;
	}
	void setPosition() {
		sprite.setPosition(xPos, yPos);
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
class Player : public Entity {
	int lives;
public:
	Player(int lives, float xPos, float yPos, float V_x, float V_y,const string& filename):Entity(xPos,yPos,V_x,V_y) {
		this->lives = lives;
		this->loadTexture(filename);
		

	}
	~Player() {

	}

	void SetOrigin() {
		this->getSprite().setOrigin(this->getSprite().getLocalBounds().width / 2, this->getSprite().getLocalBounds().height / 2);
	}
	void SetScale(float x,float y) {
		this->getSprite().setScale(x, y);
	}
	void moveRight() {
		this->getSprite().move(this->getVx(), 0);
	}
	void moveLeft() {
		this->getSprite().move(-this->getVx(), 0);
	}
};

class Bullet :public Entity {
	int damage;
	bool pierceFlag;
public:
	Bullet() {}
	Bullet(int d,bool pierce, float xPos, float yPos, float V_x, float V_y, const string& filename) :Entity(xPos, yPos, V_x, V_y) {
		damage = d;
		pierceFlag = pierce;
		this->loadTexture(filename);
	}
	void moveUp() {
		this->getSprite().move(0, -this->getVy());
	}
	void SetOrigin() {
		this->getSprite().setOrigin(this->getSprite().getLocalBounds().width / 2, this->getSprite().getLocalBounds().height / 2);
	}
	void SetScale(float x, float y) {
		this->getSprite().setScale(x, y);
	}
	float getYpos()const{
		return this->getYpos();
	}
};

int main()
{

	RenderWindow window(VideoMode::getDesktopMode(), "SFML Works!", Style::Fullscreen);
	Player Me(3,window.getSize().x/2, window.getSize().y/1.1, .5f, .5f, "C:/Users/Administrator/source/repos/SpaceShooter/SpaceShooter/Images/Player/pngegg (4).png");

	Me.SetOrigin();
	Me.SetScale(.2f, .2f);
	Me.setPosition();

	Bullet** PlayerBullets = nullptr;
	int PlayerBulletCount = 0;
	
	Vector2u windowSize = window.getSize();

	while (window.isOpen()) {
		FloatRect bounds = Me.getSprite().getGlobalBounds();
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::P)) {
				window.close();
			}
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
				
				Bullet** temp = new Bullet*[PlayerBulletCount+1];
				for (int i = 0; i < PlayerBulletCount; i++) {
					temp[i] = PlayerBullets[i];
				}
				
				temp[PlayerBulletCount] = new Bullet(3, false, bounds.left + (bounds.width / 2.0f), bounds.top, 0.5f, .30f, "C:/Users/Administrator/source/repos/SpaceShooter/SpaceShooter/Images/Player/Powerful-Laser-Beam-Eyes-Vision-PNG.png");
				temp[PlayerBulletCount]->SetScale(0.2f, 0.15f);
				temp[PlayerBulletCount]->SetOrigin();
				temp[PlayerBulletCount]->setPosition();
				delete[] PlayerBullets;
				PlayerBullets = temp;    
				PlayerBulletCount++;
			}
		}
		
		if (Keyboard::isKeyPressed(Keyboard::D) && bounds.left + bounds.width < window.getSize().x) {
				Me.moveRight();
		}
		if (Keyboard::isKeyPressed(Keyboard::A) && bounds.left > 0) {
			Me.moveLeft();
		}
		for (int i = 0; i < PlayerBulletCount; i++) {
			PlayerBullets[i]->moveUp();
		}
		for (int i = 0; i < PlayerBulletCount; i++) {
			if (PlayerBullets[i]->getSprite().getGlobalBounds().top <= 0) {
				delete PlayerBullets[i];
				Bullet** temp = new Bullet*[PlayerBulletCount - 1];
				for (int j = 0,k=0; j < PlayerBulletCount; j++) {
					if (i == j)
						continue;
					temp[k++] = PlayerBullets[j];
				}
				delete[] PlayerBullets;
				PlayerBullets = temp;
				PlayerBulletCount--;
				i--;
			}
		}
		window.clear();
		Me.draw(window);
		for (int i = 0; i < PlayerBulletCount; i++) {
			PlayerBullets[i]->draw(window);
		}
		
		window.display();
		
    }
	cout << "Hello World\n";
}
