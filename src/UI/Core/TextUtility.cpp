#include "TextUtility.h"

namespace UI
{
    void TextUtility::WrapText(sf::Text& text, float maxWidth)
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
}
