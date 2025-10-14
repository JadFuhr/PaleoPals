#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <json.hpp>
#include "constants.h"

struct LayerType
{
    std::string name;
    sf::Texture texture;
    int hardness;
};

class Tile
{
public:
    sf::Sprite sprite;
    int layerHardness = 0;

    // Constructor 
    Tile(const sf::Texture& texture, const sf::Vector2f& pos, int hardness)
        : sprite(texture), layerHardness(hardness)
    {
        sprite.setPosition(pos);
    }

    // Delete default constructor 
    Tile() = delete;
};


class Map
{
public:
    Map();
    bool loadFromConfig(const std::string& filepath);
    void generateGrid(int rows, int cols, float tileSize, float windowWidth, float windowHeight);
    void draw(sf::RenderWindow& window);

    void updateHover(const sf::RenderWindow& window, float tileSize, int cols);
    void drawDebug(sf::RenderWindow& window);
    void toggleDebugMode(); 

private:
    std::vector<LayerType> m_layerTypes; // list of all available terrain textures
    std::vector<Tile> m_tiles;           // grid of tiles created from those textures

    int m_hoveredIndex = -1;
    sf::RectangleShape m_hoverOutline;
    bool m_debugMode = false; // toggle debug mode 
};
