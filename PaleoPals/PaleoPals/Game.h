#ifndef GAME_HPP
#define GAME_HPP
#pragma warning(push)
#pragma warning(disable : 4275)

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional> 
#include "Map.h"
#include "Menu.h"
#include <json.hpp>
#include <fstream>
#include "constants.h"


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


    Map m_map;
    Menu m_menu;

    sf::RenderWindow m_window; // main SFML window

    GameState m_currentState{ GameState::MainMenu };

    bool m_DELETEexitGame; // control exiting game
};

#pragma warning(pop)
#endif // !GAME_HPP
