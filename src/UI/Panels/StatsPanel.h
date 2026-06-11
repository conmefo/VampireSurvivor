#pragma once

#include "UIPanel.h"

class TextureAtlas;

class StatsPanel : public UIPanel
{
public:
    StatsPanel(TextureAtlas& atlas) : UIPanel(atlas, "frameB9", 10, 10, 10, 10)
    {
    }
};
