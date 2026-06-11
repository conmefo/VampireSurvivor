#pragma once

#include "UIPanel.h"

class TextureAtlas;

class RosterGridPanel : public UIPanel
{
public:
    RosterGridPanel(TextureAtlas& atlas) : UIPanel(atlas, "frameB9", 10, 10, 10, 10)
    {
    }
};
