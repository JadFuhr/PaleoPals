#pragma once
#ifndef PAUSED_HPP
#define PAUSED_HPP

#include <SFML/Graphics.hpp>
#include "constants.h"

class PauseMenu
{
public:
    PauseMenu() = default;

    void initPauseMenu();
    void updatePauseMenu(const sf::RenderWindow& window);
    void drawPauseMenu(sf::RenderWindow& window);
    GameState handlePauseMenuClick(const sf::RenderWindow& window);

private:
    sf::Texture m_pauseTexture;
    sf::Sprite m_pauseSprite{ m_pauseTexture };

    sf::Texture m_resumeButtonTexture;
    sf::Sprite m_resumeButton{ m_resumeButtonTexture };

    sf::Texture m_settingsButtonTexture;
    sf::Sprite m_settingsButton{ m_settingsButtonTexture };

    sf::Texture m_quitButtonTexture;
    sf::Sprite m_quitButton{ m_quitButtonTexture };
};

#endif // !PAUSED_HPP
