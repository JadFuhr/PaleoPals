#include "Menu.h"
#include <iostream>

void Menu::initMenu()
{
    if (!m_backgroundTexture.loadFromFile("ASSETS/IMAGES/Screens/Menu.png"))
        std::cout << "Failed to load background texture\n";
    else
        std::cout << "Background loaded successfully!\n";


    sf::Vector2u texSize = m_backgroundTexture.getSize(); //360x180

    m_backgroundSprite.setTexture(m_backgroundTexture);
    m_backgroundSprite.setTextureRect(sf::IntRect({ 0,0 }, { 360,180 }));
    m_backgroundSprite.setPosition(sf::Vector2f(0, 0));
    m_backgroundSprite.setScale(sf::Vector2f(WINDOW_X / texSize.x, WINDOW_Y / texSize.y));
    //m_backgroundSprite.setColor(sf::Color::Red);

    if (!m_startButtonTexture.loadFromFile("ASSETS/IMAGES/Screens/StartButton.png"))
        std::cout << "Failed to load start button texture\n";

    m_startButton.setTexture(m_startButtonTexture);
    m_startButton.setTextureRect(sf::IntRect({ 0,0 }, { 92,34 }));
    m_startButton.setPosition(sf::Vector2f(860, 450));
    m_startButton.setOrigin(sf::Vector2f(46, 17));
    m_startButton.setScale(sf::Vector2f(3, 3));

    if (!m_quitButtonTexture.loadFromFile("ASSETS/IMAGES/Screens/QuitButton.png"))
        std::cout << "Failed to load quit button texture\n";

    m_quitButton.setTexture(m_quitButtonTexture);
    m_quitButton.setTextureRect(sf::IntRect({ 0,0 }, { 92,34 }));
    m_quitButton.setPosition(sf::Vector2f( 860, 650));
    m_quitButton.setOrigin(sf::Vector2f(46, 17));
    m_quitButton.setScale(sf::Vector2f(3, 3));


    //std::cout << "Background size: " << texSize.x << "x" << texSize.y << "\n";
    //std::cout << "Background position: " << m_backgroundSprite.getPosition().x << "," << m_backgroundSprite.getPosition().y << "\n";
    //std::cout << "Background scale: " << m_backgroundSprite.getScale().x << "," << m_backgroundSprite.getScale().y << "\n";


}

void Menu::update(const sf::RenderWindow& window)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Start hover
    if (m_startButton.getGlobalBounds().contains(mouse))
        m_startButton.setTextureRect(sf::IntRect({ 92,0 }, { 92,34 }));
    else
        m_startButton.setTextureRect(sf::IntRect({ 0,0 }, { 92,34 }));

    // Quit hover
    if (m_quitButton.getGlobalBounds().contains(mouse))
        m_quitButton.setTextureRect(sf::IntRect({ 92,0 }, {92,34 }));
    else
        m_quitButton.setTextureRect(sf::IntRect({ 0,0 }, { 92,34 }));
}

void Menu::draw(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    window.draw(m_startButton);
    window.draw(m_quitButton);
}

GameState Menu::handleClick(const sf::RenderWindow& window)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (m_startButton.getGlobalBounds().contains(mouse))
    {
        return GameState::Gameplay;
    }

    if (m_quitButton.getGlobalBounds().contains(mouse))
    {
        return GameState::Exit;
    }

    return GameState::MainMenu;
}
