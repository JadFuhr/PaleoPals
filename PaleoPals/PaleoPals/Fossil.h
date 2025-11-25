#pragma once
#ifndef FOSSIL_H
#define FOSSIL_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Represents a single fossil piece in the world
class FossilPiece
{
public:
    sf::Sprite sprite;
    std::string fossilId;        
    std::string dinosaurName;    
    std::string category;        
    bool isDiscovered = false;
    int gridRow;                 // Which tile row it's behind
    int gridCol;                 // Which tile col it's behind

    FossilPiece(const sf::Texture& texture, const sf::Vector2f& pos,
        const std::string& id, const std::string& dinoName,
        const std::string& cat, int row, int col)
        : sprite(texture), fossilId(id), dinosaurName(dinoName),
        category(cat), gridRow(row), gridCol(col)
    {
        sprite.setPosition(pos);
    }

    FossilPiece() = delete;
};

// Holds data for a dinosaur species from JSON
struct DinosaurData
{
    std::string name;
    std::string category;
    std::string backgroundTexture;

    struct Piece
    {
        std::string id;
        std::string texturePath;
    };

    std::vector<Piece> pieces;
};

// Manages all fossils in the game
class FossilManager
{
public:
    FossilManager();

    bool loadFossilsFromConfig(const std::string& filepath);

    void generateFossils(int totalRows, int totalCols, float tileSize,
        float windowWidth, float windowHeight);

    void drawFossils(sf::RenderWindow& window);

    // Check if a fossil exists at this tile position
    FossilPiece* getFossilAtTile(int row, int col);

    // Get all discovered fossils for a specific dinosaur
    std::vector<FossilPiece*> getDiscoveredPiecesForDinosaur(const std::string& dinoName);

    int getTotalFossilCount() const { return m_fossilPieces.size(); }
    int getDiscoveredCount() const;

private:
    std::vector<DinosaurData> m_dinosaurData;
    std::vector<sf::Texture> m_fossilTextures;
    std::vector<FossilPiece> m_fossilPieces;

    bool isPositionOccupied(int row, int col) const;
};

#endif // FOSSIL_H