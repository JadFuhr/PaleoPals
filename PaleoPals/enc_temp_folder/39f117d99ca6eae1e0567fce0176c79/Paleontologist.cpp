#include "Paleontologist.h"
#include "Map.h"
#include <iostream>

//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
Paleontologist::Paleontologist()
{
    std::cout << "Paleontologist constructor START\n";

    m_behaviorManager = std::make_unique<BehaviorManager>();
    std::cout << "BehaviorManager created\n";

    // Load texture - expecting a sprite sheet with 4 frames horizontally
    if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
    {
        std::cerr << "Failed to load paleontologist texture! Creating placeholder...\n";
    }
    else
    {
        std::cout << "Texture loaded successfully\n";
    }

    m_sprite.setTexture(m_texture);

    // Set texture rect to first frame
    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { m_frameWidth, m_frameHeight }));

    // Set origin to center of sprite
    m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight / 2.0f));

    // Initialize position to prevent null access
    m_sprite.setPosition(sf::Vector2f(400.0f, 300.0f));
    std::cout << "Sprite position initialized\n";

    // Scale up the sprite
    m_sprite.setScale(sf::Vector2f(0.2f, 0.2f));

    // Setup progress bar for mining
    m_progressBarBackground.setSize(sf::Vector2f(40.0f, 6.0f));
    m_progressBarBackground.setFillColor(sf::Color(50, 50, 50));
    m_progressBarBackground.setOutlineColor(sf::Color::White);
    m_progressBarBackground.setOutlineThickness(1.0f);

    m_progressBar.setSize(sf::Vector2f(40.0f, 6.0f));
    m_progressBar.setFillColor(sf::Color::Green);

    // Start with frame 0 (idle)
    setFrame(0);

    std::cout << "Paleontologist constructor END\n";
}

//------------------------------------------------------------
// Destructor
//------------------------------------------------------------
Paleontologist::~Paleontologist()
{
}

//------------------------------------------------------------
// Update
//------------------------------------------------------------
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

//------------------------------------------------------------
// Update Animation
//------------------------------------------------------------
void Paleontologist::updateAnimation(sf::Time deltaTime)
{
    BehaviorState currentState = getCurrentState();

    // Idle and Mining: use frame 0
    if (currentState == BehaviorState::Idle || currentState == BehaviorState::Mining)
    {
        setFrame(0);
        m_animationTimer = 0.0f;
    }
    // Wandering and Searching: animate walk cycle (frames 0-3)
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

//------------------------------------------------------------
// Set Animation Frame
//------------------------------------------------------------
void Paleontologist::setFrame(int frame)
{
    if (frame < 0 || frame >= m_totalFrames)
    {
        frame = 0;
    }

    m_currentFrame = frame;

    // Safety check: make sure texture is valid
    if (m_texture.getSize().x == 0 || m_texture.getSize().y == 0)
    {
        return; // Don't try to set rect on invalid texture
    }

    // Update texture rect to show the correct frame
    // Frames are laid out horizontally: [0][1][2][3]
    int xOffset = frame * m_frameWidth;
    m_sprite.setTextureRect(sf::IntRect({ xOffset, 0 }, { m_frameWidth, m_frameHeight }));
}

//------------------------------------------------------------
// Draw
//------------------------------------------------------------
void Paleontologist::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);

    // Draw mining progress bar if mining OR searching (digging down)
    BehaviorState currentState = getCurrentState();
    if ((currentState == BehaviorState::Mining || currentState == BehaviorState::SearchingForFossil)
        && m_miningProgress > 0.0f)
    {
        window.draw(m_progressBarBackground);
        window.draw(m_progressBar);
    }
}

//------------------------------------------------------------
// Move
//------------------------------------------------------------
void Paleontologist::move(sf::Vector2f direction)
{
    m_sprite.move(direction);
}

//------------------------------------------------------------
// Get current behavior state
//------------------------------------------------------------
BehaviorState Paleontologist::getCurrentState() const
{
    return m_behaviorManager->getCurrentState();
}

//------------------------------------------------------------
// Update progress bar position and size
//------------------------------------------------------------
void Paleontologist::updateProgressBar()
{
    sf::Vector2f spritePos = m_sprite.getPosition();

    // Position above the sprite
    m_progressBarBackground.setPosition(sf::Vector2f(spritePos.x - 20.0f, spritePos.y - 25.0f));

    m_progressBar.setPosition(m_progressBarBackground.getPosition());
    m_progressBar.setSize(sf::Vector2f(40.0f * m_miningProgress, 6.0f));
}