#include "MuseumInterior.h"
#include "constants.h"
#include <iostream>
#include <algorithm>

//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
MuseumInterior::MuseumInterior()
    : m_leftArrow(m_arrowsTex),
      m_rightArrow(m_arrowsTex),
      m_backSprite(m_backTex),
	  m_interiorSprite(m_interiorTex),
	  m_dinoNameText(m_font)
{

    if (!m_interiorTex.loadFromFile("ASSETS/IMAGES/Screens/Museum_Interior.png"))
    {
		std::cout << "MuseumInterior: failed to load interior background texture\n";
    }
	m_interiorSprite.setTexture(m_interiorTex);
	m_interiorSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 2000, 1000 }));
    m_interiorSprite.setOrigin(sf::Vector2f(1000.0f, 500.0f));
	m_interiorSprite.setPosition(sf::Vector2f(WINDOW_X / 2.0f, WINDOW_Y / 2.0f));
	m_interiorSprite.setScale(sf::Vector2f(0.6f, 0.6f));

    // ---- Arrow buttons ----
    // The sheet has 4 equal-width frames side by side:
    //   col 0 = black  left  (normal)
    //   col 1 = white  left  (hover)
    //   col 2 = black  right (normal)
    //   col 3 = white  right (hover)
    if (!m_arrowsTex.loadFromFile("ASSETS/IMAGES/Screens/DirectionArrows.png"))
    {
		std::cout << "MuseumInterior: failed to load arrows texture\n";
    }

    {
        sf::Vector2u sz = m_arrowsTex.getSize();
        m_arrowFrameW = static_cast<int>(sz.x) / 4;  // 4 frames wide
        m_arrowFrameH = static_cast<int>(sz.y);
    }

    m_leftArrow.setTexture(m_arrowsTex);
    m_rightArrow.setTexture(m_arrowsTex);

    // Scale arrows up a bit so they're easy to click
    float arrowScale = 0.5f;
    m_leftArrow.setScale(sf::Vector2f(arrowScale, arrowScale));
    m_rightArrow.setScale(sf::Vector2f(arrowScale, arrowScale));

    // ---- Back button ----
    // Sheet has 2 frames: col 0 = normal, col 1 = hover
    if (!m_backTex.loadFromFile("ASSETS/IMAGES/Screens/BackButton.png"))
    {
		std::cout << "MuseumInterior: failed to load back button texture\n";
    }

    {
        sf::Vector2u size = m_backTex.getSize();
        m_backFrameW = static_cast<int>(size.x) / 2;
        m_backFrameH = static_cast<int>(size.y);
    }

    m_backSprite.setTexture(m_backTex);
    m_backSprite.setScale(sf::Vector2f(0.7f, 0.7f));

    // ---- Human sprite for size comparison ----
    if (!m_humanTex.loadFromFile("ASSETS/IMAGES/Screens/Human1.png"))
    {
        std::cout << "MuseumInterior: failed to load human sprite texture\n";
    }
    m_humanSprite.setTexture(m_humanTex);
    m_humanSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 72, 214 }));
    m_humanSprite.setOrigin(sf::Vector2f(36, 107));

    if (!m_font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
    {
		std::cout << "MuseumInterior: failed to load font\n";
    }

	m_dinoNameText.setFont(m_font);
	m_dinoNameText.setCharacterSize(24);
	m_dinoNameText.setFillColor(sf::Color::White);
	m_dinoNameText.setStyle(sf::Text::Bold);

}

// loadAssets
// Loads per-dino textures from the paths in DinosaurData.
// Called after FossilManager has finished loading the JSON.
bool MuseumInterior::loadAssets(const std::vector<DinosaurData>& dinoData)
{
    m_dinos.clear();
    m_dinos.reserve(dinoData.size());

    for (const auto& data : dinoData)
    {
        auto display = std::make_unique<DinoDisplay>();
        display->name = data.name;

        // Load background texture and create sprite with it
        if (!display->backgroundTex.loadFromFile(data.backgroundTexture))
        {
            std::cerr << "MuseumInterior: failed to load background for " << data.name << "\n";
        }
        else
        {
            // Recreate the sprite with the loaded texture
            display->backgroundSprite = sf::Sprite(display->backgroundTex);
        }

        // Load piece textures and create sprites
        for (const auto& piece : data.pieces)
        {
            int idx = pieceIdToIndex(piece.id);
            if (idx < 0 || idx > 3) continue;

            if (!display->pieceTex[idx].loadFromFile(piece.texturePath))
            {
                std::cerr << "MuseumInterior: failed to load piece " << piece.id
                    << " for " << data.name << "\n";
            }
            else
            {
                // Recreate the sprite with the loaded texture
                display->pieceSprite[idx] = sf::Sprite(display->pieceTex[idx]);
            }
        }

        m_dinos.push_back(std::move(display));
    }

    std::cout << "MuseumInterior: loaded " << m_dinos.size() << " dinosaur displays\n";
    return !m_dinos.empty();
}

//------------------------------------------------------------
// onFossilCollected
// Called by Player whenever a fossil piece is picked up
//------------------------------------------------------------
void MuseumInterior::onFossilCollected(const std::string& dinoName, const std::string& pieceId)
{
    int idx = pieceIdToIndex(pieceId);
    if (idx < 0 || idx > 3) return;

    for (auto& dino : m_dinos)
    {
        if (dino && dino->name == dinoName)
        {
            dino->collected[idx] = true;
            std::cout << "MuseumInterior: marked " << pieceId << " of " << dinoName << " as collected\n";
            return;
        }
    }
}

//------------------------------------------------------------
// open / close
//------------------------------------------------------------
void MuseumInterior::open()
{
    m_open = true;
    if (m_currentDinoIndex >= static_cast<int>(m_dinos.size()))
        m_currentDinoIndex = 0;
}

void MuseumInterior::close()
{
    m_open = false;
}

//------------------------------------------------------------
// handleClick  (screen coordinates)
// Returns true when the Back button was pressed (Game should close museum)
//------------------------------------------------------------
bool MuseumInterior::handleClick(const sf::Vector2f& screenPos)
{
    if (!m_open) return false;

    if (containsPoint(m_backSprite, screenPos))
    {
        close();
        return true;
    }

    if (!m_dinos.empty())
    {
        if (containsPoint(m_leftArrow, screenPos))
        {
            m_currentDinoIndex = (m_currentDinoIndex - 1 + static_cast<int>(m_dinos.size()))
                % static_cast<int>(m_dinos.size());
        }
        else if (containsPoint(m_rightArrow, screenPos))
        {
            m_currentDinoIndex = (m_currentDinoIndex + 1)
                % static_cast<int>(m_dinos.size());
        }
    }

    return false; // museum still open
}

//------------------------------------------------------------
// update  – hover detection in screen coords
//------------------------------------------------------------
void MuseumInterior::update(const sf::RenderWindow& window)
{
    if (!m_open) return;

    updateButtonPositions(window);

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f screenPos(static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y));

    m_hoverLeft = containsPoint(m_leftArrow, screenPos);
    m_hoverRight = containsPoint(m_rightArrow, screenPos);
    m_hoverBack = containsPoint(m_backSprite, screenPos);

    // Set arrow frames based on hover
    // Left arrow: col 0 = normal black, col 1 = hover white
    m_leftArrow.setTextureRect(sf::IntRect(
        { m_hoverLeft ? m_arrowFrameW : 0, 0 },
        { m_arrowFrameW, m_arrowFrameH }));

    // Right arrow: col 2 = normal black, col 3 = hover white
    m_rightArrow.setTextureRect(sf::IntRect(
        { m_hoverRight ? m_arrowFrameW * 3 : m_arrowFrameW * 2, 0 },
        { m_arrowFrameW, m_arrowFrameH }));

    // Back button: col 0 = normal, col 1 = hover
    m_backSprite.setTextureRect(sf::IntRect(
        { m_hoverBack ? m_backFrameW : 0, 0 },
        { m_backFrameW, m_backFrameH }));
}

//------------------------------------------------------------
// draw  – everything is in screen space (use default view)
//------------------------------------------------------------
void MuseumInterior::draw(sf::RenderWindow& window)
{
    if (!m_open) return;

    // Switch to default (screen) view so UI is fixed on screen
    sf::View prev = window.getView();
    window.setView(window.getDefaultView());
	window.draw(m_interiorSprite);

    // Current dinosaur display
    if (!m_dinos.empty() && m_dinos[m_currentDinoIndex])
    {
        DinoDisplay& dino = *m_dinos[m_currentDinoIndex];
		m_dinoNameText.setString(dino.name);
        m_dinoNameText.setPosition(sf::Vector2f(WINDOW_X / 2.f - 180, 22.f));
		
        // Draw the dino background silhouette centred on screen
        sf::Vector2u bgSize = dino.backgroundTex.getSize();
        if (bgSize.x > 0 && bgSize.y > 0)
        {
            // Get display settings for this dinosaur (includes custom scale/position)
            auto settings = getDisplaySettings(dino.name, bgSize);

            dino.backgroundSprite.setScale(sf::Vector2f(settings.scale, settings.scale));
            dino.backgroundSprite.setOrigin(sf::Vector2f(static_cast<float>(bgSize.x) / 2.f, static_cast<float>(bgSize.y) / 2.f));
            dino.backgroundSprite.setPosition(settings.position);

            // Draw as a dim silhouette (greyed out)
            dino.backgroundSprite.setColor(sf::Color(180, 180, 180, 180));
            window.draw(dino.backgroundSprite);
            dino.backgroundSprite.setColor(sf::Color::White); // reset

            // Draw human sprite for size comparison
            sf::Vector2u humanSize = m_humanTex.getSize();
            if (humanSize.x > 0 && humanSize.y > 0)
            {
                m_humanSprite.setScale(sf::Vector2f(settings.humanScale, settings.humanScale));
                m_humanSprite.setOrigin(sf::Vector2f(static_cast<float>(humanSize.x) / 2.f, static_cast<float>(humanSize.y) / 2.f));
                m_humanSprite.setPosition(settings.humanPosition);
                m_humanSprite.setColor(sf::Color::White);
                window.draw(m_humanSprite);
            }
        }

        // Draw each collected piece on top at the same position/scale
        for (int i = 0; i < 4; ++i)
        {
            if (!dino.collected[i]) continue;

            sf::Vector2u pieceSize = dino.pieceTex[i].getSize();
            if (pieceSize.x == 0 || pieceSize.y == 0) continue;

            auto settings = getDisplaySettings(dino.name, bgSize);

            dino.pieceSprite[i].setScale(sf::Vector2f(settings.scale, settings.scale));
            dino.pieceSprite[i].setOrigin(sf::Vector2f(static_cast<float>(pieceSize.x) / 2.f, static_cast<float>(pieceSize.y) / 2.f));
            dino.pieceSprite[i].setPosition(settings.position);

            window.draw(dino.pieceSprite[i]);
        }

        // For now draw a small dark bar at the top
        sf::RectangleShape nameBar(sf::Vector2f(400.f, 30.f));
        nameBar.setFillColor(sf::Color(30, 30, 30, 200));
        nameBar.setPosition(sf::Vector2f(WINDOW_X / 2.f - 200.f, 20.f));
        window.draw(nameBar);

        // Piece collection indicators (4 small squares near the bottom)
        std::string pieceNames[4] = { "Skull", "Torso", "Pelvis", "Tail" };
        float indicatorSize = 24.f;
        float spacing = 40.f;
        float startX = WINDOW_X - 540;
        float startY = WINDOW_Y - 180;

        for (int i = 0; i < 4; ++i)
        {
            sf::RectangleShape indicator(sf::Vector2f(indicatorSize, indicatorSize));
            indicator.setPosition(sf::Vector2f(startX + i * spacing, startY));
            indicator.setOutlineThickness(2.f);

            if (dino.collected[i])
            {
                indicator.setFillColor(sf::Color(100, 220, 100));    // green = have it
                indicator.setOutlineColor(sf::Color(50, 180, 50));
            }
            else
            {
                indicator.setFillColor(sf::Color(50, 50, 50, 150));  // dark = missing
                indicator.setOutlineColor(sf::Color(150, 150, 150));
            }

            window.draw(indicator);
        }
    }

    window.draw(m_dinoNameText);
    window.draw(m_leftArrow);
    window.draw(m_rightArrow);
    window.draw(m_backSprite);

    // Restore previous (world) view
    window.setView(prev);
}

//------------------------------------------------------------
// updateButtonPositions  (screen coords)
//------------------------------------------------------------
void MuseumInterior::updateButtonPositions(const sf::RenderWindow& window)
{
    sf::Vector2u winSize = window.getSize();

    // Left arrow – vertically centred, close to left edge
    m_leftArrow.setPosition(sf::Vector2f(310.f, winSize.y / 2.f - m_arrowFrameH * m_leftArrow.getScale().y / 2.f));

    // Right arrow – mirrored on the right edge
    m_rightArrow.setPosition(sf::Vector2f(winSize.x - 310.f - m_arrowFrameW * m_rightArrow.getScale().x, winSize.y / 2.f - m_arrowFrameH * m_rightArrow.getScale().y / 2.f));

    // Back button – bottom-left corner
    m_backSprite.setPosition(sf::Vector2f(310.f, winSize.y - m_backFrameH * m_backSprite.getScale().y - 160.f));
}

//------------------------------------------------------------
// pieceIdToIndex
// Maps a piece id string to [0]=skull [1]=torso [2]=pelvis [3]=tail
// Works by looking for keywords anywhere in the id (case insensitive)
//------------------------------------------------------------
int MuseumInterior::pieceIdToIndex(const std::string& pieceId) const
{
    // Lowercase copy for matching
    std::string lower = pieceId;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find("skull") != std::string::npos) return 0;
    if (lower.find("torso") != std::string::npos) return 1;
    if (lower.find("pelvis") != std::string::npos) return 2;
    if (lower.find("tail") != std::string::npos) return 3;

    std::cerr << "MuseumInterior: unknown piece id '" << pieceId << "'\n";
    return -1;
}

//------------------------------------------------------------
// containsPoint helpers
//------------------------------------------------------------
bool MuseumInterior::containsPoint(const sf::Sprite& sprite, const sf::Vector2f& pt) const
{
    return sprite.getGlobalBounds().contains(pt);
}

  
MuseumInterior::DisplaySettings MuseumInterior::getDisplaySettings(const std::string& dinoName, const sf::Vector2u& bgSize) const
{
    float targetH = WINDOW_Y * 0.60f;
    float scale = targetH / static_cast<float>(bgSize.y);
    float posX = WINDOW_X / 2.f;
    float posY = WINDOW_Y / 2.f;
    float humanScale = 0.8f;  // default human scale
    float humanPosX = WINDOW_X * 0.25f;  // default left side for comparison
    float humanPosY = WINDOW_Y * 0.65f;  // default bottom alignment

    if (dinoName.find("Tyrannosaurus rex") != std::string::npos)
    {
        scale *= 0.6f;
        posY += 90.f;
        humanScale = 0.6f;
        humanPosX = posX - 200;
        humanPosY += 60;
    }
    else if (dinoName.find("Allosaurus fragilis") != std::string::npos)
    {
        scale *= 0.5f;
        posY += 125.f;
        humanScale = 0.8f;
        humanPosX = posX -300;
        humanPosY += 40;
    }
    else if (dinoName.find("Spinosaurus aegyptiacus") != std::string::npos)
    {
        scale *= 0.7f;
        posX += 20.f;
        posY += 75.f;
        humanScale = 0.6f;
        humanPosX = posX - 300;
        humanPosY += 60;
    }
    else if (dinoName.find("Maip macrothorax") != std::string::npos)
    {
        scale *= 0.56f;
        posY += 115.f;
        humanScale = 0.75f;
        humanPosX = posX - 250;
        humanPosY += 40;
    }
    else if (dinoName.find("Triceratops horridus") != std::string::npos)
    {
        scale *= 0.7f;
        posX -= 50.f;
        posY += 90.f;
        humanScale = 0.7f;
        humanPosX = posX - 200;
        humanPosY += 40;
    }
    else if (dinoName.find("Dreadnoughtus schrani") != std::string::npos)
    {
        scale *= 0.9f;
        posY += 15.f;
        humanScale = 0.3f;
        humanPosX = posX - 150;
        humanPosY += 83;
    }
    else if (dinoName.find("Ankylosaurus magniventris") != std::string::npos)
    {
        scale *= 0.55f;
        posX += 50.f;
        posY += 120.f;
        humanScale = 0.75f;
        humanPosX = posX - 300;
        humanPosY += 40;
    }
    else if (dinoName.find("Therizinosaurus cheloniformis") != std::string::npos)
    {
        scale *= 0.9f;
        posY += 30.f;
        humanScale = 0.55f;
        humanPosX = posX - 200;
        humanPosY += 60;
    }
    else if (dinoName.find("Pteranodon longiceps") != std::string::npos)
    {
        scale *= 0.6f;
        posX += 20.f;
        posY += 75.f;
        humanScale = 0.75f;
        humanPosX = posX - 300;
        humanPosY += 40;
    }
    else if (dinoName.find("Quetzalcoatlus northropi") != std::string::npos)
    {
        scale *= 0.6f;
        posX += 20.f;
        posY += 75.f;
        humanScale = 0.5f;
        humanPosX = posX - 200;
        humanPosY += 60;
    }

    return { scale, sf::Vector2f(posX, posY), humanScale, sf::Vector2f(humanPosX, humanPosY) };
}