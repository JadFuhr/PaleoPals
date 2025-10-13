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
