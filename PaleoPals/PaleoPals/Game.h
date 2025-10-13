#ifndef GAME_HPP
#define GAME_HPP
#pragma warning(push)
#pragma warning(disable : 4275)

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional> /
#include "Map.h"

const sf::Color ULTRAMARINE{ 5, 55, 242, 255 }; // const colour

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

    void setupTexts();
    void setupSprites();
    void setupAudio();
    void setupMap(); // NEW: loads and generates the map grid

    Map m_map;

    sf::RenderWindow m_window; // main SFML window
    sf::Font m_jerseyFont;     // font used by message

    sf::Text m_DELETEwelcomeMessage{ m_jerseyFont };  // on-screen text
    sf::Texture m_DELETElogoTexture;                  // texture used for SFML logo
    sf::Sprite m_DELETElogoSprite{ m_DELETElogoTexture }; // sprite used for SFML logo

    sf::SoundBuffer m_DELETEsoundBuffer; // buffer for beep sound
    sf::Sound m_DELETEsound{ m_DELETEsoundBuffer }; // sound object to play

    bool m_DELETEexitGame; // control exiting game
};

#pragma warning(pop)
#endif // !GAME_HPP
