#include "NPC.h"
#include <iostream>
#include <cmath>
#include <random>
#include <queue>
#include <algorithm>

NPC::NPC()
{

	if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
	{
		std::cerr << "failed to load npc sprite \n";
	}
	else
	{
		std::cout << "npc loaded from file \n";
	}

	m_sprite.setTexture(m_texture);
	m_sprite.setTextureRect(sf::IntRect({ 0,0 }, { m_frameWidth,m_frameHeight }));
	m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight));
	m_sprite.setScale(sf::Vector2f(0.12f, 0.12f));
	m_sprite.setColor(sf::Color::Cyan);

	m_sprite.setPosition(sf::Vector2f(WINDOW_X / 2.0f - 150.0f, WINDOW_Y / 2.0f - 40.0f));	

}

void NPC::updateNPC(sf::Time dt, Map& map)
{
	if (m_root)
	{
		m_root->tick(dt.asSeconds());
	}
	updateNPCAnimation(dt);
}

void NPC::drawNPC(sf::RenderWindow& window, Map& map)
{
	drawPath(window, map);
	window.draw(m_sprite);

}

void NPC::updateReturn(sf::Time dt, Map& map)
{
	if (m_returnIndex >= m_returnPath.size()) {
		m_state = NPCState::WANDERTHESURFACE;
		m_returningToSurface = false;
		m_returnPath.clear();
		m_returnIndex = 0;
		return;
	}

	sf::Vector2i targetTile = m_returnPath[m_returnIndex];
	sf::Vector2f targetPos = tileToWorld(targetTile, map);
	sf::Vector2f pos = m_sprite.getPosition();
	sf::Vector2f dir = targetPos - pos;

	float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

	if (dist < 4.f) {
		m_returnIndex++;
		return;
	}

	dir /= dist;
	m_velocity = dir * m_moveSpeed;
	m_sprite.move(m_velocity * dt.asSeconds());
	m_facingRight = (dir.x >= 0);
}

void NPC::updateMining(sf::Time dt, Map& map)
{
	//normal mining
	if (m_miningIndex >= m_miningPath.size())
	{
		m_returningToSurface = true;
		return;
	}

	sf::Vector2i targetTile = m_miningPath[m_miningIndex];
	sf::Vector2f targetPos = tileToWorld(targetTile, map);
	sf::Vector2f pos = m_sprite.getPosition();
	sf::Vector2f dir = targetPos - pos;

	float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

	// If tile is still solid damage it over time
	if (map.getTileHardness(targetTile.y, targetTile.x) > 0)
	{
		npcMiningDamageCooldown -= dt.asSeconds();
		if (npcMiningDamageCooldown <= 0.f)
		{
			mineTile(map, targetTile);
			npcMiningDamageCooldown = npcMiningTickDelay;
		}
		return; // stay until tile breaks
	}

	// Tile is broken move toward it
	if (dist < 4.f)
	{
		m_miningIndex++;
		return;
	}

	dir /= dist;
	m_velocity = dir * m_moveSpeed;
	m_sprite.move(m_velocity * dt.asSeconds());
	m_facingRight = (dir.x >= 0);
}	

void NPC::mineTile(Map& map, sf::Vector2i tile)
{
	if (tile.y < 0 || tile.x < 0 || tile.y >= map.getRowCount() || tile.x >= map.getColumnCount())
	{
		return;
	}

	if (map.getTileHardness(tile.y, tile.x) > 0)
	{
		map.damageTile(tile.y, tile.x, m_npcMiningDamage);
	}
}

void NPC::generateMiningPath(Map& map)
{
	m_miningPath.clear();	// remove old path 
	m_returnPath.clear();	

	m_miningIndex = 0;	// start on index 0
	m_returnIndex = 0;

	m_returningToSurface = false;

	sf::Vector2i start = worldToTile(m_sprite.getPosition(), map);	// convert npc world coords to grid coords
	m_miningStartTile = start;

	std::vector<sf::Vector2i> stack;
	stack.push_back(start);	// push start pos onto S/Q

	std::vector<std::vector<bool>> visited(map.getRowCount(), std::vector<bool>(map.getColumnCount(), false));	// array matching map size, tells me whether tile was visited

	auto inBounds = [&](sf::Vector2i t)		// returns true if t is inside map 
		{
			return t.x >= 0 && t.x < map.getColumnCount() && t.y >= 0 && t.y < map.getRowCount();
		};

	auto neighbours = [&](sf::Vector2i t)	// returns a tiles 4 neighbors (R,L.D,U)
		{
			return std::vector<sf::Vector2i>{
				{t.x + 1, t.y},
				{ t.x - 1, t.y },
				{ t.x, t.y + 1 },
				{ t.x, t.y - 1 }};
		};
	
	while (!stack.empty())
	{
		int maxDepth = 20 + m_miningDepthLevel * 10; // max depth of search, can be upgraded

		if (m_miningPath.size() >= maxDepth) 
		{
			break;
		}

		sf::Vector2i currentTile;

		if (m_useDFS) // dfs, treat as stack (lst in frst out)
		{
			currentTile = stack.back();
			stack.pop_back();
			//std::cout << "dfs used \n"; 
		}

		if (!inBounds(currentTile)) continue;		// not ibounds = skip

		if (visited[currentTile.y][currentTile.x]) continue;	// already processed = skip 

		visited[currentTile.y][currentTile.x] = true;	// if processed mark visited

		m_miningPath.push_back(currentTile);	// Add tile to path

		//map.colourTile(currentTile.y, currentTile.x, sf::Color::Red);	// Colour tile red

		auto neigh = neighbours(currentTile);
		std::shuffle(neigh.begin(), neigh.end(), std::mt19937(std::random_device{}()));	// sshuffe to make pathing feel more organic 

		for (auto& n : neigh)	// Explore neighbours
		{
			if (inBounds(n) && !visited[n.y][n.x])
				stack.push_back(n);
		}
	}

	std::cout << "NPC mining path generated: " << m_miningPath.size() << " tiles\n";
}

void NPC::generateReturnPath(Map& map)
{
	m_returnPath.clear();
	m_returnIndex = 0;

	sf::Vector2i start = worldToTile(m_sprite.getPosition(), map);
	sf::Vector2i goal = m_miningStartTile;

	int rows = map.getRowCount();
	int cols = map.getColumnCount();

	auto inBounds = [&](sf::Vector2i t)		// check if in bounds
		{
			return t.x >= 0 && t.x < map.getColumnCount() && t.y >= 0 && t.y < map.getRowCount();
		};


	auto isWalkable = [&](sf::Vector2i t)	// only walk on already broken tiles
		{
			if (!inBounds(t))
			{
				return false;
			}

			return map.getTileHardness(t.y, t.x) <= 0;
		};


	auto neighbours = [&](sf::Vector2i t)		// return neighbours in 4 directions
		{
			return std::vector<sf::Vector2i>{
				{t.x + 1, t.y},
				{ t.x - 1, t.y },
				{ t.x, t.y + 1 },
				{ t.x, t.y - 1 }};
		};

	std::queue<sf::Vector2i> queue;
	std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
	std::vector<std::vector<sf::Vector2i>> parent(rows, std::vector<sf::Vector2i>(cols, sf::Vector2i(-1, -1)));

	if (!inBounds(start) || !inBounds(goal))
	{
		return;
	}

	queue.push(start);
	visited[start.y][start.x] = true;
	bool found = false;

	while (!queue.empty())
	{
		sf::Vector2i current = queue.front();
		queue.pop();

		if (current == goal)
		{
			found = true;
			break;
		}

		for (sf::Vector2i next : neighbours(current))
		{
			if (!isWalkable(next))
			{
				continue;
			}

			if (visited[next.y][next.x])
			{
				continue;
			}

			visited[next.y][next.x] = true;
			parent[next.y][next.x] = current;
			queue.push(next);
		}
	}

	if (!found)
	{
		std::cout << "npc cant find return path \n";
		return;
	}

	sf::Vector2i current = goal;

	while (current != start)
	{
		m_returnPath.push_back(current);
		current = parent[current.y][current.x];

	}

		std::reverse(m_returnPath.begin(), m_returnPath.end());

		std::cout << "return path generated \n";
	
}

void NPC::generateFossilPath(Map& map, sf::Vector2i goal)
{
	m_fossilPath.clear();
	m_fossilIndex = 0;

	sf::Vector2i start = worldToTile(m_sprite.getPosition(), map);

	auto inBounds = [&](sf::Vector2i t)		// check if in bounds
		{
			return t.x >= 0 && t.x < map.getColumnCount() &&
				   t.y >= 0 && t.y < map.getRowCount();
		};

	auto isWalkable = [&](sf::Vector2i t)	// only walk on already broken tiles
		{
			if (!inBounds(t))
			{
				return false;
			}
			return map.getTileHardness(t.y, t.x) <= 0;
		};

	auto heuristic = [&](sf::Vector2i a, sf::Vector2i b)	// Manhattan distance heuristic  
		{
			return std::abs(a.x - b.x) + std::abs(a.y - b.y);
		};


	struct Node
	{
		sf::Vector2i pos;
		int gCost;
		int hCost;
		sf::Vector2i parent;
	};


	struct Vector2iCompare
	{
		bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const
		{
			if (a.y == b.y) return a.x < b.x;
			return a.y < b.y;
		}
	};

	struct NodeCompare
	{
		bool operator()(const std::pair<int, Node>& a, const std::pair<int, Node>& b) const
		{
			return a.first > b.first; // min-heap based on f score
		}
	};

	std::priority_queue<std::pair<int, Node>, std::vector<std::pair<int, Node>>, NodeCompare>open;

	// tile position comparison lambda for map 
	std::map<sf::Vector2i, Node, Vector2iCompare> allNodes;

	open.push({ heuristic(start, goal), { start, 0, heuristic(start, goal), sf::Vector2i(-1, -1) } });

	allNodes[start] = { start, 0, heuristic(start, goal), sf::Vector2i(-1, -1) };

	bool found = false;

	while (!open.empty())
	{
		auto [f, current] = open.top();

		open.pop();

		if(current.pos == goal)
		{
			found = true;
			break;
		}

		std::vector<sf::Vector2i> neighbours = {
			{current.pos.x + 1, current.pos.y},
			{current.pos.x - 1, current.pos.y},
			{current.pos.x, current.pos.y + 1},
			{current.pos.x, current.pos.y - 1}
		};

		for (auto& n : neighbours)
		{
			if (!isWalkable(n))
			{
				continue; 
			}

			int g = allNodes[current.pos].gCost + 1;
			int h = heuristic(n, goal);
			int f = g + h;

			if(!allNodes.count(n)|| g<allNodes[n].gCost)
			{
				allNodes[n] = { n, g, h, current.pos };
				open.push({ f, allNodes[n] });
			}
		}
	}

	if (!found)
	{
		std::cout << "npc cant find fossil path \n";
		generateReturnPath(map); // gen return path if fossil path not found
		updateReturn(sf::seconds(0), map); // immediately start returning
		return;
	}

	// Reconstruct path

	sf::Vector2i current = goal;
	
	while (current != start)
	{
		m_fossilPath.push_back(current);
		current = allNodes[current].parent;
	}

	std::reverse(m_fossilPath.begin(), m_fossilPath.end());

	std::cout << "fossil path generated: " << m_fossilPath.size() << " tiles " << std::endl;
}

void NPC::updateFossilPath(sf::Time dt, Map& map)
{
	if (m_fossilIndex >= m_fossilPath.size())
	{
		// Finished fossil path
		m_fossilPath.clear();
		m_fossilIndex = 0;
		return;
	}

	sf::Vector2i targetTile = m_fossilPath[m_fossilIndex];
	sf::Vector2f targetPos = tileToWorld(targetTile, map);
	sf::Vector2f pos = m_sprite.getPosition();
	sf::Vector2f dir = targetPos - pos;

	float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

	if (dist < 4.f)
	{
		m_fossilIndex++;

		if (m_fossilIndex >= m_fossilPath.size())
		{
			m_fossilPath.clear();
			m_fossilIndex = 0;
		}

		return;
	}

	dir /= dist;
	m_velocity = dir * m_moveSpeed;
	m_sprite.move(m_velocity * dt.asSeconds());
	m_facingRight = (dir.x >= 0);

	std::cout << "NPC moving towards fossil tile " << targetTile.x << "," << targetTile.y << " index " << m_fossilIndex << "/" << m_fossilPath.size() << std::endl;
}

void NPC::updateSurfaceWandering(sf::Time dt, Map& map)
{

	float tileSize = map.getTileSize();
	int cols = map.getColumnCount();
	int rows = map.getRowCount();

	// same offset logic as Player
	float offsetX = (WINDOW_X - cols * tileSize) / 2.0f;
	float offsetY = WINDOW_Y / 2.0f;

	sf::Vector2f pos = m_sprite.getPosition();

	// horizontal wandering: move left/right
	if (m_facingRight)
	{
		m_velocity.x = m_moveSpeed;
	}
	else
	{
		m_velocity.x = -m_moveSpeed;
	}


	pos += m_velocity * dt.asSeconds();

	// clamp to surface horizontally
	const float halfWidth = tileSize * 0.35f;
	float minX = offsetX + halfWidth;
	float maxX = offsetX + cols * tileSize - halfWidth;

	if (pos.x < minX)
	{
		pos.x = minX;
		m_facingRight = true;
	}
	else if (pos.x > maxX)
	{
		pos.x = maxX;
		m_facingRight = false;
	}

	float surfaceY = offsetY + tileSize * 0.0f; // row 0
	pos.y = surfaceY;

	m_sprite.setPosition(pos);
}

void NPC::drawPath(sf::RenderWindow& window, Map& map)
{
	if (m_fossilPath.empty())
	{
		return;
	}

	float tileSize = map.getTileSize();
	sf::RectangleShape tileOverlay(sf::Vector2f(tileSize, tileSize));
	tileOverlay.setFillColor(sf::Color(255, 255, 0, 100)); // transparent-ish yellow

	for (size_t i = m_fossilIndex; i < m_fossilPath.size(); ++i)
	{
		sf::Vector2f centerPos = tileToWorld(m_fossilPath[i], map);

		// align shape with top left of tile
		tileOverlay.setPosition(sf::Vector2f(centerPos.x - tileSize / 2.0f, centerPos.y - tileSize / 2.0f));
		window.draw(tileOverlay);
	}
}

void NPC::updateNPCAnimation(sf::Time dt)
{

	m_animationTimer += dt.asSeconds();

	if (m_animationTimer >= m_frameTime)
	{
		m_animationTimer -= m_frameTime;
		m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
		setNPCFrames(m_currentFrame);
	}

}

void NPC::setNPCFrames(int frame)
{
	if (frame < 0 || frame >= m_totalFrames)
	{
		frame = 0;
	}

	m_currentFrame = frame;

	if (m_texture.getSize().x == 0 || m_texture.getSize().y == 0)
	{
		return;
	}

	int xOffset = frame * m_frameWidth;
	m_sprite.setTextureRect(sf::IntRect({ xOffset, 0 }, { m_frameWidth, m_frameHeight }));

	sf::Vector2f scale = m_sprite.getScale();
	scale.x = m_facingRight ? std::abs(scale.x) : -std::abs(scale.x);
	m_sprite.setScale(scale);

}

sf::Vector2i NPC::worldToTile(sf::Vector2f pos, Map& map)
{
	float tileSize = map.getTileSize();
	float offsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
	float offsetY = WINDOW_Y / 2.0f;

	int col = static_cast<int>((pos.x - offsetX) / tileSize);
	int row = static_cast<int>((pos.y - offsetY) / tileSize);

	return sf::Vector2i(col, row);
}

sf::Vector2f NPC::tileToWorld(sf::Vector2i tile, Map& map)
{
	float tileSize = map.getTileSize();
	float offsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
	float offsetY = WINDOW_Y / 2.0f;

	float x = tile.x * tileSize + offsetX + tileSize / 2.f;
	float y = tile.y * tileSize + offsetY + tileSize / 2.f;

	return sf::Vector2f(x, y);
}