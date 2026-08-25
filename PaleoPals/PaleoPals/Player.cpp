#include "Player.h"
#include "Map.h"
#include "Fossil.h"
#include <iostream>
#include <cmath>

Player::Player()
{
    std::cout << "Player constructor START\n";

    if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
    {
        std::cerr << "Failed to load player texture!\n";

    }

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { m_frameWidth, m_frameHeight }));
    m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight));
    m_sprite.setScale(sf::Vector2f(0.2f, 0.2f));
    m_sprite.setPosition(sf::Vector2f(400.0f, 300.0f));

    std::cout << "Player constructor END\n";
}

Player::~Player()
{
}

void Player::update(sf::Time deltaTime, Map& map, const sf::RenderWindow& window, const sf::View& cameraView)
{
    handleInput(deltaTime, map);
    applyPhysics(deltaTime, map);
    updateAnimation(deltaTime);

    bool mouseHeld = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    m_isMining = mouseHeld;
    if (m_isMining)
    {
        updateMiningRay(deltaTime, map, window, cameraView);
    }

    if (!mouseHeld)
    {
		m_isMining = false;
    }
}

void Player::handleInput(sf::Time deltaTime, Map& map)
{

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        m_velocity.x = -m_moveSpeed;
        m_facingRight = false;
        m_state = PlayerState::Walking;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        m_velocity.x = m_moveSpeed;
        m_facingRight = true;
        m_state = PlayerState::Walking;
    }
    else
    {
        m_velocity.x = 0;
        if (m_isGrounded)
        {
            m_state = PlayerState::Idle;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && m_canJump && m_isGrounded)
    {
		m_velocity.y = getJumpForce();
        m_canJump = false;
        m_state = PlayerState::Jumping;
    }

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        m_canJump = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
    {
        tryPickupCollectible(map);
    }
}

void Player::applyPhysics(sf::Time deltaTime, Map& map)
{
    m_velocity.y += m_gravity * deltaTime.asSeconds();

    if (m_velocity.y > 600.0f)
    {
        m_velocity.y = 600.0f;
    }

    m_sprite.move(m_velocity * deltaTime.asSeconds());

    checkCollisions(map);

    if (!m_isGrounded)
    {
        if (m_velocity.y < 0)
        {
            m_state = PlayerState::Jumping;
        }
        else
        {
            m_state = PlayerState::Falling;
        }
    }


    {
        float tileSize = map.getTileSize();
        int cols = map.getColumnCount();
        int rows = map.getRowCount();

        
        float offsetX = (WINDOW_X - cols * tileSize) / 2.0f;
        float offsetY = WINDOW_Y / 2.0f;

        const float halfW = tileSize * 0.35f; // same as collision width
        float minX = offsetX + halfW;
        float maxX = offsetX + cols * tileSize - halfW;

        sf::Vector2f pos = m_sprite.getPosition();

        if (pos.x < minX) pos.x = minX;
        if (pos.x > maxX) pos.x = maxX;

        float maxY = offsetY + rows * tileSize - 1.0f;

        if (pos.y > maxY)
        {
            pos.y = maxY;
            m_velocity.y = 0;
            m_isGrounded = true;
        }

        m_sprite.setPosition(pos);
    }
}

void Player::checkCollisions(Map& map)
{
    float tileSize = map.getTileSize();

    const float halfW = tileSize * 0.35f;   
    const float playerHeight = tileSize * 1.6f; 

    m_isGrounded = false;


    float tileOffsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
    float tileOffsetY = WINDOW_Y / 2.0f;

    auto toTileCol = [&](float worldX) -> int {
        return static_cast<int>(std::floor((worldX - tileOffsetX) / tileSize));
        };

    auto toTileRow = [&](float worldY) -> int {
        return static_cast<int>(std::floor((worldY - tileOffsetY) / tileSize));
        };

    auto tileLeft = [&](int col) -> float {
        return col * tileSize + tileOffsetX;
        };

    auto tileTop = [&](int row) -> float {
        return row * tileSize + tileOffsetY;
        };

    auto inBounds = [&](int row, int col) -> bool {
        return row >= 0 && row < map.getRowCount() &&
            col >= 0 && col < map.getColumnCount();
        };
    auto solid = [&](int row, int col) -> bool {
        return inBounds(row, col) && map.getTileHardness(row, col) > 0;
        };

    sf::Vector2f pos = m_sprite.getPosition(); 


    if (m_velocity.y >= 0)
    {
        float feetY = pos.y;                    
        int   footRow = toTileRow(feetY);        

        int colL = toTileCol(pos.x - halfW + 1.f);
        int colR = toTileCol(pos.x + halfW - 1.f);

        if (solid(footRow, colL) || solid(footRow, colR))
        {
            float surfaceY = tileTop(footRow); 

            if (feetY >= surfaceY - 2.0f)
            {
                m_sprite.setPosition(sf::Vector2f(pos.x, surfaceY));
                m_velocity.y = 0;
                m_isGrounded = true;
                pos = m_sprite.getPosition();
            }
        }
    }
    else
    {
       
        float headY = pos.y - playerHeight;
        int   headRow = toTileRow(headY);

        int colL = toTileCol(pos.x - halfW + 1.f);
        int colR = toTileCol(pos.x + halfW - 1.f);

        if (solid(headRow, colL) || solid(headRow, colR))
        {
            float ceilBottom = tileTop(headRow) + tileSize;
            m_sprite.setPosition(sf::Vector2f(pos.x, ceilBottom + playerHeight));
            m_velocity.y = 0;
            pos = m_sprite.getPosition();
        }
    }


    if (m_velocity.x != 0)
    {
        float sampleYs[3] = {
            pos.y - playerHeight + 2.f,   
            pos.y - playerHeight * 0.5f,  
            pos.y - 2.f                   
        };

        if (m_velocity.x < 0)
        {
           
            float leftEdge = pos.x - halfW;
            int leftCol = toTileCol(leftEdge);

            bool hit = false;
            for (auto sampleY : sampleYs)
            {
                int row = toTileRow(sampleY);
                if (solid(row, leftCol)) { hit = true; break; }
            }
            if (hit)
            {
                m_velocity.x = 0;
                float newX = tileLeft(leftCol) + tileSize + halfW;
                m_sprite.setPosition(sf::Vector2f(newX, pos.y));
            }
        }
        else
        {
            float rightEdge = pos.x + halfW;
            int rightCol = toTileCol(rightEdge);

            bool hit = false;
            for (auto sampleY : sampleYs)
            {
                int row = toTileRow(sampleY);
                if (solid(row, rightCol)) { hit = true; break; }
            }
            if (hit)
            {
                m_velocity.x = 0;
                float newX = tileLeft(rightCol) - halfW;
                m_sprite.setPosition(sf::Vector2f(newX, pos.y));
            }
        }
    }
}

void Player::tryPickupCollectible(Map& map)
{
    sf::Vector2f playerPos = m_sprite.getPosition();
    FossilManager& fossilManager = map.getFossilManager();
    float tileSize = map.getTileSize();
    float tileOffsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
    float tileOffsetY = WINDOW_Y / 2.0f;

    
    sf::Vector2f bodyCentre = playerPos - sf::Vector2f(0.f, tileSize * 0.8f);
    int playerCol = static_cast<int>(std::floor((bodyCentre.x - tileOffsetX) / tileSize));
    int playerRow = static_cast<int>(std::floor((bodyCentre.y - tileOffsetY) / tileSize));

	auto& allCollectibles = fossilManager.getAllCollectibles();
    
    for (auto& c : allCollectibles)
    {

        if (c.isPickedUp) continue;

        sf::Vector2f collectiblePos = c.sprite.getPosition();
        sf::Vector2f diff = collectiblePos - bodyCentre;
        float distSq = diff.x * diff.x + diff.y * diff.y;
        float pickupRadius = getPickupRadius();

        if (distSq > pickupRadius * pickupRadius)
            continue;

        CollectedItem item;
        item.collectibleIndex = c.collectibleIndex;
        item.monetaryValue = c.monetaryValue;

        if (c.collectibleIndex <= 6)
        {
            item.type = "fossil";
            item.dinosaurName = c.assignedDinosaurName;
            item.pieceId = c.assignedPieceId;
            item.category = c.assignedCategory;
            item.name = c.assignedPieceId + " of " + c.assignedDinosaurName;
			m_money += c.monetaryValue;
        }
        else if (c.collectibleIndex <= 8)
        {
            item.type = "amber";
            item.name = (c.monetaryValue == 75) ? "Small Amber" : "Large Amber";
            m_money += c.monetaryValue;
        }
        else
        {
            item.type   = "trash";
            item.name = "Trash";
			m_money += c.monetaryValue;
        }

        m_inventory.push_back(item);
        m_newPickups.push_back(item);
        
        c.isPickedUp = true;
        c.sprite.setPosition(sf::Vector2f(-10000.f, -10000.f));

        std::cout << "[Pickup] " << item.name << " (type: " << item.type << ")" << " | Inventory size: " << m_inventory.size() << "\n";

        return; 
    }
}

void Player::updateAnimation(sf::Time deltaTime)
{
    if (m_state == PlayerState::Idle)
    {
        setFrame(0);
        m_animationTimer = 0.0f;
    }
    else if (m_state == PlayerState::Walking)
    {
        m_animationTimer += deltaTime.asSeconds();

        if (m_animationTimer >= m_frameTime)
        {
            m_animationTimer -= m_frameTime;
            m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
            setFrame(m_currentFrame);
        }
    }
}

void Player::setFrame(int frame)
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
    if (m_facingRight)
    {
        scale.x = std::abs(scale.x);
    }
    else
    {
        scale.x = -std::abs(scale.x);
    }
    m_sprite.setScale(scale);
}

void Player::draw(sf::RenderWindow& window)
{
    if (m_isMining)
    {
        sf::Vector2f start = m_sprite.getPosition();
        start.y -= 15.f;

        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

        sf::Vector2f dir = mouseWorld - start;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) dir /= len;

        float rayLength = m_rayBaseLength + RayLengthLevel * 10.f;
        sf::Vector2f end = start + dir * rayLength;

        sf::Vertex line[2];
        line[0].position = start;
        line[0].color = sf::Color::Yellow;

        line[1].position = end;
        line[1].color = sf::Color::Red;

        window.draw(line, 2, sf::PrimitiveType::Lines);
    }

    window.draw(m_sprite);

}

sf::Vector2i Player::worldToTile(sf::Vector2f worldPos, Map& map)
{
    float tileSize = map.getTileSize();
    float offsetX = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
    float offsetY = WINDOW_Y / 2.0f;

    int col = static_cast<int>((worldPos.x - offsetX) / tileSize);
    int row = static_cast<int>((worldPos.y - offsetY) / tileSize);

    return sf::Vector2i(col, row);
}

sf::Vector2f Player::tileToWorld(sf::Vector2i tilePos, Map& map)
{
    float tileSize = map.getTileSize();
    float offsetX = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
    float offsetY = WINDOW_Y / 2.0f;

    float x = tilePos.x * tileSize + offsetX + tileSize / 2.0f;
    float y = tilePos.y * tileSize + offsetY + tileSize / 2.0f;

    return sf::Vector2f(x, y);
}

void Player::setPosition(sf::Vector2f pos)
{
    m_sprite.setPosition(pos);
}

void Player::updateMiningRay(sf::Time dt, Map& map, const sf::RenderWindow& window, const sf::View& cameraView)
{
    m_rayDamageCooldown -= dt.asSeconds();
    if (m_rayDamageCooldown > 0.f)
        return;

    sf::Vector2f playerPos = m_sprite.getPosition();
    playerPos.y -= 15.f;

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel, cameraView);

    sf::Vector2f dir = mouseWorld - playerPos;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0) return;
    dir /= len;

    float rayLength = m_rayBaseLength + RayLengthLevel * 10.f;
    float tileSize = map.getTileSize();

    for (float t = 0; t < rayLength; t += tileSize * 0.5f)
    {
        sf::Vector2f samplePoint = playerPos + dir * t;
        sf::Vector2i tile = worldToTile(samplePoint, map);

        if (tile.x < 0 || tile.y < 0 || tile.x >= map.getColumnCount() || tile.y >= map.getRowCount())
            continue;

        if (map.getTileHardness(tile.y, tile.x) > 0)
        {
            map.damageTile(tile.y, tile.x, getRayDamage());
            //break;
        }
    }

    m_rayDamageCooldown = m_rayTickDelay;

}

void Player::addMoney(int amount)
{
	m_money += amount;
}

float Player::getPickupRadius()
{
    return pickupRadius * (1.0f + pickupRadiusLevel * 0.15f);
}

float Player::getJumpForce()
{
    return m_jumpForce + (jumpLevel * -40.0f);
}

void Player::addCollectedItem(const CollectedItem& item)
{
    m_inventory.push_back(item);
	m_newPickups.push_back(item);
}

float Player::getRayLength() const
{
    return m_rayBaseLength * (1.0f + RayLengthLevel * 0.25f);
}

int Player::getRayDamage() const
{
    return 1 + rayDamageLevel; 
}

