#pragma once
#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <json.hpp>
#include "constants.h"
#include "Museum.h"
#include "Trader.h"
#include "Fossil.h"

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
    int determineLayerAtDepth(int row, int totalRows);
    void setupBackground();

    void removeTile(int row, int col);
    int getTileHardness(int row, int col) const;

    void drawMap(sf::RenderWindow& window);

    void updateHover(const sf::RenderWindow& window, float tileSize, int cols);
    void drawDebug(sf::RenderWindow& window);
    void toggleDebugMode();

    void handleMouseHold(const sf::RenderWindow& window, float tileSize, int cols);

    // update museum sprite

    void updateMuseum(sf::RenderWindow& window);
    void updateTrader(sf::RenderWindow& window);

    // return true if the given world coordinate lies on the trader
    bool isPointOnTrader(const sf::Vector2f& worldPos) const;

    int getRowCount() const { return m_rows; }
    int getColumnCount() const { return m_cols; }
    float getTileSize() const { return m_tileSize; }

    //fossil system
    FossilManager& getFossilManager() { return m_fossilManager; }

private:

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite{ m_backgroundTexture };

    int m_rowsGenerated = 0;      // How many rows are currently generated
    float m_tileSize = 0.f;
    //int m_totalRows = 0;  // the TOTAL intended depth
    //int m_batchSize = 50; // how many rows to generate at once

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

    FossilManager m_fossilManager;

    // Ladder supports: store indices of tiles that have ladder/support squares
    std::vector<bool> m_ladders; // parallel to m_tiles, true means ladder present

public:
    void addLadder(int row, int col);
    void removeLadder(int row, int col);
    bool hasLadder(int row, int col) const;
};

#endif // !MAP_H
