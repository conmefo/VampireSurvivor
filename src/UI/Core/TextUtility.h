#pragma once

#include <SFML/Graphics/Text.hpp>
#include <string>

namespace UI
{
    class TextUtility
    {
    public:
        static void WrapText(sf::Text& text, float maxWidth);
    };
}
