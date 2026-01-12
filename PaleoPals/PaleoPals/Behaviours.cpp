#include "Behaviours.h"
#include "Paleontologist.h"
#include "Map.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <set>
#include <cstdlib>
#include <ctime>
#include <limits>

//------------------------------------------------------------
// Behavior Manager Implementation
//------------------------------------------------------------
BehaviorManager::BehaviorManager() : m_currentState(BehaviorState::Idle)
{
    // Seed random for behavior decisions
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_behaviors[0] = std::make_unique<IdleBehavior>();
    m_behaviors[1] = std::make_unique<WanderingBehavior>();
    m_behaviors[2] = std::make_unique<SearchingBehavior>();
    m_behaviors[3] = std::make_unique<SearchingBehavior>(); // MovingToTarget uses searching
    m_behaviors[4] = std::make_unique<MiningBehavior>();

    m_currentBehavior = m_behaviors[0].get();

    std::cout << "BehaviorManager initialized\n";
}

void BehaviorManager::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent passed to BehaviorManager::update!\n";
        return;
    }

    if (!m_currentBehavior)
    {
        std::cerr << "ERROR: Null behavior in BehaviorManager::update!\n";
        return;
    }

    BehaviorState nextState = m_currentBehavior->update(agent, deltaTime, map);

    if (nextState != m_currentState)
    {
        changeState(nextState);
    }
}

void BehaviorManager::changeState(BehaviorState newState)
{
    if (m_currentBehavior)
    {
        m_currentBehavior->exit(nullptr);
    }

    m_currentState = newState;
    m_currentBehavior = m_behaviors[static_cast<int>(newState)].get();
    m_currentBehavior->enter(nullptr);

    std::cout << "AI State changed to: " << static_cast<int>(newState) << std::endl;
}

//------------------------------------------------------------
// Idle Behavior Implementation
//------------------------------------------------------------
void IdleBehavior::enter(Paleontologist* agent)
{
    m_idleTimer = 0.0f;
    m_idleDuration = 1.0f + static_cast<float>(rand() % 200) / 100.0f; // 1-3 seconds
    std::cout << "AI: Entering Idle state (on surface)\n";
}

BehaviorState IdleBehavior::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    m_idleTimer += deltaTime.asSeconds();

    if (m_idleTimer >= m_idleDuration)
    {
        // After idling, start walking horizontally or search for fossil
        int choice = rand() % 10;
        if (choice < 3) // 30% chance to dig for fossil
        {
            return BehaviorState::SearchingForFossil;
        }
        else // 70% chance to wander horizontally
        {
            return BehaviorState::Wandering;
        }
    }

    return BehaviorState::Idle;
}

void IdleBehavior::exit(Paleontologist* agent)
{
}

//------------------------------------------------------------
// Wandering Behavior Implementation
//------------------------------------------------------------
void WanderingBehavior::enter(Paleontologist* agent)
{
    m_wanderTimer = 0.0f;
    m_wanderDuration = 3.0f + static_cast<float>(rand() % 400) / 100.0f; // 3-7 seconds
    m_hasTarget = false;
    std::cout << "AI: Entering Wandering state (walking horizontally)\n";
}

BehaviorState WanderingBehavior::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent in WanderingBehavior::update!\n";
        return BehaviorState::Idle;
    }

    if (!m_hasTarget)
    {
        pickRandomWalkableTarget(agent, map);
        m_hasTarget = true;
    }

    // Move towards target HORIZONTALLY ONLY (stay at surface)
    sf::Vector2f agentPos = agent->getPosition();

    // Keep Y position at surface level
    float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;

    // Move horizontally
    sf::Vector2f direction = m_targetPosition - agentPos;
    direction.y = 0; // Zero out vertical movement

    float distance = std::abs(direction.x);

    if (distance > 5.0f)
    {
        direction.x = (direction.x > 0) ? 1.0f : -1.0f; // Normalize X only
        agent->move(sf::Vector2f(direction.x * agent->getSpeed() * deltaTime.asSeconds(), 0));

        // Force Y position to stay at surface
        sf::Vector2f newPos = agent->getPosition();
        agent->setPosition(sf::Vector2f(newPos.x, surfaceY));
    }

    m_wanderTimer += deltaTime.asSeconds();

    if (m_wanderTimer >= m_wanderDuration)
    {
        return BehaviorState::Idle;
    }

    return BehaviorState::Wandering;
}

void WanderingBehavior::exit(Paleontologist* agent)
{
}

void WanderingBehavior::pickRandomWalkableTarget(Paleontologist* agent, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent in pickRandomWalkableTarget!\n";
        return;
    }

    sf::Vector2f currentPos = agent->getPosition();
    float tileSize = map.getTileSize();

    // Pick a random horizontal position (within 10-20 tiles left or right)
    int direction = (rand() % 2 == 0) ? 1 : -1;
    int distance = 10 + (rand() % 10);
    int offsetX = direction * distance;

    // Stay at surface Y position
    float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;
    m_targetPosition = sf::Vector2f(currentPos.x + offsetX * tileSize, surfaceY);

    // Keep within reasonable horizontal bounds
    float offsetX_grid = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
    float minX = offsetX_grid + 100.0f;
    float maxX = offsetX_grid + (map.getColumnCount() * tileSize) - 100.0f;

    m_targetPosition.x = std::max(minX, std::min(m_targetPosition.x, maxX));

    std::cout << "AI: Picked wander target at X=" << m_targetPosition.x << "\n";
}

//------------------------------------------------------------
// Searching Behavior Implementation
//------------------------------------------------------------
void SearchingBehavior::enter(Paleontologist* agent)
{
    m_hasPath = false;
    m_currentPathIndex = 0;
    m_path.clear();
    m_returningToSurface = false;

    if (agent)
    {
        m_surfacePosition = agent->getPosition(); // Store current surface position
    }

    std::cout << "AI: Entering Searching state (looking for fossil to dig)\n";
}

BehaviorState SearchingBehavior::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent in SearchingBehavior::update!\n";
        return BehaviorState::Idle;
    }

    if (!m_hasPath && !m_returningToSurface)
    {
        if (!findNearestFossil(agent, map))
        {
            // No fossils found, return to idle
            std::cout << "AI: No fossils found, going back to wandering\n";
            return BehaviorState::Idle;
        }

        // Create a path that digs STRAIGHT DOWN from current position
        sf::Vector2f agentPos = agent->getPosition();
        float tileSize = map.getTileSize();
        float offsetX = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
        float offsetY = WINDOW_Y / 2.0f;

        // Convert agent position to tile coordinates
        int agentCol = static_cast<int>((agentPos.x - offsetX) / tileSize);
        int agentRow = static_cast<int>((agentPos.y - offsetY) / tileSize);

        // Safety check on coordinates
        if (agentRow < 0) agentRow = 0;
        if (agentCol < 0 || agentCol >= map.getColumnCount())
        {
            std::cout << "AI: Agent outside grid bounds, going idle\n";
            return BehaviorState::Idle;
        }

        // Create straight down path to fossil depth
        m_path.clear();
        for (int row = agentRow + 1; row <= m_targetTile.y; ++row)
        {
            m_path.push_back(sf::Vector2i(agentCol, row));
        }

        if (m_path.empty())
        {
            std::cout << "AI: Fossil at surface level or path invalid, going idle\n";
            return BehaviorState::Idle;
        }

        m_hasPath = true;
        m_currentPathIndex = 0;

        std::cout << "AI: Digging down to fossil at depth " << m_targetTile.y << " (path length: " << m_path.size() << ")\n";
    }

    // If returning to surface
    if (m_returningToSurface)
    {
        sf::Vector2f agentPos = agent->getPosition();
        sf::Vector2f direction = m_surfacePosition - agentPos;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f)
        {
            // Back at surface, go idle
            std::cout << "AI: Returned to surface\n";
            return BehaviorState::Idle;
        }

        // Move back up
        direction /= distance;
        agent->move(direction * agent->getSpeed() * 2.0f * deltaTime.asSeconds()); // 2x speed going up
        return BehaviorState::SearchingForFossil;
    }

    // Follow the path downward
    if (m_currentPathIndex < static_cast<int>(m_path.size()))
    {
        sf::Vector2i targetTile = m_path[m_currentPathIndex];
        float tileSize = map.getTileSize();
        float offsetX = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
        float offsetY = WINDOW_Y / 2.0f;

        sf::Vector2f targetPos(
            targetTile.x * tileSize + offsetX + tileSize / 2.0f,
            targetTile.y * tileSize + offsetY + tileSize / 2.0f
        );

        sf::Vector2f agentPos = agent->getPosition();
        sf::Vector2f direction = targetPos - agentPos;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 10.0f)
        {
            m_currentPathIndex++;

            // Mine this tile as we pass through it
            map.removeTile(targetTile.y, targetTile.x);
        }
        else
        {
            direction /= distance;
            agent->move(direction * agent->getSpeed() * deltaTime.asSeconds());
        }
    }
    else
    {
        // Reached the fossil depth, start mining
        agent->setTargetTile(m_targetTile);
        std::cout << "AI: Reached fossil depth, starting mining\n";
        return BehaviorState::Mining;
    }

    return BehaviorState::SearchingForFossil;
}

void SearchingBehavior::exit(Paleontologist* agent)
{
    // When exiting, check if we need to return to surface
    if (!agent) return;

    sf::Vector2f agentPos = agent->getPosition();
    float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;

    if (agentPos.y > surfaceY + 50.0f)
    {
        // Underground, need to return to surface
        m_returningToSurface = true;
    }
}

bool SearchingBehavior::findNearestFossil(Paleontologist* agent, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent in findNearestFossil!\n";
        return false;
    }

    // Search for undiscovered fossils near the agent
    FossilManager& fossilManager = map.getFossilManager();
    sf::Vector2f agentPos = agent->getPosition();
    float tileSize = map.getTileSize();

    float closestDistance = std::numeric_limits<float>::max();
    bool foundFossil = false;

    int rowsToCheck = std::min(map.getRowCount(), 30); // Only check first 30 rows
    int colsToCheck = map.getColumnCount();

    // Scan through the grid looking for fossils (limited search)
    for (int row = 1; row < rowsToCheck; ++row) // Start at row 1 (skip topsoil)
    {
        for (int col = 0; col < colsToCheck; ++col)
        {
            FossilPiece* fossil = fossilManager.getFossilAtTile(row, col);

            if (fossil != nullptr && !fossil->isDiscovered)
            {
                float offsetX = (WINDOW_X - (colsToCheck * tileSize)) / 2.0f;
                float offsetY = WINDOW_Y / 2.0f;

                sf::Vector2f fossilPos(
                    col * tileSize + offsetX,
                    row * tileSize + offsetY
                );

                sf::Vector2f diff = fossilPos - agentPos;
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

                // Prefer fossils that are reasonably deep (row > 5)
                float depthBonus = (row > 5) ? 0.0f : 500.0f; // Penalize shallow fossils

                if (distance + depthBonus < closestDistance)
                {
                    closestDistance = distance + depthBonus;
                    m_targetTile = sf::Vector2i(col, row);
                    foundFossil = true;
                }
            }
        }
    }

    if (foundFossil)
    {
        std::cout << "AI: Found fossil at grid position (" << m_targetTile.x << ", " << m_targetTile.y << ")\n";
    }
    else
    {
        std::cout << "AI: No fossils found in search area\n";
    }

    return foundFossil;
}

std::vector<sf::Vector2i> SearchingBehavior::findPath(sf::Vector2i start, sf::Vector2i goal, Map& map)
{
    // Simple straight-line pathfinding
    // TODO: Implement full A* with obstacle avoidance in the future
    std::vector<sf::Vector2i> path;

    int dx = goal.x - start.x;
    int dy = goal.y - start.y;
    int steps = std::max(std::abs(dx), std::abs(dy));

    if (steps == 0)
    {
        path.push_back(goal);
        return path;
    }

    for (int i = 1; i <= steps; ++i)
    {
        int x = start.x + (dx * i) / steps;
        int y = start.y + (dy * i) / steps;
        path.push_back(sf::Vector2i(x, y));
    }

    return path;
}

float SearchingBehavior::heuristic(sf::Vector2i a, sf::Vector2i b)
{
    // Manhattan distance
    return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

//------------------------------------------------------------
// Mining Behavior Implementation
//------------------------------------------------------------
void MiningBehavior::enter(Paleontologist* agent)
{
    m_miningProgress = 0.0f;

    if (!agent)
    {
        std::cerr << "ERROR: Null agent in MiningBehavior::enter!\n";
        return;
    }

    m_targetTile = agent->getTargetTile();

    // Safety check for valid tile
    if (m_targetTile.x < 0 || m_targetTile.y < 0)
    {
        std::cerr << "ERROR: Invalid target tile in mining behavior!\n";
        return;
    }

    std::cout << "AI: Entering Mining state at tile (" << m_targetTile.x << ", " << m_targetTile.y << ")\n";
}

BehaviorState MiningBehavior::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent in MiningBehavior::update!\n";
        return BehaviorState::Idle;
    }

    // Safety check for valid tile
    if (m_targetTile.x < 0 || m_targetTile.y < 0)
    {
        std::cerr << "ERROR: Mining invalid tile, returning to idle\n";
        return BehaviorState::Idle;
    }

    // Get tile hardness and calculate mining duration
    int tileHardness = map.getTileHardness(m_targetTile.y, m_targetTile.x);

    if (tileHardness == 0)
    {
        std::cout << "AI: Tile already mined or invalid, returning to surface\n";

        // Return to surface
        sf::Vector2f agentPos = agent->getPosition();
        float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;
        agent->setPosition(sf::Vector2f(agentPos.x, surfaceY));

        return BehaviorState::Idle;
    }

    // Base mining time is 0.8 seconds, + 0.4s per hardness level
    m_miningDuration = 0.8f + (tileHardness * 0.4f);

    m_miningProgress += deltaTime.asSeconds();

    if (m_miningProgress >= m_miningDuration)
    {
        // Mining complete - remove the tile
        map.removeTile(m_targetTile.y, m_targetTile.x);
        std::cout << "AI: Mined tile (" << m_targetTile.x << ", " << m_targetTile.y
            << ") with hardness " << tileHardness << "\n";

        // After mining, return to surface
        std::cout << "AI: Finished mining, returning to surface\n";

        // Move agent back to surface quickly
        sf::Vector2f agentPos = agent->getPosition();
        float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;
        agent->setPosition(sf::Vector2f(agentPos.x, surfaceY));

        return BehaviorState::Idle;
    }

    // Visual feedback - update progress bar
    agent->setMiningProgress(m_miningProgress / m_miningDuration);

    return BehaviorState::Mining;
}

void MiningBehavior::exit(Paleontologist* agent)
{
    if (agent)
    {
        agent->setMiningProgress(0.0f);
    }
}