#include "Paleontologist.h"
#include "Map.h"
#include <iostream>


Paleontologist::Paleontologist()
{
    std::cout << "Paleontologist constructor START\n";

    m_behaviorManager = std::make_unique<BehaviorManager>();
    std::cout << "BehaviorManager created\n";

    if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
    {
        std::cerr << "Failed to load paleontologist texture! Creating placeholder...\n";
    }
    else
    {
        std::cout << "Texture loaded successfully\n";
    }

    m_sprite.setTexture(m_texture);

    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { m_frameWidth, m_frameHeight }));

    m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight / 2.0f));

    m_sprite.setPosition(sf::Vector2f(400.0f, 300.0f));

    m_sprite.setScale(sf::Vector2f(0.2f, 0.2f));

    m_progressBarBackground.setSize(sf::Vector2f(40.0f, 6.0f));
    m_progressBarBackground.setFillColor(sf::Color(50, 50, 50));
    m_progressBarBackground.setOutlineColor(sf::Color::White);
    m_progressBarBackground.setOutlineThickness(1.0f);

    m_progressBar.setSize(sf::Vector2f(40.0f, 6.0f));
    m_progressBar.setFillColor(sf::Color::Green);

    setFrame(0);

}


Paleontologist::~Paleontologist()
{
}


void Paleontologist::update(sf::Time deltaTime, Map& map)
{
    if (!m_behaviorManager)
    {
        std::cerr << "ERROR: BehaviorManager is null!\n";
        return;
    }

    m_behaviorManager->update(this, deltaTime, map);
    updateAnimation(deltaTime);
    updateProgressBar();
}


void Paleontologist::updateAnimation(sf::Time deltaTime)
{
    BehaviorState currentState = getCurrentState();

    if (currentState == BehaviorState::Idle || currentState == BehaviorState::Mining)
    {
        setFrame(0);
        m_animationTimer = 0.0f;
    }
    else if (currentState == BehaviorState::Wandering ||
        currentState == BehaviorState::SearchingForFossil ||
        currentState == BehaviorState::MovingToTarget)
    {
        m_animationTimer += deltaTime.asSeconds();

        if (m_animationTimer >= m_frameTime)
        {
            m_animationTimer -= m_frameTime;
            m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
            setFrame(m_currentFrame);
        }
    }
}


void Paleontologist::setFrame(int frame)
{
    if (frame < 0 || frame >= m_totalFrames)
    {
        frame = 0;
    }

    m_currentFrame = frame;

    if (m_texture.getSize().x == 0 || m_texture.getSize().y == 0)
    {
        return; 
    }

    int xOffset = frame * m_frameWidth;
    m_sprite.setTextureRect(sf::IntRect({ xOffset, 0 }, { m_frameWidth, m_frameHeight }));
}


void Paleontologist::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);

    BehaviorState currentState = getCurrentState();
    if ((currentState == BehaviorState::Mining || currentState == BehaviorState::SearchingForFossil)
        && m_miningProgress > 0.0f)
    {
        window.draw(m_progressBarBackground);
        window.draw(m_progressBar);
    }
}


void Paleontologist::move(sf::Vector2f direction)
{
    m_sprite.move(direction);
}


BehaviorState Paleontologist::getCurrentState() const
{
    return m_behaviorManager->getCurrentState();
}


void Paleontologist::updateProgressBar()
{
    sf::Vector2f spritePos = m_sprite.getPosition();

    m_progressBarBackground.setPosition(sf::Vector2f(spritePos.x - 20.0f, spritePos.y - 25.0f));

    m_progressBar.setPosition(m_progressBarBackground.getPosition());
    m_progressBar.setSize(sf::Vector2f(40.0f * m_miningProgress, 6.0f));
}