#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <json.hpp>

struct Layer
{
    std::string name;
    sf::Texture texture;
    sf::Sprite sprite;
    int depth;

    // Constructor using move semantics
    Layer(const std::string& n, sf::Texture&& tex, sf::Sprite&& spr, int d)
        : name(n), texture(std::move(tex)), sprite(std::move(spr)), depth(d) {
    }

    // delete, default and copy constructors
    Layer() = delete;
    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;

    // Allow move
    Layer(Layer&&) = default;
    Layer& operator=(Layer&&) = default;
};

class Map
{
public:
    Map();
    bool loadFromConfig(const std::string& filepath);
    void draw(sf::RenderWindow& window);

private:
    std::vector<Layer> m_layers;
};
