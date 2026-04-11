#include "Fossil.h"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <json.hpp>

using json = nlohmann::json;

FossilManager::FossilManager()
{
}

//------------------------------------------------------------
// loadFossilsFromConfig
// Purpose: Reads the JSON config file and extracts dinosaur data
//------------------------------------------------------------
bool FossilManager::loadFossilsFromConfig(const std::string& filepath)
{
    // Open the JSON file for reading
    std::ifstream file(filepath);

    // Check if file opened successfully
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON config for fossils: " << filepath << std::endl;
        return false;
    }

    try
    {
        // Parse the JSON file into a json object
        json config;
        file >> config;

        // Load collectible types from config
        if (config.contains("collectibles"))
        {
            auto collectibles = config["collectibles"];
            for (auto& collectNode : collectibles)
            {
                CollectibleType collectType;
                collectType.index = collectNode["index"].get<int>();
                collectType.name = collectNode["name"].get<std::string>();
                collectType.type = collectNode["type"].get<std::string>();
                collectType.texture = collectNode["texture"].get<std::string>();
                collectType.frameWidth = collectNode["frameWidth"].get<int>();
                collectType.frameHeight = collectNode["frameHeight"].get<int>();
                collectType.frameIndex = collectNode["frameIndex"].get<int>();
                collectType.monetaryValue = collectNode["monetaryValue"].get<int>();
                
                m_collectibleTypes.push_back(collectType);
            }
            std::cout << "Loaded " << m_collectibleTypes.size() << " collectible types from config\n";
        }

        // Check if the "dinosaurs" section exists in the JSON
        if (!config.contains("dinosaurs"))
        {
            std::cerr << "No dinosaurs section found in config\n";
            return false;
        }

        // Get the array of dinosaurs from the JSON
        auto dinosaurs = config["dinosaurs"];

        // Loop through each dinosaur in the array
        for (auto& dinoNode : dinosaurs)
        {
            // Create a new DinosaurData struct to hold this dino's info
            DinosaurData dino;

            // Extract basic info: name, category (Carnivore/Herbivore/Pterosaur)
            dino.name = dinoNode["name"].get<std::string>();
            dino.category = dinoNode["category"].get<std::string>();
            dino.backgroundTexture = dinoNode["background"].get<std::string>();

            // Loop through each piece (skull, torso, pelvis, tail) for this dinosaur
            for (auto& pieceNode : dinoNode["pieces"])
            {
                // Create a Piece struct to hold this fossil piece's data
                DinosaurData::Piece piece;
                piece.id = pieceNode["id"].get<std::string>();          
                piece.texturePath = pieceNode["texture"].get<std::string>(); 

                // Add this piece to the dinosaur's list of pieces
                dino.pieces.push_back(piece);
            }

            // Add this complete dinosaur (with all its pieces) to our data collection
            m_dinosaurData.push_back(std::move(dino));
        }

        // Success message showing how many species we loaded
        //std::cout << "Loaded " << m_dinosaurData.size() << " dinosaur species\n";
    }
    catch (const std::exception& e)
    {
        // If any error occurs during JSON parsing, catch it and report
        std::cerr << "Error loading JSON fossil config: " << e.what() << std::endl;
        return false;
    }

    if (!m_collectibleTypes.empty())
    {
        const std::string& sheetPath = m_collectibleTypes[0].texture;

        if (!m_collectibleTexture.loadFromFile(sheetPath))
        {
            std::cerr << "Failed to load collectibles sheet: " << sheetPath << "\n";
            return false;
        }

        m_textureLoaded = true;
        std::cout << "Collectibles sheet loaded: " << sheetPath << "\n";
    }
    else
    {
        std::cerr << "No collectible types loaded, cannot load texture\n";
        return false;
    }

    return true;
}


// Called by Map after generateGrid
void FossilManager::cacheGridOffsets(float offsetX, float offsetY)
{
    m_cachedOffsetX = offsetX;
    m_cachedOffsetY = offsetY;
}


// Called every time a tile is fully destroyed.
// Rolls m_spawnChancePercent; on success, builds a Collectible
bool FossilManager::trySpawnCollectible(int row, int col, float tileSize, float windowWidth, float windowHeight)
{
    if (!m_textureLoaded)
    {
        std::cerr << "FossilManager: texture not loaded, cannot spawn collectible\n";
        return false;
    }

    // Drop-chance roll
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> chanceRoll(0, 99);

    if (chanceRoll(gen) >= m_spawnChancePercent)
        return false;

    // World position = centre of the tile that just broke
    float xPos = col * tileSize + m_cachedOffsetX + tileSize / 2.0f;
    float yPos = row * tileSize + m_cachedOffsetY + tileSize / 2.0f;

    int collectibleIndex = pickRandomCollectibleIndex();

    // Build the Collectible
    Collectible c(m_collectibleTexture, sf::Vector2f(xPos, yPos), collectibleIndex, row, col);

    // 64x64, displayed at half size
    c.sprite.setScale(sf::Vector2f(0.5f, 0.5f));

    if (collectibleIndex < static_cast<int>(m_collectibleTypes.size()))
    {
        const CollectibleType& cfg = m_collectibleTypes[collectibleIndex];

        c.sprite.setTextureRect(sf::IntRect({ cfg.frameIndex * cfg.frameWidth, 0 }, { cfg.frameWidth, cfg.frameHeight }));

        c.monetaryValue = cfg.monetaryValue;
    }
    else
    {
        // Fallback if config table is incomplete
        c.sprite.setTextureRect(sf::IntRect({ collectibleIndex * 64, 0 }, { 64, 64 }));
    }

    // Fossil: assign a random dinosaur piece
    if (collectibleIndex <= 6)
        assignRandomFossilToPiece(c);

    // Debug log
    const char* typeName = (collectibleIndex <= 6) ? "Fossil" :
        (collectibleIndex <= 8) ? "Amber" : "Trash";
    std::cout << "[Drop] " << typeName << " (idx=" << collectibleIndex
        << ") at tile (" << row << "," << col << ")\n";

    m_collectibles.push_back(std::move(c));
    return true;
}

void FossilManager::drawCollectibles(sf::RenderWindow& window)
{
    sf::View view = window.getView();
    sf::Vector2f vc = view.getCenter();
    sf::Vector2f vs = view.getSize();
    const float pad = 100.f;

    sf::FloatRect viewBounds(
        sf::Vector2f(vc.x - vs.x / 2.f - pad, vc.y - vs.y / 2.f - pad),
        sf::Vector2f(vs.x + pad * 2.f, vs.y + pad * 2.f));

    for (auto& c : m_collectibles)
    {
        if (c.isPickedUp) continue;

        if (!viewBounds.findIntersection(c.sprite.getGlobalBounds()))
            continue;

        c.sprite.setColor(sf::Color::White);
        window.draw(c.sprite);
    }
}

Collectible* FossilManager::getCollectibleNearTile(int playerRow, int playerCol, int range)
{
    for (auto& c : m_collectibles)
    {
        if (c.isPickedUp) continue;
        if (std::abs(c.gridRow - playerRow) <= range &&
            std::abs(c.gridCol - playerCol) <= range)
        {
            return &c;
        }
    }
    return nullptr;
}

//------------------------------------------------------------
// assignRandomFossilToPiece
// Purpose: Randomly assign a dinosaur piece to a fossil collectible
//------------------------------------------------------------
void FossilManager::assignRandomFossilToPiece(Collectible& collectible)
{
    if (m_dinosaurData.empty())
    {
        std::cerr << "No dinosaur data available for fossil assignment\n";
        return;
    }

    // Random number generation
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Pick a random dinosaur
    std::uniform_int_distribution<> dinoDist(0, m_dinosaurData.size() - 1);
    int dinoIndex = dinoDist(gen);
    const DinosaurData& selectedDino = m_dinosaurData[dinoIndex];

    // Pick a random piece from that dinosaur
    std::uniform_int_distribution<> pieceDist(0, selectedDino.pieces.size() - 1);
    int pieceIndex = pieceDist(gen);
    const DinosaurData::Piece& selectedPiece = selectedDino.pieces[pieceIndex];

    // Assign to collectible
    collectible.assignedDinosaurName = selectedDino.name;
    collectible.assignedPieceId = selectedPiece.id;
    collectible.assignedCategory = selectedDino.category;
}

// pickRandomCollectibleIndex
// Weighted distribution:
//   Fossils (0-6)  60%
//   Amber   (7-8)  25%   (small amber more common than large)
//   Trash   (9-11) 15%
int FossilManager::pickRandomCollectibleIndex()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> roll(0, 99);

    int r = roll(gen);

    if (r < 60)
    {
        std::uniform_int_distribution<> fossilDist(0, 6);
        return fossilDist(gen);
    }
    else if (r < 85)
    {
        // 60% small (7), 40% large (8)
        std::uniform_int_distribution<> amberRoll(0, 9);
        return (amberRoll(gen) < 6) ? 7 : 8;
    }
    else
    {
        std::uniform_int_distribution<> trashDist(9, 11);
        return trashDist(gen);
    }
}