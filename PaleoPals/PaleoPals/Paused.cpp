#include "Paused.h"
#include <iostream>


void PauseMenu::initPauseMenu()
{

    // Load pause background (dim overlay)
    if (!m_pauseTexture.loadFromFile("ASSETS/IMAGES/Screens/PausedScreen.png"))
    {
        std::cout << "Failed to load pause background\n";
    }


    // Load buttons
    if (!m_resumeButtonTexture.loadFromFile("ASSETS/IMAGES/Screens/ResumeButton.png"))
    {
        std::cout << "Failed to load Resume button\n";
    }


    if (!m_settingsButtonTexture.loadFromFile("ASSETS/IMAGES/Screens/SettingsButton.png"))
    {
        std::cout << "Failed to load Settings button\n";
    }
 

    if (!m_quitButtonTexture.loadFromFile("ASSETS/IMAGES/Screens/QuitButton.png"))
    {
        std::cout << "Failed to load Quit button\n";
    }

    m_pauseSprite.setTexture(m_pauseTexture);
    m_pauseSprite.setTextureRect(sf::IntRect({ 0,0 }, { 180,90 }));
    m_pauseSprite.setOrigin(sf::Vector2f(90, 45));
    m_pauseSprite.setPosition(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));
    m_pauseSprite.setScale(sf::Vector2f(5, 5));

    m_resumeButton.setTexture(m_resumeButtonTexture);
    m_resumeButton.setOrigin(sf::Vector2f(46,17));
    m_resumeButton.setPosition(sf::Vector2f(WINDOW_X / 2, 355));
    m_resumeButton.setScale(sf::Vector2f(2, 2));

    m_settingsButton.setTexture(m_settingsButtonTexture);
    m_settingsButton.setOrigin(sf::Vector2f(46, 17));
    m_settingsButton.setPosition(sf::Vector2f(WINDOW_X / 2, 455));
    m_settingsButton.setScale(sf::Vector2f(2, 2));

    m_quitButton.setTexture(m_quitButtonTexture);
    m_quitButton.setOrigin(sf::Vector2f(46, 17));
    m_quitButton.setPosition(sf::Vector2f(WINDOW_X / 2, 555));
    m_quitButton.setScale(sf::Vector2f(2, 2));


}

void PauseMenu::updatePauseMenu(const sf::RenderWindow& window)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Hover effects
    if (m_resumeButton.getGlobalBounds().contains(mouse))
    {
        m_resumeButton.setTextureRect(sf::IntRect({ 92, 0 }, { 92, 34 }));
    }
    else
    {
        m_resumeButton.setTextureRect(sf::IntRect({ 0, 0 }, { 92, 34 }));
    }

    if (m_settingsButton.getGlobalBounds().contains(mouse))
    {
        m_settingsButton.setTextureRect(sf::IntRect({ 92, 0 }, { 92, 34 }));
    }
    else
    {
        m_settingsButton.setTextureRect(sf::IntRect({ 0, 0 }, { 92, 34 }));
    }

    if (m_quitButton.getGlobalBounds().contains(mouse))
    {
        m_quitButton.setTextureRect(sf::IntRect({ 92, 0 }, { 92, 34 }));
    }
    else
    {
        m_quitButton.setTextureRect(sf::IntRect({ 0, 0 }, { 92, 34 }));
    }
}

void PauseMenu::drawPauseMenu(sf::RenderWindow& window)
{
    window.draw(m_pauseSprite);

    // Draw buttons
    window.draw(m_resumeButton);
    window.draw(m_settingsButton);
    window.draw(m_quitButton);
}

GameState PauseMenu::handlePauseMenuClick(const sf::RenderWindow& window)
{
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (m_resumeButton.getGlobalBounds().contains(mousePos))
        {
            return GameState::Gameplay; // Resume the game
        }
        if (m_settingsButton.getGlobalBounds().contains(mousePos))
        {
            return GameState::Settings; // settings state
        }
        if (m_quitButton.getGlobalBounds().contains(mousePos))
        {
            return GameState::MainMenu; // Back to main menu
        }
    }

    return GameState::Paused; // Stay paused otherwise
}
