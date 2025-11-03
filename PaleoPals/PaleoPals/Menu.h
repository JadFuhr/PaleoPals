#pragma once
#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include "constants.h"

class Menu
{
public:
    Menu() {};

    void initMenu();
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    GameState handleClick(const sf::RenderWindow& window);

private:
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite{m_backgroundTexture};

    sf::Texture m_startButtonTexture;
    sf::Sprite m_startButton{ m_startButtonTexture };

    sf::Texture m_quitButtonTexture;
    sf::Sprite m_quitButton{m_quitButtonTexture};
};
#endif