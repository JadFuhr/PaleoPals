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
        std::cout << "Loaded " << m_dinosaurData.size() << " dinosaur species\n";
    }
    catch (const std::exception& e)
    {
        // If any error occurs during JSON parsing, catch it and report
        std::cerr << "Error loading JSON fossil config: " << e.what() << std::endl;
        return false;
    }

    return true;
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

//------------------------------------------------------------
// generateFossils
// Purpose: Places collectibles randomly across the map grid
// Splits work into fossil generation and amber/trash generation
// Parameters:
//   collectiblesPerTile - How many collectibles per 100 tiles (0-100%)
//                        1 = sparse, 5 = moderate, 10+ = dense
//------------------------------------------------------------
void FossilManager::generateFossils(int totalRows, int totalCols, float tileSize, float windowWidth, float windowHeight, int collectiblesPerTile)
{
    // Clear any existing collectibles (in case this is called multiple times)
    m_collectibles.clear();
    m_collectibleTextures.clear();
    m_collectedPiecesPerDino.clear();

    // === LOAD COLLECTIBLES SHEET TEXTURE ===
    sf::Texture collectiblesSheetTexture;
    if (!collectiblesSheetTexture.loadFromFile("ASSETS/IMAGES/Fossils/1. Pickups/Collectibles_Sheet.png"))
    {
        std::cerr << "Failed to load collectibles sheet texture\n";
        return;
    }
    m_collectibleTextures.push_back(std::move(collectiblesSheetTexture));

    // === CALCULATE HOW MANY COLLECTIBLES TO SPAWN ===
    int totalTiles = totalRows * totalCols;
    int collectibleCount = (totalTiles * collectiblesPerTile) / 100;
    
    // Clamp to reasonable values
    collectibleCount = std::max(10, std::min(collectibleCount, totalTiles / 2));

    std::cout << "Spawning " << collectibleCount << " total collectibles (spawn rate: " << collectiblesPerTile << "% per 100 tiles)\n";

    // === CALCULATE DISTRIBUTION: 70% fossils, 20% amber, 10% trash ===
    int fossilCount = (collectibleCount * 70) / 100;
    int amberTrashCount = collectibleCount - fossilCount;

    // === GENERATE EACH TYPE SEPARATELY ===
    generateFossilCollectibles(totalRows, totalCols, tileSize, windowWidth, windowHeight, fossilCount);
    generateAmberAndTrashCollectibles(totalRows, totalCols, tileSize, windowWidth, windowHeight, amberTrashCount);

    // Initialize collected pieces per dino counter
    for (const auto& dino : m_dinosaurData)
    {
        m_collectedPiecesPerDino[dino.name] = 0;
    }

    // Count collectible types for debug
    int fossilCountFinal = 0, amberCount = 0, trashCount = 0;
    for (const auto& c : m_collectibles)
    {
        if (c.collectibleIndex < 7) fossilCountFinal++;
        else if (c.collectibleIndex < 9) amberCount++;
        else trashCount++;
    }

    std::cout << "=== FINAL COLLECTIBLE COUNT ===\n";
    std::cout << "Total: " << m_collectibles.size() << " | Fossils: " << fossilCountFinal << " | Amber: " << amberCount << " | Trash: " << trashCount << "\n";
}

//------------------------------------------------------------
// generateFossilCollectibles
// Purpose: Spawn only fossil pieces (indices 0-6)
//------------------------------------------------------------
void FossilManager::generateFossilCollectibles(int totalRows, int totalCols, float tileSize, 
    float windowWidth, float windowHeight, int fossilCount)
{
    // Calculate the offset to center the grid on screen
    float offsetY = windowHeight / 2.0f;
    float offsetX = (windowWidth - (totalCols * tileSize)) / 2.0f;

    // === RANDOM NUMBER GENERATION ===
    std::random_device rd;
    std::mt19937 gen(rd());

    // Skip the first 6 rows (topsoil/near-surface area)
    int minRow = 6;

    std::uniform_int_distribution<> rowDist(minRow, totalRows - 1);
    std::uniform_int_distribution<> colDist(0, totalCols - 1);
    std::uniform_int_distribution<> fossilTypeDist(0, 6); // 7 fossil types

    const sf::Texture& texRef = m_collectibleTextures[0];

    std::cout << "Generating " << fossilCount << " fossil collectibles...\n";

    // === SPAWN FOSSILS ===
    for (int i = 0; i < fossilCount; ++i)
    {
        // Find unique position
        int row, col;
        int attempts = 0;
        const int maxAttempts = 50;

        do
        {
            row = rowDist(gen);
            col = colDist(gen);
            attempts++;
        } while (isPositionOccupied(row, col) && attempts < maxAttempts);

        if (attempts >= maxAttempts)
            continue;

        // Calculate world position
        float xPos = col * tileSize + offsetX + (tileSize / 2.0f);
        float yPos = row * tileSize + offsetY + (tileSize / 2.0f);

        // Random fossil type (0-6)
        int collectibleIndex = fossilTypeDist(gen);

        // === CREATE THE COLLECTIBLE ===
        Collectible collectible(texRef, sf::Vector2f(xPos, yPos), collectibleIndex, row, col);

        // SCALE & POSITION
        collectible.sprite.setScale(sf::Vector2f(0.5f, 0.5f));
        collectible.sprite.setOrigin(sf::Vector2f(texRef.getSize().x / 2.0f, texRef.getSize().y / 2.0f));

        // === SET TEXTURE RECT FOR FRAME ===
        int frameX = (collectibleIndex % 4) * 64;
        int frameY = (collectibleIndex / 4) * 64;
        collectible.sprite.setTextureRect(sf::IntRect({ frameX, frameY }, { 64, 64 }));

        // Assign random dinosaur piece
        assignRandomFossilToPiece(collectible);
        collectible.monetaryValue = 0;

        // Add to collection
        m_collectibles.push_back(std::move(collectible));
    }
}

//------------------------------------------------------------
// generateAmberAndTrashCollectibles
// Purpose: Spawn amber (7-8) and trash (9-11) pieces
//------------------------------------------------------------
void FossilManager::generateAmberAndTrashCollectibles(int totalRows, int totalCols, float tileSize, 
    float windowWidth, float windowHeight, int amberTrashCount)
{
    // Calculate the offset to center the grid on screen
    float offsetY = windowHeight / 2.0f;
    float offsetX = (windowWidth - (totalCols * tileSize)) / 2.0f;

    // === RANDOM NUMBER GENERATION ===
    std::random_device rd;
    std::mt19937 gen(rd());

    // Skip the first 6 rows (topsoil/near-surface area)
    int minRow = 6;

    std::uniform_int_distribution<> rowDist(minRow, totalRows - 1);
    std::uniform_int_distribution<> colDist(0, totalCols - 1);
    std::uniform_int_distribution<> amberVsTrashDist(7, 11); // 60% amber, 40% trash

    const sf::Texture& texRef = m_collectibleTextures[0];

    std::cout << "Generating " << amberTrashCount << " amber/trash collectibles...\n";

    // === SPAWN AMBER AND TRASH ===
    for (int i = 0; i < amberTrashCount; ++i)
    {
        // Find unique position
        int row, col;
        int attempts = 0;
        const int maxAttempts = 50;

        do
        {
            row = rowDist(gen);
            col = colDist(gen);
            attempts++;
        } while (isPositionOccupied(row, col) && attempts < maxAttempts);

        if (attempts >= maxAttempts)
            continue;

        // Calculate world position
        float xPos = col * tileSize + offsetX + (tileSize / 2.0f);
        float yPos = row * tileSize + offsetY + (tileSize / 2.0f);

        // === DETERMINE TYPE: Amber or Trash ===
        int collectibleIndex;
        int roll = amberVsTrashDist(gen);
        
        if (roll < 6)
        {
            // 60% Amber
            if (roll < 3)
                collectibleIndex = 7; // Small amber
            else
                collectibleIndex = 8; // Large amber
        }
        else
        {
            // 40% Trash (9-11)
            std::uniform_int_distribution<> trashDist(9, 11);
            collectibleIndex = trashDist(gen);
        }

        // === CREATE THE COLLECTIBLE ===
        Collectible collectible(texRef, sf::Vector2f(xPos, yPos), collectibleIndex, row, col);

        // SCALE & POSITION
        collectible.sprite.setScale(sf::Vector2f(0.5f, 0.5f));
        collectible.sprite.setOrigin(sf::Vector2f(texRef.getSize().x / 2.0f, texRef.getSize().y / 2.0f));

        // === SET TEXTURE RECT FOR FRAME ===
        int frameX = (collectibleIndex % 4) * 64;
        int frameY = (collectibleIndex / 4) * 64;
        collectible.sprite.setTextureRect(sf::IntRect({ frameX, frameY }, { 64, 64 }));

        // === ASSIGN MONETARY VALUE ===
        if (collectibleIndex == 7)
        {
            collectible.monetaryValue = 50;
            std::cout << "Spawned Small Amber at (" << row << "," << col << ")\n";
        }
        else if (collectibleIndex == 8)
        {
            collectible.monetaryValue = 100;
            std::cout << "Spawned Large Amber at (" << row << "," << col << ")\n";
        }
        else // Trash (9-11)
        {
            collectible.monetaryValue = 0;
            std::cout << "Spawned Trash (idx=" << collectibleIndex << ") at (" << row << "," << col << ")\n";
        }

        // Add to collection
        m_collectibles.push_back(std::move(collectible));
    }
}

//------------------------------------------------------------
// drawFossils
// Purpose: Renders all collectibles to the screen (with frustum culling)
//------------------------------------------------------------
void FossilManager::drawFossils(sf::RenderWindow& window)
{
    // Get the current camera view bounds for culling
    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();
    
    // Calculate view bounds with proper FloatRect constructor (left, top, width, height)
    float padding = 100.0f;
    float viewLeft = viewCenter.x - viewSize.x / 2.0f - padding;
    float viewTop = viewCenter.y - viewSize.y / 2.0f - padding;
    float viewWidth = viewSize.x + padding * 2.0f;
    float viewHeight = viewSize.y + padding * 2.0f;
    
    sf::FloatRect viewBounds(sf::Vector2f(viewLeft, viewTop), sf::Vector2f(viewWidth, viewHeight));

    int drawnCount = 0, culledCount = 0;

    // Loop through every collectible
    for (auto& collectible : m_collectibles)
    {
        // Frustum culling: skip collectibles outside the view bounds
        if (!viewBounds.findIntersection(collectible.sprite.getGlobalBounds()))
        {
            culledCount++;
            continue;
        }

        drawnCount++;

        // Check discovery status
        if (!collectible.isDiscovered)
        {
            // UNDISCOVERED COLLECTIBLES: semi-transparent (alpha = 200 for better visibility)
            collectible.sprite.setColor(sf::Color(255, 255, 255, 200));
        }
        else
        {
            // DISCOVERED COLLECTIBLES: Make fully visible (alpha = 255)
            collectible.sprite.setColor(sf::Color::White);
        }

        // Draw the collectible sprite to the window
        window.draw(collectible.sprite);
    }

    // Optional: Uncomment for culling stats
    // std::cout << "Collectibles - Drawn: " << drawnCount << " | Culled: " << culledCount << "\n";
}

//------------------------------------------------------------
// getCollectibleAtTile
// Purpose: Check if there's an undiscovered collectible at a specific grid position
//------------------------------------------------------------
Collectible* FossilManager::getCollectibleAtTile(int row, int col)
{
    // Search through all collectibles
    for (auto& collectible : m_collectibles)
    {
        // Check if this collectible is at the specified grid position
        // AND hasn't been discovered yet
        if (collectible.gridRow == row && collectible.gridCol == col && !collectible.isDiscovered)
        {
            return &collectible; // Return pointer to this collectible
        }
    }

    // No collectible found at this position
    return nullptr;
}


// getDiscoveredPiecesForDinosaur
// Purpose: Get all discovered pieces for a specific dinosaur species
std::vector<Collectible*> FossilManager::getDiscoveredPiecesForDinosaur(const std::string& dinoName)
{
    std::vector<Collectible*> pieces;

    // Search through all collectibles
    for (auto& collectible : m_collectibles)
    {
        // Check if this collectible is a fossil (index 0-6)
        // AND belongs to the requested dinosaur
        // AND is discovered
        if (collectible.collectibleIndex < 7 && 
            collectible.assignedDinosaurName == dinoName && 
            collectible.isDiscovered)
        {
            pieces.push_back(&collectible);
        }
    }

    // Return the collection of discovered pieces
    // If all 4 pieces found, pieces.size() will be 4
    return pieces;
}

int FossilManager::getDiscoveredCount() const
{
    int count = 0;

    // Loop through all collectibles and count the discovered ones
    for (const auto& collectible : m_collectibles)
    {
        if (collectible.isDiscovered)
        {
            count++;
        }
    }

    return count;
}

//------------------------------------------------------------
// isPositionOccupied
// Purpose: Check if a grid position already has a collectible
// Returns true if a collectible is already there, false if empty prevent collectibles from spawning on top of each other
//------------------------------------------------------------
bool FossilManager::isPositionOccupied(int row, int col) const
{
    // Search through all existing collectibles
    for (const auto& collectible : m_collectibles)
    {
        // If we find a collectible at this exact position
        if (collectible.gridRow == row && collectible.gridCol == col)
        {
            return true; // Position is occupied
        }
    }

    // No collectible found at this position
    return false;
}

//------------------------------------------------------------
// hasDinosaurSkeleton
// Purpose: Check if all 4 pieces of a dinosaur have been collected
//------------------------------------------------------------
bool FossilManager::hasDinosaurSkeleton(const std::string& dinoName) const
{
    // Get the dinosaur data
    for (const auto& dino : m_dinosaurData)
    {
        if (dino.name == dinoName)
        {
            // Found the dinosaur - it needs all of its pieces
            // Count how many unique pieces we have discovered
            std::vector<std::string> foundPieces;

            for (const auto& collectible : m_collectibles)
            {
                if (collectible.collectibleIndex < 7 && 
                    collectible.assignedDinosaurName == dinoName && 
                    collectible.isDiscovered)
                {
                    // Check if we already have this piece ID
                    bool hasPiece = false;
                    for (const auto& found : foundPieces)
                    {
                        if (found == collectible.assignedPieceId)
                        {
                            hasPiece = true;
                            break;
                        }
                    }

                    // Add new unique piece
                    if (!hasPiece)
                    {
                        foundPieces.push_back(collectible.assignedPieceId);
                    }
                }
            }

            // Check if we have all pieces (4 for each dino)
            return foundPieces.size() >= dino.pieces.size();
        }
    }

    // Dinosaur not found
    return false;
}