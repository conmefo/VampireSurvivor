#pragma once

#include <SFML/Graphics.hpp>

class NineSliceComponent : public sf::Drawable, public sf::Transformable
{
public:
    NineSliceComponent();
    ~NineSliceComponent() override = default;

    void SetTexture(const sf::Texture& texture);
    void SetMargins(float left, float top, float right, float bottom);
    void SetSize(const sf::Vector2f& size);
    void SetColor(const sf::Color& color);
    
    void Update();

private:
    void RecalculateVertices();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    const sf::Texture* m_sourceTexture;
    float m_marginLeft;
    float m_marginTop;
    float m_marginRight;
    float m_marginBottom;
    sf::Vector2f m_targetSize;
    sf::Color m_color;
    
    sf::VertexArray m_vertexArray;
    bool m_isDirty;
};
