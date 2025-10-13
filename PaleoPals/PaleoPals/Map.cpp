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

void Map::generateGrid(int rows, int cols, float tileSize)
{
    m_tiles.clear();

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            // Pick layer based on depth (e.g. top rows = topsoil, deeper = bedrock)
            int layerIndex = std::min(row / 4, (int)m_layerTypes.size() - 1); // adjust 4 to control layer thickness
            const LayerType& layerType = m_layerTypes[layerIndex];

            Tile tile;
            tile.sprite.setTexture(layerType.texture);
            tile.sprite.setPosition(sf::Vector2f(col * tileSize, row * tileSize));
            tile.sprite.setScale(sf::Vector2f(tileSize / layerType.texture.getSize().x, tileSize / layerType.texture.getSize().y));
            tile.layerDepth = layerType.depth;

            m_tiles.push_back(tile);
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
