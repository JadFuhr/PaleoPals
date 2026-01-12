#pragma once
#include <SFML/Graphics.hpp>

const int WINDOW_X = 1800;
const int WINDOW_Y = 900;
const int BACKGROUND_LENGTH = 5264;


enum class GameState
{
    MainMenu,
    Gameplay,
    Museum,
    Trader,
    Paused,
    Settings,
    Exit
};