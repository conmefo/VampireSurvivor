#include "WarningState.h"
#include "../Title/TitleState.h"
#include "../StateManager.h"
#include "../../Core/WindowSettings.h"
#include <iostream>

WarningState::WarningState(StateContext context)
    : BaseState(std::move(context))
    , m_fadeDurationIn(0.5f)
    , m_fadeDurationOut(0.3f)
    , m_elapsedTime(0.0f)
    , m_opacity(0.0f)
    , m_fadeState(FadeState::FadeIn)
    , m_inputLocked(false)
{
}

void WarningState::Init()
{
    const sf::Font* fontPtr = m_context.fonts.GetPtr(FontID::Warning);
    if (!fontPtr)
    {
        try
        {
            m_context.fonts.Load(FontID::Warning, "assets/fonts/LiberationSans.ttf");
            fontPtr = m_context.fonts.GetPtr(FontID::Warning);
        }
        catch (...)
        {
            std::cerr << "Failed to load LiberationSans.ttf" << std::endl;
        }
    }

    if(fontPtr)
    {
        m_font = *fontPtr;
    }

    m_headerText.setFont(m_font);
    m_headerText.setString("PHOTOSENSITIVITY WARNING");
    m_headerText.setCharacterSize(55);
    m_headerText.setFillColor(sf::Color(255, 68, 68));

    m_bodyText1.setFont(m_font);
    m_bodyText1.setString("This game contains bright flashing lights. Please immediately stop playing and consult a doctor if you experience lightheadedness, altered vision, eye or face twitching, jerking or shaking of arms or legs, disorientation, confusion, or momentary loss of awareness.");
    m_bodyText1.setCharacterSize(33);
    m_bodyText1.setFillColor(sf::Color::White);

    m_bodyText2.setFont(m_font);
    m_bodyText2.setString("If you or any of your relatives have a history of seizures or epilepsy, consult a doctor before playing.");
    m_bodyText2.setCharacterSize(33);
    m_bodyText2.setFillColor(sf::Color::White);

    m_promptText.setFont(m_font);
    m_promptText.setString("PRESS TO START");
    m_promptText.setCharacterSize(35);
    m_promptText.setFillColor(sf::Color::White);

    WrapText(m_bodyText1, 1150.0f);
    WrapText(m_bodyText2, 1150.0f);

    // Calculate origins and positions once after text is wrapped
    sf::Vector2f center(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f);

    sf::FloatRect headerRect = m_headerText.getLocalBounds();
    m_headerText.setOrigin(headerRect.left + headerRect.width / 2.0f, headerRect.top + headerRect.height / 2.0f);
    m_headerText.setPosition(center.x, center.y - 280.0f);


    sf::FloatRect body1Rect = m_bodyText1.getLocalBounds();
    m_bodyText1.setOrigin(body1Rect.left + body1Rect.width / 2.0f, body1Rect.top + body1Rect.height / 2.0f);
    m_bodyText1.setPosition(center.x, center.y - 70.0f);

    sf::FloatRect body2Rect = m_bodyText2.getLocalBounds();
    m_bodyText2.setOrigin(body2Rect.left + body2Rect.width / 2.0f, body2Rect.top + body2Rect.height / 2.0f);
    m_bodyText2.setPosition(center.x, center.y + 100.0f);

    sf::FloatRect promptRect = m_promptText.getLocalBounds();
    m_promptText.setOrigin(promptRect.left + promptRect.width / 2.0f, promptRect.top + promptRect.height / 2.0f);
    m_promptText.setPosition(center.x, center.y + 310.0f);

    UpdateOpacity(m_opacity);
}

void WarningState::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    if(m_fadeState == FadeState::Visible && !m_inputLocked)
    {
        if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::JoystickButtonPressed)
        {
            m_inputLocked = true;
            m_fadeState = FadeState::FadeOut;
            m_elapsedTime = 0.0f;
        }
    }
}

void WarningState::Update(float dt)
{
    m_elapsedTime += dt;

    if(m_fadeState == FadeState::FadeIn)
    {
        m_opacity = m_elapsedTime / m_fadeDurationIn;
        if(m_opacity >= 1.0f)
        {
            m_opacity = 1.0f;
            m_fadeState = FadeState::Visible;
            m_elapsedTime = 0.0f;
        }
        UpdateOpacity(m_opacity);
    }
    else if(m_fadeState == FadeState::FadeOut)
    {
        m_opacity = 1.0f - (m_elapsedTime / m_fadeDurationOut);
        if(m_opacity <= 0.0f)
        {
            m_opacity = 0.0f;
            m_fadeState = FadeState::Finished;
            m_context.stateManager.PopState();
            m_context.stateManager.AddState(std::make_unique<TitleState>(m_context));
        }
        UpdateOpacity(m_opacity);
    }
}

void WarningState::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);

    window.draw(m_headerText);
    window.draw(m_bodyText1);
    window.draw(m_bodyText2);
    window.draw(m_promptText);
}

void WarningState::WrapText(sf::Text& text, float maxWidth)
{
    std::string sourceStr = text.getString().toAnsiString();
    std::string resultStr = "";
    std::string currentLine = "";
    std::string word = "";

    for(char c : sourceStr)
    {
        if(c == ' ' || c == '\n')
        {
            if(!word.empty())
            {
                std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                text.setString(testLine);
                if(text.getLocalBounds().width > maxWidth)
                {
                    resultStr += currentLine + "\n";
                    currentLine = word;
                }
                else
                {
                    currentLine = testLine;
                }
                word = "";
            }
            if(c == '\n')
            {
                resultStr += currentLine + "\n";
                currentLine = "";
            }
        }
        else
        {
            word += c;
        }
    }

    if(!word.empty())
    {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        text.setString(testLine);
        if(text.getLocalBounds().width > maxWidth)
        {
            resultStr += currentLine + "\n";
            currentLine = word;
        }
        else
        {
            currentLine = testLine;
        }
    }
    resultStr += currentLine;
    text.setString(resultStr);
}

void WarningState::UpdateOpacity(float opacity)
{
    sf::Uint8 alpha = static_cast<sf::Uint8>(opacity * 255.0f);

    sf::Color headerColor = m_headerText.getFillColor();
    headerColor.a = alpha;
    m_headerText.setFillColor(headerColor);

    sf::Color body1Color = m_bodyText1.getFillColor();
    body1Color.a = alpha;
    m_bodyText1.setFillColor(body1Color);

    sf::Color body2Color = m_bodyText2.getFillColor();
    body2Color.a = alpha;
    m_bodyText2.setFillColor(body2Color);

    sf::Color promptColor = m_promptText.getFillColor();
    promptColor.a = alpha;
    m_promptText.setFillColor(promptColor);
}
