#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include <vector>
#include <string>
#include <algorithm>

// Forward declarations
class Map;
class Collectible;

struct CollectedItem
{
    int collectibleIndex;
    std::string name;
    std::string type; // "fossil", "amber", "trash"
    int monetaryValue;

    // Fossil-specific
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

    // Core functions
    void update(sf::Time deltaTime, Map& map, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void handleInput(sf::Time deltaTime, Map& map);

    // Interaction
    void tryPickupCollectible(Map& map);

    // Getters
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    const sf::Sprite& getSprite() const { return m_sprite; }
    const std::vector<CollectedItem>& getInventory() const { return m_inventory; }
    const std::vector<CollectedItem>& getNewPickups() const { return m_newPickups; }
    void clearNewPickups() { m_newPickups.clear(); }
    int getMoney() const { return m_money; }

    // Collection methods
    void collectFossil(const std::string& dinosaurName, const std::string& pieceId, const std::string& category);
    void collectAmber(int monetaryValue);
    void collectTrash();
        
	//pickaxe methods
    void updatePickaxe(const sf::RenderWindow& window, Map& map);
    void checkPickaxeHit(const sf::RenderWindow& window, Map& map);
    void updatePickaxeAnimation(sf::Time dt);

    float getPickaxeRadius() const;

    int getPickaxeDamage() const;

    // Setters
    void setPosition(sf::Vector2f pos);

    //Mining 

    bool m_isSwinging = false;
    float m_pickaxeAngle = 0.f;
    float m_pickaxeRadius = 20.f; // 1 tile radius
    sf::Vector2f m_pickaxeTip;
    float m_pickaxeTipDistance = 32.f; 
    float m_pickaxeTipRadius = 6.f;    // collision circle radius

    // upgrades
    int pickaxeRadiusLevel = 0;
    int damageLevel = 0;
    void spendMoney(int amount) { m_money -= amount; }



private:
    // Rendering
    sf::Texture m_texture;
    sf::Sprite m_sprite{m_texture};
    sf::CircleShape m_pickupRadiusVisual; // Visual debug circle
	float pickupRadius = 24.0f; // 1 tile radius

    sf::Texture m_pickaxeTexture;
    sf::Sprite m_pickaxeSprite{ m_pickaxeTexture };

    // Animation
    int m_currentFrame = 0;
    float m_animationTimer = 0.0f;
    float m_frameTime = 0.15f;
    const int m_frameWidth = 192;
    const int m_frameHeight = 192;
    const int m_totalFrames = 4;
    bool m_facingRight = true;

    // pickaxe animation
	int m_pickaxeCurrentFrame = 0;
	int m_pickaxeFrameDirection = 1;
	float m_pickaxeAnimationTimer = 0.0f;
	float m_pickaxeFrameTime = 0.01f; // 10ms per frame
	int m_pickaxeTotalFrames = 4;

    // Physics
    sf::Vector2f m_velocity;
    float m_moveSpeed = 150.0f;
    float m_jumpForce = -400.0f;
    float m_gravity = 1200.0f;
    bool m_isGrounded = false;
    bool m_canJump = true;

    // Player state
    PlayerState m_state = PlayerState::Idle;

    // Inventory
    std::vector<CollectedItem> m_inventory;
	std::vector<CollectedItem> m_newPickups; // Items picked up current frame
    float m_interactionRadius = 24.0f; // 1 tile radius
    int m_money = 0; // Money from selling amber/trash

    //mining cooldown
    float m_pickaxeCooldown = 0.0f;
    float m_pickaxeHitDelay = 0.15f; // 150ms between hits

    // Helper functions
    void updateAnimation(sf::Time deltaTime);
    void setFrame(int frame);
    void applyPhysics(sf::Time deltaTime, Map& map);
    void checkCollisions(Map& map);
    bool isOnGround(Map& map);
    sf::Vector2i worldToTile(sf::Vector2f worldPos, Map& map);
    sf::Vector2f tileToWorld(sf::Vector2i tilePos, Map& map);

};

#endif // !PLAYER_H