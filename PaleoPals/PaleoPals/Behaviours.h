#pragma once
#ifndef BEHAVIOURS_H
#define BEHAVIOURS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <memory>

// Forward declarations
class Map;
class Paleontologist;

//------------------------------------------------------------
// Behavior State Enum
//------------------------------------------------------------
enum class BehaviorState
{
    Idle,
    Wandering,
    SearchingForFossil,
    MovingToTarget,
    Mining
};

//------------------------------------------------------------
// Pathfinding Node (for A* algorithm)
//------------------------------------------------------------
struct PathNode
{
    int row, col;
    float gCost; // Distance from start
    float hCost; // Heuristic distance to goal
    float fCost() const { return gCost + hCost; }
    PathNode* parent;

    PathNode(int r, int c) : row(r), col(c), gCost(0), hCost(0), parent(nullptr) {}

    bool operator>(const PathNode& other) const { return fCost() > other.fCost(); }
};

//------------------------------------------------------------
// Base Behavior Class
//------------------------------------------------------------
class Behavior
{
public:
    virtual ~Behavior() = default;
    virtual void enter(Paleontologist* agent) = 0;
    virtual BehaviorState update(Paleontologist* agent, sf::Time deltaTime, Map& map) = 0;
    virtual void exit(Paleontologist* agent) = 0;
};

//------------------------------------------------------------
// Idle Behavior
//------------------------------------------------------------
class IdleBehavior : public Behavior
{
public:
    void enter(Paleontologist* agent) override;
    BehaviorState update(Paleontologist* agent, sf::Time deltaTime, Map& map) override;
    void exit(Paleontologist* agent) override;

private:
    float m_idleTimer = 0.0f;
    float m_idleDuration = 2.0f;
};

//------------------------------------------------------------
// Wandering Behavior
//------------------------------------------------------------
class WanderingBehavior : public Behavior
{
public:
    void enter(Paleontologist* agent) override;
    BehaviorState update(Paleontologist* agent, sf::Time deltaTime, Map& map) override;
    void exit(Paleontologist* agent) override;

private:
    sf::Vector2f m_targetPosition;
    float m_wanderTimer = 0.0f;
    float m_wanderDuration = 3.0f;
    bool m_hasTarget = false;

    void pickRandomWalkableTarget(Paleontologist* agent, Map& map);
};

//------------------------------------------------------------
// Searching Behavior (uses A* to find fossils)
//------------------------------------------------------------
class SearchingBehavior : public Behavior
{
public:
    void enter(Paleontologist* agent) override;
    BehaviorState update(Paleontologist* agent, sf::Time deltaTime, Map& map) override;
    void exit(Paleontologist* agent) override;

private:
    std::vector<sf::Vector2i> m_path;
    int m_currentPathIndex = 0;
    sf::Vector2i m_targetTile;
    sf::Vector2f m_surfacePosition; // Store surface position to return to
    bool m_hasPath = false;
    bool m_returningToSurface = false;

    // Mining state for each tile while digging
    bool m_isMiningTile = false;
    float m_miningProgress = 0.0f;
    float m_currentTileDuration = 1.0f;

    // Track the last tile that was mined so we can place/remove a ladder support
    sf::Vector2i m_lastMinedTile{ -1, -1 };
    bool m_hasLastMinedTile = false;

    // Track all mined tiles (stack) so ladders can be removed as the agent climbs back up
    std::vector<sf::Vector2i> m_minedTiles;

    bool findNearestFossil(Paleontologist* agent, Map& map);
    std::vector<sf::Vector2i> findPath(sf::Vector2i start, sf::Vector2i goal, Map& map);
    float heuristic(sf::Vector2i a, sf::Vector2i b);
};

//------------------------------------------------------------
// Mining Behavior
//------------------------------------------------------------
class MiningBehavior : public Behavior
{
public:
    void enter(Paleontologist* agent) override;
    BehaviorState update(Paleontologist* agent, sf::Time deltaTime, Map& map) override;
    void exit(Paleontologist* agent) override;

private:
    float m_miningProgress = 0.0f;
    float m_miningDuration = 1.0f;
    sf::Vector2i m_targetTile;
    bool m_returningToSurface = false; // when true, move agent back up smoothly
};

//------------------------------------------------------------
// Behavior Manager (State Machine)
//------------------------------------------------------------
class BehaviorManager
{
public:
    BehaviorManager();
    void update(Paleontologist* agent, sf::Time deltaTime, Map& map);
    void changeState(BehaviorState newState, Paleontologist* agent);
    BehaviorState getCurrentState() const { return m_currentState; }

private:
    std::unique_ptr<Behavior> m_behaviors[5];
    BehaviorState m_currentState;
    Behavior* m_currentBehavior;
};

#endif // !BEHAVIOURS_H