#include "Map.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

Map::Map(){}

bool Map::loadMapFromConfig(const std::string& filepath)
{
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON config: " << filepath << std::endl;
        return false;
    }
    try
    {
        json config;
        file >> config;

        auto layers = config["map"]["layers"];

        for (auto& layerNode : layers)
        {
            LayerType layer;
            layer.name = layerNode["name"].get<std::string>();
            layer.hardness = layerNode["hardness"].get<int>();

            if (!layer.texture.loadFromFile(layerNode["texture"].get<std::string>()))
            {
                std::cerr << "Failed to load texture for layer: " << layer.name << std::endl;
                return false;
            }

            m_layerTypes.push_back(std::move(layer));
        }
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

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading JSON map config: " << e.what() << std::endl;
        return false;
    }

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

			Tile newTile(layer.texture, sf::Vector2f(xPos, yPos), layer.hardness);
            newTile.sprite.setScale(sf::Vector2f(tileSize / layer.texture.getSize().x, tileSize / layer.texture.getSize().y));

			m_tiles.emplace_back(std::move(newTile));
        }
    }

	m_rowsGenerated += rows;
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

    if(!m_backgroundTexture.loadFromFile("ASSETS/IMAGES/TERRAIN/Background.png"))
    {
        std::cout << "failed to load background texture" << std::endl;
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);
    

    m_backgroundSprite.setTextureRect(sf::IntRect({ 0, 0 }, { WINDOW_X, WINDOW_Y }));

    // Position background at (0,0)
	m_backgroundSprite.setPosition(sf::Vector2f(0.f, 0.f));
}


void Map::drawMap(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);

    for (auto& tile : m_tiles)
    {
        window.draw(tile.sprite);
    }

    m_museum.drawMuseum(window);
    m_trader.drawTrader(window);
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

void Map::updateMuseum(sf::RenderWindow& window)
{
    m_museum.updateMuseumHover(window);
}

void Map::updateTrader(sf::RenderWindow& window)
{
    m_trader.updateTraderHover(window);
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

            std::cout << "Tile type: " << tileName << " | Hardness: " << tileHardness << std::endl;
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



void Map::drawDebug(sf::RenderWindow& window)
{
    if (m_debugMode && m_hoveredIndex != -1)
    {
        window.draw(m_hoverOutline);
    }
}
