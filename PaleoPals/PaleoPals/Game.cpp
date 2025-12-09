/// author Jad Fuhr

#include "Game.h"
#include "Map.h"
#include <iostream>


//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
Game::Game() :
    m_window{ sf::VideoMode{sf::Vector2u{WINDOW_X, WINDOW_Y},32 }, "PaleoPals" },
    m_DELETEexitGame{ false }
{
    m_cameraView.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y));
    m_cameraView.setCenter(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));
    m_window.setView(m_cameraView);

    setupMap();
    m_menu.initMenu();
    m_pause.initPauseMenu();

}

//------------------------------------------------------------
// Destructor
//------------------------------------------------------------
Game::~Game()
{
}

//------------------------------------------------------------
// Main Game Loop
//------------------------------------------------------------
void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const float fps{ 60.0f };
    sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps

    while (m_window.isOpen())
    {
        processEvents();
        timeSinceLastUpdate += clock.restart();

        while (timeSinceLastUpdate > timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;
            processEvents();
            update(timePerFrame);
        }

        render();
    }
}

//------------------------------------------------------------
// Process Events (inputs, system events)
//------------------------------------------------------------
void Game::processEvents()
{
    while (const std::optional newEvent = m_window.pollEvent())
    {

        if (newEvent->is<sf::Event::KeyPressed>())
        {
            processKeys(newEvent);
        }
        else if (newEvent->is<sf::Event::MouseButtonPressed>())
        {
            // Handle mouse clicks for menu
            if (m_currentState == GameState::MainMenu)
            {
                m_currentState = m_menu.handleClick(m_window);
            }
            else if (m_currentState == GameState::Paused)
            {
                m_currentState = m_pause.handlePauseMenuClick(m_window);
            }
        }

    }
}

//------------------------------------------------------------
// Handle key presses
//------------------------------------------------------------
void Game::processKeys(const std::optional<sf::Event> t_event)
{
    const sf::Event::KeyPressed* newKeypress = t_event->getIf<sf::Event::KeyPressed>();

    if (newKeypress)
    {
        if (newKeypress->code == sf::Keyboard::Key::Escape)
        {
            if (m_currentState == GameState::Gameplay)
            {
                m_currentState = GameState::Paused; // Pause the game
            }
            else if (m_currentState == GameState::Paused)
            {
                m_currentState = GameState::Gameplay; // Resume the game
            }
            else if (m_currentState == GameState::MainMenu)
            {
                m_currentState = GameState::Exit; // Quit if pressed on main menu
            }
        }

        // Keep F3 toggle for debug mode
        if (m_currentState == GameState::Gameplay && newKeypress->code == sf::Keyboard::Key::F3)
        {
            m_map.toggleDebugMode();
        }
    }
}

//------------------------------------------------------------
// Check continuous keyboard state
//------------------------------------------------------------
void Game::checkKeyboardState()
{
    if (m_currentState == GameState::MainMenu)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            m_currentState = GameState::Exit;
        }
    }
    if (m_currentState == GameState::Gameplay)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
        {
            m_currentState = GameState::MainMenu;
        }
    }

}

//------------------------------------------------------------
// Update the game world
//------------------------------------------------------------
void Game::update(sf::Time t_deltaTime)
{
  

    checkKeyboardState();

    switch (m_currentState)
    {
    case GameState::MainMenu:
        m_menu.update(m_window);
        break;

    case GameState::Gameplay:
        moveCamera(t_deltaTime);

        //m_map.updateIfNeeded(m_cameraView.getCenter().y, 50);
        m_map.handleMouseHold(m_window, 24, 75);
        m_map.updateHover(m_window, 24.0f, 75);
        m_map.updateMuseum(m_window);
        m_map.updateTrader(m_window);
        break;

    case GameState::Paused:
        m_pause.updatePauseMenu(m_window);
        break;

    case GameState::Exit:
        m_window.close();
        break;

    default:
        break;
    }


}

//------------------------------------------------------------
// Render everything
//------------------------------------------------------------
void Game::render()
{

    m_window.clear();

   

    switch (m_currentState)
    {
    case GameState::MainMenu:
        //std::cout << "current state menu" << std::endl;
        m_window.setView(m_window.getDefaultView());

        m_menu.draw(m_window);
        break;

    case GameState::Gameplay:
        m_window.setView(m_cameraView);

        m_map.drawMap(m_window);
        m_map.drawDebug(m_window);
        break;
    case GameState::Paused:
        m_window.setView(m_cameraView);

        m_map.drawMap(m_window);
        m_map.drawDebug(m_window);

        m_window.setView(m_window.getDefaultView());

        m_pause.drawPauseMenu(m_window);
    default:
        break;
    }

    m_window.display();
}


//------------------------------------------------------------
// Setup Map
//------------------------------------------------------------
void Game::setupMap()
{
    if (!m_map.loadMapFromConfig("ASSETS/CONFIG/map.json"))
    {
        std::cerr << "Failed to load map config file!\n";
    }

    int cols = 75;
    int totalRows = 30;
    float tileSize = 24.0f; // 24x24 pixels per tile

    //m_map.setMapDimensions(totalRows, cols, tileSize, WINDOW_X, WINDOW_Y);
    //int initialBatch = 20;

	m_map.setupBackground();

    m_map.generateGrid(totalRows, cols, tileSize, WINDOW_X, WINDOW_Y);
}

void Game::moveCamera(sf::Time t_deltaTime)
{

    float moveAmount = cameraSpeed * t_deltaTime.asSeconds();


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        m_cameraView.move(sf::Vector2f(0, -moveAmount));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        m_cameraView.move(sf::Vector2f(0, moveAmount));
    }


}


































//------------------------------------------------------------
// Load the font and setup text
//------------------------------------------------------------
//void Game::setupTexts()
//{
//    if (!m_jerseyFont.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
//    {
//        std::cout << "Problem loading font\n";
//    }
//
//    m_DELETEwelcomeMessage.setFont(m_jerseyFont);
//    m_DELETEwelcomeMessage.setString("PaleoPals");
//    m_DELETEwelcomeMessage.setPosition(sf::Vector2f{ 205.0f, 240.0f });
//    m_DELETEwelcomeMessage.setCharacterSize(96U);
//    m_DELETEwelcomeMessage.setOutlineColor(sf::Color::Black);
//    m_DELETEwelcomeMessage.setFillColor(sf::Color::Red);
//    m_DELETEwelcomeMessage.setOutlineThickness(2.0f);
//}

//------------------------------------------------------------
// Load texture and setup logo sprite
//------------------------------------------------------------
//void Game::setupSprites()
//{
//    if (!m_DELETElogoTexture.loadFromFile("ASSETS/IMAGES/SFML-LOGO.png"))
//    {
//        std::cout << "Problem loading logo\n";
//    }
//
//    m_DELETElogoSprite.setTexture(m_DELETElogoTexture, true);
//    m_DELETElogoSprite.setPosition(sf::Vector2f{ 150.0f, 50.0f });
//}

//------------------------------------------------------------
// Load audio
//------------------------------------------------------------
//void Game::setupAudio()
//{
//    if (!m_DELETEsoundBuffer.loadFromFile("ASSETS/AUDIO/beep.wav"))
//    {
//        std::cout << "Error loading beep sound\n";
//    }
//
//    m_DELETEsound.setBuffer(m_DELETEsoundBuffer);
//    m_DELETEsound.play();
//}
