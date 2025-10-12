#include "Map.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

Map::Map()
{
    
}

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
            std::string name = layerNode["name"].get<std::string>();
            int depth = layerNode["depth"].get<int>();
            sf::Texture texture;

            if (!texture.loadFromFile(layerNode["texture"].get<std::string>()))
            {
                std::cerr << "Failed to load texture for layer: " << name << std::endl;
                return false;
            }

            sf::Sprite sprite{ texture };
            sprite.setTexture(texture);
            sprite.setPosition(sf::Vector2f(0.f, 0.f));

            // Move into the vector to avoid copying non-copyable SFML objects
            m_layers.emplace_back(Layer{ name, std::move(texture), std::move(sprite), depth });
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading JSON map config: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void Map::draw(sf::RenderWindow& window)
{
    // Draw layers back to front by depth
    std::sort(m_layers.begin(), m_layers.end(), [](const Layer& a, const Layer& b) { return a.depth < b.depth; });

    for (auto& layer : m_layers)
    {
        window.draw(layer.sprite);
    }
}
