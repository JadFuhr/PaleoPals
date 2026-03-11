#pragma once
#ifndef FOSSIL_H
#define FOSSIL_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>

// Represents a single collectible type configuration
struct CollectibleType
{
    int index;
    std::string name;
    std::string type; // "fossil", "amber", "trash"
    std::string texture;
    int frameWidth;
    int frameHeight;
    int frameIndex;
    int monetaryValue;
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

// Represents a single collectible in the world
class Collectible
{
public:
    sf::Sprite sprite;
    int collectibleIndex;           // 0-11: which collectible type
    int gridRow;                    // Which tile row it's behind
    int gridCol;                    // Which tile col it's behind
    bool isDiscovered = false;

    // Fossil-specific data (only used if collectibleIndex is 0-6)
    std::string assignedDinosaurName;
    std::string assignedPieceId;
    std::string assignedCategory;

    // Monetary value (for amber and trash types)
    int monetaryValue = 0;

    Collectible(const sf::Texture& texture, const sf::Vector2f& pos, int index, int row, int col)
        : sprite(texture), collectibleIndex(index), gridRow(row), gridCol(col)
    {
        sprite.setPosition(pos);
    }

    Collectible() = delete;
};

// Legacy type alias for compatibility
using FossilPiece = Collectible;

// Manages all collectibles in the game
class FossilManager
{
public:
    FossilManager();

    bool loadFossilsFromConfig(const std::string& filepath);

    void generateFossils(int totalRows, int totalCols, float tileSize,
        float windowWidth, float windowHeight, int collectiblesPerTile = 1);

    void drawFossils(sf::RenderWindow& window);

    // Check if a collectible exists at this tile position
    Collectible* getCollectibleAtTile(int row, int col);
    
    // Legacy method name for compatibility
    FossilPiece* getFossilAtTile(int row, int col) { return getCollectibleAtTile(row, col); }

    // Get all discovered fossil pieces for a specific dinosaur
    std::vector<Collectible*> getDiscoveredPiecesForDinosaur(const std::string& dinoName);

    // Check if all pieces of a dinosaur have been collected
    bool hasDinosaurSkeleton(const std::string& dinoName) const;

    // Get all dinosaurs and which ones are complete
    const std::map<std::string, int>& getCollectedPiecesPerDino() const { return m_collectedPiecesPerDino; }

    int getTotalCollectibleCount() const { return m_collectibles.size(); }
    int getDiscoveredCount() const;

    // Get dinosaur data for museum display
    const std::vector<DinosaurData>& getDinosaurData() const { return m_dinosaurData; }

private:
    std::vector<DinosaurData> m_dinosaurData;
    std::vector<sf::Texture> m_collectibleTextures;
    std::vector<Collectible> m_collectibles;
    std::vector<CollectibleType> m_collectibleTypes;  // Store config data for each collectible type
    
    // Track discovered pieces per dinosaur (for quick completion check)
    std::map<std::string, int> m_collectedPiecesPerDino;

    bool isPositionOccupied(int row, int col) const;

    // Helper to pick a random dinosaur and piece for fossil collectibles
    void assignRandomFossilToPiece(Collectible& collectible);

    // Separate generation functions for different collectible types
    void generateFossilCollectibles(int totalRows, int totalCols, float tileSize, 
        float windowWidth, float windowHeight, int fossilCount);
    
    void generateAmberAndTrashCollectibles(int totalRows, int totalCols, float tileSize, 
        float windowWidth, float windowHeight, int amberTrashCount);
};

#endif // FOSSIL_H