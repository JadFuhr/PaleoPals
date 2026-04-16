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

class Player;

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
    sf::Sprite crackedSprite;
    int currentHP = 0;
    int layerHardness = 0;
    int crackedFrameIndex = 0;



    // Constructor 
    Tile(const sf::Texture& texture,
        const sf::Vector2f& pos,
        int hardnessValue,
        const sf::Texture& crackTexture)
        : sprite(texture),
        layerHardness(hardnessValue),
        currentHP(hardnessValue),
		crackedSprite(crackTexture)
    {
        sprite.setPosition(pos);
		crackedSprite.setPosition(pos);
    }

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
    void removeTile(int row, int col, Player& player); 

    int getTileHardness(int row, int col) const;
    int getTileCurrentHP(int row, int col) const;

    void damageTile(int row, int col, int dmg);

    void drawMap(sf::RenderWindow& window);
    void drawDebug(sf::RenderWindow& window);

    void updateHover(const sf::RenderWindow& window, float tileSize, int cols);
    void toggleDebugMode();
    void handleMouseHold(const sf::RenderWindow& window, float tileSize, int cols);

    sf::Vector2f tileToWorld(sf::Vector2i tilePos) const;


    void updateMuseum(sf::RenderWindow& window);
    void updateTrader(sf::RenderWindow& window);

    bool isPointOnTrader(const sf::Vector2f& worldPos) const;

    int getRowCount() const { return m_rows; }
    int getColumnCount() const { return m_cols; }
    float getTileSize() const { return m_tileSize; }

    //fossil system
    FossilManager& getFossilManager() { return m_fossilManager; }
    Museum& getMuseum() { return m_museum; }
    Trader& getTrader() { return m_trader; }

    void addLadder(int row, int col);
    void removeLadder(int row, int col);
    bool hasLadder(int row, int col) const;

    bool isWalkable(int row, int col) const;
    sf::Vector2i worldToTile(sf::Vector2f worldPos) const;


private:

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite{ m_backgroundTexture };
	sf::Texture m_crackedOverlayTexture; 

    int m_rowsGenerated = 0;      
    float m_tileSize = 0.f;

    float m_windowWidth = 0.f;
    float m_windowHeight = 0.f;

    int m_rows = 0;
    int m_cols = 0;

    std::vector<LayerType> m_layerTypes; 
    std::vector<Tile> m_tiles;           
    std::vector<bool> m_ladders; 

    int m_hoveredIndex = -1;
    sf::RectangleShape m_hoverOutline;
    bool m_debugMode = false; 

    Museum m_museum;
    Trader m_trader;
    FossilManager m_fossilManager;


};

#endif // !MAP_H
