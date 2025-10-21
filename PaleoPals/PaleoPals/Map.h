#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <json.hpp>
#include "constants.h"
#include "Museum.h"
#include "Trader.h"

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

    Tile(const sf::Texture& texture, const sf::Vector2f& pos, int hardnessValue)
        : sprite(texture), layerHardness(hardnessValue)
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
    bool loadMapFromConfig(const std::string& filepath);
    void generateGrid(int rows, int cols, float tileSize, float windowWidth, float windowHeight);

   // void updateIfNeeded(float playerY);

    void drawMap(sf::RenderWindow& window);

    void updateHover(const sf::RenderWindow& window, float tileSize, int cols);
    void drawDebug(sf::RenderWindow& window);
    void toggleDebugMode(); 

    // update museum sprite

    void updateMuseum(sf::RenderWindow& window);
    void updateTrader(sf::RenderWindow& window);

    int getRowCount() const { return m_rows; }
    int getColumnCount() const { return m_cols; }
    float getTileSize() const { return m_tileSize; }

private:
    int m_rowsGenerated = 0;      // How many rows are currently generated
    float m_tileSize = 0.f;

    float m_windowWidth = 0.f;
    float m_windowHeight = 0.f;

    int m_rows = 0;
    int m_cols = 0;

    std::vector<LayerType> m_layerTypes; // list of all terrain textures
    std::vector<Tile> m_tiles;           // grid of tiles created from those textures

    int m_hoveredIndex = -1;
    sf::RectangleShape m_hoverOutline;
    bool m_debugMode = false; // toggle debug mode 

    Museum m_museum;
    Trader m_trader;
};
