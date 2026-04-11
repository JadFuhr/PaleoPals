 #include "Player.h"
#include "Map.h"
#include "Fossil.h"
#include <iostream>
#include <cmath>

//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
Player::Player()
{
    std::cout << "Player constructor START\n";

    // Load texture
    if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
    {
        std::cerr << "Failed to load player texture!\n";

    }

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { m_frameWidth, m_frameHeight }));
    m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight));
    m_sprite.setScale(sf::Vector2f(0.2f, 0.2f));
    m_sprite.setPosition(sf::Vector2f(400.0f, 300.0f));

    // Setup interaction radius visualization (for debug)
    m_interactionRadiusVisual.setSize(sf::Vector2f(m_interactionRadius * 2.0f, m_interactionRadius * 2.0f));
    m_interactionRadiusVisual.setOrigin(sf::Vector2f(m_interactionRadius, m_interactionRadius));
    m_interactionRadiusVisual.setFillColor(sf::Color::Transparent);
    m_interactionRadiusVisual.setOutlineColor(sf::Color(0, 255, 0, 100));
    m_interactionRadiusVisual.setOutlineThickness(1.0f);

    if (!m_pickaxeTexture.loadFromFile("ASSETS/IMAGES/Items/pickaxe.png"))
    {
        std::cerr << "Failed to load pickaxe texture!\n";
    }
    m_pickaxeSprite.setTexture(m_pickaxeTexture);
    m_pickaxeSprite.setOrigin(sf::Vector2f(0.f, m_pickaxeTexture.getSize().y));
    m_pickaxeSprite.setTextureRect(sf::IntRect({ 0,0 }, { 64,64 }));
    m_pickaxeSprite.setScale(sf::Vector2f(0.4f, 0.4f));


    std::cout << "Player constructor END\n";
}

//------------------------------------------------------------
// Destructor
//------------------------------------------------------------
Player::~Player()
{
}

//------------------------------------------------------------
// Update
//------------------------------------------------------------
void Player::update(sf::Time deltaTime, Map& map, const sf::RenderWindow& window)
{
    handleInput(deltaTime, map);
    applyPhysics(deltaTime, map);
    updateAnimation(deltaTime);

    // Update interaction radius position
    m_interactionRadiusVisual.setPosition(sf::Vector2f(m_sprite.getPosition() + sf::Vector2f(0, -16.0f)));

    bool mouseHeld = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    m_isSwinging = mouseHeld;
    m_pickaxeCooldown -= deltaTime.asSeconds();

    if (m_isSwinging && m_pickaxeCooldown <= 0.0f)
    {
		updatePickaxeAnimation(deltaTime);
        updatePickaxe(window, map);
		checkPickaxeHit(window, map);
        m_pickaxeCooldown = m_pickaxeHitDelay;
    }

    if (!mouseHeld)
    {
		m_isSwinging = false;
		m_pickaxeCurrentFrame = 0;
		m_pickaxeFrameDirection = 1;
    }
    

}

//------------------------------------------------------------
// Handle Input
//------------------------------------------------------------
void Player::handleInput(sf::Time deltaTime, Map& map)
{

    // Horizontal movement
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

    // Jumping
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && m_canJump && m_isGrounded)
    {
        m_velocity.y = m_jumpForce;
        m_canJump = false;
        m_state = PlayerState::Jumping;
        //std::cout << "Player jumped!\n";
    }

    // Release jump key to allow next jump
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        m_canJump = true;
    }

    static bool eWasPressed = false;
    bool eIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);


    if (eIsPressed && !eWasPressed)
    {
        tryPickupCollectible(map);
    }
    eWasPressed = eIsPressed;
}

//------------------------------------------------------------
// Apply Physics
//------------------------------------------------------------
void Player::applyPhysics(sf::Time deltaTime, Map& map)
{

    // Apply gravity
    m_velocity.y += m_gravity * deltaTime.asSeconds();

    // Clamp vertical velocity
    if (m_velocity.y > 600.0f)
    {
        m_velocity.y = 600.0f;
    }

    // Move player
    m_sprite.move(m_velocity * deltaTime.asSeconds());

    // Check ground collision
    checkCollisions(map);

    // Update state based on velocity
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
}

//------------------------------------------------------------
// Check Collisions
//
// The player sprite origin is at (frameWidth/2, frameHeight) i.e. bottom-centre.
// At scale 0.2 the visible half-width is roughly (192*0.2)/2 = 19.2 px and the
// height is 192*0.2 = 38.4 px, but we use a tighter hitbox so the
// player fits inside a single-tile tunnel.
//
// playerPos.y  == bottom of the player (feet)
// playerPos.y - playerHeight == top of the player (head)
// playerPos.x  == horizontal centre of the player
//------------------------------------------------------------
void Player::checkCollisions(Map& map)
{
    float tileSize = map.getTileSize();

    // Hitbox dimensions - keep narrower than a tile so the player fits in dug tunnels
    const float halfW = tileSize * 0.35f;   // 8.4 px  (side clearance from centre)
    const float playerHeight = tileSize * 1.6f; // 38 px  (two tiles tall)

    m_isGrounded = false;


    // Helper lambdas: tile position helpers
    float tileOffsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
    float tileOffsetY = WINDOW_Y / 2.0f;

    // Convert world pos to tile col/row 
    auto toTileCol = [&](float worldX) -> int {
        return static_cast<int>(std::floor((worldX - tileOffsetX) / tileSize));
        };
    auto toTileRow = [&](float worldY) -> int {
        return static_cast<int>(std::floor((worldY - tileOffsetY) / tileSize));
        };
    // Left edge of a tile in world space
    auto tileLeft = [&](int col) -> float {
        return col * tileSize + tileOffsetX;
        };
    // Top edge of a tile in world space
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

    sf::Vector2f pos = m_sprite.getPosition(); // feet centre


    // VERTICAL COLLISION
    if (m_velocity.y >= 0)
    {
        // GROUND: check tile(s) directly under both bottom corners
        float feetY = pos.y;                    // feet are at pos.y
        int   footRow = toTileRow(feetY);        // row the feet are sitting on top of

        int colL = toTileCol(pos.x - halfW + 1.f);
        int colR = toTileCol(pos.x + halfW - 1.f);

        // The tile whose top the player should land on is footRow
        // because tileTop(footRow) <= feetY < tileTop(footRow+1)
        if (solid(footRow, colL) || solid(footRow, colR))
        {
            float surfaceY = tileTop(footRow); // top edge of that tile

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
        // CEILING: check tile(s) above head 
        float headY = pos.y - playerHeight;
        int   headRow = toTileRow(headY);

        int colL = toTileCol(pos.x - halfW + 1.f);
        int colR = toTileCol(pos.x + halfW - 1.f);

        if (solid(headRow, colL) || solid(headRow, colR))
        {
            // Bottom edge of the tile the head hit
            float ceilBottom = tileTop(headRow) + tileSize;
            m_sprite.setPosition(sf::Vector2f(pos.x, ceilBottom + playerHeight));
            m_velocity.y = 0;
            pos = m_sprite.getPosition();
        }
    }


    // HORIZONTAL COLLISION
    // Checks three vertical points: head, mid, feet
    if (m_velocity.x != 0)
    {
        // Sample rows: just below head, mid-body, just above feet
        float sampleYs[3] = {
            pos.y - playerHeight + 2.f,   // head region
            pos.y - playerHeight * 0.5f,  // mid
            pos.y - 2.f                   // feet region
        };

        if (m_velocity.x < 0)
        {
            // Moving left – check left edge
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
                // Push right so left edge aligns with right side of blocking tile
                float newX = tileLeft(leftCol) + tileSize + halfW;
                m_sprite.setPosition(sf::Vector2f(newX, pos.y));
            }
        }
        else
        {
            // Moving right – check right edge
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
                // Push left so right edge aligns with left side of blocking tile
                float newX = tileLeft(rightCol) - halfW;
                m_sprite.setPosition(sf::Vector2f(newX, pos.y));
            }
        }
    }
}

//------------------------------------------------------------
// Try Pickup Collectible
//------------------------------------------------------------
void Player::tryPickupCollectible(Map& map)
{
    sf::Vector2f playerPos = m_sprite.getPosition();
    FossilManager& fossilManager = map.getFossilManager();
    float tileSize = map.getTileSize();
    float tileOffsetX = (WINDOW_X - map.getColumnCount() * tileSize) / 2.0f;
    float tileOffsetY = WINDOW_Y / 2.0f;

    
    // player tile pos (feet)
    int playerCol = static_cast<int>(std::floor((playerPos.x - tileOffsetX) / tileSize));
    int playerRow = static_cast<int>(std::floor((playerPos.y - tileOffsetY) / tileSize));

	auto& allCollectibles = fossilManager.getAllCollectibles();
    
    for (auto& c : allCollectibles)
    {

        if (c.isPickedUp) continue;

        // Must be within 2-tile radius
        if (std::abs(c.gridRow - playerRow) > 2 || std::abs(c.gridCol - playerCol) > 2)
            continue;

        // Build the inventory item
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
        }
        else if (c.collectibleIndex <= 8)
        {
            item.type = "amber";
            item.name = (c.monetaryValue == 50) ? "Small Amber" : "Large Amber";
            m_money += c.monetaryValue;
        }
        else
        {
            item.type = "trash";
            item.name = "Trash";
        }

        m_inventory.push_back(item);
        m_newPickups.push_back(item);

        // Mark as collected and move off-screen
        c.isPickedUp = true;
        c.sprite.setPosition(sf::Vector2f(-10000.f, -10000.f));

        std::cout << "[Pickup] " << item.name << " (type=" << item.type << ")" << " | Inventory size: " << m_inventory.size() << "\n";

        return; // one item per key press
    }
}

//------------------------------------------------------------
// Update Animation
//------------------------------------------------------------
void Player::updateAnimation(sf::Time deltaTime)
{
    // Idle and Mining: use frame 0
    if (m_state == PlayerState::Idle)
    {
        setFrame(0);
        m_animationTimer = 0.0f;
    }
    // Walking: animate walk cycle
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

//------------------------------------------------------------
// Set Frame
//------------------------------------------------------------
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

    // Flip sprite based on facing direction
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

//------------------------------------------------------------
// Draw
//------------------------------------------------------------
void Player::draw(sf::RenderWindow& window)
{
    // Draw interaction radius (debug)
    window.draw(m_interactionRadiusVisual);

    if (m_isSwinging)
    {
		window.draw(m_pickaxeSprite);
    }

    window.draw(m_sprite);


    m_sprite.setColor(sf::Color::Red);

}


//------------------------------------------------------------
// World to Tile Conversion
//------------------------------------------------------------
sf::Vector2i Player::worldToTile(sf::Vector2f worldPos, Map& map)
{
    float tileSize = map.getTileSize();
    float offsetX = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
    float offsetY = WINDOW_Y / 2.0f;

    int col = static_cast<int>((worldPos.x - offsetX) / tileSize);
    int row = static_cast<int>((worldPos.y - offsetY) / tileSize);

    return sf::Vector2i(col, row);
}

//------------------------------------------------------------
// Tile to World Conversion
//------------------------------------------------------------
sf::Vector2f Player::tileToWorld(sf::Vector2i tilePos, Map& map)
{
    float tileSize = map.getTileSize();
    float offsetX = (WINDOW_X - (map.getColumnCount() * tileSize)) / 2.0f;
    float offsetY = WINDOW_Y / 2.0f;

    float x = tilePos.x * tileSize + offsetX + tileSize / 2.0f;
    float y = tilePos.y * tileSize + offsetY + tileSize / 2.0f;

    return sf::Vector2f(x, y);
}

//------------------------------------------------------------
// Set Position
//------------------------------------------------------------
void Player::setPosition(sf::Vector2f pos)
{
    m_sprite.setPosition(pos);
}

//------------------------------------------------------------
// Collect Fossil
//------------------------------------------------------------
void Player::collectFossil(const std::string& dinosaurName, const std::string& pieceId, const std::string& category)
{
    CollectedItem item;
    item.collectibleIndex = 0; // Fossil type
    item.type = "fossil";
    item.monetaryValue = 0;
    item.dinosaurName = dinosaurName;
    item.pieceId = pieceId;
    item.category = category;
    item.name = pieceId + " of " + dinosaurName;

    m_inventory.push_back(item);
    m_newPickups.push_back(item);

    std::cout << "✓ Collected: " << item.name << "\n";
}

//------------------------------------------------------------
// Collect Amber (and add to money)
//------------------------------------------------------------
void Player::collectAmber(int monetaryValue)
{
    CollectedItem item;
    item.collectibleIndex = 7; // Amber type
    item.type = "amber";
    item.monetaryValue = monetaryValue;
    item.name = (monetaryValue == 50) ? "Small Amber" : "Large Amber";

    m_inventory.push_back(item);
    m_money += monetaryValue;

    std::cout << "✓ " << item.name << " (+$" << monetaryValue << " | Total: $" << m_money << ")\n";
}

//------------------------------------------------------------
// Collect Trash (worthless)
//------------------------------------------------------------
void Player::collectTrash()
{
    CollectedItem item;
    item.collectibleIndex = 9; // Trash type
    item.type = "trash";
    item.monetaryValue = 0;
    item.name = "Trash";

    m_inventory.push_back(item);

    std::cout << "✓ Collected: Trash (worthless)\n";
}

void Player::updatePickaxe(const sf::RenderWindow& window, Map& map)
{
    sf::Vector2f playerPos = m_sprite.getPosition();
    playerPos.y -= 15.0f;

    // Mouse world position
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

    // Direction from player → mouse
    sf::Vector2f dir = mouseWorld - playerPos;
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

    m_pickaxeAngle = angle;

    // Position pickaxe at radius around player
    float rad = angle * 0.01745f;
    sf::Vector2f offset(std::cos(rad), std::sin(rad));
    offset *= m_pickaxeRadius;

    // Set sprite position FIRST
    m_pickaxeSprite.setPosition(playerPos + offset);
    m_pickaxeSprite.setRotation(sf::degrees(angle + 45));

    // Compute tip AFTER setting sprite position
    sf::Vector2f tipOffset(
        std::cos(rad) * m_pickaxeTipDistance,
        std::sin(rad) * m_pickaxeTipDistance
    );

    m_pickaxeTip = m_pickaxeSprite.getPosition() + tipOffset;
}
  
void Player::checkPickaxeHit(const sf::RenderWindow& window, Map& map)
{
    if (!m_isSwinging) return;

    const float radius = m_pickaxeTipRadius;
    const sf::Vector2f tip = m_pickaxeTip;
    float tileSize = map.getTileSize();
    sf::Vector2i playerTile = worldToTile(m_sprite.getPosition(), map);

    auto circleIntersectsRect = [](sf::Vector2f c, float r, const sf::FloatRect& rect)
        {
            float cx = std::clamp(c.x, rect.position.x, rect.position.x + rect.size.x);
            float cy = std::clamp(c.y, rect.position.y, rect.position.y + rect.size.y);
            float dx = c.x - cx, dy = c.y - cy;
            return (dx * dx + dy * dy) <= (r * r);
        };

    for (int r = playerTile.y - 2; r <= playerTile.y + 2; r++)
    {
        for (int c = playerTile.x - 2; c <= playerTile.x + 2; c++)
        {
            if (r < 0 || c < 0 || r >= map.getRowCount() || c >= map.getColumnCount())
                continue;

            sf::Vector2f tilePos = map.tileToWorld({ c, r });
            sf::FloatRect tileRect({ tilePos.x - tileSize / 2.f, tilePos.y - tileSize / 2.f }, { tileSize, tileSize });

            if (circleIntersectsRect(tip, radius, tileRect))
            {
                if (map.getTileCurrentHP(r, c) > 0)
                {
                    map.damageTile(r, c, 1);
                    return;
                }
            }
        }
    }
}

void Player::updatePickaxeAnimation(sf::Time dt)
{
    m_pickaxeAnimationTimer += dt.asSeconds();

    if (m_pickaxeAnimationTimer >= m_pickaxeFrameTime)
    {
        m_pickaxeAnimationTimer = 0.f;

        // Advance forward only
        m_pickaxeCurrentFrame++;

        // Loop back to 0 when reaching the end
        if (m_pickaxeCurrentFrame >= m_pickaxeTotalFrames)
        {
            m_pickaxeCurrentFrame = 0;
        }
    }

    // --- This MUST be outside the if-block ---
    int frameWidth = static_cast<int>(m_pickaxeTexture.getSize().x / m_pickaxeTotalFrames);
    int frameHeight = static_cast<int>(m_pickaxeTexture.getSize().y);

    int left = m_pickaxeCurrentFrame * frameWidth;
    int top = 0;

    m_pickaxeSprite.setTextureRect(sf::IntRect({ left, top }, { frameWidth, frameHeight }));
}
