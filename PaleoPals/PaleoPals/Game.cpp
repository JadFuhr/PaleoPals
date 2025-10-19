/// author Jad Fuhr

#include "Game.h"
#include "Map.h"
#include "constants.h"
#include <iostream>


//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
Game::Game() :
    m_window{ sf::VideoMode{ sf::Vector2u{WINDOW_X, WINDOW_Y}, 32U }, "PaleoPals" },
    m_DELETEexitGame{ false }
{
    //setupTexts();   // load font and text
    //setupSprites(); // load logo sprite
    //setupAudio();   // load and play sound
    setupMap();     // NEW: load and generate the map
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
        if (newEvent->is<sf::Event::Closed>())
        {
            m_DELETEexitGame = true;
        }
        if (newEvent->is<sf::Event::KeyPressed>())
        {
            processKeys(newEvent);
        }
    }
}

//------------------------------------------------------------
// Handle key presses
//------------------------------------------------------------
void Game::processKeys(const std::optional<sf::Event> t_event)
{
    const sf::Event::KeyPressed* newKeypress = t_event->getIf<sf::Event::KeyPressed>();

    if (sf::Keyboard::Key::Escape == newKeypress->code)
    {
        m_DELETEexitGame = true;
    }

    if (sf::Keyboard::Key::F3 == newKeypress->code)
    {
        m_map.toggleDebugMode();
    }
}

//------------------------------------------------------------
// Check continuous keyboard state
//------------------------------------------------------------
void Game::checkKeyboardState()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        m_DELETEexitGame = true;
    }
}

//------------------------------------------------------------
// Update the game world
//------------------------------------------------------------
void Game::update(sf::Time t_deltaTime)
{
    checkKeyboardState();

    m_map.updateHover(m_window, 24.0f, 75); 
    m_map.updateMuseum(m_window);
    m_map.updateTrader(m_window);

    if (m_DELETEexitGame)

    {
        m_window.close();
    }
}

//------------------------------------------------------------
// Render everything
//------------------------------------------------------------
void Game::render()
{
    m_window.clear();

    // Draw the map and assets included in that 
    m_map.drawMap(m_window);
    m_map.drawDebug(m_window);

    // Draw logo and text on top
   // m_window.draw(m_DELETElogoSprite);
   // m_window.draw(m_DELETEwelcomeMessage);

    m_window.display();
}

//------------------------------------------------------------
// Load the font and setup text
//------------------------------------------------------------
void Game::setupTexts()
{
    if (!m_jerseyFont.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
    {
        std::cout << "Problem loading font\n";
    }

    m_DELETEwelcomeMessage.setFont(m_jerseyFont);
    m_DELETEwelcomeMessage.setString("PaleoPals");
    m_DELETEwelcomeMessage.setPosition(sf::Vector2f{ 205.0f, 240.0f });
    m_DELETEwelcomeMessage.setCharacterSize(96U);
    m_DELETEwelcomeMessage.setOutlineColor(sf::Color::Black);
    m_DELETEwelcomeMessage.setFillColor(sf::Color::Red);
    m_DELETEwelcomeMessage.setOutlineThickness(2.0f);
}

//------------------------------------------------------------
// Load texture and setup logo sprite
//------------------------------------------------------------
void Game::setupSprites()
{
    if (!m_DELETElogoTexture.loadFromFile("ASSETS/IMAGES/SFML-LOGO.png"))
    {
        std::cout << "Problem loading logo\n";
    }

    m_DELETElogoSprite.setTexture(m_DELETElogoTexture, true);
    m_DELETElogoSprite.setPosition(sf::Vector2f{ 150.0f, 50.0f });
}

//------------------------------------------------------------
// Load audio
//------------------------------------------------------------
void Game::setupAudio()
{
    if (!m_DELETEsoundBuffer.loadFromFile("ASSETS/AUDIO/beep.wav"))
    {
        std::cout << "Error loading beep sound\n";
    }

    m_DELETEsound.setBuffer(m_DELETEsoundBuffer);
    m_DELETEsound.play();
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
    int rows = 19;
    float tileSize = 24.0f; // 24x24 pixels per tile

    m_map.generateGrid(rows, cols, tileSize, WINDOW_X, WINDOW_Y);
}
