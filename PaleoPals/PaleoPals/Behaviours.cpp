#include "Behaviours.h"
#include "Paleontologist.h"
#include "Map.h"
#include "Pathfinding.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>

BehaviorManager::BehaviorManager() : m_currentState(BehaviorState::Idle)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_behaviors[0] = std::make_unique<IdleBehavior>();
    m_behaviors[1] = std::make_unique<WanderingBehavior>();
    m_behaviors[2] = std::make_unique<SearchingBehavior>();
    m_behaviors[3] = std::make_unique<SearchingBehavior>(); // optional MovingToTarget
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
        changeState(nextState, agent);
    }
}

void BehaviorManager::changeState(BehaviorState newState, Paleontologist* agent)
{
    if (m_currentBehavior)
    {
        m_currentBehavior->exit(agent);
    }

    m_currentState = newState;
    m_currentBehavior = m_behaviors[static_cast<int>(newState)].get();
    m_currentBehavior->enter(agent);

}


void IdleBehavior::enter(Paleontologist* agent)
{
    m_idleTimer = 0.0f;
    m_idleDuration = 1.0f + static_cast<float>(rand() % 200) / 100.0f; // 1–3 seconds
}

BehaviorState IdleBehavior::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    m_idleTimer += deltaTime.asSeconds();

    if (m_idleTimer >= m_idleDuration)
    {
        int choice = rand() % 10;
        if (choice < 5)
        {
            return BehaviorState::SearchingForFossil;
        }
        else
        {
            return BehaviorState::Wandering;
        }
    }

    return BehaviorState::Idle;
}

void IdleBehavior::exit(Paleontologist* agent)
{
}


void WanderingBehavior::enter(Paleontologist* agent)
{
    m_wanderTimer = 0.0f;
    m_wanderDuration = 3.0f + static_cast<float>(rand() % 400) / 100.0f; // 3–7 seconds
    m_hasTarget = false;
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

    sf::Vector2f agentPos = agent->getPosition();
    float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;

    sf::Vector2f direction = m_targetPosition - agentPos;
    direction.y = 0;

    float distance = std::abs(direction.x);

    if (distance > 5.0f)
    {
        direction.x = (direction.x > 0) ? 1.0f : -1.0f;
        agent->move(sf::Vector2f(direction.x * agent->getSpeed() * deltaTime.asSeconds(), 0));

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

    int direction = (rand() % 2 == 0) ? 1 : -1;
    int distance = 10 + (rand() % 10);
    int offsetX = direction * distance;

    float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;
    m_targetPosition = sf::Vector2f(currentPos.x + offsetX * tileSize, surfaceY);

    float offsetX_grid = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
    float minX = offsetX_grid + 100.0f;
    float maxX = offsetX_grid + (map.getColumnCount() * tileSize) - 100.0f;

    m_targetPosition.x = std::max(minX, std::min(m_targetPosition.x, maxX));
}


void SearchingBehavior::enter(Paleontologist* agent)
{
    m_hasPath = false;
    m_currentPathIndex = 0;
    m_path.clear();
}

BehaviorState SearchingBehavior::update(Paleontologist* agent, sf::Time dt, Map& map)
{
    if (!agent) return BehaviorState::Idle;

    float tileSize = map.getTileSize();
    sf::Vector2f pos = agent->getPosition();

    float offsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
    float offsetY = WINDOW_Y / 2.0f;

    int col = static_cast<int>((pos.x - offsetX) / tileSize);
    int row = static_cast<int>((pos.y - offsetY) / tileSize);

    // If no path, choose a random tile to dig toward
    if (!m_hasPath)
    {
        int targetRow = row + (rand() % 10 + 5);   // 5–15 tiles down
        int targetCol = col + (rand() % 7 - 3);    // ±3 tiles sideways

        targetRow = std::clamp(targetRow, 0, map.getRowCount() - 1);
        targetCol = std::clamp(targetCol, 0, map.getColumnCount() - 1);

        m_targetTile = sf::Vector2i(targetCol, targetRow);

        GridPos start{ row, col };
        GridPos goal{ targetRow, targetCol };

        auto gridPath = AStarPathfinder::findPath(map, start, goal);

        m_path.clear();
        for (auto& p : gridPath)
            m_path.emplace_back(p.col, p.row);

        if (m_path.empty())
            return BehaviorState::Idle;

        m_currentPathIndex = 0;
        m_hasPath = true;
    }

    // Follow A* path
    if (m_currentPathIndex < m_path.size())
    {
        sf::Vector2i nextTile = m_path[m_currentPathIndex];
        sf::Vector2f nextWorld = map.tileToWorld(nextTile);

        sf::Vector2f dir = nextWorld - pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        // If close enough, go to next tile
        if (dist < 20.0f)
        {
            m_currentPathIndex++;
            return BehaviorState::SearchingForFossil;
        }

        // If tile is solid → mine it
        int hardness = map.getTileHardness(nextTile.y, nextTile.x);
        if (hardness > 0)
        {
            map.damageTile(nextTile.y, nextTile.x, agent->getMiningDamage());
            return BehaviorState::SearchingForFossil;
        }

        // Move toward tile
        dir /= dist;
        agent->move(dir * agent->getSpeed() * dt.asSeconds());
        return BehaviorState::SearchingForFossil;
    }

    // Path finished → choose a new one
    m_hasPath = false;
    return BehaviorState::SearchingForFossil;
}



void SearchingBehavior::exit(Paleontologist* agent)
{
}

bool SearchingBehavior::findNearestFossil(Paleontologist* agent, Map& map)
{
    auto& items = map.getFossilManager().getAllCollectibles();
    sf::Vector2f agentPos = agent->getPosition();

    float closest = std::numeric_limits<float>::max();
    bool found = false;

    for (auto& c : items)
    {
        if (c.isPickedUp) continue;
        if (c.collectibleIndex > 6) continue; // only fossils 0–6

        sf::Vector2f pos = c.sprite.getPosition();
        sf::Vector2f diff = pos - agentPos;
        float dist = diff.x * diff.x + diff.y * diff.y;

        if (dist < closest)
        {
            closest = dist;

            // Convert world → tile
            float tileSize = map.getTileSize();
            float offsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
            float offsetY = WINDOW_Y / 2.0f;

            int col = static_cast<int>((pos.x - offsetX) / tileSize);
            int row = static_cast<int>((pos.y - offsetY) / tileSize);

            m_targetTile = sf::Vector2i(col, row);
            found = true;
        }
    }

    return found;
}



std::vector<sf::Vector2i> SearchingBehavior::findPath(sf::Vector2i start, sf::Vector2i goal, Map& map)
{
    GridPos s{ start.y, start.x }; // row = y, col = x
    GridPos g{ goal.y, goal.x };

    auto gridPath = AStarPathfinder::findPath(map, s, g);

    std::vector<sf::Vector2i> result;
    result.reserve(gridPath.size());
    for (const auto& p : gridPath)
    {
        result.emplace_back(p.col, p.row);
    }
    return result;
}

float SearchingBehavior::heuristic(sf::Vector2i a, sf::Vector2i b)
{
    return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}


void MiningBehavior::enter(Paleontologist* agent)
{
    m_miningProgress = 0.0f;

    if (!agent)
    {
        std::cerr << "ERROR: Null agent in MiningBehavior::enter!\n";
        return;
    }

    m_targetTile = agent->getTargetTile();

    if (m_targetTile.x < 0 || m_targetTile.y < 0)
    {
        std::cerr << "ERROR: Invalid target tile in mining behavior!\n";
        return;
    }
}

BehaviorState MiningBehavior::update(Paleontologist* agent, sf::Time deltaTime, Map& map)
{
    if (!agent)
    {
        std::cerr << "ERROR: Null agent in MiningBehavior::update!\n";
        return BehaviorState::Idle;
    }

    if (m_returningToSurface)
    {
        sf::Vector2f agentPos = agent->getPosition();
        float surfaceY = (WINDOW_Y / 2.0f) - 20.0f;
        sf::Vector2f targetPos(agentPos.x, surfaceY);

        sf::Vector2f direction = targetPos - agentPos;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f)
        {
            agent->setPosition(sf::Vector2f(agentPos.x, surfaceY));
            m_returningToSurface = false;
            agent->setMiningProgress(0.0f);
            return BehaviorState::Idle;
        }

        direction /= distance;
        agent->move(direction * agent->getSpeed() * 2.0f * deltaTime.asSeconds());
        return BehaviorState::Mining;
    }

    if (m_targetTile.x < 0 || m_targetTile.y < 0)
    {
        return BehaviorState::Idle;
    }

    int tileHardness = map.getTileHardness(m_targetTile.y, m_targetTile.x);

    if (tileHardness == 0)
    {
        m_returningToSurface = true;
        return BehaviorState::Mining;
    }

    m_miningDuration = 0.8f + (tileHardness * 0.4f);

    m_miningProgress += deltaTime.asSeconds();

    if (m_miningProgress >= m_miningDuration)
    {
        map.removeTile(m_targetTile.y, m_targetTile.x);
        std::cout << "AI: Mined tile (" << m_targetTile.x << ", " << m_targetTile.y
            << ") with hardness " << tileHardness << "\n";

        m_miningProgress = 0.0f;
        m_returningToSurface = true;
        agent->setMiningProgress(0.0f);

        return BehaviorState::Mining;
    }

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
