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
            dino.skinTexture = dinoNode["skinTexture"].get<std::string>();
            


            for (auto& pieceNode : dinoNode["pieces"])
            {
                DinosaurData::Piece piece;
                piece.id = pieceNode["id"].get<std::string>();          
                piece.texturePath = pieceNode["texture"].get<std::string>(); 

                dino.pieces.push_back(piece);
            }

            m_dinosaurData.push_back(std::move(dino));
        }

    }
    catch (const std::exception& e)
    {
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

void FossilManager::cacheGridOffsets(float offsetX, float offsetY)
{
    m_cachedOffsetX = offsetX;
    m_cachedOffsetY = offsetY;
}

bool FossilManager::trySpawnCollectible(int row, int col, float tileSize, float windowWidth, float windowHeight)
{
    if (!m_textureLoaded)
    {
        std::cerr << "FossilManager: texture not loaded, cannot spawn collectible\n";
        return false;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> chanceRoll(0, 99);

    if (chanceRoll(gen) >= m_spawnChancePercent)
        return false;

    float xPos = col * tileSize + m_cachedOffsetX + tileSize / 2.0f;
    float yPos = row * tileSize + m_cachedOffsetY + tileSize / 2.0f;

    int collectibleIndex = pickRandomCollectibleIndex();

    Collectible c(m_collectibleTexture, sf::Vector2f(xPos, yPos), collectibleIndex, row, col);

    c.sprite.setScale(sf::Vector2f(0.5f, 0.5f));

    if (collectibleIndex < static_cast<int>(m_collectibleTypes.size()))
    {
        const CollectibleType& cfg = m_collectibleTypes[collectibleIndex];

		c.sprite.setOrigin(sf::Vector2f(cfg.frameWidth / 2.f, cfg.frameHeight / 2.f));
        c.sprite.setTextureRect(sf::IntRect({ cfg.frameIndex * cfg.frameWidth, 0 }, { cfg.frameWidth, cfg.frameHeight }));

        c.monetaryValue = cfg.monetaryValue;
    }
    else
    {
        c.sprite.setTextureRect(sf::IntRect({ collectibleIndex * 64, 0 }, { 64, 64 }));
		c.sprite.setOrigin(sf::Vector2f(32.f, 32.f));
    }

    if (collectibleIndex <= 6)
        assignRandomFossilToPiece(c);

    const char* typeName = (collectibleIndex <= 6) ? "Fossil" :
        (collectibleIndex <= 8) ? "Amber" : "Trash";

    std::cout << "[Drop] " << typeName << " (idx=" << collectibleIndex << ") at tile (" << row << "," << col << ")\n";

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

void FossilManager::assignRandomFossilToPiece(Collectible& collectible)
{
    if (m_dinosaurData.empty())
    {
        std::cerr << "No dinosaur data available for fossil assignment\n";
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dinoDist(0, m_dinosaurData.size() - 1);
    int dinoIndex = dinoDist(gen);
    const DinosaurData& selectedDino = m_dinosaurData[dinoIndex];

    std::uniform_int_distribution<> pieceDist(0, selectedDino.pieces.size() - 1);
    int pieceIndex = pieceDist(gen);
    const DinosaurData::Piece& selectedPiece = selectedDino.pieces[pieceIndex];

    collectible.assignedDinosaurName = selectedDino.name;
    collectible.assignedPieceId = selectedPiece.id;
    collectible.assignedCategory = selectedDino.category;
}

int FossilManager::pickRandomCollectibleIndex()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> roll(0, 99);

    int r = roll(gen);

    if (r < 15)
    {
        std::uniform_int_distribution<> fossilDist(0, 6);
        return fossilDist(gen);
    }
    else if (r < 95)
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