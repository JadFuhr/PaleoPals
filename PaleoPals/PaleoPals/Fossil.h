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
    int frameWidth = 64;
    int frameHeight = 64;
    int frameIndex = 0;
    int monetaryValue = 0;
};

// Holds data for a dinosaur species from JSON
struct DinosaurData
{
    std::string name;
    std::string category;
    std::string backgroundTexture;
	std::string skinTexture;

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
    int collectibleIndex = 0;           // 0-11
    int gridRow = -1;
    int gridCol = -1;
    bool isPickedUp = false;

    // Fossil-specific data (only used if collectibleIndex is 0-6)
    std::string assignedDinosaurName;
    std::string assignedPieceId;
    std::string assignedCategory;

    // Monetary value (for amber and trash types)
    int monetaryValue = 0;

    Collectible(const sf::Texture& texture,
        const sf::Vector2f& pos,
        int index, int row, int col)
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
     
    // Load collectible type config + dinosaur data from map.json.
    // Also pre-loads the shared sprite-sheet texture.
    bool loadFossilsFromConfig(const std::string& filepath);

    // Must be called once after the map grid is built so spawned
   // collectibles appear at the correct world positions.
    void cacheGridOffsets(float offsetX, float offsetY);


    // Called by Map::removeTile when a tile is fully destroyed.
    // Rolls the spawn chance; if it succeeds, creates a Collectible
    // at the centre of the broken tile.  Returns true on spawn.
    bool trySpawnCollectible(int row, int col, float tileSize, float windowWidth, float windowHeight);

    // Draw all live (not yet picked up) collectibles.
    void drawCollectibles(sf::RenderWindow& window);

    // Return pointer to a live collectible within [range] tiles of
    // (playerRow, playerCol), or nullptr if none found.
    Collectible* getCollectibleNearTile(int playerRow, int playerCol, int range = 1);

    // Direct access to every collectible (for Player::tryPickupCollectible)
    std::vector<Collectible>& getAllCollectibles() { return m_collectibles; }

    // Dinosaur data for MuseumInterior
    const std::vector<DinosaurData>& getDinosaurData() const { return m_dinosaurData; }

    // Legacy helper kept for compatibility
    FossilPiece* getFossilAtTile(int row, int col) { return getCollectibleNearTile(row, col, 0); }

    int getTotalCollectibleCount() const { return static_cast<int>(m_collectibles.size()); }

    // Tune drop rate (0-100).  Default = 40 (40% chance per broken tile).
    void setSpawnChance(int percent) { m_spawnChancePercent = percent; }



private:
    std::vector<DinosaurData> m_dinosaurData;
    sf::Texture m_collectibleTexture;
    std::vector<Collectible> m_collectibles;
    std::vector<CollectibleType> m_collectibleTypes;  // Store config data for each collectible type

    bool m_textureLoaded = false;

    // Cached from Map after the grid is generated
    float m_cachedOffsetX = 0.f;
    float m_cachedOffsetY = 0.f;

    int m_spawnChancePercent = 45;

    // Helper to pick a random dinosaur and piece for fossil collectibles
    void assignRandomFossilToPiece(Collectible& collectible);

    int  pickRandomCollectibleIndex();

};

#endif // FOSSIL_H