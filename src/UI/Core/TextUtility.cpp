#include "TextUtility.h"
#include <SFML/System/String.hpp>

namespace UI
{
    void TextUtility::WrapText(sf::Text& text, float maxWidth)
    {
        sf::String sourceStr = text.getString();
        sf::String resultStr;
        sf::String currentLine;
        sf::String word;

        for (std::size_t i = 0; i < sourceStr.getSize(); ++i)
        {
            sf::Uint32 c = sourceStr[i];
            if (c == ' ' || c == '\n')
            {
                if (!word.isEmpty())
                {
                    sf::String testLine = currentLine.isEmpty() ? word : currentLine + " " + word;
                    text.setString(testLine);
                    if (text.getLocalBounds().width > maxWidth)
                    {
                        resultStr += currentLine + "\n";
                        currentLine = word;
                    }
                    else
                    {
                        currentLine = testLine;
                    }
                    word.clear();
                }
                if (c == '\n')
                {
                    resultStr += currentLine + "\n";
                    currentLine.clear();
                }
            }
            else
            {
                word += c;
            }
        }

        if (!word.isEmpty())
        {
            sf::String testLine = currentLine.isEmpty() ? word : currentLine + " " + word;
            text.setString(testLine);
            if (text.getLocalBounds().width > maxWidth)
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
}
