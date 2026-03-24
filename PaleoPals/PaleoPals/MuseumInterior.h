#pragma once
#ifndef MUSEUM_INTERIOR_H
#define MUSEUM_INTERIOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include "Fossil.h"

class MuseumInterior
{
public:
    MuseumInterior();

    // Call once after FossilManager has loaded its dino data.
    // all piece textures from the paths stored in DinosaurData.
    bool loadAssets(const std::vector<DinosaurData>& dinoData);

    // Called by Game when the player picks up a fossil piece
    void onFossilCollected(const std::string& dinoName, const std::string& pieceId);

    void open();
    void close();
    bool isOpen() const { return m_open; }

    // Returns true if the Back button was clicked
    bool handleClick(const sf::Vector2f& screenPos);

    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

private:
    void updateButtonPositions(const sf::RenderWindow& window);
    bool containsPoint(const sf::Sprite& sprite, const sf::Vector2f& pt) const;
    int  pieceIdToIndex(const std::string& pieceId) const;

    // state
    bool m_open = false;
    int  m_currentDinoIndex = 0;

    // per-dino display data
    struct DinoDisplay
    {
        std::string name;
        sf::Texture backgroundTex;
        sf::Sprite  backgroundSprite;

        // [0]=skull  [1]=torso  [2]=pelvis  [3]=tail
        std::array<sf::Texture, 4> pieceTex;
        std::array<sf::Sprite, 4> pieceSprite;
        bool collected[4] = { false, false, false, false };

        // Constructor: initialize all sprites with valid textures
        DinoDisplay()
            : backgroundSprite(backgroundTex),
              pieceSprite{{
                  sf::Sprite(pieceTex[0]),
                  sf::Sprite(pieceTex[1]),
                  sf::Sprite(pieceTex[2]),
                  sf::Sprite(pieceTex[3])
              }}
        {
        }
    };


    std::vector<std::unique_ptr<DinoDisplay>> m_dinos;
   
    sf::Texture m_interiorTex;
    sf::Sprite  m_interiorSprite{ m_interiorTex };

    sf::Texture m_arrowsTex;
    sf::Sprite  m_leftArrow{ m_arrowsTex };
    sf::Sprite  m_rightArrow{ m_arrowsTex };
    int         m_arrowFrameW = 0;
    int         m_arrowFrameH = 0;
    bool        m_hoverLeft = false;
    bool        m_hoverRight = false;

    sf::Texture m_backTex;
    sf::Sprite  m_backSprite{ m_backTex };
    int         m_backFrameW = 0;
    int         m_backFrameH = 0;
    bool        m_hoverBack = false;
};

#endif // MUSEUM_INTERIOR_H