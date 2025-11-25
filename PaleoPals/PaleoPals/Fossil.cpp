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

bool FossilManager::loadFossilsFromConfig(const std::string& filepath)
{
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON config for fossils: " << filepath << std::endl;
        return false;
    }

    try
    {
        json config;
        file >> config;

        if (!config.contains("dinosaurs"))
        {
            std::cerr << "No dinosaurs section found in config\n";
            return false;
        }

        auto dinosaurs = config["dinosaurs"];

        for (auto& dinoNode : dinosaurs)
        {
            DinosaurData dino;
            dino.name = dinoNode["name"].get<std::string>();
            dino.category = dinoNode["category"].get<std::string>();
            dino.backgroundTexture = dinoNode["background"].get<std::string>();

            for (auto& pieceNode : dinoNode["pieces"])
            {
                DinosaurData::Piece piece;
                piece.id = pieceNode["id"].get<std::string>();
                piece.texturePath = pieceNode["texture"].get<std::string>();
                dino.pieces.push_back(piece);
            }

            m_dinosaurData.push_back(std::move(dino));
        }

        std::cout << "Loaded " << m_dinosaurData.size() << " dinosaur species\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading JSON fossil config: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void FossilManager::generateFossils(int totalRows, int totalCols, float tileSize, float windowWidth, float windowHeight)
{
    m_fossilPieces.clear();
    m_fossilTextures.clear();

    float offsetY = windowHeight / 2.0f;
    float offsetX = (windowWidth - (totalCols * tileSize)) / 2.0f;

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // We'll skip the first few rows (topsoil area) and place fossils deeper
    int minRow = 3; // Start placing fossils from row 3 onwards

    std::uniform_int_distribution<> rowDist(minRow, totalRows - 1);
    std::uniform_int_distribution<> colDist(0, totalCols - 1);

    // Place each fossil piece randomly
    for (const auto& dino : m_dinosaurData)
    {
        for (const auto& piece : dino.pieces)
        {
            // Load texture
            sf::Texture texture;
            if (!texture.loadFromFile(piece.texturePath))
            {
                std::cerr << "Failed to load fossil texture: " << piece.texturePath << std::endl;
                continue;
            }

            // Find a unique position for this fossil
            int row, col;
            int attempts = 0;
            const int maxAttempts = 100;

            do
            {
                row = rowDist(gen);
                col = colDist(gen);
                attempts++;
            } while (isPositionOccupied(row, col) && attempts < maxAttempts);

            if (attempts >= maxAttempts)
            {
                std::cerr << "Could not find free position for " << piece.id << std::endl;
                continue;
            }

            // Calculate world position (center of the tile)
            float xPos = col * tileSize + offsetX + (tileSize / 2.0f);
            float yPos = row * tileSize + offsetY + (tileSize / 2.0f);

            // Store texture (needed to keep texture alive)
            m_fossilTextures.push_back(std::move(texture));
            const sf::Texture& texRef = m_fossilTextures.back();

            // Create fossil piece
            FossilPiece fossil(texRef, sf::Vector2f(xPos, yPos),
                piece.id, dino.name, dino.category, row, col);

            // Scale fossil to fit within tile (slightly smaller than tile)
            float scaleX = (tileSize * 0.8f) / texRef.getSize().x;
            float scaleY = (tileSize * 0.8f) / texRef.getSize().y;
            float scale = std::min(scaleX, scaleY);
            fossil.sprite.setScale(sf::Vector2f(scale, scale));

            // Center the sprite origin
            fossil.sprite.setOrigin(sf::Vector2f(texRef.getSize().x / 2.0f, texRef.getSize().y / 2.0f));

            m_fossilPieces.push_back(std::move(fossil));
        }
    }

    std::cout << "Generated " << m_fossilPieces.size() << " fossil pieces\n";
}

void FossilManager::drawFossils(sf::RenderWindow& window)
{
    for (auto& fossil : m_fossilPieces)
    {
        // Only draw undiscovered fossils (they'll be hidden until dug up)
        // For now, we'll make them semi-transparent so you can see them during testing
        if (!fossil.isDiscovered)
        {
            // Make fossils slightly visible for testing (change alpha to 0 in final game)
            fossil.sprite.setColor(sf::Color(255, 255, 255, 100));
        }
        else
        {
            fossil.sprite.setColor(sf::Color::White);
        }

        window.draw(fossil.sprite);
    }
}

FossilPiece* FossilManager::getFossilAtTile(int row, int col)
{
    for (auto& fossil : m_fossilPieces)
    {
        if (fossil.gridRow == row && fossil.gridCol == col && !fossil.isDiscovered)
        {
            return &fossil;
        }
    }
    return nullptr;
}

std::vector<FossilPiece*> FossilManager::getDiscoveredPiecesForDinosaur(const std::string& dinoName)
{
    std::vector<FossilPiece*> pieces;

    for (auto& fossil : m_fossilPieces)
    {
        if (fossil.dinosaurName == dinoName && fossil.isDiscovered)
        {
            pieces.push_back(&fossil);
        }
    }

    return pieces;
}

int FossilManager::getDiscoveredCount() const
{
    int count = 0;
    for (const auto& fossil : m_fossilPieces)
    {
        if (fossil.isDiscovered)
        {
            count++;
        }
    }
    return count;
}

bool FossilManager::isPositionOccupied(int row, int col) const
{
    for (const auto& fossil : m_fossilPieces)
    {
        if (fossil.gridRow == row && fossil.gridCol == col)
        {
            return true;
        }
    }
    return false;
}
