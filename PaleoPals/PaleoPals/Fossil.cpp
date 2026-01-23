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
// generateFossils
// Purpose: Places all fossil pieces randomly across the map grid
//------------------------------------------------------------
void FossilManager::generateFossils(int totalRows, int totalCols, float tileSize, float windowWidth, float windowHeight)
{
    // Clear any existing fossils (in case this is called multiple times)
    m_fossilPieces.clear();
    m_fossilTextures.clear();

    // Calculate the offset to center the grid on screen
    // Same calculation used in Map::generateGrid
    float offsetY = windowHeight / 2.0f;
    float offsetX = (windowWidth - (totalCols * tileSize)) / 2.0f;

    // === RANDOM NUMBER GENERATION SETUP ===
    std::random_device rd;  // Hardware random seed
    std::mt19937 gen(rd()); // Mersenne Twister random number generator

    // Skip the first 6 rows (topsoil/near-surface area)
    // Fossils should be deeper underground for realism
    int minRow = 6;

    // Create random distributions for row and column selection
    std::uniform_int_distribution<> rowDist(minRow, totalRows - 1); // Random row from 6 to 199
    std::uniform_int_distribution<> colDist(0, totalCols - 1);       // Random column from 0 to 74

    // load ALL textures BEFORE creating sprites
    // prevents SFML's texture destruction bug
    for (const auto& dino : m_dinosaurData)
    {
        // For each piece of this dinosaur (skull, torso, pelvis, tail)
        for (const auto& piece : dino.pieces)
        {
            sf::Texture texture;

            // Try to load the texture from file
            if (!texture.loadFromFile(piece.texturePath))
            {
                std::cerr << "Failed to load fossil texture: " << piece.texturePath << std::endl;
                continue; // Skip this piece if texture fails to load
            }

            // Store the texture in our permanent storage vector
            // The texture will stay here and won't be destroyed
            m_fossilTextures.push_back(std::move(texture));
        }
    }

    // CREATE FOSSIL PIECES
    // all textures are safely stored, create the actual fossil sprites
    size_t textureIndex = 0; // Track which texture we're currently using

    for (const auto& dino : m_dinosaurData)
    {
        for (const auto& piece : dino.pieces)
        {
            // Safety check: make sure we haven't run out of textures
            if (textureIndex >= m_fossilTextures.size())
            {
                std::cerr << "Texture index out of range\n";
                break;
            }
            // FIND INIQUE POS
            // We need to make sure no two fossils occupy the same tile
            int row, col;
            int attempts = 0;
            const int maxAttempts = 100; // Give up after 100 tries

            do
            {
                // Generate random row and column
                row = rowDist(gen);
                col = colDist(gen);
                attempts++;
            }
            // Keep trying if this position is already occupied
            while (isPositionOccupied(row, col) && attempts < maxAttempts);

            // If we couldn't find a spot after 100 tries, skip this fossil
            if (attempts >= maxAttempts)
            {
                std::cerr << "Could not find free position for " << piece.id << std::endl;
                textureIndex++;
                continue;
            }

            // CALC WORLD POS
            // Convert grid coordinates (row, col) to pixel coordinates (x, y)
            float xPos = col * tileSize + offsetX + (tileSize / 2.0f);
            float yPos = row * tileSize + offsetY + (tileSize / 2.0f);

            // Get a reference to the stored texture 
            const sf::Texture& texRef = m_fossilTextures[textureIndex];

            // === CREATE THE FOSSIL PIECE ===
            FossilPiece fossil(texRef, sf::Vector2f(xPos, yPos), piece.id, dino.name, dino.category, row, col);

            // SCALE SPRITE
            fossil.sprite.setScale(sf::Vector2f(0.15f, 0.15f));

            // === SET ORIGIN TO CENTER ===
            // By default, sprites rotate/scale from top-left corner
            // Setting origin to center makes positioning and rotation more intuitive
            fossil.sprite.setOrigin(sf::Vector2f(texRef.getSize().x / 2.0f, texRef.getSize().y / 2.0f));

            // Add this fossil to our collection
            m_fossilPieces.push_back(std::move(fossil));

            // Move to the next texture for the next fossil piece
            textureIndex++;
        }
    }

    // Confirmation message
    std::cout << "Generated " << m_fossilPieces.size() << " fossil pieces\n";
}

//------------------------------------------------------------
// drawFossils
// Purpose: Renders all fossils to the screen (with frustum culling)
//------------------------------------------------------------
void FossilManager::drawFossils(sf::RenderWindow& window)
{
    // Get the current camera view bounds for culling
    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();
    sf::FloatRect viewBounds(sf::Vector2f(viewCenter.x - viewSize.x / 2.f, viewCenter.y - viewSize.y / 2.f), viewSize);

    // Loop through every fossil piece
    for (auto& fossil : m_fossilPieces)
    {
        // Frustum culling: skip fossils outside the view 
        if (!viewBounds.findIntersection(fossil.sprite.getGlobalBounds()))
        {
            continue;
        }

        // Check discovery status
        if (!fossil.isDiscovered)
        {
            // UNDISCOVERED FOSSILS: semi-transparent (alpha = 100)
            // This is for TESTING 
            fossil.sprite.setColor(sf::Color(255, 255, 255, 100));
        }
        else
        {
            // DISCOVERED FOSSILS: Make fully visible (alpha = 255)
            fossil.sprite.setColor(sf::Color::White);
        }

        // Draw the fossil sprite to the window
        window.draw(fossil.sprite);
    }
}

//------------------------------------------------------------
// getFossilAtTile
// Purpose: Check if there's an undiscovered fossil at a specific grid position
//------------------------------------------------------------
FossilPiece* FossilManager::getFossilAtTile(int row, int col)
{
    // Search through all fossil pieces
    for (auto& fossil : m_fossilPieces)
    {
        // Check if this fossil is at the specified grid position
        // AND hasn't been discovered yet
        if (fossil.gridRow == row && fossil.gridCol == col && !fossil.isDiscovered)
        {
            return &fossil; // Return pointer to this fossil
        }
    }

    // No fossil found at this position
    return nullptr;
}


// getDiscoveredPiecesForDinosaur
// Purpose: Get all discovered pieces for a specific dinosaur species

std::vector<FossilPiece*> FossilManager::getDiscoveredPiecesForDinosaur(const std::string& dinoName)
{
    std::vector<FossilPiece*> pieces;

    // Search through all fossils
    for (auto& fossil : m_fossilPieces)
    {
        // Check if this fossil belongs to the requested dinosaur AND is discovered
        if (fossil.dinosaurName == dinoName && fossil.isDiscovered)
        {
            pieces.push_back(&fossil);
        }
    }

    // Return the collection of discovered pieces
    // If all 4 pieces found, pieces.size() will be 4
    return pieces;
}



int FossilManager::getDiscoveredCount() const
{
    int count = 0;

    // Loop through all fossils and count the discovered ones
    for (const auto& fossil : m_fossilPieces)
    {
        if (fossil.isDiscovered)
        {
            count++;
        }
    }

    return count;
}

//------------------------------------------------------------
// isPositionOccupied
// Purpose: Check if a grid position already has a fossil
// Returns true if a fossil is already there, false if empty prevent fossils from spawning on top of each other
//------------------------------------------------------------
bool FossilManager::isPositionOccupied(int row, int col) const
{
    // Search through all existing fossils
    for (const auto& fossil : m_fossilPieces)
    {
        // If we find a fossil at this exact position
        if (fossil.gridRow == row && fossil.gridCol == col)
        {
            return true; // Position is occupied
        }
    }

    // No fossil found at this position
    return false;
}