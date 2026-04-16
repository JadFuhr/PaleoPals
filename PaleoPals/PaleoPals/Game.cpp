/// author Jad Fuhr

#include "Game.h"
#include "Map.h"
#include <iostream>

Game::Game() :
    m_window{ sf::VideoMode{sf::Vector2u{WINDOW_X, WINDOW_Y},32 }, "PaleoPals" },
    m_DELETEexitGame{ false }
{
    m_cameraView.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y));
    m_cameraView.setCenter(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));
    m_cameraView.zoom(0.5f);
    m_window.setView(m_cameraView);

    setupMap();
    m_menu.initMenu();
    m_pause.initPauseMenu();

    m_uiFont.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf");
    m_moneyText.setFont(m_uiFont);
    m_moneyText.setCharacterSize(28);
    m_moneyText.setFillColor(sf::Color::Yellow);


}

Game::~Game()
{
}

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
            else if (m_currentState == GameState::Gameplay)
            {
                // Get mouse screen position (for UI menus)
                sf::Vector2i mousePixel = sf::Mouse::getPosition(m_window);
                sf::Vector2f screenPos(static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y));

                // Also get world position for world-space clicks
                sf::Vector2f worldPos = m_window.mapPixelToCoords(mousePixel);

                auto mouseButton = newEvent->getIf<sf::Event::MouseButtonPressed>();

                if (mouseButton && mouseButton->button == sf::Mouse::Button::Left)
                {
                    // Museum interior open - it consumes all clicks
                    if (m_museumInterior.isOpen())
                    {
                        m_museumInterior.handleClick(screenPos);
                        continue;
                    }

                    // Trader menu open - it consumes all clicks
                    if (m_traderMenu.isOpen())
                    {
                        HireAction action = m_traderMenu.handleClick(screenPos, m_window);

                        if (action == HireAction::HirePaleontologist)
                        {
                            auto newPaleo = std::make_unique<Paleontologist>();
                            newPaleo->setPosition(worldPos + sf::Vector2f(16.f, 0.f));
                            newPaleo->setSpeed(60.0f);
                            m_paleontologists.push_back(std::move(newPaleo));
                        }
                        else if (action == HireAction::HireResearcher)
                        {
                            std::cout << "Researcher hiring not yet implemented\n";
                        }
                        if (action == HireAction::Upgrade1)
                        {
                            int cost = m_traderMenu.getUpgrade1Cost();

                            if (m_player.getMoney() >= cost)
                            {
                                m_player.spendMoney(cost);
                                m_player.pickaxeRadiusLevel++;
                                m_traderMenu.upgrade1Level++;
                            }
                        }
                        else if (action == HireAction::Upgrade2)
                        {
                            int cost = m_traderMenu.getUpgrade2Cost();

                            if (m_player.getMoney() >= cost)
                            {
                                m_player.spendMoney(cost);
                                m_player.damageLevel++;
                                m_traderMenu.upgrade2Level++;
                            }
                        }
                        if (action == HireAction::Upgrade3)
                        {
                            int cost = m_traderMenu.getUpgrade3Cost();
                            if (m_player.getMoney() >= cost)
                            {
                                m_player.spendMoney(cost);
                                m_player.pickupRadiusLevel++;
                                m_traderMenu.upgrade3Level++;
                            }
                        }

                        if (action == HireAction::Upgrade4)
                        {
                            int cost = m_traderMenu.getUpgrade4Cost();
                            if (m_player.getMoney() >= cost)
                            {
                                m_player.spendMoney(cost);
                                m_player.jumpLevel++;
                                m_traderMenu.upgrade4Level++;
                            }
                        }



                        continue;
                    }

                    // Click on museum building - open interior
                    if (m_map.getMuseum().containsPoint(worldPos))
                    {
                        
                        m_museumInterior.open();
                        continue;
                    }

                    // Click on trader building - open trader menu
                    if (m_map.getTrader().containsPoint(worldPos))
                    {
                        m_traderMenu.openAt(worldPos);
                        continue;
                    }
                }

            }
        }

    }
}

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

        m_map.handleMouseHold(m_window, 24, 75);
        m_map.updateHover(m_window, 24.0f, 75);
        m_map.updateMuseum(m_window);
        m_map.updateTrader(m_window);

        m_moneyText.setString("Money: " + std::to_string(m_player.getMoney()));


        if (m_museumInterior.isOpen())
        {
            m_museumInterior.update(m_window);
        }
        else
        {

            m_player.update(t_deltaTime, m_map, m_window, m_cameraView);

			sf::Vector2f playerPos = m_player.getPosition();

            float mapWidth = m_map.getColumnCount() * m_map.getTileSize();
            float mapHeight = m_map.getRowCount() * m_map.getTileSize();

            sf::Vector2f halfView(m_cameraView.getSize().x / 2.f, m_cameraView.getSize().y / 2.f);
            sf::Vector2f camPos = playerPos;

            camPos.x = std::clamp(camPos.x, halfView.x, mapWidth - halfView.x);
            camPos.y = std::clamp(camPos.y, halfView.y, mapHeight - halfView.y);

            m_cameraView.setCenter(camPos);


            for (const auto& item : m_player.getNewPickups())
            {
                if (item.type == "fossil")
                {
                    m_museumInterior.onFossilCollected(item.dinosaurName, item.pieceId);
                }
            }
            m_player.clearNewPickups();

        }
        try
        {
            for (auto& p : m_paleontologists) 
            {
                if (p) p->update(t_deltaTime, m_map);
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception updating paleontologist: " << e.what() << "\n";
        }
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

void Game::render()
{

    m_window.clear();

    switch (m_currentState)
    {
    case GameState::MainMenu:
        m_window.setView(m_window.getDefaultView());

        m_menu.draw(m_window);
        break;

    case GameState::Gameplay:

        m_window.setView(m_cameraView);
        m_map.drawMap(m_window);

		m_window.setView(m_window.getDefaultView());
        m_window.draw(m_moneyText);


		m_window.setView(m_cameraView);

        {
            sf::Vector2f viewCenter = m_cameraView.getCenter();
            sf::Vector2f viewSize = m_cameraView.getSize();
            sf::FloatRect viewBounds(sf::Vector2f(viewCenter.x - viewSize.x / 2.f, viewCenter.y - viewSize.y / 2.f), viewSize);

            for (auto& p : m_paleontologists)
            {
             
                if (viewBounds.findIntersection(p->getSprite().getGlobalBounds()))
                {
                    p->draw(m_window);
                }
            }

            m_player.draw(m_window);
        }

		m_window.setView(m_window.getDefaultView());
        m_traderMenu.draw(m_window);
        m_museumInterior.draw(m_window);

        m_map.drawDebug(m_window);

        break;
    case GameState::Paused:
        m_window.setView(m_cameraView);

        m_map.drawMap(m_window);
        m_map.drawDebug(m_window);

        m_window.setView(m_window.getDefaultView());
        m_window.draw(m_moneyText);

        for (auto& p : m_paleontologists)
        {
            if (p) p->draw(m_window);
        }

        m_pause.drawPauseMenu(m_window);
    default:
        break;
    }

    m_window.display();
}

void Game::setupMap()
{
    if (!m_map.loadMapFromConfig("ASSETS/CONFIG/map.json"))
    {
        std::cerr << "Failed to load map config file!\n";
    }

    int cols = 75;
    int totalRows = 100;

    float tileSize = 24.0f; 

    m_map.setupBackground();
    m_map.generateGrid(totalRows, cols, tileSize, WINDOW_X, WINDOW_Y);
	m_museumInterior.loadAssets(m_map.getFossilManager().getDinosaurData());


    auto initialPaleo = std::make_unique<Paleontologist>();
    initialPaleo->setPosition(sf::Vector2f(WINDOW_X / 2.0f, WINDOW_Y / 2.0f - 20.0f));
    initialPaleo->setSpeed(60.0f);
    m_paleontologists.push_back(std::move(initialPaleo));


    m_player.setPosition(sf::Vector2f(WINDOW_X / 2.0f + 100.0f, WINDOW_Y / 2.0f));

}

void Game::moveCamera(sf::Time t_deltaTime)
{

    float moveAmount = cameraSpeed * t_deltaTime.asSeconds();


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        m_cameraView.move(sf::Vector2f(0, -moveAmount));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        m_cameraView.move(sf::Vector2f(0, moveAmount));
    }


}

