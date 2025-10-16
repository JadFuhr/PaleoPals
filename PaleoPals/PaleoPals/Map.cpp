#include "Map.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

Map::Map() {}

bool Map::loadFromConfig(const std::string& filepath)
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
            layer.depth = layerNode["depth"].get<int>();

            if (!layer.texture.loadFromFile(layerNode["texture"].get<std::string>()))
            {
                std::cerr << "Failed to load texture for layer: " << layer.name << std::endl;
                return false;
            }

            m_layerTypes.push_back(std::move(layer));
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
    m_tiles.clear();

    float totalGridHeight = rows * tileSize;
    float totalGridWidth = cols * tileSize;


    // Start at the middle of the screen
    float offsetY = windowHeight / 2.0f;

    float offsetX = (windowWidth - totalGridWidth) / 2.0f;

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            int layerIndex = 0; // default topsoil

            if (row == 0)
            {
                layerIndex = 0; // topsoil
            }
            else if (row < 7)
            {
                layerIndex = 1; // sediment (rows 1–6)
            }               
            else if (row < 12)
            {
                layerIndex = 2; // rock (rows 7–11)
            }
            else
            {
                layerIndex = 3; // bedrock (rows 12+)
            }


            const LayerType& layerType = m_layerTypes[layerIndex];

            m_tiles.emplace_back(layerType.texture, sf::Vector2f(col * tileSize + offsetX, row * tileSize + offsetY), layerType.depth);

            m_tiles.back().sprite.setScale(sf::Vector2f(tileSize / layerType.texture.getSize().x, tileSize / layerType.texture.getSize().y));
        }
    }
}


void Map::draw(sf::RenderWindow& window)
{
    for (auto& tile : m_tiles)
    {
        window.draw(tile.sprite);
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
    if (!m_debugMode) return; // (only if debug is activated)

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
            for (const auto& layer : m_layerTypes)
            {
                if (layer.depth == hoveredTile.layerDepth)
                {
                    tileName = layer.name;
                    break;
                }
            }

            std::cout << "Tile type: " << tileName  << ", " << tileSize << " px " << std::endl;
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
