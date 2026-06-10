#include "NineSliceComponent.h"

#include "../../Core/Resources/TextureAtlas.h"
#include <cmath>
#include <algorithm>

NineSliceComponent::NineSliceComponent()
    : m_sourceTexture(nullptr)
    , m_marginLeft(0.0f)
    , m_marginTop(0.0f)
    , m_marginRight(0.0f)
    , m_marginBottom(0.0f)
    , m_targetSize(0.0f, 0.0f)
    , m_color(sf::Color::White)
    , m_cornerScale(1.0f)
    , m_vertexArray(sf::Quads, 36)
    , m_isDirty(true)
{
}

void NineSliceComponent::SetTexture(const TextureAtlas& atlas, const std::string& assetId)
{
    AssetTextureData data = atlas.GetTextureData(assetId);
    if(data.texture)
    {
        m_sourceTexture = data.texture;
        m_textureRect = data.rect;
        
        if (m_textureRect.width == 0 || m_textureRect.height == 0)
        {
            m_textureRect = sf::IntRect(0, 0, m_sourceTexture->getSize().x, m_sourceTexture->getSize().y);
        }
        
        m_isDirty = true;
    }
}

void NineSliceComponent::SetMargins(float left, float top, float right, float bottom)
{
    m_marginLeft = left;
    m_marginTop = top;
    m_marginRight = right;
    m_marginBottom = bottom;
    m_isDirty = true;
}

void NineSliceComponent::SetSize(const sf::Vector2f& size)
{
    m_targetSize = size;
    m_isDirty = true;
}

void NineSliceComponent::SetColor(const sf::Color& color)
{
    m_color = color;
    m_isDirty = true;
}

void NineSliceComponent::SetCornerScale(float scale)
{
    m_cornerScale = scale;
    m_isDirty = true;
}

void NineSliceComponent::Update()
{
    if(m_isDirty)
    {
        RecalculateVertices();
        m_isDirty = false;
    }
}

void NineSliceComponent::RecalculateVertices()
{
    if(m_sourceTexture == nullptr)
    {
        return;
    }

    float targetCornerScale = m_cornerScale;

    // Geometry Partitioning
    float scaledMarginLeft = m_marginLeft * targetCornerScale;
    float scaledMarginRight = m_marginRight * targetCornerScale;
    float scaledMarginTop = m_marginTop * targetCornerScale;
    float scaledMarginBottom = m_marginBottom * targetCornerScale;

    float xCoords[4] = { 0.0f, scaledMarginLeft, m_targetSize.x - scaledMarginRight, m_targetSize.x };
    float yCoords[4] = { 0.0f, scaledMarginTop, m_targetSize.y - scaledMarginBottom, m_targetSize.y };

    float uCoords[4] = { 
        static_cast<float>(m_textureRect.left), 
        static_cast<float>(m_textureRect.left) + m_marginLeft, 
        static_cast<float>(m_textureRect.left + m_textureRect.width) - m_marginRight, 
        static_cast<float>(m_textureRect.left + m_textureRect.width) 
    };
    
    float vCoords[4] = { 
        static_cast<float>(m_textureRect.top), 
        static_cast<float>(m_textureRect.top) + m_marginTop, 
        static_cast<float>(m_textureRect.top + m_textureRect.height) - m_marginBottom, 
        static_cast<float>(m_textureRect.top + m_textureRect.height) 
    };

    int vertexIndex = 0;

    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            m_vertexArray[vertexIndex + 0].position = sf::Vector2f(xCoords[x], yCoords[y]);
            m_vertexArray[vertexIndex + 0].texCoords = sf::Vector2f(uCoords[x], vCoords[y]);
            m_vertexArray[vertexIndex + 0].color = m_color;

            m_vertexArray[vertexIndex + 1].position = sf::Vector2f(xCoords[x + 1], yCoords[y]);
            m_vertexArray[vertexIndex + 1].texCoords = sf::Vector2f(uCoords[x + 1], vCoords[y]);
            m_vertexArray[vertexIndex + 1].color = m_color;

            m_vertexArray[vertexIndex + 2].position = sf::Vector2f(xCoords[x + 1], yCoords[y + 1]);
            m_vertexArray[vertexIndex + 2].texCoords = sf::Vector2f(uCoords[x + 1], vCoords[y + 1]);
            m_vertexArray[vertexIndex + 2].color = m_color;

            m_vertexArray[vertexIndex + 3].position = sf::Vector2f(xCoords[x], yCoords[y + 1]);
            m_vertexArray[vertexIndex + 3].texCoords = sf::Vector2f(uCoords[x], vCoords[y + 1]);
            m_vertexArray[vertexIndex + 3].color = m_color;

            vertexIndex += 4;
        }
    }
}

void NineSliceComponent::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if(m_sourceTexture == nullptr)
    {
        return;
    }

    states.transform *= getTransform();
    states.texture = m_sourceTexture;
    
    target.draw(m_vertexArray, states);
}
