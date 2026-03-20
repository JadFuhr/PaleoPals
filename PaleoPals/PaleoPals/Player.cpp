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

    // Setup mining progress bar
    m_miningProgressBarBg.setSize(sf::Vector2f(40.0f, 6.0f));
    m_miningProgressBarBg.setFillColor(sf::Color(50, 50, 50));
    m_miningProgressBarBg.setOutlineColor(sf::Color::White);
    m_miningProgressBarBg.setOutlineThickness(1.0f);

    m_miningProgressBar.setSize(sf::Vector2f(40.0f, 6.0f));
    m_miningProgressBar.setFillColor(sf::Color::Yellow);

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
void Player::update(sf::Time deltaTime, Map& map)
{
    handleInput(deltaTime, map);
    applyPhysics(deltaTime, map);
    updateAnimation(deltaTime);
    updateMining(deltaTime, map);
    updateMiningProgressBar();

    // Update interaction radius position
    m_interactionRadiusVisual.setPosition(sf::Vector2f(m_sprite.getPosition() + sf::Vector2f(0, -16.0f)));
}

//------------------------------------------------------------
// Handle Input
//------------------------------------------------------------
void Player::handleInput(sf::Time deltaTime, Map& map)
{
    // Don't allow movement while mining
    if (m_isMining)
    {
        return;
    }

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
        std::cout << "Player jumped!\n";
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
    if (m_isMining)
    {
        // No physics while mining
        return;
    }

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

    // Use floor-based conversion (same as checkCollisions) so the tile row/col
    // correctly matches the rows stored in each Collectible's gridRow/gridCol.
    int playerCol = static_cast<int>(std::floor((playerPos.x - tileOffsetX) / tileSize));
    int playerRow = static_cast<int>(std::floor((playerPos.y - tileOffsetY) / tileSize));

    std::cout << " PICKUP ATTEMPT \n";
    std::cout << "Player at tile (" << playerCol << ", " << playerRow << ")\n";

    int collectiblesFound = 0;

    // Search all collectibles – find discovered (isDiscovered==true)
    // because getCollectibleAtTile() only returns undiscovered ones.
    auto& allCollectibles = fossilManager.getAllCollectibles();

    for (auto& collectible : allCollectibles)
    {
        int dy = collectible.gridRow - playerRow;
        int dx = collectible.gridCol - playerCol;

        // Only check within a 1-tile radius
        if (std::abs(dy) > 1 || std::abs(dx) > 1)
            continue;

        int hardness = map.getTileHardness(collectible.gridRow, collectible.gridCol);

        collectiblesFound++;
        std::cout << "  Found collectible at (" << collectible.gridCol << ", " << collectible.gridRow
            << ") - Discovered: " << (collectible.isDiscovered ? "YES" : "NO")
            << " - Hardness: " << hardness << "\n";

        // A collectible is pickupable when the tile covering it has been mined (hardness == 0)
        if (hardness == 0)
        {
            CollectedItem item;
            item.collectibleIndex = collectible.collectibleIndex;
            item.type = (collectible.collectibleIndex <= 6) ? "fossil" :
                (collectible.collectibleIndex <= 8) ? "amber" : "trash";
            item.monetaryValue = collectible.monetaryValue;

            if (item.type == "fossil")
            {
                item.dinosaurName = collectible.assignedDinosaurName;
                item.pieceId = collectible.assignedPieceId;
                item.category = collectible.assignedCategory;
                item.name = collectible.assignedPieceId + " of " + collectible.assignedDinosaurName;
            }
            else
            {
                item.name = item.type;
            }

            m_inventory.push_back(item);

            std::cout << " PICKED UP: " << item.name << " (Type: " << item.type << ") \n";
            std::cout << "    Inventory size: " << m_inventory.size() << "\n";

            // Remove from world by moving far off-screen and marking collected
            collectible.sprite.setPosition(sf::Vector2f(-10000.f, -10000.f));
            collectible.isDiscovered = false;
            collectible.gridRow = -1;
            collectible.gridCol = -1;

            return; // Only pick up one item per press
        }
    }

    if (collectiblesFound == 0)
    {
        std::cout << "  No collectibles found in 1-tile radius\n";
    }
    else
    {
        std::cout << "  Found " << collectiblesFound << " collectible(s) but tile(s) not yet mined\n";
        std::cout << "===================\n";
    }
}

//------------------------------------------------------------
// Try Mine At Mouse
//------------------------------------------------------------
void Player::tryMineAtMouse(const sf::RenderWindow& window, Map& map)
{
    if (m_isMining)
    {
        return; // Already mining
    }

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

    // Get tile at mouse position
    sf::Vector2i mouseTile = worldToTile(mouseWorld, map);
    sf::Vector2f playerPos = m_sprite.getPosition();
    sf::Vector2i playerTile = worldToTile(playerPos, map);

    // Check if mouse tile is within mining range (2 tiles in any direction)
    float dx = static_cast<float>(mouseTile.x - playerTile.x);
    float dy = static_cast<float>(mouseTile.y - playerTile.y);
    float distance = std::sqrt(dx * dx + dy * dy);

    std::cout << "Mining attempt: Player tile (" << playerTile.x << ", " << playerTile.y 
        << "), Mouse tile (" << mouseTile.x << ", " << mouseTile.y 
        << "), Distance: " << distance << "\n";

    if (distance <= 2.5f)
    {
        // Check if tile exists and has hardness
        int hardness = map.getTileHardness(mouseTile.y, mouseTile.x);

        std::cout << "  Within range! Hardness: " << hardness << "\n";

        if (hardness > 0)
        {
            // Start mining
            m_isMining = true;
            m_miningProgress = 0.0f;
            m_miningTarget = mouseTile;
            m_miningDuration = 0.8f + (hardness * 0.4f);
            m_state = PlayerState::Mining;

            std::cout << "Started mining tile (" << mouseTile.x << ", " << mouseTile.y
                << ") with hardness " << hardness << "\n";
        }
    }
    else
    {
        std::cout << "  OUT OF RANGE (distance " << distance << " > 2.0)\n";
    }
}

//------------------------------------------------------------
// Update Mining
//------------------------------------------------------------
void Player::updateMining(sf::Time deltaTime, Map& map)
{
    if (!m_isMining)
    {
        return;
    }

    m_miningProgress += deltaTime.asSeconds();

    if (m_miningProgress >= m_miningDuration)
    {
        // Mining complete
        map.removeTile(m_miningTarget.y, m_miningTarget.x);
        std::cout << "Mined tile (" << m_miningTarget.x << ", " << m_miningTarget.y << ")\n";

        // Reset mining state
        m_isMining = false;
        m_miningProgress = 0.0f;
        m_miningTarget = sf::Vector2i(-1, -1);
        m_state = PlayerState::Idle;
    }
}

//------------------------------------------------------------
// Update Animation
//------------------------------------------------------------
void Player::updateAnimation(sf::Time deltaTime)
{
    // Idle and Mining: use frame 0
    if (m_state == PlayerState::Idle || m_state == PlayerState::Mining)
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

    window.draw(m_sprite);

    m_sprite.setColor(sf::Color::Red);

    // Draw mining progress bar
    if (m_isMining && m_miningProgress > 0.0f)
    {
        window.draw(m_miningProgressBarBg);
        window.draw(m_miningProgressBar);
    }
}

//------------------------------------------------------------
// Update Mining Progress Bar
//------------------------------------------------------------
void Player::updateMiningProgressBar()
{
    sf::Vector2f spritePos = m_sprite.getPosition();

    m_miningProgressBarBg.setPosition(sf::Vector2f(spritePos.x - 20.0f, spritePos.y - 50.0f));
    m_miningProgressBar.setPosition(m_miningProgressBarBg.getPosition());

    float progress = m_miningProgress / m_miningDuration;
    m_miningProgressBar.setSize(sf::Vector2f(40.0f * progress, 6.0f));
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