#ifndef GAME_HPP
#define GAME_HPP
#pragma warning(push)
#pragma warning(disable : 4275)

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional> 
#include "Map.h"
#include "Menu.h"
#include "Paused.h"
#include "Paleontologist.h"
#include <json.hpp>
#include <fstream>
#include "constants.h"

#include <vector>
#include <memory>

#include "TraderMenu.h"


class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void processKeys(const std::optional<sf::Event> t_event);
    void checkKeyboardState();
    void update(sf::Time t_deltaTime);
    void render();

    //void setupTexts();
    //void setupSprites();
   //void setupAudio();
    void setupMap(); // loads and generates the map grid
    void moveCamera(sf::Time t_deltaTime);



    Map m_map;
    Menu m_menu;
    PauseMenu m_pause;
    Museum m_museum;
    std::vector<std::unique_ptr<Paleontologist>> m_paleontologists; // multiple paleontologist instances

    sf::RenderWindow m_window; // main SFML window
    sf::View m_cameraView;

    float cameraSpeed = 150.0f;

    GameState m_currentState{ GameState::MainMenu };

    bool m_DELETEexitGame; // control exiting game

    TraderMenu m_traderMenu;

    // Get the camera view bounds for frustum culling
    sf::FloatRect getCameraViewBounds() const
    {
        sf::Vector2f center = m_cameraView.getCenter();
        sf::Vector2f size = m_cameraView.getSize();
        return sf::FloatRect(sf::Vector2f(center.x - size.x / 2.f, center.y - size.y / 2.f), size);
    }
};

#pragma warning(pop)
#endif // !GAME_HPP
