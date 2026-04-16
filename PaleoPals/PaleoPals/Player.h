#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include <vector>
#include <string>
#include <algorithm>

class Map;
class Collectible;

struct CollectedItem
{
    int collectibleIndex;
    std::string name;
    std::string type; 
    int monetaryValue;

    std::string dinosaurName;
    std::string pieceId;
    std::string category;
};

enum class PlayerState
{
    Idle,
    Walking,
    Jumping,
    Falling,
    Mining
};

class Player
{
public:
    Player();
    ~Player();

    void update(sf::Time deltaTime, Map& map, const sf::RenderWindow& window, const sf::View& cameraView);
    void draw(sf::RenderWindow& window);
    void handleInput(sf::Time deltaTime, Map& map);

    void tryPickupCollectible(Map& map);

    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    const sf::Sprite& getSprite() const { return m_sprite; }
    const std::vector<CollectedItem>& getInventory() const { return m_inventory; }
    const std::vector<CollectedItem>& getNewPickups() const { return m_newPickups; }
    void clearNewPickups() { m_newPickups.clear(); }
    int getMoney() const { return m_money; }

    void collectFossil(const std::string& dinosaurName, const std::string& pieceId, const std::string& category);
    void collectAmber(int monetaryValue);
    void collectTrash();

    void updatePickaxe(const sf::RenderWindow& window, Map& map, const sf::View& cameraView);
    void checkPickaxeHit(const sf::RenderWindow& window, Map& map, const sf::View& cameraView);
    void updatePickaxeAnimation(sf::Time dt);

    float getPickaxeRadius() const;

    int getPickaxeDamage() const;

    void setPosition(sf::Vector2f pos);


    bool m_isSwinging = false;
    float m_pickaxeAngle = 0.f;
    float m_pickaxeRadius = 20.f; // 1 tile radius
    sf::Vector2f m_pickaxeTip;
    float m_pickaxeTipDistance = 32.f; 
    float m_pickaxeTipRadius = 6.f;    // collision circle radius

    int pickaxeRadiusLevel = 0;
    int damageLevel = 0;
    int pickupRadiusLevel = 0;
    int jumpLevel = 0;


    sf::Vector2f m_velocity;
    float m_moveSpeed = 150.0f;
    float m_jumpForce = -400.0f;
    float m_gravity = 1200.0f;
    bool m_isGrounded = false;
    bool m_canJump = true;

    void spendMoney(int amount) { m_money -= amount; }
    float getPickupRadius();
    float getJumpForce();



private:
    sf::Texture m_texture;
    sf::Sprite m_sprite{m_texture};
    sf::CircleShape m_pickupRadiusVisual; 
	float pickupRadius = 24.0f; 

    sf::Texture m_pickaxeTexture;
    sf::Sprite m_pickaxeSprite{ m_pickaxeTexture };

    int m_currentFrame = 0;
    float m_animationTimer = 0.0f;
    float m_frameTime = 0.15f;
    const int m_frameWidth = 192;
    const int m_frameHeight = 192;
    const int m_totalFrames = 4;
    bool m_facingRight = true;

	int m_pickaxeCurrentFrame = 0;
	int m_pickaxeFrameDirection = 1;
	float m_pickaxeAnimationTimer = 0.0f;
	float m_pickaxeFrameTime = 0.01f; 
	int m_pickaxeTotalFrames = 4;


    PlayerState m_state = PlayerState::Idle;

    std::vector<CollectedItem> m_inventory;
	std::vector<CollectedItem> m_newPickups; 
    float m_interactionRadius = 24.0f; 
    int m_money = 0; 

    float m_pickaxeCooldown = 0.0f;
    float m_pickaxeHitDelay = 0.15f; 

    void updateAnimation(sf::Time deltaTime);
    void setFrame(int frame);
    void applyPhysics(sf::Time deltaTime, Map& map);
    void checkCollisions(Map& map);
    bool isOnGround(Map& map);
    sf::Vector2i worldToTile(sf::Vector2f worldPos, Map& map);
    sf::Vector2f tileToWorld(sf::Vector2i tilePos, Map& map);

    sf::CircleShape m_pickaxeDebugCircle;


};

#endif // !PLAYER_H