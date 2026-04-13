#include "Map.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

Map::Map() {}

bool Map::loadMapFromConfig(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON config: " << filepath << "\n";
        return false;
    }

    try
    {
        json config;
        file >> config;

        // --- Terrain layers ---
        for (auto& layerNode : config["map"]["layers"])
        {
            LayerType layer;
            layer.name = layerNode["name"].get<std::string>();
            layer.hardness = layerNode["hardness"].get<int>();

            if (!layer.texture.loadFromFile(layerNode["texture"].get<std::string>()))
            {
                std::cerr << "Failed to load texture for layer: " << layer.name << "\n";
                return false;
            }
            m_layerTypes.push_back(std::move(layer));
        }

        // --- Buildings ---
        if (config.contains("museum"))
        {
            if (!m_museum.loadMuseumFromConfig(config["museum"]))
            {
                std::cerr << "Failed to load museum\n";
                return false;
            }
        }
        if (config.contains("trader"))
        {
            if (!m_trader.loadTraderFromConfig(config["trader"]))
            {
                std::cerr << "Failed to load trader\n";
                return false;
            }
        }

        // --- Collectible + dinosaur config ---
        if (!m_fossilManager.loadFossilsFromConfig(filepath))
        {
            std::cerr << "Failed to load fossil config\n";
            return false;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading JSON map config: " << e.what() << "\n";
        return false;
    }

    // Crack overlay texture
    if (!m_crackedOverlayTexture.loadFromFile("ASSETS/IMAGES/Terrain/Cracks.png"))
        std::cerr << "Failed to load crack texture!\n";

    return true;
}

void Map::generateGrid(int rows, int cols, float tileSize, float windowWidth, float windowHeight)
{

    if (m_rowsGenerated == 0)
    {
        m_tiles.clear();
        m_tileSize = tileSize;
        m_windowHeight = windowHeight;
        m_windowWidth = windowWidth;
        m_rows = rows;
        m_cols = cols;
    }

    float offsetY = m_windowHeight / 2.0f;
    float offsetX = (m_windowWidth - (cols * tileSize)) / 2.0f;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int row = m_rowsGenerated; row < m_rowsGenerated + rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            int layerIndex = determineLayerAtDepth(row, m_rows);

            const LayerType& layer = m_layerTypes[layerIndex];

            float yPos = row * tileSize + offsetY;
            float xPos = col * tileSize + offsetX;

            Tile newTile(layer.texture, sf::Vector2f(xPos, yPos), layer.hardness, m_crackedOverlayTexture);

            newTile.sprite.setScale(sf::Vector2f(tileSize / layer.texture.getSize().x, tileSize / layer.texture.getSize().y));

            m_tiles.emplace_back(std::move(newTile));

            // Ensure ladder vector is in sync
            m_ladders.push_back(false);
        }
    }

    m_rowsGenerated += rows;

    if (m_rowsGenerated >= m_rows)
    {
        m_fossilManager.cacheGridOffsets(offsetX, offsetY);
        std::cout << "Grid complete \n";
    }
}

int Map::determineLayerAtDepth(int row, int totalRows)
{
    if (row == 0)
    {
        return 0; // Topsoil    
    }

    // calc depth as a ratio

    float depthRatio = static_cast<float>(row) / static_cast<float>(totalRows);

    // generate random value for blending

    float randVal = static_cast<float>(std::rand()) / RAND_MAX;

    if (depthRatio < 0.20f)
    {
        // Near surface: mostly topsoil (0) transitioning to sediment (1)
        if (randVal < 0.7f)
        {
            return 1; // Sediment
        }
        else
        {
            return 2;
        }

    }
    else if (depthRatio < 0.40f)
    {
        // Upper layers: mostly sediment
        if (randVal < 0.85f)
        {
            return 1; // Sediment
        }
        else
        {
            return 2; // Occasional rock
        }
    }
    else if (depthRatio < 0.60f)
    {
        // Middle layers: sediment/rock mix
        if (randVal < 0.5f)
        {
            return 1; // Sediment
        }
        else
        {
            return 2; // Rock
        }
    }
    else if (depthRatio < 0.80f)
    {
        // Deep layers: mostly rock
        if (randVal < 0.8f)
        {
            return 2; // Rock
        }
        else
        {
            return 3; // Some bedrock appearing
        }
    }
    else if (depthRatio < 0.90f)
    {
        // Very deep: rock/bedrock transition
        if (randVal < 0.5f)
        {
            return 2; // Rock
        }
        else
        {
            return 3; // Bedrock
        }
    }
    else
    {
        // Deepest layers: mostly bedrock
        if (randVal < 0.9f)
        {
            return 3; // Bedrock
        }
        else
        {
            return 2; // Occasional rock
        }
    }
}

void Map::setupBackground()
{

    if (!m_backgroundTexture.loadFromFile("ASSETS/IMAGES/TERRAIN/Background.png"))
    {
        std::cout << "failed to load background texture" << std::endl;
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);


    m_backgroundSprite.setTextureRect(sf::IntRect({ 0, 0 }, { WINDOW_X, BACKGROUND_LENGTH }));

    // Position background at (0,0)
    m_backgroundSprite.setPosition(sf::Vector2f(0.f, 0.f));
}

void Map::removeTile(int row, int col)
{
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
    {
		return;
    }

	int index = row * m_cols + col;

    if (index < 0 || index >= static_cast<int>(m_tiles.size()))
    {
		return;
    }

	Tile& tile = m_tiles[index];

    // already removed tiles 
    if (tile.sprite.getColor() == sf::Color::Transparent)
    {
        return;
    }

	tile.sprite.setColor(sf::Color::Transparent);
	tile.layerHardness = 0;
	tile.currentHP = 0;

    m_fossilManager.trySpawnCollectible(row, col, m_tileSize, m_windowWidth, m_windowHeight);

}

void Map::removeTile(int row, int col, Player& player)  //calls the base removeTile; player pickup is handled by player::tryPickupCollectivle
{
    removeTile(row, col);
}

int Map::getTileHardness(int row, int col) const
{
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
    {
        return 0;
    }

    int index = row * m_cols + col;

    if (index >= 0 && index < static_cast<int>(m_tiles.size()))
    {
        return m_tiles[index].layerHardness;
    }

    return 0;
}

int Map::getTileCurrentHP(int row, int col) const
{
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
        return 0;

    int index = row * m_cols + col;


    return m_tiles[index].currentHP;
}

void Map::damageTile(int row, int col, int dmg)
{
    int index = row * m_cols + col;
    Tile& t = m_tiles[index];

    if (t.currentHP <= 0)
        return;

    t.currentHP -= dmg;

    float hpPercent = static_cast<float>(t.currentHP) / t.layerHardness;

    if (hpPercent > 0.75f)
    {
        t.crackedFrameIndex = 0;
    }
    else if (hpPercent > 0.50f)
    {
        t.crackedFrameIndex = 1;
    }
    else if (hpPercent > 0.25f)
    {
        t.crackedFrameIndex = 2;
    }
    else if (hpPercent > 0.10f)
    {
        t.crackedFrameIndex = 3;
    }
    else
    {
        t.crackedFrameIndex = 4;
    }

    int frameWidth = 24; // crack sprite frame width 

    t.crackedSprite.setTextureRect(sf::IntRect({ t.crackedFrameIndex * frameWidth, 0 }, { frameWidth, frameWidth }));


    if (t.currentHP <= 0)
    {
		removeTile(row, col);       // tile destroyed now triggers collectible spawn 
    }
}

void Map::drawMap(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);

    // Get view bounds for frustum culling
    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();
    sf::FloatRect viewBounds(sf::Vector2f(viewCenter.x - viewSize.x / 2.f, viewCenter.y - viewSize.y / 2.f), viewSize);

    // Only draw tiles that intersect with the view
    for (int i = 0; i < static_cast<int>(m_tiles.size()); ++i)
    {
        const Tile& tile = m_tiles[i];
        
        // Frustum culling: skip tiles outside the view
        if (!viewBounds.findIntersection(tile.sprite.getGlobalBounds()))
        {
            continue;
        }

        window.draw(tile.sprite);

        if (tile.currentHP > 0 && tile.crackedFrameIndex > 0)
        {
			window.draw(tile.crackedSprite);
        }

        // Draw ladder support if present
        if (i >= 0 && i < static_cast<int>(m_ladders.size()) && m_ladders[i])
        {
            // Draw small white square centered on tile
            sf::RectangleShape ladderShape;
            ladderShape.setSize(sf::Vector2f(m_tileSize * 0.3f, m_tileSize * 0.3f));
            ladderShape.setFillColor(sf::Color::White);
            ladderShape.setPosition(sf::Vector2f(tile.sprite.getPosition().x + (m_tileSize - ladderShape.getSize().x) / 2.0f, tile.sprite.getPosition().y + (m_tileSize - ladderShape.getSize().y) / 2.0f));
            window.draw(ladderShape);
        }
    }

    m_fossilManager.drawCollectibles(window);

    // Only draw museum if it's in view
    if (viewBounds.findIntersection(m_museum.getSprite().getGlobalBounds()))
    {
        m_museum.drawMuseum(window);
    }

    // Only draw trader if it's in view
    if (viewBounds.findIntersection(m_trader.getSprite().getGlobalBounds()))
    {
        m_trader.drawTrader(window);
    }
}

void Map::toggleDebugMode()
{
    m_debugMode = !m_debugMode;

    if (m_debugMode)
    {
        std::cout << "Debug mode ON\n";
    }
    else
    {
        std::cout << "Debug mode OFF\n";
    }
}

void Map::updateHover(const sf::RenderWindow& window, float tileSize, int cols)
{
    if (!m_debugMode) return; // (only if debug is active)



    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

    // Compute grid starting offsets (like generateGrid)
    int rows = static_cast<int>(m_tiles.size() / cols);
    float totalGridHeight = rows * tileSize;
    float totalGridWidth = cols * tileSize;
    float offsetY = WINDOW_Y / 2.0f;
    float offsetX = (WINDOW_X - totalGridWidth) / 2.0f;


    float localX = mouseWorld.x - offsetX;
    float localY = mouseWorld.y - offsetY;

    // Skip if outside the grid bounds
    if (localX < 0 || localY < 0 || localX >= totalGridWidth || localY >= totalGridHeight)
    {
        m_hoveredIndex = -1;
        return;
    }

    // Calculate which tile the mouse is over
    int tileX = static_cast<int>(localX / tileSize);
    int tileY = static_cast<int>(localY / tileSize);
    int index = tileY * cols + tileX;

    if (index >= 0 && index < static_cast<int>(m_tiles.size()))
    {
        if (index != m_hoveredIndex)
        {
            m_hoveredIndex = index;

            const Tile& hoveredTile = m_tiles[m_hoveredIndex];
            std::string tileName = "Unknown";
            int tileHardness = hoveredTile.layerHardness;

            for (const auto& layer : m_layerTypes)
            {
                if (layer.hardness == hoveredTile.layerHardness)
                {
                    tileName = layer.name;
                    break;
                }
            }

            //std::cout << "Tile type: " << tileName << " | Hardness: " << tileHardness << std::endl;
        }

        // Draw outline on hovered tile
        const Tile& hoveredTile = m_tiles[m_hoveredIndex];
        m_hoverOutline.setSize(sf::Vector2f(tileSize, tileSize));
        m_hoverOutline.setPosition(hoveredTile.sprite.getPosition());
        m_hoverOutline.setFillColor(sf::Color::Transparent);
        m_hoverOutline.setOutlineColor(sf::Color::White);
        m_hoverOutline.setOutlineThickness(1.f);
    }
    else
    {
        m_hoveredIndex = -1;
    }
}

void Map::handleMouseHold(const sf::RenderWindow& window, float tileSize, int cols)
{

    if (!m_debugMode) return; // Only work in debug mode

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        return;
    }

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

    // Compute grid starting offsets (same as updateHover)
    int rows = static_cast<int>(m_tiles.size() / cols);
    float totalGridHeight = rows * tileSize;
    float totalGridWidth = cols * tileSize;
    float offsetY = WINDOW_Y / 2.0f;
    float offsetX = (WINDOW_X - totalGridWidth) / 2.0f;

    float localX = mouseWorld.x - offsetX;
    float localY = mouseWorld.y - offsetY;

    // Skip if outside the grid bounds
    if (localX < 0 || localY < 0 || localX >= totalGridWidth || localY >= totalGridHeight)
    {
        return;
    }

    // Calculate which tile was clicked
    int tileX = static_cast<int>(localX / tileSize);
    int tileY = static_cast<int>(localY / tileSize);
    int index = tileY * cols + tileX;

    if (index >= 0 && index < static_cast<int>(m_tiles.size()))
    {
        // Only remove if not already transparent 
        if (m_tiles[index].sprite.getColor() != sf::Color::Transparent)
        {
            // Call removeTile instead of just setting color transparent
            removeTile(tileY, tileX);
        }
    }
}

void Map::drawDebug(sf::RenderWindow& window)
{
    if (m_debugMode && m_hoveredIndex != -1)
    {
        window.draw(m_hoverOutline);
    }
}

sf::Vector2f Map::tileToWorld(sf::Vector2i tilePos) const
{
    float x = tilePos.x * m_tileSize + (m_windowWidth - m_cols * m_tileSize) / 2.0f + m_tileSize / 2.0f;
    float y = tilePos.y * m_tileSize + m_windowHeight / 2.0f + m_tileSize / 2.0f;
    return sf::Vector2f(x, y);
}
bool Map::isPointOnTrader(const sf::Vector2f& worldPos) const
{
    return m_trader.containsPoint(worldPos);
}

void Map::updateMuseum(sf::RenderWindow& window)
{
    m_museum.updateMuseumHover(window);
}

void Map::updateTrader(sf::RenderWindow& window)
{
    m_trader.updateTraderHover(window);
}

void Map::addLadder(int row, int col)
{
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
        return;

    int index = row * m_cols + col;
    if (index >= 0 && index < static_cast<int>(m_ladders.size()))
    {
        m_ladders[index] = true;
    }
}

void Map::removeLadder(int row, int col)
{
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
        return;

    int index = row * m_cols + col;
    if (index >= 0 && index < static_cast<int>(m_ladders.size()))
    {
        m_ladders[index] = false;
    }
}

bool Map::hasLadder(int row, int col) const
{
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
        return false;

    int index = row * m_cols + col;
    if (index >= 0 && index < static_cast<int>(m_ladders.size()))
    {
        return m_ladders[index];
    }
    return false;
}

bool Map::isWalkable(int row, int col) const
{
    // Walkable if inside bounds and hardness == 0 (dug) OR has ladder
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols)
        return false;

    int index = row * m_cols + col;
    if (index < 0 || index >= static_cast<int>(m_tiles.size()))
        return false;

    const Tile& t = m_tiles[index];

    // Solid if hardness > 0
    if (t.layerHardness > 0)
        return false;

    // Dug tile or ladder tile is walkable
    return true;
}

sf::Vector2i Map::worldToTile(sf::Vector2f worldPos) const
{
    float offsetY = m_windowHeight / 2.0f;
    float offsetX = (m_windowWidth - (m_cols * m_tileSize)) / 2.0f;

    float localX = worldPos.x - offsetX;
    float localY = worldPos.y - offsetY;

    int col = static_cast<int>(std::floor(localX / m_tileSize));
    int row = static_cast<int>(std::floor(localY / m_tileSize));

    return sf::Vector2i(col, row);
}
