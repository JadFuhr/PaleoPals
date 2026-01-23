#pragma once
#ifndef PALEONTOLOGIST_H
#define PALEONTOLOGIST_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "Behaviours.h"
#include <memory>

class Map;

class Paleontologist
{
public:
    Paleontologist();
    ~Paleontologist();

    // Delete copy constructor and copy assignment
    Paleontologist(const Paleontologist&) = delete;
    Paleontologist& operator=(const Paleontologist&) = delete;

    // Core functions
    void update(sf::Time deltaTime, Map& map);
    void draw(sf::RenderWindow& window);

    // Movement
    void move(sf::Vector2f direction);
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    void setPosition(sf::Vector2f pos) { m_sprite.setPosition(pos); }

    // Getters/Setters
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

    sf::Vector2i getTargetTile() const { return m_targetTile; }
    void setTargetTile(sf::Vector2i tile) { m_targetTile = tile; }

    void setMiningProgress(float progress) { m_miningProgress = progress; }
    float getMiningProgress() const { return m_miningProgress; }

    const sf::Sprite& getSprite() const { return m_sprite; }

    BehaviorState getCurrentState() const;

private:
    // Rendering
    sf::Texture m_texture;
    sf::Sprite m_sprite{m_texture};
    sf::RectangleShape m_progressBar;
    sf::RectangleShape m_progressBarBackground;

    // Animation
    int m_currentFrame = 0;
    float m_animationTimer = 0.0f;
    float m_frameTime = 0.15f; // Time per frame (150ms)
    const int m_frameWidth = 192;  
    const int m_frameHeight = 192; 
    const int m_totalFrames = 4;

    // Properties
    float m_speed = 60.0f;
    sf::Vector2i m_targetTile{ -1, -1 }; // Initialize to invalid tile
    float m_miningProgress = 0.0f;

    // AI Behavior
    std::unique_ptr<BehaviorManager> m_behaviorManager;

    // Helper functions
    void updateProgressBar();
    void updateAnimation(sf::Time deltaTime);
    void setFrame(int frame);
};

#endif // !PALEONTOLOGIST_H