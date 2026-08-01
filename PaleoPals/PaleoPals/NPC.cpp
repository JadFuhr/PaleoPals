#include "NPC.h"
#include <iostream>
#include <cmath>
#include <random>

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
	switch (m_state)
	{
	case NPCState::WANDERTHESURFACE:
		updateSurfaceWandering(dt, map);
		
		if (std::abs(m_sprite.getPosition().x - WINDOW_X / 2.f) < 10.f) // for now triggering mining when NPC reaches center
		{
			generateMiningPath(map);
			m_state = NPCState::MINING;
		}
		break;
	case NPCState::MINING:
		updateMining(dt, map);
		break;
	case NPCState::SEARCHING:
		break;
	}
	updateNPCAnimation(dt);


}

void NPC::drawNPC(sf::RenderWindow& window)
{

	window.draw(m_sprite);

}

void NPC::updateMining(sf::Time dt, Map& map)
{
	if (m_miningPath.empty())
	{
		// Mining complete return to surface
		m_state = NPCState::WANDERTHESURFACE;
		return;
	}

	if (m_miningIndex >= (int)m_miningPath.size())
	{
		m_state = NPCState::WANDERTHESURFACE;
		return;
	}

	if (m_miningIndex < 0 || m_miningIndex >= m_miningPath.size())
	{
		m_state = NPCState::WANDERTHESURFACE;
		return;
	}

	sf::Vector2i targetTile = m_miningPath[m_miningIndex];
	sf::Vector2f targetPos = tileToWorld(targetTile, map);

	sf::Vector2f pos = m_sprite.getPosition();
	sf::Vector2f dir = targetPos - pos;

	float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

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

void NPC::generateMiningPath(Map& map)
{
	m_miningPath.clear();	// remove old path 
	m_miningIndex = 0;	// start on index 0

	sf::Vector2i start = worldToTile(m_sprite.getPosition(), map);	// convert npc world coords to grid coords

	std::vector<sf::Vector2i> stackAndQueue;

	stackAndQueue.push_back(start);	// push start pos onto S/Q

	std::vector<std::vector<bool>> visited(map.getRowCount(), std::vector<bool>(map.getColumnCount(), false));	// array matching map size, tells me whether tile was visited

	auto inBounds = [&](sf::Vector2i t)		// returns true if t is inside map 
		{
			return t.x >= 0 && t.x < map.getColumnCount() && t.y >= 0 && t.y < map.getRowCount();
		};

	auto neighbours = [&](sf::Vector2i t)	// returns a tiles 4 neighbors (R,L.D,U)
		{
			return std::vector<sf::Vector2i>{{t.x + 1, t.y}, { t.x - 1, t.y }, { t.x, t.y + 1 }, { t.x, t.y - 1 }};
		};

	while (!stackAndQueue.empty())
	{
		if (m_miningPath.size() >= 20) // depth of search is 20
		{
			break;
		}

		sf::Vector2i currentTile;

		if (m_useDFS) // dfs, treat as stack (lst in frst out)
		{
			currentTile = stackAndQueue.back();
			stackAndQueue.pop_back();
			std::cout << "dfs used \n"; 
		}
		else	// bfs treat as queue (frst in frst out)
		{
			currentTile = stackAndQueue.front();
			stackAndQueue.erase(stackAndQueue.begin());
			std::cout << "bfs used \n"; 
		}

		if (!inBounds(currentTile)) continue;		// not ibounds = skip

		if (visited[currentTile.y][currentTile.x]) continue;	// already processed = skip 

		visited[currentTile.y][currentTile.x] = true;	// if processed mark visited

		m_miningPath.push_back(currentTile);	// Add tile to path

		map.colourTile(currentTile.y, currentTile.x, sf::Color::Red);	// Colour tile red

		auto neigh = neighbours(currentTile);
		std::shuffle(neigh.begin(), neigh.end(), std::mt19937(std::random_device{}()));	// sshuffe to make pathing feel more organic 

		for (auto& n : neigh)	// Explore neighbours
		{
			if (inBounds(n) && !visited[n.y][n.x])
				stackAndQueue.push_back(n);
		}
	}

	std::cout << "NPC mining path generated: " << m_miningPath.size() << " tiles\n";
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