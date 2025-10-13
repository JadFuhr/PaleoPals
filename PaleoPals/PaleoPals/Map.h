#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <json.hpp>

struct LayerType
{
    std::string name;
    sf::Texture texture;
    int depth;
};

class Tile
{
public:
    sf::Sprite sprite;
    int layerDepth = 0;

    // Default constructor
    Tile() = default;

    // Constructor with parameters
    Tile(const sf::Texture& texture, const sf::Vector2f& pos, int depth)
        : layerDepth(depth)
    {
        sprite = sf::Sprite(texture);
        sprite.setPosition(pos);
    }
};

class Map
{
public:
    Map();
    bool loadFromConfig(const std::string& filepath);
    void generateGrid(int rows, int cols, float tileSize);
    void draw(sf::RenderWindow& window);

private:
    std::vector<LayerType> m_layerTypes; // list of all available terrain textures
    std::vector<Tile> m_tiles;           // grid of tiles created from those textures
};
